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



#include "sal/config.h"

#include "jvmaccess/unovirtualmachine.hxx"

#include "osl/diagnose.h"

#include "jvmaccess/virtualmachine.hxx"

#if defined SOLAR_JAVA
#include "jni.h"
#endif

namespace jvmaccess {

UnoVirtualMachine::CreationException::CreationException() {}

UnoVirtualMachine::CreationException::CreationException(
    CreationException const &)
{}

UnoVirtualMachine::CreationException::~CreationException() {}

UnoVirtualMachine::CreationException &
UnoVirtualMachine::CreationException::operator =(CreationException const &) {
    return *this;
}

UnoVirtualMachine::UnoVirtualMachine(
    rtl::Reference< jvmaccess::VirtualMachine > const & virtualMachine,
    void * classLoader):
    m_virtualMachine(virtualMachine),
    m_classLoader(0)
{
#if defined SOLAR_JAVA
    try {
        m_classLoader =
            jvmaccess::VirtualMachine::AttachGuard(m_virtualMachine).
            getEnvironment()->NewGlobalRef(static_cast< jobject >(classLoader));
    } catch (jvmaccess::VirtualMachine::AttachGuard::CreationException &) {}
#endif
    if (m_classLoader == 0) {
        throw CreationException();
    }
}

rtl::Reference< jvmaccess::VirtualMachine >
UnoVirtualMachine::getVirtualMachine() const {
    return m_virtualMachine;
}

void * UnoVirtualMachine::getClassLoader() const {
    return m_classLoader;
}

UnoVirtualMachine::~UnoVirtualMachine() {
#if defined SOLAR_JAVA
    try {
        jvmaccess::VirtualMachine::AttachGuard(m_virtualMachine).
            getEnvironment()->DeleteGlobalRef(
                static_cast< jobject >(m_classLoader));
    } catch (jvmaccess::VirtualMachine::AttachGuard::CreationException &) {
        OSL_TRACE(
            "jvmaccess::UnoVirtualMachine::~UnoVirtualMachine:"
            " jvmaccess::VirtualMachine::AttachGuard::CreationException" );
    }
#endif
}

}
