///////////////////////////////////////////////////////////////////////////////////
//
// DPO_RealToVariableArrayPS.cpp
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

#include "DPO_RealToVariableArray.h"
#include "DPO_RealToVariableArrayPS.h"


DPO_RealToVariableArrayPS::RealSelUIC::RealSelUIC() : realObjRefUI(typeid(DO_Real))
{
}

void DPO_RealToVariableArrayPS::RealSelUIC::SetAllSensitiveCB(void* currObj)
{
    static_cast<DPO_RealToVariableArrayPS::RealSelUIC*>(currObj)->SetAllSensitive();
}



void DPO_RealToVariableArrayPS::RealSelUIC::InitRealSel()
{
    ArgListC args;

    args.TopFormAttach();
    args.LeftFormAttach();
    doRealUI.StdToggleInit(" ", outsideForm.GetWidget(), args);
    doRealUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(10, 60);
    realObjRefUI.InitWidget(outsideForm.GetWidget(), args);

    args.StdSplitFinish(40);
    realIDUI.InitWidget(outsideForm.GetWidget(), args);
}

void DPO_RealToVariableArrayPS::RealSelUIC::SetRealSel(const DFO_RealToVariableArray::RealSel& realData)
{
    doRealUI.SetValue(realData.doReal);
    realObjRefUI.SetFuncObjRef(realData.realObjRef);
    realIDUI.SetFieldString(realData.realID);
    SetAllSensitive();
}

void DPO_RealToVariableArrayPS::RealSelUIC::GetRealSel(DFO_RealToVariableArray::RealSel& realData)
{
    realData.realObjRef = realObjRefUI.GetFuncObjRef();
    realData.doReal = doRealUI.GetValue();
    realIDUI.GetFieldString(realData.realID, DC_Variable::varIDLen);
}

bool DPO_RealToVariableArrayPS::RealSelUIC::RealSelOK(bool& prevOK)
{
    if (doRealUI.GetValue())
    {
        realObjRefUI.FuncObjRefOK(prevOK, "Real input");
        realIDUI.FieldStringOK(prevOK, "Real variable description");
    }
    return prevOK;
}


void DPO_RealToVariableArrayPS::RealSelUIC::SetAllSensitive()
{
    doRealUI.SetSensitive(true);
    bool doReal = doRealUI.GetValue();
    realObjRefUI.SetSensitive(doReal);
    realIDUI.SetSensitive(doReal);
}

CSize DPO_RealToVariableArrayPS::RealScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 800);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}

/////////////////////////////////////////////////////////////////
// DPO_RealToVariableArrayPS

DPO_RealToVariableArrayPS::DPO_RealToVariableArrayPS()
{
}


void DPO_RealToVariableArrayPS::SetAllSensitive()
{
    for (int i = 0; i < DFO_RealToVariableArray::maxRealSel; i++)
        realSelUI[i].SetAllSensitive();
}



void DPO_RealToVariableArrayPS::InitMenu()

{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.AllFormAttach();
    realSelFrame.InitFramedUI("Reals To Combine in Variable Array", PPform.GetWidget(), args);

    args.AllFormAttach();
    args.LeftOffset(5);
    args.RightOffset(5);

    //  Radim: if this is not put in the space where the horizontal scroll bar would
    //  go wipes out the top of the attached statusFrame
    args.BottomOffset(25);

    scrollArea.InitWidget(realSelFrame.GetForm(), args);

    args.Clear();
    args.StdFormAttach();
    for (int i = 0; i < DFO_RealToVariableArray::maxRealSel; i++)
    {
        realSelUI[i].outsideForm.InitWidget(scrollArea, args);
        realSelUI[i].InitRealSel();

        args.StdTopObjectAttach(realSelUI[i].outsideForm.GetWidget());
        realSelUI[i].hSep.InitWidget(scrollArea, args);

        args.StdTopObjectAttach(realSelUI[i].hSep.GetWidget());
    }
}


void DPO_RealToVariableArrayPS::MenuOpen(DPO_RealToVariableArray&   inData)
{
    MenuBaseC::MenuOpen(false, true);
    for (int i = 0; i < DFO_RealToVariableArray::maxRealSel; i++)
    {
        realSelUI[i].realObjRefUI.StdMenuInit(inData.realSelData[i].realObjRef);
    }
}


void DPO_RealToVariableArrayPS::SetMenuData(DPO_RealToVariableArray&    inData)
{
    for (int i = 0; i < DFO_RealToVariableArray::maxRealSel; i++)
        realSelUI[i].SetRealSel(inData.realSelData[i]);
    MenuBaseC::SetMenuData(inData);

    SetAllSensitive();
}

bool DPO_RealToVariableArrayPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    for (int i = 0; i < DFO_RealToVariableArray::maxRealSel; i++)
        if (!realSelUI[i].RealSelOK(allOK))
            return false;
    return allOK;
}

void DPO_RealToVariableArrayPS::GetMenuData(DPO_RealToVariableArray&    outData)
{
    MenuBaseC::GetMenuData(outData);
    for (int i = 0; i < DFO_RealToVariableArray::maxRealSel; i++)
        realSelUI[i].GetRealSel(outData.realSelData[i]);
}

void DPO_RealToVariableArrayPS::MenuClear()
{
    for (int i = 0; i < DFO_RealToVariableArray::maxRealSel; i++)
    {
        realSelUI[i].doRealUI.SetValue(false);
        realSelUI[i].SetAllSensitive();
    }
}

