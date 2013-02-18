///////////////////////////////////////////////////////////////////////////////////
//
// PPO_CovarLimMatrixPS.cpp
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

#include "PPO_CovarLimMatrixPS.h"

// ****************************************

PPO_CovarLimMatrixPS::PPO_CovarLimMatrixPS() :
    PPO_ColorBasePS(typeid(DO_CovarArray))
{
}

void PPO_CovarLimMatrixPS::SetAllSensitive()
{
    bool plotEll = plotEllipseUI.GetValue();
    cLimitsUI.SetSensitive(plotEll);
    covarEllipseTypeUI.SetSensitive(plotEll);
    ndualPtsUI.SetSensitive(plotEll);
    covarLineThkUI.SetSensitive(plotEll
                    && (covarEllipseTypeUI.GetChoice() == PFO_CovarLimMatrix::cetLine));
    addOptionsFrame.SetSensitive(plotEll);

    pointFrame.SetSensitive(plotBestEstUI.GetValue());

    bool useCM = plotWithColorMapUI.GetBoolChoice();
    covarPenUI.SetSensitive(!useCM);
    colorAttributeUI.SetSensitive(useCM);
    limitAndMapForm.SetSensitive(useCM);
    outsideForm.SetSensitive(useCM);

    if (useCM)
    {
        PPO_ColorBasePS::SetAllSensitive();
        PPO_ColorBasePS::limitSelectionMS.SetSensitive(useCM);
    }

    plotAreaLimFrame.SetSensitive(plotAreaLimUI.GetValue());
    badCovXFrame.SetSensitive(plotBadCovUI.GetValue());

}

void PPO_CovarLimMatrixPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_CovarLimMatrixPS*>(inObj)->SetAllSensitive();
}

