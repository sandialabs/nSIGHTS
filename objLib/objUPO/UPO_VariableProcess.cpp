///////////////////////////////////////////////////////////////////////////////////
//
// UPO_VariableProcess.cpp
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sandia Corporation nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////////
//
// DESCRIPTION:
//
//      File I/O and user-interface for objUFO.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genApp/AppConfigFile.h>

#ifndef ADCONSOLEAPP
#include "UPO_VariableProcessPS.h"
#endif
#include "UPO_VariableProcess.h"


const char* UPO_VariableProcess::objLongDesc = "Pre-Process Variable Data";

UPO_VariableProcess :: UPO_VariableProcess() : MenuObjC(this, objLongDesc)
{
    RegisterFileName(directiveFileName);
    RegisterFileName(outputFileName);
}

UPO_VariableProcess :: UPO_VariableProcess(const UPO_VariableProcess& a) : UFO_VariableProcess(a), MenuObjC(this, objLongDesc)
{
    RegisterFileName(directiveFileName);
    RegisterFileName(outputFileName);
}

UPO_VariableProcess& UPO_VariableProcess::operator= (const UPO_VariableProcess& a)
{
    if (&a != this)
       *((UFO_VariableProcess*) this) = a;
    return *this;
}

bool  UPO_VariableProcess::ReadFromFile()
{
    ReadObjectHeader();
    ReadFileName(directiveFileName);
    ReadFileName(outputFileName);
    ReadFuncObjRef(variableObjRef);
    openOutputFile  = ReadBool();
    if (currentObjReadMajorVersion > 0)
        ReadNodeFileArray(mpiFiles);

    CheckForInputFailure("Reading UPO_VariableProcess");
    return true;
}


void UPO_VariableProcess::WriteToFile()
{
    const int objMajVer = 1;  // added MPI node support
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    WriteText(directiveFileName);
    WriteText(outputFileName);
    WriteFuncObjRef(variableObjRef);

    WriteBool(openOutputFile);
    WriteLine();

    WriteNodeFileArray(mpiFiles);
}


#ifndef ADCONSOLEAPP

UPO_VariableProcessPS& UPO_VariableProcess::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<UPO_VariableProcessPS&>(*assocMenu);
}


void UPO_VariableProcess::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void UPO_VariableProcess::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new UPO_VariableProcessPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void UPO_VariableProcess::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

