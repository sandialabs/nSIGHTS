///////////////////////////////////////////////////////////////////////////////////
//
// PPO_2DXYDataPS.cpp
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


#include "PPO_2DXYData.h"
#include "PPO_2DXYDataPS.h"


PPO_2DXYDataPS::XYDataUIC::XYDataUIC() : xyDataObjRefUI(typeid(DO_XYData))
{
}

void PPO_2DXYDataPS::XYDataUIC::SetAllSensitiveCB(void* currObj)
{
 static_cast<PPO_2DXYDataPS::XYDataUIC*>(currObj)->SetAllSensitive();
}

void PPO_2DXYDataPS::XYDataUIC::InitSeriesData()
{
    ArgListC args;

    args.StdFormAttach();
    topRow.InitWidget(outsideForm.GetWidget(), args);

    args.Clear();
    args.BottomFormAttach();
    args.TopFormAttach();
    args.LeftPosAttach(35);
    args.RightFormAttach();
    seriesSpecUI.InitFormUI(topRow.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.LeftFormAttach();
    doSeriesUI.StdToggleInit(" ", topRow.GetWidget(), args);
    doSeriesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.BottomFormAttach();
    args.LeftFormAttach();
    args.RightUICAttach(seriesSpecUI);
    xyDataObjRefUI.InitFormUI(topRow.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRow.GetWidget());
    nextRow.InitWidget(outsideForm.GetWidget(), args);

    args.Clear();
    args.BottomFormAttach();
    args.TopFormAttach();
    args.LeftPosAttach(10);
    args.RightFormAttach();
    legendOverrideUI.InitFormUI(nextRow.GetWidget(), args);
 legendOverrideUI.overrideDefaultLabelUI.AddOtherObjectCallback(SetAllSensitiveCB, this);
}

void PPO_2DXYDataPS::XYDataUIC::SetSeriesData(const PFO_2DXYData::XYSeries& seriesData)
{
    seriesSpecUI.SetSeriesData(seriesData);
 legendOverrideUI.SetLegendOverride(seriesData);
    doSeriesUI.SetValue(seriesData.doSeries);
 xyDataObjRefUI.SetFuncObjRef(seriesData.xyDataObjRef);
}

void PPO_2DXYDataPS::XYDataUIC::GetSeriesData(PFO_2DXYData::XYSeries& seriesData)
{
    seriesSpecUI.GetSeriesData(seriesData);
 legendOverrideUI.GetLegendOverride(seriesData);

seriesData.xyDataObjRef = xyDataObjRefUI.GetFuncObjRef();
    seriesData.doSeries = doSeriesUI.GetValue();
}

bool PPO_2DXYDataPS::XYDataUIC::SeriesDataOK(bool& prevOK)
{
    if (doSeriesUI.GetValue())
    {
        legendOverrideUI.UIdataOK(prevOK);
        xyDataObjRefUI.FuncObjRefOK(prevOK, "XY input");
    }
    return prevOK;
}


void PPO_2DXYDataPS::XYDataUIC::SetAllSensitive()
{
    doSeriesUI.SetSensitive(true);
    bool doSeries = doSeriesUI.GetValue();

    seriesSpecUI.SetSensitive(doSeries);
    legendOverrideUI.SetSensitive(doSeries);
    xyDataObjRefUI.SetSensitive(doSeries);
}

CSize PPO_2DXYDataPS::XYDataScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 650);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}

/////////////////////////////////////////////////////////////////
// PPO_2DXYDataPS

PPO_2DXYDataPS::PPO_2DXYDataPS()
{
}


void PPO_2DXYDataPS::SetAllSensitive()
{
    for (int i = 0; i < PFO_2DXYData::maxXYSeries; i++)
        xySeriesUI[i].SetAllSensitive();
}


void PPO_2DXYDataPS::InitMenu(const PPO_2DXYData&   inData)

{
    PPO_BasePS::InitMenu(inData);

//  scrolled window in middle
    ArgListC args;
    args.Clear();
    args.AllFormAttach();
    args.LeftOffset(3);
    args.RightOffset(3);

    //scrollArea.needHorizontal = false;
    scrollArea.InitWidget(PPform.GetWidget(), args);
    args.Clear();
    args.StdFormAttach();

    for (int i = 0; i < PFO_2DXYData::maxXYSeries; i++)
    {
        xySeriesUI[i].outsideForm.InitWidget(scrollArea, args);
        xySeriesUI[i].InitSeriesData();

        args.Clear();
        args.StdTopObjectAttach(xySeriesUI[i].outsideForm.GetWidget());
        xySeriesUI[i].hSep.InitWidget(scrollArea, args);

        args.Clear();
        args.StdTopObjectAttach(xySeriesUI[i].hSep.GetWidget());
    }
}


void PPO_2DXYDataPS::MenuOpen(PPO_2DXYData& inData)
{
    MenuBaseC::MenuOpen(true, false);
    for (int i = 0; i < PFO_2DXYData::maxXYSeries; i++)
    {
        xySeriesUI[i].xyDataObjRefUI.StdMenuInit(inData.xySeries[i].xyDataObjRef);
        xySeriesUI[i].seriesSpecUI.seriesPen.ResetPens(inData.objPenSet);
    }
}


void PPO_2DXYDataPS::SetMenuData(PPO_2DXYData&  inData)
{
    for (int i = 0; i < PFO_2DXYData::maxXYSeries; i++)
        xySeriesUI[i].SetSeriesData(inData.xySeries[i]);
    PPO_BasePS::SetMenuData(inData);
    SetAllSensitive();
}

bool PPO_2DXYDataPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    for (int i = 0; i < PFO_2DXYData::maxXYSeries; i++)
        if (!xySeriesUI[i].SeriesDataOK(allOK))
            return false;

    return allOK;
}

void PPO_2DXYDataPS::GetMenuData(PPO_2DXYData&  outData)
{
    PPO_BasePS::GetMenuData(outData);
    for (int i = 0; i < PFO_2DXYData::maxXYSeries; i++)
        xySeriesUI[i].GetSeriesData(outData.xySeries[i]);
}


