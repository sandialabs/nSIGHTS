///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ExecuteProcess.cpp
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genApp/AppConfigFile.h>

#ifndef ADCONSOLEAPP
#include "DPO_ExecuteProcessPS.h"
#endif
#include "DPO_ExecuteProcess.h"

const char* DPO_ExecuteProcess::objLongDesc = "Execute Process";

DPO_ExecuteProcess :: DPO_ExecuteProcess() :
    MenuObjC(this, objLongDesc)
{
    RegisterFileName(processToExecute);
    RegisterFileName(processCmdLine);
    RegisterFileName(processExeDirectory);
}

DPO_ExecuteProcess :: DPO_ExecuteProcess(const DPO_ExecuteProcess& a) :
    DFO_ExecuteProcess(a),
    MenuObjC(this, objLongDesc)
{
    RegisterFileName(processToExecute);
    RegisterFileName(processCmdLine);
    RegisterFileName(processExeDirectory);
}

DPO_ExecuteProcess& DPO_ExecuteProcess::operator= (const DPO_ExecuteProcess& a)
{
    if (&a != this)
    {
       *((DFO_ExecuteProcess*) this) = a;
    }
    return *this;
}


bool  DPO_ExecuteProcess::ReadFromFile()
{
    ReadObjectHeader();

    ReadFileName(processToExecute);
    ReadFileName(processCmdLine);
    ReadFileName(processExeDirectory);

    useUpstreamTrigger = ReadBool();
    upstreamMustBeOK = ReadBool();
    if (currentObjReadMajorVersion > 0)
        executeOnApply = ReadBool(); // added v1
    if (currentObjReadMajorVersion > 3)
        ignoreGlobalDoNotExecute = ReadBool(); // added v4
    if (currentObjReadMajorVersion > 1)
        hideConsoleWindow = ReadBool(); // added v1

    useSpecificEXE = ReadBool();
    useCommandLine = ReadBool();
    if (currentObjReadMajorVersion > 0)
        addExePar0 = ReadBool(); // added v1

    setExeDirectory = ReadBool();

    if (currentObjReadMajorVersion > 2)
    {
        addMPINodeNumber = ReadBool();
        mpiFiles[0].nodeBasedFile = ReadBool();
        mpiFiles[1].nodeBasedFile = ReadBool();
        mpiFiles[2].nodeBasedFile = ReadBool();
    }

    ReadFuncObjRef(upstreamTriggerObjRef);

    CheckForInputFailure("Reading DPO_ExecuteProcess");
    return true;
}

void DPO_ExecuteProcess::WriteToFile()
{
    const int objMajVer = 4;  // added ignoreGlobalDoNotExecute
//    const int objMajVer = 3;  // added MPI node support
//    const int objMajVer = 2;  // added hide window
//    const int objMajVer = 1;  // added execute on apply and exe par0
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteText(processToExecute);
    WriteText(processCmdLine);
    WriteText(processExeDirectory);

    WriteBool(useUpstreamTrigger);
    WriteBool(upstreamMustBeOK);
    WriteBool(executeOnApply); // added v1
    WriteBool(ignoreGlobalDoNotExecute); // added v4
    WriteBool(hideConsoleWindow); // added v2

    WriteBool(useSpecificEXE);
    WriteBool(useCommandLine);
    WriteBool(addExePar0); // added v1
    WriteBool(setExeDirectory);

    // added v3
    WriteLine();
    WriteBool(addMPINodeNumber);
    WriteBool(mpiFiles[0].nodeBasedFile);
    WriteBool(mpiFiles[1].nodeBasedFile);
    WriteBool(mpiFiles[2].nodeBasedFile);

    WriteLine();
    WriteFuncObjRef(upstreamTriggerObjRef);
}


#ifndef ADCONSOLEAPP

DPO_ExecuteProcessPS& DPO_ExecuteProcess::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ExecuteProcessPS&>(*assocMenu);
}

void DPO_ExecuteProcess::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_ExecuteProcess::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_ExecuteProcessPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_ExecuteProcess::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

