///////////////////////////////////////////////////////////////////////////////////
//
// PPO_SingleConfLimitsPS.cpp
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
//      File I/O and user interface for all osPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <osClass/DO_CovarArray.h>
#include <genClass/DO_ColorMap.h>

#include "PPO_SingleConfLimitsPS.h"

// ****************************************

PPO_SingleConfLimitsPS::PPO_SingleConfLimitsPS() :
    PPO_ColorBasePS(typeid(DO_CovarArray)),
    covarDataOK(false)
{
}

void PPO_SingleConfLimitsPS::SetAllSensitive()
{
    xDataIndxUI.SetSensitive(covarDataOK && !allParamUI.GetBoolChoice());

    bool plotLine = plotConfLimitsUI.GetValue();
    covarLineThkUI.SetSensitive(plotLine);
    cLimitsUI.SetSensitive(plotLine);

    bool plotSymbol = plotBestEstimateUI.GetValue();
    pointTypeUI.SetSensitive(plotSymbol);
    pointSizeUI.SetSensitive(plotSymbol);

    int wSpec = widthSpecUI.GetChoice();
    widthSpecValueUI.SetSensitive(wSpec == 1);
    widthSpecPixelUI.SetSensitive(wSpec == 2);
    widthSpecPctUI.SetSensitive(wSpec == 3);

    bool useCM = plotWithColorMapUI.GetBoolChoice();
    fixedColorPenUI.SetSensitive(!useCM);
    colorMapValueUI.SetSensitive(useCM);
    limitAndMapForm.SetSensitive(useCM);
    outsideForm.SetSensitive(useCM);

    if (useCM)
    {
        PPO_ColorBasePS::SetAllSensitive();
        PPO_ColorBasePS::limitSelectionMS.SetSensitive(useCM);
    }
}

void PPO_SingleConfLimitsPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_SingleConfLimitsPS*>(inObj)->SetAllSensitive();
}


void PPO_SingleConfLimitsPS::SetCovarIndexCB(void* inObj)

{
  static_cast<PPO_SingleConfLimitsPS*>(inObj)->SetCovarIndex();
}

void PPO_SingleConfLimitsPS::SetCovarIndex()
{

    covarDataOK = inputDataObjRefUI.DataObjAvail();
    DO_CovarArray* dataDO;
    if (covarDataOK)
    {
        dataDO = static_cast<DO_CovarArray*>(inputDataObjRefUI.GetDataObj());
        covarDataOK = dataDO->covarHeader->Size() > 0;
    }

    if (covarDataOK)
    {
        SC_StringArray covarDesc;
        dataDO->covarHeader->GetIDs(covarDesc);
        xDataIndxUI.SetListText(covarDesc);
    }
    else
    {
        xDataIndxUI.SetListNA();
    }
    SetAllSensitive();
}


void PPO_SingleConfLimitsPS::InitMenu(const PPO_SingleConfLimits& inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    inputDataObjRefUI.topOffset = 0;
    inputDataObjRefUI.InitFramedUI("Covariance Data", PPform.GetWidget(), args);
    inputDataObjRefUI.AddOtherObjectCallback(SetCovarIndexCB, this);

    args.StdTopObjectAttach(inputDataObjRefUI.GetForm());
    indexRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    allParamUI.PreInitSetUp("Single", "All");
    allParamUI.InitFramedUI("Parameters to Plot", indexRow.GetWidget(), args);
    allParamUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    xDataIndxUI.topOffset = 0;
    xDataIndxUI.listWidth = 20;

    args.StdSplitFinish();
    xDataIndxUI.InitFramedUI("X Variable", indexRow.GetWidget(), args);

    args.StdTopObjectAttach(indexRow.GetWidget());
    lineFrame.InitFramedUI("Confidence Limit Line", PPform.GetWidget(), args);

    args.StdSplitSetup(20);
    args.LeftOffset(15);
    args.TopOffset(5);
    plotConfLimitsUI.StdToggleInit("Plot", lineFrame.GetForm(), args);
    plotConfLimitsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(30, 60);
    static const char* cLimitsStrings[] = {"99.0", "95.4", "90.0", "68.3"};
    cLimitsUI.PreInitSetUp(cLimitsStrings, 4);
    cLimitsUI.InitLabeledUI("Confidence Limits", lineFrame.GetForm(), args);

    args.StdSplitFinish(20);
    covarLineThkUI.InitLabeledUI("Thickness", lineFrame.GetForm(), args);

    args.StdTopObjectAttach(lineFrame.GetForm());
    pointFrame.InitFramedUI("Best Estimate Symbol", PPform.GetWidget(), args);

    args.StdSplitSetup(20);
    args.LeftOffset(15);
    plotBestEstimateUI.StdToggleInit("Plot", pointFrame.GetForm(), args);
    plotBestEstimateUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(20, 40);
    pointTypeUI.InitWidget(pointFrame.GetForm(), args);

    args.StdSplitMiddle(40, 60);
    pointSizeUI.InitWidget(pointFrame.GetForm(), args);

    args.StdTopObjectAttach(pointFrame.GetForm());
    widthRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(50);
    static const char* widthSpecLabels[] = {"Line", "X Value", "Pixel", "% Avail"};
    widthSpecUI.PreInitSetUp(widthSpecLabels, 4);
    widthSpecUI.InitFramedUI("Multiple Width", widthRow.GetWidget(), args);
    widthSpecUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(50, 67);
    widthSpecValueUI.textFieldLen = 5;
    widthSpecValueUI.InitFramedUI("X Value", widthRow.GetWidget(), args);

    args.StdSplitMiddle(67, 83);
    widthSpecPixelUI.InitFramedUI("Pixels", widthRow.GetWidget(), args);

    args.StdSplitFinish(17);
    widthSpecPctUI.textFieldLen = 5;
    widthSpecPctUI.InitFramedUI("% Available", widthRow.GetWidget(), args);

    args.StdTopObjectAttach(widthRow.GetWidget());
    colorForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(30);
    plotWithColorMapUI.rightOffset = 50;
    plotWithColorMapUI.PreInitSetUp("Pen", "Color Map", false);
    plotWithColorMapUI.InitFramedUI("Line/Symbol Color", colorForm.GetWidget(), args);
    plotWithColorMapUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(30, 60);
    fixedColorPenUI.InitFramedUI("Single Color Pen", colorForm.GetWidget(), args);

    args.StdSplitFinish(40);
    colorMapValueUI.PreInitSetUp("SSE", "Index");
    colorMapValueUI.InitFramedUI("Color Map Value", colorForm.GetWidget(), args);

    args.StdTopObjectAttach(colorForm.GetWidget());
    const char* viewStrings[] = {"All data"};
    PPO_ColorBasePS::InitMenu(inData, args, viewStrings, 1);

    args.StdTopObjectAttach(outsideForm.GetWidget());
    legendOverrideUI.InitFramedUI("Legend Description", PPform.GetWidget(), args);
}


