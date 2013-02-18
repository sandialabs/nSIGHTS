///////////////////////////////////////////////////////////////////////////////////
//
// DPO_Normalize.cpp
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
#include <genApp/ExposedReal.h>

#ifndef ADCONSOLEAPP
#include "DPO_NormalizePS.h"
#endif
#include "DPO_Normalize.h"

DPO_Normalize :: DPO_Normalize(DFO_Normalize* bPO, const char* longDesc) :
    MenuObjC(bPO, longDesc),
    bpRef(*bPO),
    exposedNormPower(*bPO, "Power", bPO->normPowerMS.enteredRealValue)
{
}

bool  DPO_Normalize::ReadFromFile()
{
    int stVer = ReadInt();

    ReadRealMSSpec(bpRef.normPowerMS);
    bpRef.isMaster = ReadBool();

    bpRef.normalizeDC.normalizeOp    = DC_Normalize::NormalizeOp(ReadInt());
    bpRef.normalizeDC.autoLimit      = ReadBool();
    bpRef.normalizeDC.inputMinimum   = ReadDouble();
    bpRef.normalizeDC.inputMaximum   = ReadDouble();
    bpRef.normalizeDC.outputMinimum  = ReadDouble();
    bpRef.normalizeDC.outputMaximum  = ReadDouble();
    bpRef.normalizeDC.normPower      = ReadDouble();

    exposedNormPower.ReadExposed();

    CheckForInputFailure("Reading DPO_Normalize");
    return true;
}


void DPO_Normalize::WriteToFile()
{
    const int stVer = 0;
    WriteInt(stVer);
    SetRealConvFormat(SC_RealFormat(ncf_Scientific, 8));

    WriteRealMSSpec(bpRef.normPowerMS);
    WriteBool(bpRef.isMaster);
    WriteLine();

    WriteInt(int(bpRef.normalizeDC.normalizeOp));
    WriteBool(bpRef.normalizeDC.autoLimit);
    WriteDouble(bpRef.normalizeDC.inputMinimum);
    WriteDouble(bpRef.normalizeDC.inputMaximum);
    WriteDouble(bpRef.normalizeDC.outputMinimum);
    WriteDouble(bpRef.normalizeDC.outputMaximum);
    WriteDouble(bpRef.normalizeDC.normPower);
    WriteLine();

    exposedNormPower.WriteExposed();
}

#ifndef ADCONSOLEAPP

// ************************* expose support

bool DPO_Normalize::ContainsExposedObject()
{
    return exposedNormPower.IsExposed();
}


// ************************* menu stuff

DPO_NormalizePS& DPO_Normalize::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_NormalizePS&>(*assocMenu);
}

void DPO_Normalize::UpdateUI()
{
    exposedNormPower.UpdateIfCreated();
    if (MenuUpdateOK())
    {
        MenuRef().normPowerMSUI.realValueUI.SetFieldValue(bpRef.normPowerMS.GetCurrentValue());
    }
}


#endif //!ADCONSOLEAPP