void PPO_CovarLimMatrixPS::InitMenu(const PPO_CovarLimMatrix& inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    topRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(75);
    inputDataObjRefUI.topOffset = 0;
    inputDataObjRefUI.InitFramedUI("Covariance Data", topRow.GetWidget(), args);

    args.StdSplitFinish(25);
    static const char* mpoStrings[] = {"Full", "Upper Only", "Lower Only",
                                        "Upper Diagonal", "Lower Diagonal", "Diagonal"};
    matrixPlotOptionUI.PreInitSetUp(mpoStrings, 6);
    matrixPlotOptionUI.InitFramedUI("Matrix Plot Options", topRow.GetWidget(), args);

    args.StdTopObjectAttach(topRow.GetWidget());
    plotOptionsFrame.InitFramedUI("Plot Options", PPform.GetWidget(), args);

    args.StdSplitSetup(25);
    args.LeftOffset(15);
    plotEllipseUI.StdToggleInit("ellipse", plotOptionsFrame.GetForm(), args);
    plotEllipseUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(25, 50);
    args.LeftOffset(15);
    plotBestEstUI.StdToggleInit("best estimate", plotOptionsFrame.GetForm(), args);
    plotBestEstUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(50, 75);
    args.LeftOffset(15);
    plotBadCovUI.StdToggleInit("bad covariance x", plotOptionsFrame.GetForm(), args);
    plotBadCovUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(25);
    args.LeftOffset(15);
    plotAreaLimUI.StdToggleInit("0-1 area limits", plotOptionsFrame.GetForm(), args);
    plotAreaLimUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(plotOptionsFrame.GetForm());
    formatRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    static const char* cLimitsStrings[] = {"99.0", "95.4", "90.0", "68.3"};
    cLimitsUI.PreInitSetUp(cLimitsStrings, 4);
    cLimitsUI.InitFramedUI("Confidence Limits", formatRow.GetWidget(), args);

    args.StdSplitMiddle(33, 67);
    static const char* ellTypeStr[] = {"Line", "Solid"};
    covarEllipseTypeUI.PreInitSetUp(ellTypeStr, 2);
    covarEllipseTypeUI.InitFramedUI("Ellipse Type", formatRow.GetWidget(), args);
    covarEllipseTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(33);
    covarLineThkUI.InitFramedUI("Line Thickness", formatRow.GetWidget(), args);

    args.StdTopObjectAttach(formatRow.GetWidget());
    nextRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    ndualPtsUI.InitFramedUI("Ellipse Pts", nextRow.GetWidget(), args);

    args.StdSplitMiddle(33, 67);
    addOptionsFrame.InitFramedUI("Additional Options", nextRow.GetWidget(), args);

    args.StdFormAttach();
    args.LeftOffset(15);
    centerEllipseUI.StdToggleInit("center ellipse", addOptionsFrame.GetForm(), args);

    args.StdSplitFinish(33);
    pointFrame.InitFramedUI("Best Estimate Symbol", nextRow.GetWidget(), args);

    args.StdSplitMiddle(10, 40);
    pointTypeUI.InitWidget(pointFrame.GetForm(), args);

    args.StdSplitMiddle(60, 90);
    pointSizeUI.InitWidget(pointFrame.GetForm(), args);

    args.StdTopObjectAttach(nextRow.GetWidget());
    colorForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(30);
    plotWithColorMapUI.rightOffset = 50;
    plotWithColorMapUI.PreInitSetUp("Pen", "Color Map", false);
    plotWithColorMapUI.InitFramedUI("Ellipse/Best Estimate Color", colorForm.GetWidget(), args);
    plotWithColorMapUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(30, 60);
    covarPenUI.InitFramedUI("Single Color Pen", colorForm.GetWidget(), args);

    args.StdSplitFinish(40);
    static const char* colorAttStrings[] = {"Index", "Main Diagonal", "Off Diagonal", "SSE"};
    colorAttributeUI.PreInitSetUp(colorAttStrings, 4);
    colorAttributeUI.InitFramedUI("Color Map Value", colorForm.GetWidget(), args);

    args.StdTopObjectAttach(colorForm.GetWidget());
    const char* viewStrings[] = {"All data"};
    PPO_ColorBasePS::InitMenu(inData, args, viewStrings, 1);

    args.StdTopObjectAttach(outsideForm.GetWidget());
    plotForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    badCovXFrame.InitFramedUI("Bad Covariance X", plotForm.GetWidget(), args);

    args.StdSplitMiddle(5, 50);
    badCovXPenUI.InitWidget(badCovXFrame.GetForm(), args);

    args.StdSplitMiddle(60, 90);
    badCovXThkUI.InitWidget(badCovXFrame.GetForm(), args);

    args.StdSplitMiddle(33, 67);
    plotAreaLimFrame.InitFramedUI("0-1 Area Limits", plotForm.GetWidget(), args);

    args.StdSplitMiddle(5, 50);
    plotAreaLimPenUI.InitWidget(plotAreaLimFrame.GetForm(), args);

    args.StdSplitMiddle(60, 90);
    plotAreaLimThkUI.InitWidget(plotAreaLimFrame.GetForm(), args);

    args.StdSplitFinish(33);
    areaPctUI.InitFramedUI("Plot Area Percentage", plotForm.GetWidget(), args);

    args.StdTopObjectAttach(plotForm.GetWidget());
    legendOverrideUI.InitFramedUI("Legend Description", PPform.GetWidget(), args);
}


void PPO_CovarLimMatrixPS::MenuOpen(PPO_CovarLimMatrix&   inData)
{
    PPO_ColorBasePS::MenuOpen(inData);
    covarPenUI.ResetPens(inData.objPenSet);
    plotAreaLimPenUI.ResetPens(inData.objPenSet);
    badCovXPenUI.ResetPens(inData.objPenSet);
}

