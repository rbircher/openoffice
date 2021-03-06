/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_framework.hxx"

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________
#include "uiconfiguration/globalsettings.hxx"
#include <threadhelp/resetableguard.hxx>
#include "services.h"

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/container/XContainer.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XEventListener.hpp>

//_________________________________________________________________________________________________________________
//	includes of other projects
//_________________________________________________________________________________________________________________
#include <rtl/ustrbuf.hxx>
#include <rtl/instance.hxx>
#include <cppuhelper/weak.hxx>
#include <tools/debug.hxx>

//_________________________________________________________________________________________________________________
//	Defines
//_________________________________________________________________________________________________________________
// 

using namespace rtl;
using namespace ::com::sun::star;

//_________________________________________________________________________________________________________________
//	Namespace
//_________________________________________________________________________________________________________________
// 

static const char GLOBALSETTINGS_ROOT_ACCESS[]              = "/org.openoffice.Office.UI.GlobalSettings/Toolbars";

static const char GLOBALSETTINGS_NODEREF_STATES[]           = "States";
static const char GLOBALSETTINGS_PROPERTY_LOCKED[]          = "Locked";
static const char GLOBALSETTINGS_PROPERTY_DOCKED[]          = "Docked";
static const char GLOBALSETTINGS_PROPERTY_STATESENABLED[]   = "StatesEnabled";

namespace framework
{

//*****************************************************************************************************************
//	Configuration access class for WindowState supplier implementation
//*****************************************************************************************************************

class GlobalSettings_Access : public ::com::sun::star::lang::XComponent	     ,
                              public ::com::sun::star::lang::XEventListener  ,
						      private ThreadHelpBase						 ,	// Struct for right initalization of mutex member! Must be first of baseclasses.
						      public ::cppu::OWeakObject
{
	public:
		GlobalSettings_Access( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager );
		virtual ~GlobalSettings_Access();

        // XInterface, XTypeProvider, XServiceInfo
		FWK_DECLARE_XINTERFACE

        // XComponent
        virtual void SAL_CALL dispose() throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener ) throw (::com::sun::star::uno::RuntimeException);

        // XEventListener
        virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException);

        // settings access
        sal_Bool HasStatesInfo( GlobalSettings::UIElementType eElementType );
        sal_Bool GetStateInfo( GlobalSettings::UIElementType eElementType, GlobalSettings::StateInfo eStateInfo, ::com::sun::star::uno::Any& aValue );
    
    private:
        sal_Bool impl_initConfigAccess();

        sal_Bool                                                                            m_bDisposed   : 1,
                                                                                            m_bConfigRead : 1;
        rtl::OUString                                                                       m_aConfigSettingsAccess;
        rtl::OUString                                                                       m_aNodeRefStates;
        rtl::OUString                                                                       m_aPropStatesEnabled;
        rtl::OUString                                                                       m_aPropLocked;
        rtl::OUString                                                                       m_aPropDocked;
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >        m_xConfigAccess;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >    m_xServiceManager;
};


//*****************************************************************************************************************
//	XInterface
//*****************************************************************************************************************
DEFINE_XINTERFACE_2     (   GlobalSettings_Access                           ,
                            OWeakObject                                     ,
                            DIRECT_INTERFACE ( css::lang::XComponent        ),
                            DIRECT_INTERFACE ( css::lang::XEventListener    )
						)

GlobalSettings_Access::GlobalSettings_Access( const css::uno::Reference< css::lang::XMultiServiceFactory >& rServiceManager ) :
    ThreadHelpBase(),
    m_bDisposed( sal_False ),
    m_bConfigRead( sal_False ),
    m_aConfigSettingsAccess( RTL_CONSTASCII_USTRINGPARAM( GLOBALSETTINGS_ROOT_ACCESS )),
    m_aNodeRefStates( RTL_CONSTASCII_USTRINGPARAM( GLOBALSETTINGS_NODEREF_STATES )),
    m_aPropStatesEnabled( RTL_CONSTASCII_USTRINGPARAM( GLOBALSETTINGS_PROPERTY_STATESENABLED )),
    m_aPropLocked( RTL_CONSTASCII_USTRINGPARAM( GLOBALSETTINGS_PROPERTY_LOCKED )),
    m_aPropDocked( RTL_CONSTASCII_USTRINGPARAM( GLOBALSETTINGS_PROPERTY_DOCKED )),
    m_xServiceManager( rServiceManager )
{
}

GlobalSettings_Access::~GlobalSettings_Access()
{
}

// XComponent
void SAL_CALL GlobalSettings_Access::dispose() 
throw ( css::uno::RuntimeException )
{
    // SAFE
    ResetableGuard aLock( m_aLock );
    
    m_xConfigAccess.clear();
    m_bDisposed = sal_True;
}
        
void SAL_CALL GlobalSettings_Access::addEventListener( const css::uno::Reference< css::lang::XEventListener >& ) 
throw (css::uno::RuntimeException)
{
}

void SAL_CALL GlobalSettings_Access::removeEventListener( const css::uno::Reference< css::lang::XEventListener >& ) 
throw (css::uno::RuntimeException)
{
}

// XEventListener
void SAL_CALL GlobalSettings_Access::disposing( const css::lang::EventObject& ) 
throw (css::uno::RuntimeException)
{
    // SAFE
    ResetableGuard aLock( m_aLock );
    m_xConfigAccess.clear();
}

