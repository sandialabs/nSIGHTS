///////////////////////////////////////////////////////////////////////////////////
//
// PPO_2DXYDataArrayPS.cpp
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

#include "PPO_2DXYDataArray.h"
#include "PPO_2DXYDataArrayPS.h"

void PPO_2DXYDataArrayPS::XYSeriesUIC::SetAllSensitiveCB(void* currObj)
{
    static_cast<XYSeriesUIC*>(currObj)->SetAllSensitive(true);
}

void PPO_2DXYDataArrayPS::XYSeriesUIC::InitSeriesData()
{
    ArgListC args;

    args.StdFormAttach();
    topRow.InitWidget(outsideForm.GetWidget(), args);

    args.Clear();
    args.BottomFormAttach();
    args.TopFormAttach();
    args.LeftPosAttach(25);
    args.RightFormAttach();
    seriesSpec.InitFormUI(topRow.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.LeftFormAttach();
    doSeries.StdToggleInit(" ", topRow.GetWidget(), args);
    doSeries.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.TopPosAttach(50);
    args.BottomFormAttach();
    args.RightUICAttach(seriesSpec);

    //  kluge -- controls entire geometry
    xyArrayIndxUI.listWidth = 15;
    xyArrayIndxUI.InitWidget(topRow.GetWidget(), args);

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

void PPO_2DXYDataArrayPS::XYSeriesUIC::SetSeriesData(const PFO_2DXYDataArray::XYSeries& seriesData)
{
    seriesSpec.SetSeriesData(seriesData);
    doSeries.SetValue(seriesData.doSeries);
    legendOverrideUI.SetLegendOverride(seriesData);
    xyArrayIndxUI.SetSelection(seriesData.xyArrayIndx);
}

void PPO_2DXYDataArrayPS::XYSeriesUIC::GetSeriesData(PFO_2DXYDataArray::XYSeries& seriesData)
{
    seriesData.doSeries         = doSeries.GetValue();
    seriesData.xyArrayIndx     = xyArrayIndxUI.GetSelection();
    seriesSpec.GetSeriesData(seriesData);
    legendOverrideUI.GetLegendOverride(seriesData);
}

void PPO_2DXYDataArrayPS::XYSeriesUIC::SetAllSensitive(bool inSens)
{
    doSeries.SetSensitive(inSens);
    bool otherSens = doSeries.GetValue() && inSens;
    xyArrayIndxUI.SetSensitive(otherSens);
    seriesSpec.SetSensitive(otherSens);
    legendOverrideUI.SetSensitive(otherSens);
}

CSize PPO_2DXYDataArrayPS::XYScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 600);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}


/////////////////////////////////////////////////////////////////
// PPO_2DXYDataArrayPS

PPO_2DXYDataArrayPS::PPO_2DXYDataArrayPS() :
    dataSource(typeid(DO_XYDataArray)),
    xyDataOK(false)
{
}

void PPO_2DXYDataArrayPS::SetAllSensitive()
{
    for ( int i = 0; i < PFO_2DXYDataArray::maxXYSeries; i++)
        seriesDataUI[i].SetAllSensitive(xyDataOK);
}

void PPO_2DXYDataArrayPS::SetAllSensitiveCB(void* currObj)
{
    static_cast<PPO_2DXYDataArrayPS*>(currObj)->SetAllSensitive();
}

void PPO_2DXYDataArrayPS::SetXYIndexesCB(void* inObj)
{
    static_cast<PPO_2DXYDataArrayPS*>(inObj)->SetXYIndexes();
}

void PPO_2DXYDataArrayPS::SetXYIndexes()
{
    xyDataOK = dataSource.CheckDataObj();
    if (xyDataOK)
    {
        DO_XYDataArray* dataDO = static_cast<DO_XYDataArray*>(dataSource.GetDataObj());

        SC_StringArray xyIDs;
        dataDO->GetIDs(xyIDs);

        for (int i = 0; i < PFO_2DXYDataArray::maxXYSeries; i++)
            seriesDataUI[i].xyArrayIndxUI.SetListText(xyIDs);
    }
    else
    {
        for ( int i = 0; i < PFO_2DXYDataArray::maxXYSeries; i++)
            seriesDataUI[i].xyArrayIndxUI.SetListNA();
    }
    SetAllSensitive();
}

void PPO_2DXYDataArrayPS::InitMenu(const PPO_2DXYDataArray& inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    dataSource.InitFramedUI("XY Data Array", PPform.GetWidget(), args);
    dataSource.AddOtherObjectCallback(SetXYIndexesCB, this);


    args.StdTopObjectAttach(dataSource);
    args.BottomFormAttach();
    args.LeftOffset(3);
    args.RightOffset(3);

    //scrollArea.needHorizontal = false;
    scrollArea.InitWidget(PPform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();

    for (int i = 0; i < PFO_2DXYDataArray::maxXYSeries; i++)
    {
        seriesDataUI[i].outsideForm.InitWidget(scrollArea, args);
        seriesDataUI[i].InitSeriesData();

        args.Clear();
        args.StdTopObjectAttach(seriesDataUI[i].outsideForm.GetWidget());
        seriesDataUI[i].hSep.InitWidget(scrollArea, args);

        args.Clear();
        args.StdTopObjectAttach(seriesDataUI[i].hSep.GetWidget());
    }
}

void PPO_2DXYDataArrayPS::MenuOpen(PPO_2DXYDataArray&   inData)
{
    MenuBaseC::MenuOpen(true, false);
    dataSource.StdMenuInit(inData.xyDataArrayObjRef);
    for (int i = 0; i < PFO_2DXYDataArray::maxXYSeries; i++)
    {
        seriesDataUI[i].seriesSpec.seriesPen.ResetPens(inData.objPenSet);
    }
}

void PPO_2DXYDataArrayPS::SetMenuData(PPO_2DXYDataArray&    inData)
{
    dataSource.SetFuncObjRef(inData.xyDataArrayObjRef);
    for (int i = 0; i < PFO_2DXYDataArray::maxXYSeries; i++)
        seriesDataUI[i].SetSeriesData(inData.seriesData[i]);

    SetXYIndexes();
    PPO_BasePS::SetMenuData(inData);
}

bool PPO_2DXYDataArrayPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    dataSource.FuncObjRefOK(allOK, "Data source");
    return allOK;
}

void PPO_2DXYDataArrayPS::GetMenuData(PPO_2DXYDataArray&    outData)
{
    PPO_BasePS::GetMenuData(outData);

    outData.xyDataArrayObjRef = dataSource.GetFuncObjRef();
    for (int i = 0; i < PFO_2DXYDataArray::maxXYSeries; i++)
        seriesDataUI[i].GetSeriesData(outData.seriesData[i]);
}

