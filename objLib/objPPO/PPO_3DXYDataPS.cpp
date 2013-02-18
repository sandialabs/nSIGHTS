///////////////////////////////////////////////////////////////////////////////////
//
// PPO_3DXYDataPS.cpp
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
//      File I/O and user interface for all objPFOGL objects
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include "PPO_3DXYDataPS.h"


PPO_3DXYDataPS::PPO_3DXYDataPS() :
    xyDataObjRefUI(typeid(DO_XYData)),
    zValueUI(typeid(DO_Real), false)
{
}

void PPO_3DXYDataPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_3DXYDataPS*>(inObj)->SetAllSensitive();
}

void PPO_3DXYDataPS::SetAllSensitive()
{
    legendOverrideUI.SetSensitive(true);
    xy3DSeriesUI.SetSensitive(true);
    zValueUI.SetAllSensitive();
}

void PPO_3DXYDataPS::InitMenu(const PPO_3DXYData&   inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;

    args.StdFormAttach();
    xyDataObjRefUI.InitFramedUI("XY Data to Plot", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(xyDataObjRefUI);

    xy3DSeriesUI.InitFramedUI("Plotting Spec.", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(xy3DSeriesUI);
    zValueUI.InitUI("Z", args, PPform.GetWidget(), true, true, SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(zValueUI);
    coordMapUI.StdInit(PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(coordMapUI.coordMapFrame);
    legendOverrideUI.InitFramedUI("Series Label", PPform.GetWidget(), args);
    legendOverrideUI.overrideDefaultLabelUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.StdTopObjectAttach(legendOverrideUI);
    offsetCoordUI.StdInit("Offset", PPform.GetWidget(), args);
}


void PPO_3DXYDataPS::MenuOpen(PPO_3DXYData& inData)
{
    MenuBaseC::MenuOpen(true, false);
    xyDataObjRefUI.StdMenuInit(inData.xyDataObjRef);
    xy3DSeriesUI.ResetPens(inData.objPenSet);
    zValueUI.MenuInit(inData.zValue);
}

void PPO_3DXYDataPS::SetMenuData(PPO_3DXYData&  inData)
{
    xyDataObjRefUI.SetFuncObjRef(inData.xyDataObjRef);
    xy3DSeriesUI.Set3DSeriesData(inData.xySeries);
    zValueUI.SetValue(inData.zValue);
    coordMapUI.Set3DCoordMap(inData.coordMap);
    legendOverrideUI.SetLegendOverride(inData.seriesLabel);

    SetAllSensitive();

    PPO_BasePS::SetMenuData(inData);
}

bool PPO_3DXYDataPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    legendOverrideUI.UIdataOK(allOK);
    xyDataObjRefUI.FuncObjRefOK(allOK, "XY input");
    zValueUI.UIdataOK(allOK);
    return allOK;
}

void PPO_3DXYDataPS::GetMenuData(PPO_3DXYData&  outData)
{
    PPO_BasePS::GetMenuData(outData);

    outData.xyDataObjRef = xyDataObjRefUI.GetFuncObjRef();
    xy3DSeriesUI.Get3DSeriesData(outData.xySeries);
    legendOverrideUI.GetLegendOverride(outData.seriesLabel);
    zValueUI.GetValue(outData.zValue);
    coordMapUI.Get3DCoordMap(outData.coordMap);
}

void PPO_3DXYDataPS::MenuDefault()
{
    PPO_BasePS::MenuDefault();

    xy3DSeriesUI.Set3DSeriesData(PSC_3DSeriesSpec());
    SetAllSensitive();
}

