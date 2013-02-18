///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CubeToIndexPS.cpp
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
#include <genClass/DO_Real.h>

#include <objClass/DO_CubeData.h>

#include "DPO_CubeToIndexPS.h"
#include "DPO_CubeToIndex.h"

DPO_CubeToIndexPS::DPO_CubeToIndexPS() :
        cubeObjRefUI(typeid(DO_CubeData))
{
}

void DPO_CubeToIndexPS::InitMenu()
{
    DPO_RangeExtractPS::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    cubeObjRefUI.InitFramedUI("Cube Input", URform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    InitDataUseUI(args, PPform.GetWidget());
    InitRECommon();
}

void DPO_CubeToIndexPS::MenuOpen(DPO_CubeToIndex& inData)
{
    DPO_RangeExtractPS::MenuOpen(inData);
    cubeObjRefUI.StdMenuInit(inData.cubeObjRef);
}

void DPO_CubeToIndexPS::SetMenuData(DPO_CubeToIndex&  inData)
{
    DPO_RangeExtractPS::SetMenuData(inData);
    cubeObjRefUI.SetFuncObjRef(inData.cubeObjRef);
}

bool DPO_CubeToIndexPS::UIdataOK()
{
    DPO_RangeExtractPS::UIdataOK();
    cubeObjRefUI.FuncObjRefOK(allOK, "Data source");
    return allOK;
}

void DPO_CubeToIndexPS::GetMenuData(DPO_CubeToIndex&  outData)
{
    DPO_RangeExtractPS::GetMenuData(outData);
    outData.cubeObjRef = cubeObjRefUI.GetFuncObjRef();
}


void DPO_CubeToIndexPS::MenuDefault()
{
    DPO_RangeExtractPS::MenuDefault();
}

