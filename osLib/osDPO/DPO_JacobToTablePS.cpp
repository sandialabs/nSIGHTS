///////////////////////////////////////////////////////////////////////////////////
//
// DPO_JacobToTablePS.cpp
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
//      I/O and user interface for all osDFO functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genClass/U_Str.h>

#include <osClass/DO_Jacobian.h>

#include "DPO_JacobToTable.h"
#include "DPO_JacobToTablePS.h"


DPO_JacobToTablePS::DPO_JacobToTablePS() :
    jacobObjRefUI(typeid(DO_Jacobian))
{
    jacDataOK = false;
}


void DPO_JacobToTablePS::SetAllSensitive()
{
    bool parJacob =  parJacobianUI.GetBoolChoice();
    fitCompIndexUI.SetSensitive(parJacob && jacDataOK);
    parCompIndexUI.SetSensitive((!parJacob) && jacDataOK);
}


void DPO_JacobToTablePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_JacobToTablePS*>(inObj)->SetAllSensitive();
}

void DPO_JacobToTablePS::SetCompIndex()
{
    jacDataOK = jacobObjRefUI.CheckDataObj();
    if (jacDataOK)
    {
        DC_Jacobian& jacData = *(static_cast<DO_Jacobian*>(jacobObjRefUI.GetDataObj())->jacobianData);

        SC_StringArray parDesc;
        parDesc.SetStringLen(OptVarDescription::varDescLen);
        int nPar = jacData.parDesc.Size();
        parDesc.Alloc(nPar + 1);
        parDesc += "All Parameters";
        int i;
        for (i = 0; i < nPar; i++)
            parDesc += jacData.parDesc[i].varDesc;
        parCompIndexUI.SetListText(parDesc);

        SC_StringArray fitDesc;
        fitDesc.SetStringLen(OptFitComponentDescription::fitIDLen);
        int nFit = jacData.fitDesc.fitComponentDescription.Size();
        fitDesc.Alloc(nFit + 1);
        fitDesc += "All Fits";
        for (i = 0; i < nFit; i++)
            fitDesc +=  jacData.fitDesc.fitComponentDescription[i].fitID;
        fitCompIndexUI.SetListText(fitDesc);
    }
    else
    {
        fitCompIndexUI.SetListNA();
        parCompIndexUI.SetListNA();
    }
    SetAllSensitive();
}


void DPO_JacobToTablePS::SetCompIndexCB(void* inObj)
{
    static_cast<DPO_JacobToTablePS*>(inObj)->SetCompIndex();
}


void DPO_JacobToTablePS::InitMenu()

{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    jacobObjRefUI.InitFramedUI("Jacobian Data To Convert", URform.GetWidget(), args);
    jacobObjRefUI.AddOtherObjectCallback(SetCompIndexCB, this);

    args.Clear();
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    parJacobianUI.PreInitSetUp("Fits", "Parameters");
    parJacobianUI.InitFramedUI("Table Columns", topRowForm.GetWidget(), args);
    parJacobianUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.ResetPlace();
    args.LeftPosAttach(33);
    args.RightPosAttach(66);
    fitCompIndexUI.InitFramedUI("Fit(s)", topRowForm.GetWidget(), args);

    args.StdSplitFinish(34);
    parCompIndexUI.InitFramedUI("Parameter(s)", topRowForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRowForm.GetWidget());
    secondRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    xareValuesUI.PreInitSetUp("Index", "X Value");
    xareValuesUI.InitFramedUI("Column 1 Data", secondRowForm.GetWidget(), args);

    args.StdSplitFinish(67);
    optionsFrame.InitFramedUI("Options", secondRowForm.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.LeftFormAttach();
    args.LeftOffset(10);
    takeAbsUI.StdToggleInit("ABS(dp/dr)", optionsFrame.GetForm(), args);
}


void DPO_JacobToTablePS::MenuOpen(DPO_JacobToTable&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    jacobObjRefUI.StdMenuInit(inData.jacobObjRef);
}


void DPO_JacobToTablePS::SetMenuData(DPO_JacobToTable&    inData)
{
    MenuBaseC::SetMenuData(inData);

    jacobObjRefUI.SetFuncObjRef(inData.jacobObjRef);

    parJacobianUI.SetBoolChoice(inData.parJacobian);

    fitCompIndexUI.SetSelection(inData.fitCompIndex + 1);
    parCompIndexUI.SetSelection(inData.parCompIndex + 1);

    xareValuesUI.SetBoolChoice(inData.xareValues);

    takeAbsUI.SetValue(inData.takeAbs);

    SetCompIndex();
}

bool DPO_JacobToTablePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    jacobObjRefUI.FuncObjRefOK(allOK, "Jacobian data source");

    return allOK;
}

void DPO_JacobToTablePS::GetMenuData(DPO_JacobToTable&    outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.jacobObjRef = jacobObjRefUI.GetFuncObjRef();

    outData.parJacobian = parJacobianUI.GetBoolChoice();

    if (outData.parJacobian)
        outData.fitCompIndex = fitCompIndexUI.GetSelection() - 1;
    else
        outData.parCompIndex = parCompIndexUI.GetSelection() - 1;

    outData.xareValues = xareValuesUI.GetBoolChoice();

    outData.takeAbs = takeAbsUI.GetValue();

}

