///////////////////////////////////////////////////////////////////////////////////
//
// PPO_CovarLimitsPS.cpp
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

#include "PPO_CovarLimitsPS.h"

// ****************************************

PPO_CovarLimitsPS::PPO_CovarLimitsPS() :
    covarArrayObjRefUI(typeid(DO_CovarArray)),
    covarDataOK(false)
{
}

void PPO_CovarLimitsPS::SetAllSensitive()
{
    xDataIndxUI.SetSensitive(covarDataOK);
    yDataIndxUI.SetSensitive(covarDataOK);

    if (is3D)
        zDataIndxUI.SetSensitive(covarDataOK && is3D);

    int currType    = covarPlotTypeUI.GetChoice();
    int currFormat = covarPlotFormatUI.GetChoice();

    // bit of a kluge: reset format if necessary
    if ((currType == 0) && (currFormat == 1))  // single, filled
    {
        covarPlotFormatUI.SetChoice(0);
        currFormat = 0;
    }

    covarPlotFormatUI.SetSensitive((currType > 0) || is3D);;

    ndualPtsUI.SetSensitive(currType == 1);
    covarLineThkUI.SetSensitive(currFormat  == 0);

    if (is3D)
    {
        ntripleSliceUI.SetSensitive(currType == 2);
        ntripleMeridianUI.SetSensitive(currType == 2);
        extrusionFrame.SetSensitive(currFormat  == 2);
    }

}

void PPO_CovarLimitsPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_CovarLimitsPS*>(inObj)->SetAllSensitive();
}


void PPO_CovarLimitsPS::SetCovarIndexCB(void* inObj)

{
  static_cast<PPO_CovarLimitsPS*>(inObj)->SetCovarIndex();
}

void PPO_CovarLimitsPS::SetCovarIndex()
{

    covarDataOK = covarArrayObjRefUI.DataObjAvail();
    DO_CovarArray* dataDO;
    if (covarDataOK)
    {
        dataDO = static_cast<DO_CovarArray*>(covarArrayObjRefUI.GetDataObj());
        covarDataOK = dataDO->covarHeader->Size() > 0;
    }

    if (covarDataOK)
    {
        SC_StringArray covarDesc;
        dataDO->covarHeader->GetIDs(covarDesc);
        xDataIndxUI.SetListText(covarDesc);
        yDataIndxUI.SetListText(covarDesc);
        if (is3D)
            zDataIndxUI.SetListText(covarDesc);
    }
    else
    {
        xDataIndxUI.SetListNA();
        yDataIndxUI.SetListNA();
        if (is3D)
            zDataIndxUI.SetListNA();
    }
    SetAllSensitive();
}