void PPO_SingleConfLimitsPS::MenuOpen(PPO_SingleConfLimits&   inData)
{
    PPO_ColorBasePS::MenuOpen(inData);
    //covarArrayObjRefUI.StdMenuInit(inData.covarArrayObjRef);
    fixedColorPenUI.ResetPens(inData.objPenSet);
}

void PPO_SingleConfLimitsPS::SetMenuData(PPO_SingleConfLimits&    inData)
{
    PPO_ColorBasePS::SetMenuData(inData, inData);

    //covarArrayObjRefUI.SetFuncObjRef(inData.covarArrayObjRef);

    plotWithColorMapUI.SetBoolChoice(inData.plotWithColorMap);
    fixedColorPenUI.SetPenSelection(inData.covarPen);

    allParamUI.SetBoolChoice(inData.allParameters);
    xDataIndxUI.SetSelection(inData.xDataIndx);

    cLimitsUI.SetChoice(inData.cLimits);

    plotConfLimitsUI.SetValue(inData.plotConfLimits);
    covarLineThkUI.SetLineThick(inData.covarLineThk);

    plotBestEstimateUI.SetValue(inData.plotBestEstimate);
    pointTypeUI.SetSymbolType(inData.bestEstSymbol.symbolType);
    pointSizeUI.SetSymbolSize(inData.bestEstSymbol.symbolSize);

    widthSpecUI.SetChoice(int(inData.widthSpec));
    widthSpecValueUI.SetFieldValue(inData.widthSpecValue);
    widthSpecPixelUI.SetSymbolSize(inData.widthSpecPixel);
    widthSpecPctUI.SetFieldValue(inData.widthSpecPct);

    colorMapValueUI.SetBoolChoice(inData.colorMapIsIndex);

    legendOverrideUI.SetLegendOverride(inData.legendOverride);

    SetCovarIndex();
}

bool PPO_SingleConfLimitsPS::UIdataOK()
{
    PPO_ColorBasePS::UIdataOK();
    //covarArrayObjRefUI.FuncObjRefOK(allOK, "covariance input");

    if (widthSpecUI.GetChoice() == 1)
        widthSpecValueUI.FieldValueOK(allOK, "histogram width", 1.0E-5, 1.0E+06);

    if (widthSpecUI.GetChoice() == 3)
        widthSpecPctUI.FieldValueOK(allOK, "histogram % width", 1.0, 100.0);

    return allOK;
}

void PPO_SingleConfLimitsPS::GetMenuData(PPO_SingleConfLimits&  outData)
{
    PPO_ColorBasePS::GetMenuData(outData, outData);

    //outData.covarArrayObjRef = covarArrayObjRefUI.GetFuncObjRef();

    outData.plotWithColorMap = plotWithColorMapUI.GetBoolChoice();
    if (outData.plotWithColorMap)
        outData.colorMapIsIndex = colorMapValueUI.GetBoolChoice();
    else
        outData.covarPen = fixedColorPenUI.GetPenSelection();

    outData.allParameters = allParamUI.GetBoolChoice();
    if (!outData.allParameters)
        outData.xDataIndx = xDataIndxUI.GetSelection();

    outData.cLimits = DC_Covar::ConfidenceLimits(cLimitsUI.GetChoice());

    outData.plotConfLimits = plotConfLimitsUI.GetValue();
    if (outData.plotConfLimits)
        outData.covarLineThk = covarLineThkUI.GetLineThick();

    outData.plotBestEstimate = plotBestEstimateUI.GetValue();
    if (outData.plotBestEstimate)
    {
        outData.bestEstSymbol.symbolType = pointTypeUI.GetSymbolType();
        outData.bestEstSymbol.symbolSize = pointSizeUI.GetSymbolSize();
    }

    outData.widthSpec = PFO_SingleConfLimits::WidthSpec(widthSpecUI.GetChoice());
    switch (outData.widthSpec) {
        case PFO_SingleConfLimits::wsSpecVal : {
            outData.widthSpecValue = widthSpecValueUI.GetFieldValue();
            break;
         }
        case PFO_SingleConfLimits::wsSpecPixel : {
            outData.widthSpecPixel = widthSpecPixelUI.GetSymbolSize();
            break;
         }
        case PFO_SingleConfLimits::wsPctAvail:{
            outData.widthSpecPct = widthSpecPctUI.GetFieldValue();
            break;
         }
    }

    legendOverrideUI.GetLegendOverride(outData.legendOverride);

}

