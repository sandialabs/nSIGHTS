///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ScaleTransform.cpp
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
// DESCRIPTION: File I/O and user interface for all genDFO functional objects.
//              All code is platform independent, but relies upon platform dependent
//              code in genApp and genClassPS.
//              File I/O for DFO_XXXX objects.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#ifndef ADCONSOLEAPP
#include "DPO_ScaleTransformPS.h"
#endif

#include <genApp/AppConfigFile.h>
#include <genApp/ExposedReal.h>

#include "DPO_ScaleTransform.h"

DPO_ScaleTransform :: DPO_ScaleTransform(DFO_ScaleTransform* bPO, const char* longDesc) :
    MenuObjC(bPO, longDesc),
    bpRef(*bPO),
    exposedScaleValue(*bPO, "Scale", bPO->scaleMS.enteredRealValue),
    exposedOffsetValue(*bPO, "Offset", bPO->offsetMS.enteredRealValue)
{
}


bool  DPO_ScaleTransform::ReadFromFile(bool skipVerNum)
{

    int stVer = 0;
    if (!skipVerNum)
        stVer = ReadInt();

    bpRef.scaleOp     = DSC_ScaleTransform::ScaleType(ReadInt());
    bpRef.tranOp      = DSC_ScaleTransform::TransformType(ReadInt());
    bpRef.scaleFirst  = ReadBool();

    bpRef.processNulls = ReadBool();
    bpRef.setNullValue  = ReadDouble();

    ReadRealMSSpec(bpRef.scaleMS);
    ReadRealMSSpec(bpRef.offsetMS);

    exposedScaleValue.ReadExposed();
    exposedOffsetValue.ReadExposed();

    CheckForInputFailure("Reading DPO_ScaleTransform");
    return true;
}

void DPO_ScaleTransform::WriteToFile()
{
    const int stVer = 0;

    WriteInt(stVer);
    SetRealConvFormat(SC_RealFormat(ncf_Scientific, 8));

    WriteInt(int(bpRef.scaleOp));
    WriteInt(int(bpRef.tranOp));
    WriteBool(bpRef.scaleFirst);
    WriteBool(bpRef.processNulls);
    WriteDouble(bpRef.setNullValue);
    WriteLine();

    WriteRealMSSpec(bpRef.scaleMS);
    WriteRealMSSpec(bpRef.offsetMS);

    exposedScaleValue.WriteExposed();
    exposedOffsetValue.WriteExposed();
}

#ifndef ADCONSOLEAPP

DPO_ScaleTransformPS& DPO_ScaleTransform::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_ScaleTransformPS&>(*assocMenu);
}


// ************************* expose support


bool DPO_ScaleTransform::ContainsExposedObject()
{
    return exposedScaleValue.IsExposed() || exposedOffsetValue.IsExposed();
}

void DPO_ScaleTransform::UpdateUI()
{
    exposedScaleValue.UpdateIfCreated();
    exposedOffsetValue.UpdateIfCreated();

    if (MenuUpdateOK())
        {
            MenuRef().scaleValueMSUI.realValueUI.SetFieldValue(bpRef.scaleMS.GetCurrentValue());
            MenuRef().offsetValueMSUI.realValueUI.SetFieldValue(bpRef.offsetMS.GetCurrentValue());
        }
}


#endif

