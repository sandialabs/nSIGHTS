///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TimeProcess.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "nsLibAfx.h"

#include <genApp/AppConfigFile.h>
#include <genApp/ExposedReal.h>

#ifndef ADCONSOLEAPP
#include "DPO_TimeProcessPS.h"
#endif

#include "DPO_TimeProcess.h"

const char* DPO_TimeProcess::objLongDesc = "Create Time Process Data";

DPO_TimeProcess :: DPO_TimeProcess() :
    MenuObjC(this, objLongDesc),
    exposedConstantTValue(*this, "Horner/Agarwal T", constantTMS.enteredRealValue)
{
}

DPO_TimeProcess :: DPO_TimeProcess(const DPO_TimeProcess& a) :
    DFO_TimeProcess(a),
    MenuObjC(this, objLongDesc),
    exposedConstantTValue(*this, "Horner/Agarwal T", constantTMS.enteredRealValue)
{
}

DPO_TimeProcess& DPO_TimeProcess::operator= (const DPO_TimeProcess& a)
{
    if (&a != this)
       *((DFO_TimeProcess*) this) = a;
    return *this;
}


bool  DPO_TimeProcess::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(inputTPObjRef);
    ReadFuncObjRef(masterObjRef);
    ReadFuncObjRef(inputTQObjRef);

    isMaster = ReadBool();
    timeProcessDC.timeProcessType = TimeProcess::TimeProcessType(ReadInt());

    if (currentObjReadMajorVersion > 0)
    {
        timeProcessDC.addReplaceLastQ = ReadBool();
        timeProcessDC.newLastQ = ReadDouble();
    }

    ReadRealMSSpec(constantTMS);

    exposedConstantTValue.ReadExposed();

    CheckForInputFailure("Reading DPO_TimeProcess");
    return true;
}


void DPO_TimeProcess::WriteToFile()
{
    const int objMajVer = 1;        // added replace Q
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(inputTPObjRef);
    WriteFuncObjRef(masterObjRef);
    WriteFuncObjRef(inputTQObjRef);
    WriteBool(isMaster);
    WriteInt(int(timeProcessDC.timeProcessType));
    WriteBool(timeProcessDC.addReplaceLastQ);
    WriteDouble(timeProcessDC.newLastQ);
    WriteLine();

    WriteRealMSSpec(constantTMS);
    WriteLine();

    exposedConstantTValue.WriteExposed();

}

#ifndef ADCONSOLEAPP


// ************************* expose support

void DPO_TimeProcess::CalcOutput(FOcalcType  calcType)
{
    DFO_TimeProcess::CalcOutput(calcType);
    if (MenuUpdateOK())
        MenuRef().constantTMSUI.realValueUI.SetFieldValue(constantTMS.GetCurrentValue());
}

bool DPO_TimeProcess::ContainsExposedObject()
{
    return exposedConstantTValue.IsExposed();
}


// ************************* menu stuff

DPO_TimeProcessPS& DPO_TimeProcess::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_TimeProcessPS&>(*assocMenu);
}


void DPO_TimeProcess::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_TimeProcess::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_TimeProcessPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_TimeProcess::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


#endif //!ADCONSOLEAPP

