///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CreateXYArrayPS.cpp
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

#include <genClass/U_Str.h>
#include <genClass/DO_XYData.h>

#include "DPO_CreateXYArrayPS.h"


DPO_CreateXYArrayPS::XYDataUIC::XYDataUIC() : xyObjRefUI(typeid(DO_XYData))
{
}

void DPO_CreateXYArrayPS::XYDataUIC::SetAllSensitiveCB(void* currObj)
{
 static_cast<DPO_CreateXYArrayPS::XYDataUIC*>(currObj)->SetAllSensitive();
}

void DPO_CreateXYArrayPS::XYDataUIC::ObjChange()
{
    if (xyObjRefUI.CheckDataObj())
    {
        newIDUI.SetFieldString(static_cast<DO_XYData*>(xyObjRefUI.GetDataObj())->xyData->dataID);
    }
}

void DPO_CreateXYArrayPS::XYDataUIC::ObjChangeCB(void* inObj)
{
     static_cast<DPO_CreateXYArrayPS::XYDataUIC*>(inObj)->ObjChange();
}


void DPO_CreateXYArrayPS::XYDataUIC::InitXYDataDesc()
{
    static const int specFormwidth = 600;

    ArgListC args;

    args.TopFormAttach();
    args.LeftFormAttach();
    doXYUI.StdToggleInit(" ", outsideForm.GetWidget(), args);
    doXYUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.TopFormAttach();
    args.RightPosAttach(50);
    xyObjRefUI.InitFormUI(outsideForm.GetWidget(), args);
    xyObjRefUI.AddOtherObjectCallback(ObjChangeCB, this);

    args.Clear();
    args.TopFormAttach();
    args.SpecifiedWidthAttach(55, 95, specFormwidth);
    newIDUI.SetFieldLen(DC_XYData::dataIDLen);
    newIDUI.InitWidget(outsideForm.GetWidget(), args);

}

void DPO_CreateXYArrayPS::XYDataUIC::SetXYDataDesc(const DFO_CreateXYArray::XYDataDesc& xyData)
{
    doXYUI.SetValue(xyData.doXY);
    xyObjRefUI.SetFuncObjRef(xyData.xyObjRef);
    newIDUI.SetFieldString(xyData.newID);
}

void DPO_CreateXYArrayPS::XYDataUIC::GetXYDataDesc(DFO_CreateXYArray::XYDataDesc& xyData)
{
    xyData.xyObjRef = xyObjRefUI.GetFuncObjRef();
    xyData.doXY = doXYUI.GetValue();
    newIDUI.GetFieldString(xyData.newID, DC_XYData::dataIDLen);
}

bool DPO_CreateXYArrayPS::XYDataUIC::XYDataDescOK(bool& prevOK)
{
    if (doXYUI.GetValue())
    {
        xyObjRefUI.FuncObjRefOK(prevOK, "Curve input");
    }
    return prevOK;
}


void DPO_CreateXYArrayPS::XYDataUIC::SetAllSensitive()
{
    doXYUI.SetSensitive(true);
    bool doXY = doXYUI.GetValue();
    xyObjRefUI.SetSensitive(doXY);
    newIDUI.SetSensitive(doXY);
}

CSize DPO_CreateXYArrayPS::CurveScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 600);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}

/////////////////////////////////////////////////////////////////
// DPO_CreateXYArrayPS

DPO_CreateXYArrayPS::DPO_CreateXYArrayPS()
{
}


void DPO_CreateXYArrayPS::SetAllSensitive()
{
    for (int i = 0; i < DFO_CreateXYArray::maxXYDataDesc; i++)
        xyDataUI[i].SetAllSensitive();
}


void DPO_CreateXYArrayPS::InitMenu()

{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.AllFormAttach();
    args.LeftOffset(3);
    args.RightOffset(3);

    //scrollArea.needHorizontal = false;
    scrollArea.InitWidget(PPform.GetWidget(), args);
    args.Clear();
    args.StdFormAttach();

    for (int i = 0; i < DFO_CreateXYArray::maxXYDataDesc; i++)
    {
        xyDataUI[i].outsideForm.InitWidget(scrollArea, args);
        xyDataUI[i].InitXYDataDesc();

        args.Clear();
        args.StdTopObjectAttach(xyDataUI[i].outsideForm.GetWidget());
        xyDataUI[i].hSep.InitWidget(scrollArea, args);

        args.Clear();
        args.StdTopObjectAttach(xyDataUI[i].hSep.GetWidget());
    }
}


void DPO_CreateXYArrayPS::MenuOpen(DPO_CreateXYArray&   inData)
{
    MenuBaseC::MenuOpen(false, true);
    for (int i = 0; i < DFO_CreateXYArray::maxXYDataDesc; i++)
    {
        xyDataUI[i].xyObjRefUI.StdMenuInit(inData.xyData[i].xyObjRef);
    }
}

void DPO_CreateXYArrayPS::MenuClose()
{
    MenuBaseC::MenuClose();
}


void DPO_CreateXYArrayPS::SetMenuData(DPO_CreateXYArray&    inData)
{
    for (int i = 0; i < DFO_CreateXYArray::maxXYDataDesc; i++)
        xyDataUI[i].SetXYDataDesc(inData.xyData[i]);
    MenuBaseC::SetMenuData(inData);
    SetAllSensitive();
}

bool DPO_CreateXYArrayPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    for (int i = 0; i < DFO_CreateXYArray::maxXYDataDesc; i++)
        if (!xyDataUI[i].XYDataDescOK(allOK))
            return false;
    return allOK;
}

void DPO_CreateXYArrayPS::GetMenuData(DPO_CreateXYArray&    outData)
{
    MenuBaseC::GetMenuData(outData);
    for (int i = 0; i < DFO_CreateXYArray::maxXYDataDesc; i++)
        xyDataUI[i].GetXYDataDesc(outData.xyData[i]);
}

void DPO_CreateXYArrayPS::MenuClear()
{
    for (int i = 0; i < DFO_CreateXYArray::maxXYDataDesc; i++)
    {
        xyDataUI[i].doXYUI.SetValue(false);
        xyDataUI[i].SetAllSensitive();
    }
}



