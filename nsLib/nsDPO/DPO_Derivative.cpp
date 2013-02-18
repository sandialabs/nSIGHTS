///////////////////////////////////////////////////////////////////////////////////
//
// DPO_Derivative.cpp
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

#include <genClass/U_Real.h>

#include <genApp/AppConfigFile.h>
#include <genApp/ExposedReal.h>

#ifndef ADCONSOLEAPP
#include "DPO_DerivativePS.h"
#endif

#include "DPO_Derivative.h"

const char* DPO_Derivative::objLongDesc = "Create Derivative Data";

DPO_Derivative :: DPO_Derivative() :
    MenuObjC(this, objLongDesc),
    exposedWindowPctSpanValue(*this, "Log/Lin % Span", windowPctSpanMS.enteredRealValue),
    exposedWindowValSpanValue(*this, "Log/Lin Value Span", windowValSpanMS.enteredRealValue)
{
}

DPO_Derivative :: DPO_Derivative(const DPO_Derivative& a) :
    DFO_Derivative(a),
    MenuObjC(this, objLongDesc),
    exposedWindowPctSpanValue(*this, "Log/Lin % Span", windowPctSpanMS.enteredRealValue),
    exposedWindowValSpanValue(*this, "Log/Lin % Span", windowValSpanMS.enteredRealValue)
{
}


DPO_Derivative& DPO_Derivative::operator= (const DPO_Derivative& a)
{
    if (&a != this)
       *((DFO_Derivative*) this) = a;
    return *this;
}


bool  DPO_Derivative::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(inputTPObjRef);
    ReadFuncObjRef(masterObjRef);
    ReadFuncObjRef(timeProcessObjRef);
    ReadRealMSSpec(windowPctSpanMS);
    if (currentObjReadMajorVersion > 1)
        ReadRealMSSpec(windowValSpanMS);

    isMaster = ReadBool();

    derivativeDC.derivativeType     = DerivativeType(ReadInt());
    derivativeDC.derivativeCalcType = DerivativeCalcType(ReadInt());
    derivativeDC.windowCalcType     = WindowCalcType(ReadInt());
    derivativeDC.timeMultType       = TimeMultType(ReadInt());
    derivativeDC.useSuperTime       = ReadBool();
    derivativeDC.windowPtsSpan      = ReadInt();

    if (currentObjReadMajorVersion > 0)
        derivativeDC.absOutputY     = ReadBool();

    if (currentObjReadMajorVersion > 1)
    {
        derivativeDC.logEps     = ReadDouble();
        derivativeDC.offsetY    = ReadDouble();
    }
    else
    {
        derivativeDC.logEps     = stdEps;
        derivativeDC.offsetY    = 0.0;
    }

    exposedWindowPctSpanValue.ReadExposed();

    if (currentObjReadMajorVersion > 1)
        exposedWindowValSpanValue.ReadExposed();

    CheckForInputFailure("Reading DPO_Derivative");
    return true;
}


void DPO_Derivative::WriteToFile()
{
    const int objMajVer = 2;        // added epsX & epsY, Val Span
//    const int objMajVer = 1;      // added abs Y
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(inputTPObjRef);
    WriteFuncObjRef(masterObjRef);
    WriteFuncObjRef(timeProcessObjRef);
    WriteRealMSSpec(windowPctSpanMS);
    WriteRealMSSpec(windowValSpanMS);
    WriteBool(isMaster);
    WriteLine();

    WriteInt(derivativeDC.derivativeType);
    WriteInt(derivativeDC.derivativeCalcType);
    WriteInt(derivativeDC.windowCalcType);
    WriteInt(derivativeDC.timeMultType);
    WriteBool(derivativeDC.useSuperTime);
    WriteInt(derivativeDC.windowPtsSpan);
    WriteBool(derivativeDC.absOutputY);
    WriteLine();

    WriteDouble(derivativeDC.logEps);
    WriteDouble(derivativeDC.offsetY);
    WriteLine();

    exposedWindowPctSpanValue.WriteExposed();
    exposedWindowValSpanValue.WriteExposed();

}

#ifndef ADCONSOLEAPP


// ************************* expose support

void DPO_Derivative::CalcOutput(FOcalcType  calcType)
{
    DFO_Derivative::CalcOutput(calcType);
    if (MenuUpdateOK())
    {
        MenuRef().windowPctSpanMSUI.realValueUI.SetFieldValue(windowPctSpanMS.GetCurrentValue());
        MenuRef().windowValSpanMSUI.realValueUI.SetFieldValue(windowValSpanMS.GetCurrentValue());
    }
}

bool DPO_Derivative::ContainsExposedObject()
{
    return exposedWindowPctSpanValue.IsExposed() || exposedWindowValSpanValue.IsExposed();
}


// ************************* menu stuff

DPO_DerivativePS& DPO_Derivative::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_DerivativePS&>(*assocMenu);
}


void DPO_Derivative::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_Derivative::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_DerivativePS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_Derivative::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}



#endif //ADCONSOLEAPP
