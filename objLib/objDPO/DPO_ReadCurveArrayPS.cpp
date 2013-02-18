///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadCurveArrayPS.cpp
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

#include <genClassPS/PS_UIBase.h>

#include "DPO_ReadCurveArrayPS.h"

void DPO_ReadCurveArrayPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.Clear();
    args.StdFormAttach();
    curveArrayFnameUI.InitFramedUI("Curve Data File ", PPform.GetWidget(), args);
    curveArrayFnameUI.SetPromptAndMask("Curve file to read",
        "Curve Files (*.nCRV)|*.nCRV|All Files (*.*)|*.*|||");

    args.Clear();
    args.StdTopObjectAttach(curveArrayFnameUI);
    statusFrame.InitFramedUI("Data Status", PPform.GetWidget(), args);

    args.Clear();
    args.LeftOffset(10);
    args.RightOffset(10);
    args.StdFormAttach(5);
    nCurve.InitLabeledUI("# of curves", statusFrame.GetForm(), args);
}

void  DPO_ReadCurveArrayPS::MenuClose()
{
    curveArrayFnameUI.CloseDialog();
    MenuBaseC::MenuClose();
}

void DPO_ReadCurveArrayPS::SetMenuData(DPO_ReadCurveArray&  inData)
{
    curveArrayFnameUI.SetFieldString(inData.curveArrayFname);

    bool isOK = (inData.GetStatus() == os_OK);
    if (isOK)
    {
        nCurve.SetFieldValue(inData.curveArray.Size());
    }
    else
    {
        nCurve.SetStatusNA();
    }
    statusFrame.SetSensitive(isOK);
    MenuBaseC::SetMenuData(inData);
}

bool DPO_ReadCurveArrayPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    curveArrayFnameUI.FieldFileOK(allOK, "Curve data file", true, false);
    return allOK;
}

void DPO_ReadCurveArrayPS::GetMenuData(DPO_ReadCurveArray&  outData)
{
    MenuBaseC::GetMenuData(outData);
    curveArrayFnameUI.GetFieldFile(outData.curveArrayFname);
}

void DPO_ReadCurveArrayPS::MenuClear()
{
    curveArrayFnameUI.SetFieldString(NULL);
}

