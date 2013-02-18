///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ExecuteObjects.cpp
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
#include "DPO_ExecuteObjectsPS.h"
#endif
#include "DPO_ExecuteObjects.h"

const char* DPO_ExecuteObjects::objLongDesc = "Execute Objects";

DPO_ExecuteObjects :: DPO_ExecuteObjects() :
    MenuObjC(this, objLongDesc)
{
}

DPO_ExecuteObjects :: DPO_ExecuteObjects(const DPO_ExecuteObjects& a) :
    DFO_ExecuteObjects(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_ExecuteObjects& DPO_ExecuteObjects::operator= (const DPO_ExecuteObjects& a)
{
    if (&a != this)
    {
       *((DFO_ExecuteObjects*) this) = a;
    }
    return *this;
}


bool  DPO_ExecuteObjects::ReadFromFile()
{
    ReadObjectHeader();

    useUpstreamTrigger = ReadBool();
    ReadFuncObjRef(upstreamTriggerObjRef);

    int maxObj  = ReadInt();
    for (int i = 0; i < maxObj; i++)
    {
        ReadInt();
        objectData[i].executeObject   = ReadBool();
        ReadFuncObjRef(objectData[i].execObjectRef);
        objectData[i].execDownstream = ObjectData::DownstreamExecType(ReadInt());
        objectData[i].stopOnError = ReadBool();
        if (currentObjReadMajorVersion > 0)
            objectData[i].statusMustBeOK = ReadBool();
    }

    CheckForInputFailure("Reading DPO_ExecuteObjects");
    return true;
}

void DPO_ExecuteObjects::WriteToFile()
{
    const int objMajVer = 1;   // added statusMustBeOK
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteBool(useUpstreamTrigger);
    WriteLine();
    WriteFuncObjRef(upstreamTriggerObjRef);

    WriteInt(maxExecObject);
    WriteLine();
    for (int i = 0; i < maxExecObject; i++)
    {
        WriteInt(i);
        WriteBool(objectData[i].executeObject);
        WriteLine();
        WriteFuncObjRef(objectData[i].execObjectRef);
        WriteInt(int(objectData[i].execDownstream));
        WriteBool(objectData[i].stopOnError);
        WriteBool(objectData[i].statusMustBeOK); // v1
       WriteLine();
    }
}


#ifndef ADCONSOLEAPP

DPO_ExecuteObjectsPS& DPO_ExecuteObjects::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ExecuteObjectsPS&>(*assocMenu);
}

void DPO_ExecuteObjects::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_ExecuteObjects::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_ExecuteObjectsPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_ExecuteObjects::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

