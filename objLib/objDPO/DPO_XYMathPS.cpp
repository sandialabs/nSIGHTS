///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYMathPS.cpp
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
#include "DPO_XYMathPS.h"

DPO_XYMathPS::DPO_XYMathPS() :
    dataASource(typeid(DO_XYData)),
    dataBSource(typeid(DO_XYData))
{
}

void DPO_XYMathPS::InitMenu()
{
    InitABUI(dataASource, dataBSource);

    ArgListC args;
    args.RightOffset(50);
    args.StdTopObjectAttach(topRowForm.GetWidget());
    operateOnXUI.PreInitSetUp("   X Data   ", "   Y Data   ");
    operateOnXUI.InitFramedUI("Data Component to Operate On", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(operateOnXUI);
    InitOpUI(args);

    args.Clear();
    args.StdTopObjectAttach(mathOpUI);
    InitDescUI(args);
}

void DPO_XYMathPS::MenuOpen(DPO_XYMath& inData)
{
    MenuBaseC::MenuOpen(false, false);
    dataASource.StdMenuInit(inData.dataInputAObjRef);
    dataBSource.StdMenuInit(inData.dataInputBObjRef);
}


void DPO_XYMathPS::SetMenuData(DPO_XYMath&  inData)
{
    dataASource.SetFuncObjRef(inData.dataInputAObjRef);
    dataBSource.SetFuncObjRef(inData.dataInputBObjRef);
    operateOnXUI.SetBoolChoice(!inData.operateOnX);

    DPO_MathBasePS::SetMenuData(inData);
    MenuBaseC::SetMenuData(inData);
}

bool DPO_XYMathPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    dataASource.FuncObjRefOK(allOK, "A Data source");
    dataBSource.FuncObjRefOK(allOK, "B Data source");
    return allOK;
}

void DPO_XYMathPS::GetMenuData(DPO_XYMath&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.dataInputAObjRef = dataASource.GetFuncObjRef();
    outData.dataInputBObjRef = dataBSource.GetFuncObjRef();
    outData.operateOnX = ! operateOnXUI.GetBoolChoice();
    DPO_MathBasePS::GetMenuData(outData);
}

