///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TransformSATablePS.cpp
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

#include "SaLibAfx.h"

#include <genClassPS/PS_UIBase.h>

#include "DPO_TransformSATablePS.h"


void DPO_TransformSATablePS::TransformDescUIC::SetAllSensitiveCB(void* currObj)
{
    static_cast<DPO_TransformSATablePS::TransformDescUIC*>(currObj)->SetAllSensitive();
}

void DPO_TransformSATablePS::TransformDescUIC::InitTransformDesc()
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

void DPO_TransformSATablePS::TransformDescUIC::SetTransformDesc(const DFO_TransformSATable::TransformDesc& trDesc)
{
    doTransformUI.SetValue(trDesc.doTransform);
    transformIDUI.SetFieldString(trDesc.transformID);
    transformDescUI.SetFieldString(trDesc.transformDesc);
    SetAllSensitive();
}

void DPO_TransformSATablePS::TransformDescUIC::GetTransformDesc(DFO_TransformSATable::TransformDesc& trDesc)
{
    trDesc.doTransform = doTransformUI.GetValue();
    if (trDesc.doTransform)
    {
        transformIDUI.GetFieldString(trDesc.transformID, DFO_TransformSATable::transformIDLen);
        transformDescUI.GetFieldString(trDesc.transformDesc, DFO_TransformSATable::transformDescLen);
    }
}

bool DPO_TransformSATablePS::TransformDescUIC::TransformDescOK(bool& prevOK)
{
    if (doTransformUI.GetValue())
    {
        transformIDUI.FieldStringOK(prevOK, "transform ID");
        transformDescUI.FieldStringOK(prevOK, "transform desc");
    }
    return prevOK;
}


void DPO_TransformSATablePS::TransformDescUIC::SetAllSensitive()
{
    doTransformUI.SetSensitive(true);
    bool doTr = doTransformUI.GetValue();
    transformIDUI.SetSensitive(doTr);
    transformDescUI.SetSensitive(doTr);
}

CSize DPO_TransformSATablePS::TableScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 800);
    szForm.cy = SetWindowPosY(0, 800);
    return szForm;
}


DPO_TransformSATablePS::DPO_TransformSATablePS() :
    table1ObjRefUI(typeid(DO_SATable)),
    table2ObjRefUI(typeid(DO_SATable))
{
    transformDescUI.Alloc(DFO_TransformSATable::maxTransform);
    for (int i = 0; i < DFO_TransformSATable::maxTransform; i++)
        transformDescUI += new TransformDescUIC();

}

void DPO_TransformSATablePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_TransformSATablePS*>(inObj)->SetAllSensitive();
}

void DPO_TransformSATablePS::SetAllSensitive()
{
    table2ObjRefUI.SetSensitive(dualInputTablesUI.GetBoolChoice());
}

void DPO_TransformSATablePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    dualInputTablesUI.PreInitSetUp("single IV or DV table", "IV and DV table");
    dualInputTablesUI.InitFramedUI("SA Table Input", URform.GetWidget(), args);
    dualInputTablesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);


    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    table1ObjRefUI.InitFramedUI("Single DV or IV SA Table", topRowForm.GetWidget(), args);

    args.StdSplitFinish();
    table2ObjRefUI.InitFramedUI("DV SA Table", topRowForm.GetWidget(), args);


    args.StdTopObjectAttach(topRowForm.GetWidget());
    args.BottomFormAttach();
    tranFrame.InitFramedUI("Defined Transforms", PPform.GetWidget(), args);

    args.AllFormAttach();
    args.LeftOffset(5);
    args.RightOffset(5);
    args.BottomOffset(25);
    scrollArea.InitWidget(tranFrame.GetForm(), args);

    args.StdFormAttach();
    for (int i = 0; i < DFO_TransformSATable::maxTransform; i++)
    {
        TransformDescUIC& currUI = transformDescUI.GetRef(i);

        currUI.outsideForm.InitWidget(scrollArea, args);
        currUI.InitTransformDesc();

        args.StdTopObjectAttach(currUI.outsideForm.GetWidget());
        currUI.hSep.InitWidget(scrollArea, args);

        args.StdTopObjectAttach(currUI.hSep.GetWidget());
    }
}

void  DPO_TransformSATablePS::MenuClose()
{
    MenuBaseC::MenuClose();
}


void DPO_TransformSATablePS::MenuOpen(DPO_TransformSATable& inData)
{
    MenuBaseC::MenuOpen(false, false);
    table1ObjRefUI.StdMenuInit(inData.table1ObjRef);
    table2ObjRefUI.StdMenuInit(inData.table2ObjRef);
}

void DPO_TransformSATablePS::SetMenuData(DPO_TransformSATable&  inData)
{
    MenuBaseC::SetMenuData(inData);

    dualInputTablesUI.SetBoolChoice(inData.dualInputTables);
    table1ObjRefUI.SetFuncObjRef(inData.table1ObjRef);
    table2ObjRefUI.SetFuncObjRef(inData.table2ObjRef);

    for (int i = 0; i < DFO_TransformSATable::maxTransform; i++)
        transformDescUI.GetRef(i).SetTransformDesc(inData.transformDesc[i]);

    SetAllSensitive();
}

bool DPO_TransformSATablePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    table1ObjRefUI.FuncObjRefOK(allOK, "IV/Single table");
    if (dualInputTablesUI.GetBoolChoice())
        table2ObjRefUI.FuncObjRefOK(allOK, "DV table");
    for (int i = 0; i < DFO_TransformSATable::maxTransform; i++)
        transformDescUI.GetRef(i).TransformDescOK(allOK);

    return allOK;
}

void DPO_TransformSATablePS::GetMenuData(DPO_TransformSATable&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.table1ObjRef = table1ObjRefUI.GetFuncObjRef();

    outData.dualInputTables = dualInputTablesUI.GetBoolChoice();
    if (outData.dualInputTables)
        outData.table2ObjRef = table2ObjRefUI.GetFuncObjRef();

    for (int i = 0; i < DFO_TransformSATable::maxTransform; i++)
        transformDescUI.GetRef(i).GetTransformDesc(outData.transformDesc[i]);

}