// settings access
sal_Bool GlobalSettings_Access::HasStatesInfo( GlobalSettings::UIElementType eElementType )
{
    ResetableGuard aLock( m_aLock );
    if ( eElementType == GlobalSettings::UIELEMENT_TYPE_DOCKWINDOW )
        return sal_False;
    else if ( eElementType == GlobalSettings::UIELEMENT_TYPE_STATUSBAR )
        return sal_False;

    if ( m_bDisposed )
        return sal_False;

    if ( !m_bConfigRead )
    {   
        m_bConfigRead = sal_True;
        impl_initConfigAccess();
    }

    if ( m_xConfigAccess.is() )
    {
        try
        {
            css::uno::Any a;
            sal_Bool      bValue = sal_Bool();
            a = m_xConfigAccess->getByName( m_aPropStatesEnabled );
            if ( a >>= bValue )
                return bValue;
        }
        catch ( css::container::NoSuchElementException& )
        {
        }
        catch ( css::uno::Exception& )
        {
        }
    }

    return sal_False;
}

sal_Bool GlobalSettings_Access::GetStateInfo( GlobalSettings::UIElementType eElementType, GlobalSettings::StateInfo eStateInfo, ::com::sun::star::uno::Any& aValue )
{
    ResetableGuard aLock( m_aLock );
    if ( eElementType == GlobalSettings::UIELEMENT_TYPE_DOCKWINDOW )
        return sal_False;
    else if ( eElementType == GlobalSettings::UIELEMENT_TYPE_STATUSBAR )
        return sal_False;

    if ( m_bDisposed )
        return sal_False;
    
    if ( !m_bConfigRead )
    {   
        m_bConfigRead = sal_True;
        impl_initConfigAccess();
    }

    if ( m_xConfigAccess.is() )
    {
        try
        {
            css::uno::Any a;
            a = m_xConfigAccess->getByName( m_aNodeRefStates );
            css::uno::Reference< css::container::XNameAccess > xNameAccess;
            if ( a >>= xNameAccess )
            {
                if ( eStateInfo == GlobalSettings::STATEINFO_LOCKED )
                    a = xNameAccess->getByName( m_aPropLocked );
                else if ( eStateInfo == GlobalSettings::STATEINFO_DOCKED )
                    a = xNameAccess->getByName( m_aPropDocked );
                
                aValue = a;
                return sal_True;
            }
        }
        catch ( css::container::NoSuchElementException& )
        {
        }
        catch ( css::uno::Exception& )
        {
        }
    }

    return sal_False;
}

sal_Bool GlobalSettings_Access::impl_initConfigAccess()
{
    css::uno::Sequence< css::uno::Any > aArgs( 2 );
    css::beans::PropertyValue           aPropValue;
    
    try
    {
        css::uno::Reference< css::lang::XMultiServiceFactory > xConfigProvider;
        if ( m_xServiceManager.is() )
            xConfigProvider = css::uno::Reference< css::lang::XMultiServiceFactory >( 
                                    m_xServiceManager->createInstance( SERVICENAME_CFGPROVIDER ), 
                                    css::uno::UNO_QUERY );
        
        if ( xConfigProvider.is() )
        {
            aPropValue.Name  = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "nodepath" ));
            aPropValue.Value = css::uno::makeAny( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( GLOBALSETTINGS_ROOT_ACCESS )));
            aArgs[0] = css::uno::makeAny( aPropValue );
            aPropValue.Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "lazywrite" ));
            aPropValue.Value = css::uno::makeAny( sal_True );
            aArgs[1] = css::uno::makeAny( aPropValue );
        
            m_xConfigAccess = css::uno::Reference< css::container::XNameAccess >( 
                                xConfigProvider->createInstanceWithArguments( 
                                    SERVICENAME_CFGREADACCESS, aArgs ), 
                                css::uno::UNO_QUERY );

            css::uno::Reference< css::lang::XComponent > xComponent( xConfigProvider, css::uno::UNO_QUERY );
            if ( xComponent.is() )
                xComponent->addEventListener( 
                    css::uno::Reference< css::lang::XEventListener >( 
                        static_cast< cppu::OWeakObject* >( this ),
                        css::uno::UNO_QUERY ));
        }

        return m_xConfigAccess.is();
    }
    catch ( css::lang::WrappedTargetException& )
    {
    }
    catch ( css::uno::Exception& )
    {
    }

    return sal_False;
}

//*****************************************************************************************************************
//	global class
//*****************************************************************************************************************

struct mutexGlobalSettings : public rtl::Static< osl::Mutex, mutexGlobalSettings > {};
static GlobalSettings_Access* pStaticSettings = 0;

static GlobalSettings_Access* GetGlobalSettings( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& rSrvMgr )
{
    osl::MutexGuard aGuard(mutexGlobalSettings::get());
    if ( !pStaticSettings ) 
        pStaticSettings = new GlobalSettings_Access( rSrvMgr );
    return pStaticSettings;
}

GlobalSettings::GlobalSettings( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& rSrvMgr ) :
    m_xSrvMgr( rSrvMgr )
{
}

GlobalSettings::~GlobalSettings()
{
}

// settings access
sal_Bool GlobalSettings::HasStatesInfo( UIElementType eElementType )
{
    GlobalSettings_Access* pSettings( GetGlobalSettings( m_xSrvMgr ));
    
    if ( pSettings )
        return pSettings->HasStatesInfo( eElementType );
    else
        return sal_False;
}

sal_Bool GlobalSettings::GetStateInfo( UIElementType eElementType, StateInfo eStateInfo, ::com::sun::star::uno::Any& aValue )
{
    GlobalSettings_Access* pSettings( GetGlobalSettings( m_xSrvMgr ));
    
    if ( pSettings )
        return pSettings->GetStateInfo( eElementType, eStateInfo, aValue );
    else
        return sal_False;
}

} // namespace framework
