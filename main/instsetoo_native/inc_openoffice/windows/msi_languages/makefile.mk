#**************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
#**************************************************************



PRJ=..$/..$/..

PRJNAME=instsetoo_native
TARGET=win_ulffiles

LANGUAGE_FILELIST=Property.ulf \
RadioBut.ulf \
Error.ulf \
CustomAc.ulf \
UIText.ulf \
ActionTe.ulf \
LaunchCo.ulf \
Nsis.ulf \
SIS.ulf \
Control.ulf

# --- Settings -----------------------------------------------------

common_build:=
.INCLUDE :  settings.mk

# ------------------------------------------------------------------

ULFFILES = \
    ActionTe.ulf \
    Control.ulf  \
    CustomAc.ulf \
    Error.ulf    \
    LaunchCo.ulf \
    Property.ulf \
    RadioBut.ulf \
    Nsis.ulf     \
    SIS.ulf      \
    UIText.ulf 
    
MLFFILES = $(foreach,i,$(ULFFILES) $(MISC)$/$(TARGET)$/$(i:b).mlf)
UULFFILES = $(MISC)$/$(TARGET)$/Nsis.uulf

# --- Targets ------------------------------------------------------

.INCLUDE :  target.mk

.IF "$(GUI)"=="WNT"

ALLTAR : $(MLFFILES) $(UULFFILES)

.ENDIF
