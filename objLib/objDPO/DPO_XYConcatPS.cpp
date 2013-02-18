///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYConcatPS.cpp
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

#include <string.h>

#include <genClass/U_Str.h>

#include "DPO_XYConcat.h"
#include "DPO_XYConcatPS.h"


DPO_XYConcatPS::XYInputDataUIC::XYInputDataUIC()
    :xyObjRefUI(typeid(DO_XYData))
{
}


void DPO_XYConcatPS::XYInputDataUIC::SetAllSensitiveCB(void* currObj)
{
    static_cast<DPO_XYConcatPS::XYInputDataUIC*>(currObj)->SetAllSensitive();
}



void DPO_XYConcatPS::XYInputDataUIC::InitXYInputData()
{
    ArgListC args;

    args.StdToggleSetup(2);
    doXYUI.StdToggleInit(" ", outsideForm.GetWidget(), args);
    doXYUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(80);
    xyObjRefUI.InitWidget(outsideForm.GetWidget(), args);
}

void DPO_XYConcatPS::XYInputDataUIC::SetXYInputData(const DFO_XYConcat::XYInputData& XYInputData)
{
    doXYUI.SetValue(XYInputData.doXY);
    xyObjRefUI.SetFuncObjRef(XYInputData.xyObjRef);
    SetAllSensitive();
}

void DPO_XYConcatPS::XYInputDataUIC::GetXYInputData(DFO_XYConcat::XYInputData& XYInputData)
{
    XYInputData.xyObjRef = xyObjRefUI.GetFuncObjRef();
    XYInputData.doXY = doXYUI.GetValue();
}

bool DPO_XYConcatPS::XYInputDataUIC::XYInputDataOK(bool& prevOK)
{
    if (doXYUI.GetValue())
    {
        xyObjRefUI.FuncObjRefOK(prevOK, "XY input");
    }
    return prevOK;
}


void DPO_XYConcatPS::XYInputDataUIC::SetAllSensitive()
{
    doXYUI.SetSensitive(true);
    bool doXY = doXYUI.GetValue();
    xyObjRefUI.SetSensitive(doXY);
}

CSize DPO_XYConcatPS::TableScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 600);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}

/////////////////////////////////////////////////////////////////
// DPO_XYConcatPS

DPO_XYConcatPS::DPO_XYConcatPS()
    : inputxyObjRefUI(typeid(DO_XYData))
{
    int ninput = DFO_XYConcat::maxInputXY - 1;
    XYInputDataUI.Alloc(ninput);
    for (int i = 0; i < ninput; i++)
        XYInputDataUI += new XYInputDataUIC();
}

DPO_XYConcatPS::~DPO_XYConcatPS()
{
}

void DPO_XYConcatPS::SetAllSensitive()
{
    for (int i = 1; i < DFO_XYConcat::maxInputXY; i++)
        XYInputDataUI.GetRef(i - 1).SetAllSensitive();
}


void DPO_XYConcatPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_XYConcatPS*>(inObj)->SetAllSensitive();
}


void DPO_XYConcatPS::InitMenu()
{
    MenuBaseC::InitMenu();
    ArgListC args;

    args.AllFormAttach();
    inputxyObjRefUI.InitFramedUI("First XY", URform.GetWidget(), args);

    args.StdFormAttach();
    optionFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    deleteOverlapsUI.StdToggleInit("delete overlaps", optionFrame.GetForm(), args);

    args.StdTopObjectAttach(optionFrame);
    args.BottomFormAttach();
    tableColFrame.InitFramedUI("XY to Add to First", PPform.GetWidget(), args);

    args.Clear();
    args.AllFormAttach();
    args.LeftOffset(5);
    args.RightOffset(5);
    args.BottomOffset(25);
    scrollArea.InitWidget(tableColFrame.GetForm(), args);

    args.StdFormAttach();
    for (int i = 0; i < DFO_XYConcat::maxInputXY - 1; i++)
    {
        XYInputDataUI.GetRef(i).outsideForm.InitWidget(scrollArea, args);
        XYInputDataUI.GetRef(i).InitXYInputData();

        args.StdTopObjectAttach(XYInputDataUI.GetRef(i).outsideForm.GetWidget());
        XYInputDataUI.GetRef(i).hSep.InitWidget(scrollArea, args);

        args.StdTopObjectAttach(XYInputDataUI.GetRef(i).hSep.GetWidget());
    }
}


void DPO_XYConcatPS::MenuOpen(DPO_XYConcat&   inData)
{
    MenuBaseC::MenuOpen(false, true);

    inputxyObjRefUI.StdMenuInit(inData.xyToConcat[0].xyObjRef);
    for (int i = 1; i < DFO_XYConcat::maxInputXY; i++)
    {
        XYInputDataUI.GetRef(i - 1).xyObjRefUI.StdMenuInit(inData.xyToConcat[i].xyObjRef);
    }
}


void DPO_XYConcatPS::SetMenuData(DPO_XYConcat&    inData)
{
    MenuBaseC::SetMenuData(inData);

    inputxyObjRefUI.SetFuncObjRef(inData.xyToConcat[0].xyObjRef);
    deleteOverlapsUI.SetValue(inData.deleteOverlaps);
    for (int i = 1; i < DFO_XYConcat::maxInputXY; i++)
        XYInputDataUI.GetRef(i - 1).SetXYInputData(inData.xyToConcat[i]);

    SetAllSensitive();
}

bool DPO_XYConcatPS::UIdataOK()
{
    MenuBaseC::UIdataOK();

    inputxyObjRefUI.FuncObjRefOK(allOK, "Base XY");
    for (int i = 1; i < DFO_XYConcat::maxInputXY; i++)
        if (!XYInputDataUI.GetRef(i - 1).XYInputDataOK(allOK))
            return false;

    return allOK;
}

void DPO_XYConcatPS::GetMenuData(DPO_XYConcat&    outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.deleteOverlaps = deleteOverlapsUI.GetValue();

    outData.xyToConcat[0].xyObjRef = inputxyObjRefUI.GetFuncObjRef();
    for (int i = 1; i < DFO_XYConcat::maxInputXY; i++)
        XYInputDataUI.GetRef(i - 1).GetXYInputData(outData.xyToConcat[i]);
}