void PPO_CovarLimMatrixPS::SetMenuData(PPO_CovarLimMatrix&    inData)
{
    PPO_ColorBasePS::SetMenuData(inData, inData);

    cLimitsUI.SetChoice(inData.cLimits);

    plotEllipseUI.SetValue(inData.plotEllipse);
    plotBestEstUI.SetValue(inData.plotBestEst);
    plotBadCovUI.SetValue(inData.plotBadCov);
    plotAreaLimUI.SetValue(inData.plotAreaLim);

    matrixPlotOptionUI.SetChoice(int(inData.matrixPlotOption));

    covarEllipseTypeUI.SetChoice(int(inData.covarEllipseType));
    ndualPtsUI.SetFieldValue(inData.ndualPts);
    covarLineThkUI.SetLineThick(inData.covarLineThk);

    centerEllipseUI.SetValue(inData.centerEllipse);

    pointTypeUI.SetSymbolType(inData.bestEstSymbol.symbolType);
    pointSizeUI.SetSymbolSize(inData.bestEstSymbol.symbolSize);

    plotWithColorMapUI.SetBoolChoice(inData.plotWithColorMap);
    covarPenUI.SetPenSelection(inData.covarPen);

    colorAttributeUI.SetChoice(int(inData.colorAttribute));

    badCovXPenUI.SetPenSelection(inData.badCovXPen);
    badCovXThkUI.SetLineThick(inData.badCovXThk);

    plotAreaLimPenUI.SetPenSelection(inData.plotAreaLimPen);
    plotAreaLimThkUI.SetLineThick(inData.plotAreaLimThk);

    areaPctUI.SetFieldValue(inData.areaPct);

    legendOverrideUI.SetLegendOverride(inData.legendOverride);

    SetAllSensitive();
}

bool PPO_CovarLimMatrixPS::UIdataOK()
{
    PPO_ColorBasePS::UIdataOK();

    ndualPtsUI.FieldValueOK(allOK, "ellipse points", 20, 1000);

    areaPctUI.FieldValueOK(allOK, "area %", 0.0, 100.0);

    return allOK;
}

void PPO_CovarLimMatrixPS::GetMenuData(PPO_CovarLimMatrix&  outData)
{
    PPO_ColorBasePS::GetMenuData(outData, outData);

    outData.cLimits = DC_Covar::ConfidenceLimits(cLimitsUI.GetChoice());

    outData.plotEllipse = plotEllipseUI.GetValue();
    outData.plotBestEst = plotBestEstUI.GetValue();
    outData.plotBadCov = plotBadCovUI.GetValue();
    outData.plotAreaLim = plotAreaLimUI.GetValue();

    outData.matrixPlotOption = PFO_CovarLimMatrix::MatrixPlotOption(matrixPlotOptionUI.GetChoice());

    if (outData.plotEllipse)
    {
        outData.covarEllipseType = PFO_CovarLimMatrix::CovarEllipseType(covarEllipseTypeUI.GetChoice());
        outData.ndualPts = ndualPtsUI.GetFieldValue();
        if (outData.covarEllipseType == PFO_CovarLimMatrix::cetLine)
            outData.covarLineThk = covarLineThkUI.GetLineThick();
        outData.centerEllipse = centerEllipseUI.GetValue();
    }

    if (outData.plotBestEst)
    {
        outData.bestEstSymbol.symbolType = pointTypeUI.GetSymbolType();
        outData.bestEstSymbol.symbolSize = pointSizeUI.GetSymbolSize();
    }

    outData.plotWithColorMap = plotWithColorMapUI.GetBoolChoice();
    if (outData.plotWithColorMap)
        outData.colorAttribute = PFO_CovarLimMatrix::ColorAttribute(colorAttributeUI.GetChoice());
    else
        outData.covarPen = covarPenUI.GetPenSelection();

    if (outData.plotBadCov)
    {
        outData.badCovXPen = badCovXPenUI.GetPenSelection();
        outData.badCovXThk = badCovXThkUI.GetLineThick();
    }

    if (outData.plotAreaLim)
    {
        outData.plotAreaLimPen = plotAreaLimPenUI.GetPenSelection();
        outData.plotAreaLimThk = plotAreaLimThkUI.GetLineThick();
    }

    outData.areaPct = areaPctUI.GetFieldValue();

    legendOverrideUI.GetLegendOverride(outData.legendOverride);

}

