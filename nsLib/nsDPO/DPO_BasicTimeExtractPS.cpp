///////////////////////////////////////////////////////////////////////////////////
//
// DPO_BasicTimeExtractPS.cpp
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

#include "nsLibAfx.h"

#include "DPO_BasicTimeExtractPS.h"

DPO_BasicTimeExtractPS::DPO_BasicTimeExtractPS() :
    xyInputObjRefUI(typeid(DO_XYData))
{
}

void DPO_BasicTimeExtractPS::InitMenu()
{
    MenuBaseC::InitMenu();


    ArgListC args;
    args.AllFormAttach();
    xyInputObjRefUI.InitFramedUI("XY Input Data", URform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    timeLimitsUI.StdInit(PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(timeLimitsUI.mainForm.GetWidget());
    interpPointsUI.StdInit(PPform.GetWidget(), args);
}

void DPO_BasicTimeExtractPS::MenuOpen(DPO_BasicTimeExtract& inData)
{
    MenuBaseC::MenuOpen(false, false);
    xyInputObjRefUI.StdMenuInit(inData.xyInputObjRef);
    timeLimitsUI.StdMenuInit(inData.timeLimits);
}


void DPO_BasicTimeExtractPS::SetMenuData(DPO_BasicTimeExtract&  inData)
{
    xyInputObjRefUI.SetFuncObjRef(inData.xyInputObjRef);
    timeLimitsUI.SetTimeLimits(inData.timeLimits);
    interpPointsUI.SetInterp(inData.interpPoints);

    MenuBaseC::SetMenuData(inData);
}

bool  DPO_BasicTimeExtractPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    xyInputObjRefUI.FuncObjRefOK(allOK, "Field data source");
    timeLimitsUI.UIdataOK(allOK);
    interpPointsUI.UIdataOK(allOK);

    return allOK;
}

void DPO_BasicTimeExtractPS::GetMenuData(DPO_BasicTimeExtract&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.xyInputObjRef = xyInputObjRefUI.GetFuncObjRef();

    timeLimitsUI.GetTimeLimits(outData.timeLimits);
    interpPointsUI.GetInterp(outData.interpPoints);
}

