///////////////////////////////////////////////////////////////////////////////////
//
// PPD_BasePS.cpp
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
//      components common to all plots (window size)
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/DO_PenSet.h>
#include <genPlotClass/C_PlotDef.h>

#include "PPD_BasePS.h"


PPD_BasePS::PPD_BasePS() : penSource(typeid(DO_PenSet)) {}

// **************************************** call backs

void PPD_BasePS::ResetPen()
{
    if (penSource.CheckDataObj())
    {
        DO_PenSet* penDO = static_cast<DO_PenSet*>(penSource.GetDataObj());
        backgroundPen.ResetPens(penDO->penSetData);
    }
}


void PPD_BasePS::ResetPenCB(void* inObj)
{
    static_cast<PPD_BasePS*>(inObj)->ResetPen();
}

void PPD_BasePS::SetUVSize()
{
    static int uSize[] = {500, 600, 640, 800, 1000, 1000, 1024, 1152, 1280};
    static int vSize[] = {800, 800, 480, 600, 625, 750, 768, 864, 1024};

    int currSize = sizeSetButtons.GetChoice();
    plotUsizeUI.SetFieldValue(uSize[currSize]);
    plotVsizeUI.SetFieldValue(vSize[currSize]);
}

void PPD_BasePS::SetUVSizeCB(void* inObj)
{
    static_cast<PPD_BasePS*>(inObj)->SetUVSize();
}

// ****************************************
void PPD_BasePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

//  pen source combo
    args.StdSplitSetup(65);
    penSource.InitFramedUI("Plot Pen Set", URform.GetWidget(), args);
    penSource.AddOtherObjectCallback(ResetPenCB, this);

//  background pen
    args.StdSplitFinish(35);
    backgroundPen.InitFramedUI("Background Pen", URform.GetWidget(), args);

    //  dimension frame
    args.StdFormAttach();
    dimFrame.InitFramedUI("Plot Dimensions", PPform.GetWidget(), args);

//  dimension buttons
    static const char* dimLabels[]  = {"500x800", "600x800", "640x480", "800x600", "1000x625", "1000x750", "1024x768", "1152x864", " 1280x1024 "};
    sizeSetButtons.PreInitSetUp(dimLabels, 9);
    sizeSetButtons.rightOffset = 40;

    args.StdFormAttach();
    sizeSetButtons.InitFormUI(dimFrame.GetForm(), args);
    sizeSetButtons.AddOtherObjectCallback(SetUVSizeCB, this);
    //sizeForm.SetOffsets(0, 0, 25, 25);

    args.StdTopObjectAttach(sizeSetButtons, 10);
    sizeForm.InitFormUI(dimFrame.GetForm(), args);

    args.StdFormAttach();
    plotUsizeUI.SetFieldLen(5);
    plotUsizeUI.InitLabeledUI("Horizontal size", sizeForm.GetLeft(), args);
    plotVsizeUI.SetFieldLen(5);
    plotVsizeUI.InitLabeledUI("Vertical size", sizeForm.GetRight(), args);


//  form for margins & format
    args.StdTopObjectAttach(dimFrame);
    lastRowForm.InitWidget(PPform.GetWidget(), args);

//  margin form
    args.StdSplitFinish(55);
    marginFrame.InitFramedUI("Minimum Plot Margins (Pixels)", lastRowForm.GetWidget(), args);

    args.StdFormAttach();
    leftMarginUI.SetFieldLen(5);
    leftMarginUI.InitLabeledUI("Left", marginFrame.GetLeft(), args);

    args.StdTopObjectAttach(leftMarginUI);
    topMarginUI.SetFieldLen(5);
    topMarginUI.InitLabeledUI("Top", marginFrame.GetLeft(), args);

    args.StdFormAttach();
    rightMarginUI.SetFieldLen(5);
    rightMarginUI.InitLabeledUI("Right", marginFrame.GetRight(), args);

    args.StdTopObjectAttach(rightMarginUI);
    bottomMarginUI.SetFieldLen(5);
    bottomMarginUI.InitLabeledUI("Bottom", marginFrame.GetRight(), args);
}

void PPD_BasePS::MenuOpen(PPD_Base& inData)
{
    MenuBaseC::MenuOpen(true, false);

    penSource.StdMenuInit(inData.basePD.plotPenSetObjRef);
    ResetPen();

    dimFrame.SetSensitive(inData.basePD.IsMasterPlot());
}

void PPD_BasePS::SetMenuData(PPD_Base&  inData)
{
    const PlotDefC& basePD = inData.basePD;

    penSource.SetFuncObjRef(basePD.plotPenSetObjRef);
    ResetPen();

    backgroundPen.SetPenSelection(basePD.plotBackgroundPen);

    plotUsizeUI.SetFieldValue(basePD.plotUwidth);
    plotVsizeUI.SetFieldValue(basePD.plotVheight);

    leftMarginUI.SetFieldValue(basePD.leftMargin);
    rightMarginUI.SetFieldValue(basePD.rightMargin);
    topMarginUI.SetFieldValue(basePD.topMargin);
    bottomMarginUI.SetFieldValue(basePD.bottomMargin);

    MenuBaseC::SetMenuData(inData);

    dimFrame.SetSensitive(inData.basePD.IsMasterPlot());

}

bool PPD_BasePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    leftMarginUI.FieldValueOK(allOK, "margin", 0, 500);
    rightMarginUI.FieldValueOK(allOK, "margin", 0, 500);
    bottomMarginUI.FieldValueOK(allOK, "margin", 0, 500);
    topMarginUI.FieldValueOK(allOK, "margin", 0, 500);
    plotUsizeUI.FieldValueOK(allOK, "plot width", 100, 3000);
    plotVsizeUI.FieldValueOK(allOK, "plot height", 100, 3000);
    if ((leftMarginUI.GetFieldValue() + rightMarginUI.GetFieldValue()) > (plotUsizeUI.GetFieldValue() -100))
    {
        PropErrorMsg("R + L margins > width - 100");
        return false;
    }
    if ((topMarginUI.GetFieldValue() + bottomMarginUI.GetFieldValue()) > (plotVsizeUI.GetFieldValue() -100))
    {
        PropErrorMsg("T + B margins > height - 100");
        return false;
    }
    return allOK;
}

void PPD_BasePS::GetMenuData(PPD_Base& outData)
{
   MenuBaseC::GetMenuData(outData);

   PlotDefC& basePD = outData.basePD;
   basePD.plotPenSetObjRef    = penSource.GetFuncObjRef();
   basePD.plotBackgroundPen   = backgroundPen.GetPenSelection();

   basePD.plotUwidth          = plotUsizeUI.GetFieldValue();
   basePD.plotVheight         = plotVsizeUI.GetFieldValue();

   basePD.leftMargin   = leftMarginUI.GetFieldValue();
   basePD.rightMargin  = rightMarginUI.GetFieldValue();
   basePD.topMargin    = topMarginUI.GetFieldValue();
   basePD.bottomMargin = bottomMarginUI.GetFieldValue();
}

void  PPD_BasePS::MenuDefault()
{
    plotUsizeUI.SetFieldValue(640);
    plotVsizeUI.SetFieldValue(480);
}


