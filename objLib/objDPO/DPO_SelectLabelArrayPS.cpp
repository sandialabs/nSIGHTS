///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SelectLabelArrayPS.cpp
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
#include <genClass/DO_LabelArray.h>

#include "DPO_SelectLabelArrayPS.h"

DPO_SelectLabelArrayPS::DPO_SelectLabelArrayPS() :
    labDataArrayObjRefUI(typeid(DO_LabelArray))
{
}


void DPO_SelectLabelArrayPS::SetIndexList()
{
    SC_StringArray dataIDs;
    if (labDataArrayObjRefUI.CheckDataObj())
    {
        dataIDs = static_cast<DO_LabelArray*>(labDataArrayObjRefUI.GetDataObj())->dataLabels;
    }

    listDataOK = !dataIDs.IsEmpty();
    if (listDataOK)
    {
        indexMSUI.indexListUI.SetListText(dataIDs);
    }
    else
    {
        indexMSUI.indexListUI.SetListNA();
    }
    SetAllSensitive();

}

void DPO_SelectLabelArrayPS::SetIndexListCB(void* inObj)
{
    static_cast<DPO_SelectLabelArrayPS*>(inObj)->SetIndexList();
}


void DPO_SelectLabelArrayPS::SetAllSensitive()
{
    sortBasisUI.SetSensitive(sortSpecUI.GetChoice() != 0);
}

void DPO_SelectLabelArrayPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_SelectLabelArrayPS*>(inObj)->SetAllSensitive();
}


void DPO_SelectLabelArrayPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    labDataArrayObjRefUI.InitFramedUI("Variable Array To Select From", URform.GetWidget(), args);
    labDataArrayObjRefUI.AddOtherObjectCallback(SetIndexListCB, this);

    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    const char* ssStrs[] = {"None", "Ascending", "Descending"};
    sortSpecUI.PreInitSetUp(ssStrs, 3);
    sortSpecUI.InitFramedUI("Label Sorting", topRowForm.GetWidget(), args);
    sortSpecUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    const char* sbStrs[] = {"X", "Y", "Z"};
    sortBasisUI.PreInitSetUp(sbStrs, 3);
    sortBasisUI.InitFramedUI("Value to Sort", topRowForm.GetWidget(), args);


    args.StdTopObjectAttach(topRowForm.GetWidget());
    args.BottomFormAttach();

    DPO_SelectBasePS::InitMenu(args, false, true);
    indexMSUI.indexListUI.SetSelectionPolicy(false);
}

void DPO_SelectLabelArrayPS::MenuOpen(DPO_SelectLabelArray& inData)
{
    MenuBaseC::MenuOpen(true, true);
    indexMSUI.MenuInit(inData.indexMS);
    labDataArrayObjRefUI.StdMenuInit(inData.labDataArrayObjRef);
}


void DPO_SelectLabelArrayPS::SetMenuData(DPO_SelectLabelArray&  inData)
{
    labDataArrayObjRefUI.SetFuncObjRef(inData.labDataArrayObjRef);

    DPO_SelectBasePS::SetMenuData(inData);

    SetIndexList();

    sortSpecUI.SetChoice(inData.sortSpec);
    sortBasisUI.SetChoice(inData.sortBasis);
    SetAllSensitive();
}

bool DPO_SelectLabelArrayPS::UIdataOK()
{
    DPO_SelectBasePS::UIdataOK();
    labDataArrayObjRefUI.FuncObjRefOK(allOK, "Data source");

    return allOK;
}

void DPO_SelectLabelArrayPS::GetMenuData(DPO_SelectLabelArray&  outData)
{
    DPO_SelectBasePS::GetMenuData(outData);
    outData.labDataArrayObjRef = labDataArrayObjRefUI.GetFuncObjRef();
    outData.sortSpec = DFO_SelectLabelArray::SortSpec(sortSpecUI.GetChoice());
    if (outData.sortSpec != DFO_SelectLabelArray::ssNone)
        outData.sortBasis = DFO_SelectLabelArray::SortBasis(sortBasisUI.GetChoice());

}

void DPO_SelectLabelArrayPS::MenuClear()
{
    indexMSUI.indexListUI.ClearAll();
}

void DPO_SelectLabelArrayPS::MenuDefault()
{
    indexMSUI.indexListUI.SelectAll();
}

