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


#ifndef _CPPUHELPER_WEAKAGG_HXX_
#define _CPPUHELPER_WEAKAGG_HXX_

#include <cppuhelper/weak.hxx>
#include <com/sun/star/uno/XAggregation.hpp>


namespace cppu
{    

/** Base class to implement an UNO object supporting weak references, i.e. the object can be held
    weakly (by a ::com::sun::star::uno::WeakReference) and aggregation, i.e. the object can be
    aggregated by another (delegator).
    This implementation copes with reference counting.  Upon last release(), the virtual dtor
    is called.
    
    @derive
    Inherit from this class and delegate acquire()/ release() calls.  Re-implement
    XAggregation::queryInterface().
*/
class OWeakAggObject
	: public ::cppu::OWeakObject
	, public ::com::sun::star::uno::XAggregation
{
public:
	/** Constructor.  No delegator set.
    */
	inline OWeakAggObject() SAL_THROW( () )
		{}
	
	/** If a delegator is set, then the delegators gets acquired.  Otherwise call is delegated to
        base class ::cppu::OWeakObject.
    */
	virtual void SAL_CALL acquire() throw();
	/** If a delegator is set, then the delegators gets released.  Otherwise call is delegated to
        base class ::cppu::OWeakObject.
    */
	virtual void SAL_CALL release() throw();
	/** If a delegator is set, then the delegator is queried for the demanded interface.  If the
        delegator cannot provide the demanded interface, it calls queryAggregation() on its
        aggregated objects.
        
        @param rType demanded interface type
        @return demanded type or empty any
        @see queryAggregation.
    */
	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType )
		throw(::com::sun::star::uno::RuntimeException);

	/** Set the delegator.  The delegator member reference is a weak reference.
        
        @param Delegator the object that delegate its queryInterface to this aggregate.
    */
	virtual void SAL_CALL setDelegator( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > & Delegator )
		throw(::com::sun::star::uno::RuntimeException);
	/** Called by the delegator or queryInterface. Re-implement this method instead of
        queryInterface.
        
        @see queryInterface
    */
	virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType )
		throw(::com::sun::star::uno::RuntimeException);

protected:
	/** Virtual dtor. Called when reference count is 0.

        @attention
        Despite the fact that a RuntimeException is allowed to be thrown, you must not throw any
        exception upon destruction!
	*/
    virtual ~OWeakAggObject() SAL_THROW( (::com::sun::star::uno::RuntimeException) );
	
	/** weak reference to delegator.
	*/
	::com::sun::star::uno::WeakReferenceHelper xDelegator;
private:
    /** @internal */
	OWeakAggObject( const OWeakAggObject & rObj ) SAL_THROW( () );
    /** @internal */
    OWeakAggObject & operator = ( const OWeakAggObject & rObj ) SAL_THROW( () );
};

}

#endif
