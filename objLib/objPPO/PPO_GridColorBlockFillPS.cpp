///////////////////////////////////////////////////////////////////////////////////
//
// PPO_GridColorBlockFillPS.cpp
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
//      File I/O and user interface for all objPFOGL objects
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <objClass/DO_GridData.h>

#include "PPO_GridColorBlockFillPS.h"

void PPO_GridColorBlockFillPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_GridColorBlockFillPS*>(inObj)->SetAllSensitive();
}

void PPO_GridColorBlockFillPS::SetAllSensitive()
{
    edgeSpecUI.SetInternalSensitive();
    if (useReport)
        reportFormatUI.SetSensitive(reportToggle.GetValue());
}

// ****************************************

PPO_GridColorBlockFillPS::PPO_GridColorBlockFillPS()
{
}

void PPO_GridColorBlockFillPS::InitMenu(const PPO_GridColorBlockFill&   inData)
{
    PPO_GridColorBasePS::InitMenu(inData);

    ArgListC args;
    args.Clear();
    args.StdTopObjectAttach(outsideForm.GetWidget());
    areaReduceField.SetFieldLen(6);
    areaReduceField.format = SC_RealFormat(5);
    areaReduceField.InitFramedUI("Area reduction", PPform.GetWidget(), args);

    //  edges
    args.Clear();
    args.StdTopObjectAttach(areaReduceField);
    edgeSpecUI.StdInit(PPform.GetWidget(), args, inData.is3DObject);
    edgeSpecUI.plotEdgesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(edgeSpecUI);
    if (useReport)
    {
        reportFormatUI.InitFramedUI("Cursor Reporting Format", PPform.GetWidget(), args);
        reportToggle.AddOtherObjectCallback(SetAllSensitiveCB, this);
        args.Clear();
        args.StdTopObjectAttach(reportFormatUI);
    }
    PPO_GridColorBasePS::InitMapAndOffset(inData, args);

}

void PPO_GridColorBlockFillPS::MenuOpen(PPO_GridColorBlockFill& inData)
{
    PPO_GridColorBasePS::MenuOpen(inData);
    edgeSpecUI.edgePenUI.ResetPens(inData.objPenSet);
}

void PPO_GridColorBlockFillPS::SetMenuData(PPO_GridColorBlockFill& inData)
{
    PPO_GridColorBasePS::SetMenuData(inData, inData);

    areaReduceField.SetFieldValue(inData.areaReduction);

    edgeSpecUI.SetEdgeSpec(inData.edgeSpec);

    if (useReport)
        reportFormatUI.SetRealFormatSettings(inData.reportFormat);

    SetAllSensitive();
}

bool PPO_GridColorBlockFillPS::UIdataOK()
{
    PPO_GridColorBasePS::UIdataOK();
    edgeSpecUI.UIdataOK(allOK);
    areaReduceField.FieldValueOK(allOK, "Area Reduction", 0.01, 1.0);
    return allOK;
}

void PPO_GridColorBlockFillPS::GetMenuData(PPO_GridColorBlockFill& outData)
{
    PPO_GridColorBasePS::GetMenuData(outData, outData);

    outData.areaReduction  = areaReduceField.GetFieldValue();
    edgeSpecUI.GetEdgeSpec(outData.edgeSpec);

    if (useReport)
        reportFormatUI.GetRealFormatSettings(outData.reportFormat);

}

