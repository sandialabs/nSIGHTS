///////////////////////////////////////////////////////////////////////////////////
//
// DPO_GridTilePS.cpp
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

#include "DPO_GridTile.h"
#include "DPO_GridTilePS.h"


DPO_GridTilePS::GridInputDataUIC::GridInputDataUIC()
    :gridObjRefUI(typeid(DO_GridData))
{
}

void DPO_GridTilePS::GridInputDataUIC::SetAllSensitiveCB(void* currObj)
{
    static_cast<DPO_GridTilePS::GridInputDataUIC*>(currObj)->SetAllSensitive();
}



void DPO_GridTilePS::GridInputDataUIC::InitGridInputData()
{
    static const int specFormwidth = 600;

    ArgListC args;

    args.StdToggleSetup(5);
    doGridUI.StdToggleInit(" ", outsideForm.GetWidget(), args);
    doGridUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(80);
    gridObjRefUI.InitWidget(outsideForm.GetWidget(), args);
}

void DPO_GridTilePS::GridInputDataUIC::SetGridInputData(const DFO_GridTile::GridInputData& gridInputData)
{
    doGridUI.SetValue(gridInputData.doGrid);
    gridObjRefUI.SetFuncObjRef(gridInputData.gridObjRef);
    SetAllSensitive();
}

void DPO_GridTilePS::GridInputDataUIC::GetGridInputData(DFO_GridTile::GridInputData& gridInputData)
{
    gridInputData.gridObjRef = gridObjRefUI.GetFuncObjRef();
    gridInputData.doGrid = doGridUI.GetValue();
}

bool DPO_GridTilePS::GridInputDataUIC::GridInputDataOK(bool& prevOK)
{
    if (doGridUI.GetValue())
    {
        gridObjRefUI.FuncObjRefOK(prevOK, "grid input");
    }
    return prevOK;
}


void DPO_GridTilePS::GridInputDataUIC::SetAllSensitive()
{
    doGridUI.SetSensitive(true);
    bool doGrid = doGridUI.GetValue();
    gridObjRefUI.SetSensitive(doGrid);
}

CSize DPO_GridTilePS::gridScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 600);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}

/////////////////////////////////////////////////////////////////
// DPO_GridTilePS

DPO_GridTilePS::DPO_GridTilePS()
    : baseGridObjRefUI(typeid(DO_GridData))
{
    gridInputDataUI.Alloc(DFO_GridTile::maxInputGrid);
    for (int i = 0; i < DFO_GridTile::maxInputGrid; i++)
        gridInputDataUI += new GridInputDataUIC();
}

DPO_GridTilePS::~DPO_GridTilePS()
{
}

void DPO_GridTilePS::SetAllSensitive()
{
    for (int i = 0; i < DFO_GridTile::maxInputGrid; i++)
        gridInputDataUI.GetRef(i).SetAllSensitive();
}




void DPO_GridTilePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_GridTilePS*>(inObj)->SetAllSensitive();
}


void DPO_GridTilePS::InitMenu()
{
    MenuBaseC::InitMenu();
    ArgListC args;

    args.AllFormAttach();
    baseGridObjRefUI.InitFramedUI("Base Grid", URform.GetWidget(), args);

    args.StdFormAttach();
    //  form for row with row/col selections
    optionFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    logInterpolateUI.StdToggleInit("log interpolate", optionFrame.GetForm(), args);

    args.StdSplitFinish(60);
    dropoutFixCountUI.AttachLabelToObject();
    dropoutFixCountUI.InitLabeledUI("Dropout fix value", optionFrame.GetForm(), args);

    args.StdTopObjectAttach(optionFrame);

    // now main frame
    args.BottomFormAttach();
    gridFrame.InitFramedUI("Grids to Add", PPform.GetWidget(), args);

    args.Clear();
    args.AllFormAttach();
    args.LeftOffset(5);
    args.RightOffset(5);

    args.BottomOffset(25);

    scrollArea.InitWidget(gridFrame.GetForm(), args);

    args.StdFormAttach();
    for (int i = 0; i < DFO_GridTile::maxInputGrid; i++)
    {
        gridInputDataUI.GetRef(i).outsideForm.InitWidget(scrollArea, args);
        gridInputDataUI.GetRef(i).InitGridInputData();

        args.StdTopObjectAttach(gridInputDataUI.GetRef(i).outsideForm.GetWidget());
        gridInputDataUI.GetRef(i).hSep.InitWidget(scrollArea, args);

        args.StdTopObjectAttach(gridInputDataUI.GetRef(i).hSep.GetWidget());
    }
}


void DPO_GridTilePS::MenuOpen(DPO_GridTile&   inData)
{
    MenuBaseC::MenuOpen(false, true);

    baseGridObjRefUI.StdMenuInit(inData.baseGridObjRef);
    for (int i = 0; i < DFO_GridTile::maxInputGrid; i++)
    {
        gridInputDataUI.GetRef(i).gridObjRefUI.StdMenuInit(inData.gridsToAdd[i].gridObjRef);
    }
}


void DPO_GridTilePS::SetMenuData(DPO_GridTile&    inData)
{
    MenuBaseC::SetMenuData(inData);

    baseGridObjRefUI.SetFuncObjRef(inData.baseGridObjRef);
    for (int i = 0; i < DFO_GridTile::maxInputGrid; i++)
        gridInputDataUI.GetRef(i).SetGridInputData(inData.gridsToAdd[i]);

    logInterpolateUI.SetValue(inData.logInterpolate);
    dropoutFixCountUI.SetFieldValue(inData.dropoutFixCount);

    SetAllSensitive();
}

bool DPO_GridTilePS::UIdataOK()
{
    MenuBaseC::UIdataOK();

    baseGridObjRefUI.FuncObjRefOK(allOK, "Base grid");
    dropoutFixCountUI.FieldValueOK(allOK, "Dropout fix", 0, 100);

    for (int i = 0; i < DFO_GridTile::maxInputGrid; i++)
        if (!gridInputDataUI.GetRef(i).GridInputDataOK(allOK))
            return false;

    return allOK;
}

void DPO_GridTilePS::GetMenuData(DPO_GridTile&    outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.baseGridObjRef = baseGridObjRefUI.GetFuncObjRef();

    outData.logInterpolate = logInterpolateUI.GetValue();
    outData.dropoutFixCount = dropoutFixCountUI.GetFieldValue();

    for (int i = 0; i < DFO_GridTile::maxInputGrid; i++)
        gridInputDataUI.GetRef(i).GetGridInputData(outData.gridsToAdd[i]);
}

