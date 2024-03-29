///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ChooseColorMapPS.cpp
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
// DESCRIPTION: File I/O and user interface for all genDFO functional objects.
//              All code is platform independent, but relies upon platform dependent
//              code in genApp and genClassPS.
//              User interface for DFO_XXXX objects.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/DO_ColorMap.h>

#include "DPO_ChooseColorMapPS.h"

DPO_ChooseColorMapPS::DPO_ChooseColorMapPS() :
    inputColorMapObjRefUI(typeid(DO_ColorMap))
{
}

void DPO_ChooseColorMapPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    inputColorMapObjRefUI.InitFramedUI("Color Map Data To Choose", URform.GetWidget(), args);
}

void DPO_ChooseColorMapPS::MenuOpen(DPO_ChooseColorMap& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputColorMapObjRefUI.StdMenuInit(inData.inputColorMapObjRef);
}


void DPO_ChooseColorMapPS::SetMenuData(DPO_ChooseColorMap&  inData)
{
    inputColorMapObjRefUI.SetFuncObjRef(inData.inputColorMapObjRef);
    MenuBaseC::SetMenuData(inData);
}

bool DPO_ChooseColorMapPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputColorMapObjRefUI.FuncObjRefOK(allOK, "Index data source");
    return allOK;
}

void DPO_ChooseColorMapPS::GetMenuData(DPO_ChooseColorMap&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputColorMapObjRef = inputColorMapObjRefUI.GetFuncObjRef();
}

