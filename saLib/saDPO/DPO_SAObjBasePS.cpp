///////////////////////////////////////////////////////////////////////////////////
//
// DPO_SAObjBasePS.cpp
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

#include <saClass/DO_SATable.h>
#include <saDFO/DFO_SAObjBase.h>

#include "DPO_SAObjBasePS.h"

DPO_SAObjBasePS::DPO_SAObjBasePS() :
indepVarObjRefUI(typeid(DO_SATable)),
depVarObjRefUI(typeid(DO_SATable))
{
}


void DPO_SAObjBasePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
    titleUI.AttachObjectToFrame();
    titleUI.InitFramedUI("Title", PPform.GetWidget(), args);

    args.StdTopObjectAttach(titleUI);
    varForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    indepVarObjRefUI.InitFramedUI("Independent Variables", varForm.GetWidget(), args);

    args.StdSplitFinish();
    depVarObjRefUI.InitFramedUI("Dependent Variables", varForm.GetWidget(), args);
}

void DPO_SAObjBasePS::MenuOpen(DFO_SAObjBase&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    indepVarObjRefUI.StdMenuInit(inData.indepVarObjRef);
    depVarObjRefUI.StdMenuInit(inData.depVarObjRef);
}


void DPO_SAObjBasePS::SetMenuData(DFO_SAObjBase&  inData)
{
    titleUI.SetFieldString(inData.title);
    indepVarObjRefUI.SetFuncObjRef(inData.indepVarObjRef);
    depVarObjRefUI.SetFuncObjRef(inData.depVarObjRef);
}

bool DPO_SAObjBasePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    indepVarObjRefUI.FuncObjRefOK(allOK, "Independent variable");
    depVarObjRefUI.FuncObjRefOK(allOK, "Dependent variable");
    return allOK;
}


void DPO_SAObjBasePS::GetMenuData(DFO_SAObjBase&  outData)
{
    outData.indepVarObjRef = indepVarObjRefUI.GetFuncObjRef();
    outData.depVarObjRef = depVarObjRefUI.GetFuncObjRef();
    titleUI.GetFieldString(outData.title, DFO_SAObjBase::titleLen);
}

