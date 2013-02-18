///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYArrayToTablePS.cpp
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

#include <genClass/DO_XYData.h>
#include <objClass/DO_XYDataArray.h>

#include "DPO_XYArrayToTable.h"
#include "DPO_XYArrayToTablePS.h"


DPO_XYArrayToTablePS::DPO_XYArrayToTablePS()
:inputXYArrayObjRefUI(typeid(DO_XYDataArray)),
interpolateToCommonXObjRefUI(typeid(DO_XYDataArray))
{
}


void DPO_XYArrayToTablePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_XYArrayToTablePS*>(inObj)->SetAllSensitive();
}

void DPO_XYArrayToTablePS::SetAllSensitive()
{
    interpolateToCommonXObjRefUI.SetSensitive(interpolateToCommonXUI.GetValue());
}



void DPO_XYArrayToTablePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
    inputXYArrayObjRefUI.InitFramedUI("XY Array Data to Convert", PPform.GetWidget(), args);

    args.StdTopObjectAttach(inputXYArrayObjRefUI);
    interpolateFrame.InitFramedUI("Interpolation", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    interpolateToCommonXUI.StdToggleInit("interpolate", interpolateFrame.GetForm(), args);
    interpolateToCommonXUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(80);
    interpolateToCommonXObjRefUI.InitWidget(interpolateFrame.GetForm(), args);
}

void DPO_XYArrayToTablePS::SetMenuData(DPO_XYArrayToTable&  inData)
{
    MenuBaseC::SetMenuData(inData);
    inputXYArrayObjRefUI.SetFuncObjRef(inData.inputXYArrayObjRef);
    interpolateToCommonXUI.SetValue(inData.interpolateToCommonX);
    interpolateToCommonXObjRefUI.SetFuncObjRef(inData.interpolateToCommonXObjRef);
}


void DPO_XYArrayToTablePS::MenuOpen(DPO_XYArrayToTable& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputXYArrayObjRefUI.StdMenuInit(inData.inputXYArrayObjRef);
    interpolateToCommonXObjRefUI.StdMenuInit(inData.interpolateToCommonXObjRef);
}


bool DPO_XYArrayToTablePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputXYArrayObjRefUI.FuncObjRefOK(allOK, "XY array input data source");
    if (interpolateToCommonXUI.GetValue())
        interpolateToCommonXObjRefUI.FuncObjRefOK(allOK, "XY interpolate input data source");
    return allOK;
}

void DPO_XYArrayToTablePS::GetMenuData(DPO_XYArrayToTable&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputXYArrayObjRef = inputXYArrayObjRefUI.GetFuncObjRef();
    outData.interpolateToCommonX = interpolateToCommonXUI.GetValue();
    if (outData.interpolateToCommonX)
        outData.interpolateToCommonXObjRef = interpolateToCommonXObjRefUI.GetFuncObjRef();

}

