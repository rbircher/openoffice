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
#include "precompiled_i18npool.hxx"

#include <inputsequencechecker.hxx>
#include <com/sun/star/i18n/InputSequenceCheckMode.hpp>
#include <com/sun/star/i18n/UnicodeType.hpp>
#include <i18nutil/unicode.hxx>
#include <rtl/ustrbuf.hxx>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::rtl;

namespace com { namespace sun { namespace star { namespace i18n {

InputSequenceCheckerImpl::InputSequenceCheckerImpl( const Reference < XMultiServiceFactory >& rxMSF ) : xMSF( rxMSF )
{
        serviceName = "com.sun.star.i18n.InputSequenceCheckerImpl";
        cachedItem = NULL;
}

InputSequenceCheckerImpl::InputSequenceCheckerImpl()
{
}

InputSequenceCheckerImpl::~InputSequenceCheckerImpl()
{
        // Clear lookuptable
        for (size_t l = 0; l < lookupTable.size(); l++)
            delete lookupTable[l];

        lookupTable.clear();
}

sal_Bool SAL_CALL 
InputSequenceCheckerImpl::checkInputSequence(const OUString& Text, sal_Int32 nStartPos,
        sal_Unicode inputChar, sal_Int16 inputCheckMode) throw(RuntimeException)
{
        if (inputCheckMode == InputSequenceCheckMode::PASSTHROUGH)
            return sal_True;

        sal_Char* language = getLanguageByScripType(Text[nStartPos], inputChar);

        if (language)
            return getInputSequenceChecker(language)->checkInputSequence(Text, nStartPos, inputChar, inputCheckMode);
        else
            return sal_True; // not a checkable languages.
}

sal_Int32 SAL_CALL 
InputSequenceCheckerImpl::correctInputSequence(OUString& Text, sal_Int32 nStartPos,
        sal_Unicode inputChar, sal_Int16 inputCheckMode) throw(RuntimeException)
{
        if (inputCheckMode != InputSequenceCheckMode::PASSTHROUGH) {
            sal_Char* language = getLanguageByScripType(Text[nStartPos], inputChar);

            if (language)
                return getInputSequenceChecker(language)->correctInputSequence(Text, nStartPos, inputChar, inputCheckMode);
        }
        Text = Text.replaceAt(++nStartPos, 0, OUString(inputChar));
        return nStartPos;
}

static ScriptTypeList typeList[] = {
        //{ UnicodeScript_kHebrew,              UnicodeScript_kHebrew },        // 10,
        //{ UnicodeScript_kArabic,              UnicodeScript_kArabic },        // 11,
        { UnicodeScript_kDevanagari,UnicodeScript_kDevanagari,          UnicodeScript_kDevanagari },    // 14,
        { UnicodeScript_kThai,  UnicodeScript_kThai,                  UnicodeScript_kThai },          // 24,

        { UnicodeScript_kScriptCount,   UnicodeScript_kScriptCount,           UnicodeScript_kScriptCount }    // 88
};

sal_Char* SAL_CALL
InputSequenceCheckerImpl::getLanguageByScripType(sal_Unicode cChar, sal_Unicode nChar)
{
    sal_Int16 type = unicode::getUnicodeScriptType( cChar, typeList, UnicodeScript_kScriptCount );

    if (type != UnicodeScript_kScriptCount && 
            type == unicode::getUnicodeScriptType( nChar, typeList, UnicodeScript_kScriptCount )) {
        switch(type) {
            case UnicodeScript_kThai:           return (sal_Char*)"th";
            //case UnicodeScript_kArabic:       return (sal_Char*)"ar";
            //case UnicodeScript_kHebrew:       return (sal_Char*)"he";
            case UnicodeScript_kDevanagari:   return (sal_Char*)"hi";
        }
    }
    return NULL;
}

Reference< XExtendedInputSequenceChecker >& SAL_CALL
InputSequenceCheckerImpl::getInputSequenceChecker(sal_Char* rLanguage) throw (RuntimeException)
{
        if (cachedItem && cachedItem->aLanguage == rLanguage) {
            return cachedItem->xISC;
        }
        else if (xMSF.is()) {
            for (size_t l = 0; l < lookupTable.size(); l++) {
                cachedItem = lookupTable[l];
                if (cachedItem->aLanguage == rLanguage)
                    return cachedItem->xISC;
            }

            Reference < uno::XInterface > xI = xMSF->createInstance(
                        OUString::createFromAscii("com.sun.star.i18n.InputSequenceChecker_") + 
                        OUString::createFromAscii(rLanguage));

            if ( xI.is() ) {
                Reference< XExtendedInputSequenceChecker > xISC;
                xI->queryInterface( getCppuType((const Reference< XExtendedInputSequenceChecker>*)0) ) >>= xISC;
                if (xISC.is()) {
                    lookupTable.push_back(cachedItem = new lookupTableItem(rLanguage, xISC));
                    return cachedItem->xISC;
                }
            }
        }
        throw RuntimeException();
}

OUString SAL_CALL
InputSequenceCheckerImpl::getImplementationName(void) throw( RuntimeException )
{
        return OUString::createFromAscii(serviceName);
}

sal_Bool SAL_CALL
InputSequenceCheckerImpl::supportsService(const OUString& rServiceName) throw( RuntimeException )
{
        return !rServiceName.compareToAscii(serviceName);
}

Sequence< OUString > SAL_CALL
InputSequenceCheckerImpl::getSupportedServiceNames(void) throw( RuntimeException )
{
        Sequence< OUString > aRet(1);
        aRet[0] = OUString::createFromAscii(serviceName);
        return aRet;
}

} } } }