void PPO_CovarLimitsPS::InitMenu(const PPO_CovarLimits& inData)
{
    PPO_BasePS::InitMenu(inData);

    is3D = inData.is3DObject;

    ArgListC    args;
    args.StdFormAttach();
    topRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(66);
    covarArrayObjRefUI.topOffset = 0;
    covarArrayObjRefUI.InitFramedUI("Covariance Data", topRow.GetWidget(), args);
    covarArrayObjRefUI.AddOtherObjectCallback(SetCovarIndexCB, this);

    args.StdSplitFinish(34);
    covarPenUI.InitFramedUI("Pen", topRow.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRow.GetWidget());
    indexRow.InitWidget(PPform.GetWidget(), args);


    xDataIndxUI.topOffset = 0;
    xDataIndxUI.listWidth = 20;
    yDataIndxUI.topOffset = 0;
    yDataIndxUI.listWidth = 20;

    if (is3D)
    {
        args.StdSplitSetup(33);
        xDataIndxUI.InitFramedUI("X Variable", indexRow.GetWidget(), args);

        args.ResetPlace();
        args.LeftPosAttach(33);
        args.RightPosAttach(66);
        yDataIndxUI.InitFramedUI("Y Variable", indexRow.GetWidget(), args);

        args.StdSplitFinish(34);
        zDataIndxUI.topOffset = 0;
        zDataIndxUI.listWidth = 20;
        zDataIndxUI.InitFramedUI("Z Variable", indexRow.GetWidget(), args);
    }
    else
    {
        args.StdSplitSetup(50);
        xDataIndxUI.InitFramedUI("X Variable", indexRow.GetWidget(), args);

        args.StdSplitFinish(50);
        yDataIndxUI.InitFramedUI("Y Variable", indexRow.GetWidget(), args);
    }

    args.Clear();
    args.StdTopObjectAttach(indexRow.GetWidget());
    formatRow.InitWidget(PPform.GetWidget(), args);

    static const char* cLimitsStrings[] = {"99.0", "95.4", "90.0", "68.3"};
    args.StdSplitSetup(33);
    cLimitsUI.PreInitSetUp(cLimitsStrings, 4);
    cLimitsUI.InitFramedUI("Confidence Limits", formatRow.GetWidget(), args);


    static const char* covarPlotTypeStrings[] = {"Single", "Dual", "Triple"};
    int nlim = 2;
    if (is3D)
        nlim = 3;
    args.ResetPlace();
    args.LeftPosAttach(33);
    args.RightPosAttach(66);
    covarPlotTypeUI.PreInitSetUp(covarPlotTypeStrings, nlim);
    covarPlotTypeUI.InitFramedUI("Limit Type", formatRow.GetWidget(), args);
    covarPlotTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);


    static const char* covarPlotFormatStrings[] = {"Line", "Solid", "Tube"};
    args.StdSplitFinish(34);
    covarPlotFormatUI.PreInitSetUp(covarPlotFormatStrings, nlim);
    covarPlotFormatUI.InitFramedUI("Plot Format", formatRow.GetWidget(), args);
    covarPlotFormatUI.AddOtherObjectCallback(SetAllSensitiveCB, this);


    args.Clear();
    args.StdTopObjectAttach(formatRow.GetWidget());
    sizeRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    ndualPtsUI.InitFramedUI("Ellipse Pts", sizeRow.GetWidget(), args);

    args.ResetPlace();
    args.LeftPosAttach(33);
    args.RightPosAttach(66);
    if (!is3D)
    {
        covarLineThkUI.InitFramedUI("Line Thickness", sizeRow.GetWidget(), args);

        args.Clear();
        args.StdTopObjectAttach(sizeRow.GetWidget());
        legendOverrideUI.InitFramedUI("Legend Description", PPform.GetWidget(), args);
    }
    else
    {
        ntripleSliceUI.InitFramedUI("Ellipsoid Slices", sizeRow.GetWidget(), args);

        args.StdSplitFinish(34);
        ntripleMeridianUI.InitFramedUI("Ellipsoid Meridians", sizeRow.GetWidget(), args);

        args.Clear();
        args.StdTopObjectAttach(sizeRow.GetWidget());
        lineRow.InitWidget(PPform.GetWidget(), args);

        args.StdSplitSetup(33);
        covarLineThkUI.InitFramedUI("Line Thickness", lineRow.GetWidget(), args);

        args.StdSplitFinish(67);
        extrusionFrame.InitFramedUI("Tube Extrusion", lineRow.GetWidget(), args);

        args.Clear();
        args.TopLeftAttach(5, 10);
        capEndsUI.StdToggleInit("Cap Ends", extrusionFrame.GetForm(), args);

        args.StdSplitSetup(33);
        args.ResetPlace();
        args.LeftPosAttach(30);
        args.RightPosAttach(55);
        nextrusionPointsUI.PreInitSetUp();
        nextrusionPointsUI.InitWidget(extrusionFrame.GetForm(), args);

        args.StdSplitFinish(25);
        extrusionPixelSizeUI.InitWidget(extrusionFrame.GetForm(), args);

        args.Clear();
        args.StdTopObjectAttach(lineRow.GetWidget());
        legendOverrideUI.InitFramedUI("Legend Description", PPform.GetWidget(), args);

        args.Clear();
        args.StdTopObjectAttach(legendOverrideUI);
        offsetCoordUI.StdInit("Offset", PPform.GetWidget(), args);
    }

}


