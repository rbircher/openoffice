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



#ifndef _DYNAMIC_RESULTSET_WRAPPER_HXX
#define _DYNAMIC_RESULTSET_WRAPPER_HXX

#include <osl/mutex.hxx>
#include <osl/conditn.hxx>
#include <ucbhelper/macros.hxx>
#include <cppuhelper/weak.hxx>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/interfacecontainer.hxx>
#include <com/sun/star/ucb/XDynamicResultSet.hpp>
#include <com/sun/star/ucb/XSourceInitialization.hpp>

#ifndef __com_sun_star_lang_DisposedException_idl__
#include <com/sun/star/lang/DisposedException.hpp>
#endif
#include <com/sun/star/ucb/XDynamicResultSetListener.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

//#define CACHED_CRS_STUB_SERVICE_NAME "com.sun.star.ucb.CachedContentResultSetStub"
//#define CACHED_CRS_STUB_FACTORY_NAME "com.sun.star.ucb.CachedContentResultSetStubFactory"

//=========================================================================

class DynamicResultSetWrapperListener;
class DynamicResultSetWrapper
				: public cppu::OWeakObject
				, public com::sun::star::ucb::XDynamicResultSet
				, public com::sun::star::ucb::XSourceInitialization
{
private:
	//management of listeners
	sal_Bool				m_bDisposed; ///Dispose call ready.
	sal_Bool				m_bInDispose;///In dispose call
	osl::Mutex				m_aContainerMutex;
	cppu::OInterfaceContainerHelper*
							m_pDisposeEventListeners;
protected:
	com::sun::star::uno::Reference< com::sun::star::ucb::XDynamicResultSetListener >
							m_xMyListenerImpl;
	DynamicResultSetWrapperListener*
							m_pMyListenerImpl;

	com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >
							m_xSMgr;

	osl::Mutex				m_aMutex;
	sal_Bool				m_bStatic;
	sal_Bool				m_bGotWelcome;

	//different Interfaces from Origin:
	com::sun::star::uno::Reference< com::sun::star::ucb::XDynamicResultSet >
							m_xSource;
	com::sun::star::uno::Reference< com::sun::star::sdbc::XResultSet >
							m_xSourceResultOne;
	com::sun::star::uno::Reference< com::sun::star::sdbc::XResultSet >
							m_xSourceResultTwo;
	//com::sun::star::uno::Reference< com::sun::star::sdbc::XResultSet >
	//						m_xSourceResultCurrent;
	//sal_Bool				m_bUseOne;
	//
	com::sun::star::uno::Reference< com::sun::star::sdbc::XResultSet >
							m_xMyResultOne;
	com::sun::star::uno::Reference< com::sun::star::sdbc::XResultSet >
							m_xMyResultTwo;
	//
	com::sun::star::uno::Reference< com::sun::star::ucb::XDynamicResultSetListener >
							m_xListener;

	osl::Condition			m_aSourceSet;
	osl::Condition			m_aListenerSet;

protected:
	void SAL_CALL impl_init();
	void SAL_CALL impl_deinit();
	void SAL_CALL
	impl_EnsureNotDisposed()
		throw( com::sun::star::lang::DisposedException,
			   com::sun::star::uno::RuntimeException );

	virtual void SAL_CALL
	impl_InitResultSetOne( const com::sun::star::uno::Reference<
				com::sun::star::sdbc::XResultSet >& xResultSet );
	virtual void SAL_CALL
	impl_InitResultSetTwo( const com::sun::star::uno::Reference<
				com::sun::star::sdbc::XResultSet >& xResultSet );

public:

	DynamicResultSetWrapper(
		com::sun::star::uno::Reference<
						com::sun::star::ucb::XDynamicResultSet > xOrigin
		, const com::sun::star::uno::Reference<
						com::sun::star::lang::XMultiServiceFactory > & xSMgr );

	virtual ~DynamicResultSetWrapper();

	//-----------------------------------------------------------------
	// XInterface
	virtual com::sun::star::uno::Any SAL_CALL
	queryInterface( const com::sun::star::uno::Type & rType )
		throw( com::sun::star::uno::RuntimeException );

	//-----------------------------------------------------------------
    // XDynamicResultSet
	virtual com::sun::star::uno::Reference< com::sun::star::sdbc::XResultSet > SAL_CALL
	getStaticResultSet()
		throw( com::sun::star::ucb::ListenerAlreadySetException
		, com::sun::star::uno::RuntimeException );

	virtual void SAL_CALL
	setListener( const com::sun::star::uno::Reference<
		com::sun::star::ucb::XDynamicResultSetListener > & Listener )
		throw( com::sun::star::ucb::ListenerAlreadySetException
			, com::sun::star::uno::RuntimeException );

	virtual void SAL_CALL
	connectToCache( const com::sun::star::uno::Reference<
		com::sun::star::ucb::XDynamicResultSet > & xCache )
		throw( com::sun::star::ucb::ListenerAlreadySetException
			, com::sun::star::ucb::AlreadyInitializedException
			, com::sun::star::ucb::ServiceNotFoundException
			, com::sun::star::uno::RuntimeException );

	virtual sal_Int16 SAL_CALL
	getCapabilities() throw( com::sun::star::uno::RuntimeException );

	//-----------------------------------------------------------------
	// XComponent ( base of XDynamicResultSet )
	virtual void SAL_CALL
	dispose() throw( com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL
	addEventListener( const com::sun::star::uno::Reference<
		com::sun::star::lang::XEventListener >& Listener )
			throw( com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL
	removeEventListener( const com::sun::star::uno::Reference<
		com::sun::star::lang::XEventListener >& Listener )
			throw( com::sun::star::uno::RuntimeException );

	//-----------------------------------------------------------------
    // XSourceInitialization

	virtual void SAL_CALL
	setSource(	const com::sun::star::uno::Reference<
		com::sun::star::uno::XInterface > & Source )
		 throw( com::sun::star::ucb::AlreadyInitializedException
				, com::sun::star::uno::RuntimeException );

	//-----------------------------------------------------------------
	// own methods:
    virtual void SAL_CALL
	impl_disposing( const com::sun::star::lang::EventObject& Source )
		throw( com::sun::star::uno::RuntimeException );

	virtual void SAL_CALL
	impl_notify( const ::com::sun::star::ucb::ListEvent& Changes )
		throw( com::sun::star::uno::RuntimeException );
};

//=========================================================================

class DynamicResultSetWrapperListener
				: public cppu::OWeakObject
				, public com::sun::star::ucb::XDynamicResultSetListener
{
protected:
	DynamicResultSetWrapper*	m_pOwner;
	osl::Mutex					m_aMutex;

public:
	DynamicResultSetWrapperListener( DynamicResultSetWrapper* pOwner );

	virtual ~DynamicResultSetWrapperListener();

	//-----------------------------------------------------------------
	// XInterface
	//-----------------------------------------------------------------
	XINTERFACE_DECL()

	//-----------------------------------------------------------------
	// XEventListener ( base of XDynamicResultSetListener )
	//-----------------------------------------------------------------
    virtual void SAL_CALL
	disposing( const com::sun::star::lang::EventObject& Source )
		throw( com::sun::star::uno::RuntimeException );
	//-----------------------------------------------------------------
	// XDynamicResultSetListener
    virtual void SAL_CALL
	notify( const ::com::sun::star::ucb::ListEvent& Changes )
		throw( com::sun::star::uno::RuntimeException );

	//-----------------------------------------------------------------
	// own methods:
	void SAL_CALL impl_OwnerDies();
};


#endif

