///////////////////////////////////////////////////////////////////////////////////
//
// LPO_JacobianPS.cpp
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
//      File I/O and user interface for all osLFO functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genClass/U_Str.h>

#include <osClass/DO_Jacobian.h>

#include "LPO_Jacobian.h"
#include "LPO_JacobianPS.h"


LPO_JacobianPS::LPO_JacobianPS() :
    jacobObjRefUI(typeid(DO_Jacobian))
{
//    jacDataOK = false;
}


void LPO_JacobianPS::SetAllSensitive()
{
//    fitCompIndexUI.SetSensitive(jacDataOK);
}


void LPO_JacobianPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<LPO_JacobianPS*>(inObj)->SetAllSensitive();
}
/*
void LPO_JacobianPS::SetCompIndex()
{
    jacDataOK = jacobObjRefUI.CheckDataObj();
    if (jacDataOK)
    {
        DC_Jacobian& jacData = *(static_cast<DO_Jacobian*>(jacobObjRefUI.GetDataObj())->jacobianData);

        SC_StringArray fitDesc;
        fitDesc.SetStringLen(OptFitComponentDescription::fitIDLen);
        int nFit = jacData.fitDesc.fitComponentDescription.Size();
        fitDesc.Alloc(nFit + 1);
        fitDesc += "All Fits";
        for (int i = 0; i < nFit; i++)
            fitDesc +=  jacData.fitDesc.fitComponentDescription[i].fitID;
        fitCompIndexUI.SetListText(fitDesc);
    }
    else
    {
        fitCompIndexUI.SetListNA();
    }
    SetAllSensitive();
}


void LPO_JacobianPS::SetCompIndexCB(void* inObj)
{
    static_cast<LPO_JacobianPS*>(inObj)->SetCompIndex();
}

*/
void LPO_JacobianPS::InitMenu()

{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    jacobObjRefUI.InitFramedUI("Jacobian Data To List", URform.GetWidget(), args);
//  jacobObjRefUI.AddOtherObjectCallback(SetCompIndexCB, this);
/*
    args.Clear();
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    fitCompIndexUI.InitFramedUI("Fit(s) to List", topRowForm.GetWidget(), args);
*/
}


void LPO_JacobianPS::MenuOpen(LPO_Jacobian&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    jacobObjRefUI.StdMenuInit(inData.jacobObjRef);
}


void LPO_JacobianPS::SetMenuData(LPO_Jacobian&    inData)
{
    MenuBaseC::SetMenuData(inData);

    jacobObjRefUI.SetFuncObjRef(inData.jacobObjRef);

//    fitCompIndexUI.SetSelection(inData.fitCompIndex + 1);
//    SetCompIndex();
}

bool LPO_JacobianPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    jacobObjRefUI.FuncObjRefOK(allOK, "Jacobian data source");

    return allOK;
}

void LPO_JacobianPS::GetMenuData(LPO_Jacobian&    outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.jacobObjRef = jacobObjRefUI.GetFuncObjRef();
//    outData.fitCompIndex = fitCompIndexUI.GetSelection() - 1;
}