void PPO_CovarLimitsPS::MenuOpen(PPO_CovarLimits&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    covarArrayObjRefUI.StdMenuInit(inData.covarArrayObjRef);
    covarPenUI.ResetPens(inData.objPenSet);
}

void PPO_CovarLimitsPS::SetMenuData(PPO_CovarLimits&    inData)
{
    covarArrayObjRefUI.SetFuncObjRef(inData.covarArrayObjRef);
    cLimitsUI.SetChoice(inData.cLimits);

    xDataIndxUI.SetSelection(inData.xDataIndx);
    yDataIndxUI.SetSelection(inData.yDataIndx);

    covarPlotTypeUI.SetChoice(inData.covarPlotType);
    covarPlotFormatUI.SetChoice(inData.covarPlotFormat);
    covarPenUI.SetPenSelection(inData.covarPen);
    ndualPtsUI.SetFieldValue(inData.ndualPts);
    covarLineThkUI.SetLineThick(inData.covarLineThk);

    legendOverrideUI.SetLegendOverride(inData.legendOverride);
    if (is3D)
    {
        zDataIndxUI.SetSelection(inData.zDataIndx);
        ntripleSliceUI.SetFieldValue(inData.ntripleSlice);
        ntripleMeridianUI.SetFieldValue(inData.ntripleMeridian);

        capEndsUI.SetValue(inData.tubeData.capEnds);

        nextrusionPointsUI.SetTubePoints(inData.tubeData.nextrusionPoints);
        extrusionPixelSizeUI.SetSymbolSize(inData.tubeData.extrusionPixelSize);
    }

    SetCovarIndex();

    PPO_BasePS::SetMenuData(inData);
}

bool PPO_CovarLimitsPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    covarArrayObjRefUI.FuncObjRefOK(allOK, "covariance input");

    int currType   = covarPlotTypeUI.GetChoice();
    int currFormat = covarPlotFormatUI.GetChoice();

    if (currType == 1)
    {
        ndualPtsUI.FieldValueOK(allOK, "ellipse points", 20, 1000);

    }
    else if (is3D && (currType == 2))
    {
        ntripleSliceUI.FieldValueOK(allOK, "ellipsoid slices", 5, 100);
        ntripleMeridianUI.FieldValueOK(allOK, "ellipsoid meridians", 5, 100);
    }

    return allOK;
}

void PPO_CovarLimitsPS::GetMenuData(PPO_CovarLimits&  outData)
{
    PPO_BasePS::GetMenuData(outData);

    outData.covarArrayObjRef = covarArrayObjRefUI.GetFuncObjRef();
    outData.cLimits = DC_Covar::ConfidenceLimits(cLimitsUI.GetChoice());

    outData.xDataIndx = xDataIndxUI.GetSelection();
    outData.yDataIndx = yDataIndxUI.GetSelection();
    outData.zDataIndx = zDataIndxUI.GetSelection();

    outData.covarPlotType = PFO_CovarLimits::CovarPlotType(covarPlotTypeUI.GetChoice());
    outData.covarPlotFormat = PFO_CovarLimits::CovarPlotFormat(covarPlotFormatUI.GetChoice());
    outData.covarPen    = covarPenUI.GetPenSelection();

    if (outData.covarPlotType == PFO_CovarLimits::cptDual)
    {
        outData.ndualPts = ndualPtsUI.GetFieldValue();
    }
    else if (is3D& (outData.covarPlotType == PFO_CovarLimits::cptTriple))
    {
        outData.ntripleSlice = ntripleSliceUI.GetFieldValue();
        outData.ntripleMeridian = ntripleMeridianUI.GetFieldValue();
    }

    outData.covarLineThk = covarLineThkUI.GetLineThick();
    legendOverrideUI.GetLegendOverride(outData.legendOverride);

    if (is3D)
    {
        outData.tubeData.capEnds = capEndsUI.GetValue();
        outData.tubeData.nextrusionPoints = nextrusionPointsUI.GetTubePoints();
        outData.tubeData.extrusionPixelSize = extrusionPixelSizeUI.GetSymbolSize();
    }

}

