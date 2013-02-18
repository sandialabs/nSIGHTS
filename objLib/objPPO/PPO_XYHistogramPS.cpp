///////////////////////////////////////////////////////////////////////////////////
//
// PPO_XYHistogramPS.cpp
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

#include <genClass/DO_XYData.h>

#include "PPO_XYHistogram.h"
#include "PPO_XYHistogramPS.h"


PPO_XYHistogramPS::PPO_XYHistogramPS() :
    xyInputObjRefUI(typeid(DO_XYData))
{
}

void PPO_XYHistogramPS::SetAllSensitive()
{
    legendDataUI.SetSensitive(true);
}

void PPO_XYHistogramPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_XYHistogramPS*>(inObj)->SetAllSensitive();
}


void PPO_XYHistogramPS::InitMenu(const PPO_XYHistogram& inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    firstRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(65);
    xyInputObjRefUI.InitFramedUI("XY Data to Plot", firstRow.GetWidget(), args);

    args.StdSplitFinish(35);
    histogramPenUI.InitFramedUI("Histogram Pen", firstRow.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(firstRow.GetWidget());

    histogramUI.StdInit(PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(histogramUI.edgeSpecUI);
    legendDataUI.InitFramedUI("Legend", PPform.GetWidget(), args);
    legendDataUI.overrideDefaultLabelUI.AddOtherObjectCallback(SetAllSensitiveCB, this);
}

void PPO_XYHistogramPS::MenuOpen(PPO_XYHistogram&   inData)
{
    MenuBaseC::MenuOpen(true, false);
    xyInputObjRefUI.StdMenuInit(inData.xyInputObjRef);
    histogramPenUI.ResetPens(inData.objPenSet);
    histogramUI.MenuOpen(inData, inData);
}

void PPO_XYHistogramPS::SetMenuData(PPO_XYHistogram&    inData)
{
    PPO_BasePS::SetMenuData(inData);
    xyInputObjRefUI.SetFuncObjRef(inData.xyInputObjRef);

    histogramPenUI.SetPenSelection(inData.histogramPen);

    histogramUI.SetHistogram(inData);

    legendDataUI.SetLegendOverride(inData.legendData);

    SetAllSensitive();
}

bool PPO_XYHistogramPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    xyInputObjRefUI.FuncObjRefOK(allOK, "XY Data");

    histogramUI.UIdataOK(allOK);

    legendDataUI.UIdataOK(allOK);

    return allOK;
}

void PPO_XYHistogramPS::GetMenuData(PPO_XYHistogram&    outData)
{
    PPO_BasePS::GetMenuData(outData);
    outData.xyInputObjRef = xyInputObjRefUI.GetFuncObjRef();

    outData.histogramPen = histogramPenUI.GetPenSelection();

    histogramUI.GetHistogram(outData);

    legendDataUI.GetLegendOverride(outData.legendData);
}


