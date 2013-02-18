///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TransformTablePS.cpp
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

#include "DPO_TransformTablePS.h"


void DPO_TransformTablePS::TransformDescUIC::SetAllSensitiveCB(void* currObj)
{
    static_cast<DPO_TransformTablePS::TransformDescUIC*>(currObj)->SetAllSensitive();
}

void DPO_TransformTablePS::TransformDescUIC::InitTransformDesc()
{
    ArgListC args;

    args.StdToggleSetup(2);
    doTransformUI.StdToggleInit(" ", outsideForm.GetWidget(), args);
    doTransformUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(8, 20);
    transformIDUI.InitWidget(outsideForm.GetWidget(), args);

    args.StdSplitFinish(78);
    transformDescUI.InitWidget(outsideForm.GetWidget(), args);
}

void DPO_TransformTablePS::TransformDescUIC::SetTransformDesc(const DFO_TransformTable::TransformDesc& trDesc)
{
    doTransformUI.SetValue(trDesc.doTransform);
    transformIDUI.SetFieldString(trDesc.transformID);
    transformDescUI.SetFieldString(trDesc.transformDesc);
    SetAllSensitive();
}

void DPO_TransformTablePS::TransformDescUIC::GetTransformDesc(DFO_TransformTable::TransformDesc& trDesc)
{
    trDesc.doTransform = doTransformUI.GetValue();
    if (trDesc.doTransform)
    {
        transformIDUI.GetFieldString(trDesc.transformID, DFO_TransformTable::transformIDLen);
        transformDescUI.GetFieldString(trDesc.transformDesc, DFO_TransformTable::transformDescLen);
    }
}

bool DPO_TransformTablePS::TransformDescUIC::TransformDescOK(bool& prevOK)
{
    if (doTransformUI.GetValue())
    {
        transformIDUI.FieldStringOK(prevOK, "transform ID");
        transformDescUI.FieldStringOK(prevOK, "transform desc");
    }
    return prevOK;
}


void DPO_TransformTablePS::TransformDescUIC::SetAllSensitive()
{
    doTransformUI.SetSensitive(true);
    bool doTr = doTransformUI.GetValue();
    transformIDUI.SetSensitive(doTr);
    transformDescUI.SetSensitive(doTr);
}

CSize DPO_TransformTablePS::TableScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 800);
    szForm.cy = SetWindowPosY(0, 800);
    return szForm;
}


DPO_TransformTablePS::DPO_TransformTablePS() :
    inputTableObjRefUI(typeid(DO_TableData))
{
    transformDescUI.Alloc(DFO_TransformTable::maxTransform);
    for (int i = 0; i < DFO_TransformTable::maxTransform; i++)
        transformDescUI += new TransformDescUIC();
}

void DPO_TransformTablePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_TransformTablePS*>(inObj)->SetAllSensitive();
}

void DPO_TransformTablePS::SetAllSensitive()
{
}

void DPO_TransformTablePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    inputTableObjRefUI.InitFramedUI("Input Table", URform.GetWidget(), args);

    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    columnDescAreVarIDUI.PreInitSetUp("C1 ... CN", "Input column IDs");
    columnDescAreVarIDUI.InitFramedUI("Variable Names", topRowForm.GetWidget(), args);

    args.StdSplitFinish();
    const char* otStrs[] = {"All", "Used Input & Transforms", "Transforms Only"};
    outputTableDataUI.PreInitSetUp(otStrs, 3);
    outputTableDataUI.InitFramedUI("Output Table Data", topRowForm.GetWidget(), args);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    args.BottomFormAttach();
    tranFrame.InitFramedUI("Defined Transforms", PPform.GetWidget(), args);

    args.AllFormAttach();
    args.LeftOffset(5);
    args.RightOffset(5);
    args.BottomOffset(25);
    scrollArea.InitWidget(tranFrame.GetForm(), args);

    args.StdFormAttach();
    for (int i = 0; i < DFO_TransformTable::maxTransform; i++)
    {
        TransformDescUIC& currUI = transformDescUI.GetRef(i);

        currUI.outsideForm.InitWidget(scrollArea, args);
        currUI.InitTransformDesc();

        args.StdTopObjectAttach(currUI.outsideForm.GetWidget());
        currUI.hSep.InitWidget(scrollArea, args);

        args.StdTopObjectAttach(currUI.hSep.GetWidget());
    }
}

void  DPO_TransformTablePS::MenuClose()
{
    MenuBaseC::MenuClose();
}


void DPO_TransformTablePS::MenuOpen(DPO_TransformTable& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputTableObjRefUI.StdMenuInit(inData.inputTableObjRef);
}

void DPO_TransformTablePS::SetMenuData(DPO_TransformTable&  inData)
{
    MenuBaseC::SetMenuData(inData);

    columnDescAreVarIDUI.SetBoolChoice(inData.columnDescAreVarID);
    outputTableDataUI.SetChoice(int(inData.outputTableData));
    inputTableObjRefUI.SetFuncObjRef(inData.inputTableObjRef);

    for (int i = 0; i < DFO_TransformTable::maxTransform; i++)
        transformDescUI.GetRef(i).SetTransformDesc(inData.transformDesc[i]);

    SetAllSensitive();
}

bool DPO_TransformTablePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputTableObjRefUI.FuncObjRefOK(allOK, "Input table");
    for (int i = 0; i < DFO_TransformTable::maxTransform; i++)
        transformDescUI.GetRef(i).TransformDescOK(allOK);

    return allOK;
}

void DPO_TransformTablePS::GetMenuData(DPO_TransformTable&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputTableObjRef = inputTableObjRefUI.GetFuncObjRef();

    outData.columnDescAreVarID = columnDescAreVarIDUI.GetBoolChoice();
    outData.outputTableData = DFO_TransformTable::OutputTableData(outputTableDataUI.GetChoice());

    for (int i = 0; i < DFO_TransformTable::maxTransform; i++)
        transformDescUI.GetRef(i).GetTransformDesc(outData.transformDesc[i]);

}

