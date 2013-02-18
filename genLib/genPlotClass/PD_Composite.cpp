///////////////////////////////////////////////////////////////////////////////////
//
// PD_Composite.cpp
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
//      defines basic composite plot - includes list of sub-plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotObj.h>
#include <genPlotClass/C_AnnoObj.h>

#include <genPlotClass/PD_Composite.h>

PSC_PlotLayout::PSC_PlotLayout()
{
    xOffset = 0.0;
    xWidth  = 100.0;
    yOffset = 0.0;
    yWidth = 100.0;
}

PSC_PlotLayout::PSC_PlotLayout(const PSC_PlotLayout& a)
{
    LocalCopy(a);
}

PSC_PlotLayout::~PSC_PlotLayout()
{
}

PSC_PlotLayout& PSC_PlotLayout::operator=(const PSC_PlotLayout& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void PSC_PlotLayout::LocalCopy(const PSC_PlotLayout& a)
{
    xOffset = a.xOffset;
    yOffset = a.yOffset;
    xWidth  = a.xWidth;
    yWidth  = a.yWidth;
}

bool PSC_PlotLayout::LayoutOK()
{
    bool xOK = (xOffset >= 0.0) && (xWidth >= 5.0) && ((xWidth + xOffset) <= 100.0);
    bool yOK = (yOffset >= 0.0) && (yWidth >= 5.0) && ((yWidth + yOffset) <= 100.0);
    return xOK && yOK;
}

void PSC_PlotLayout::SetSubplotLocation(const PD_Composite& masterPlot,
                                                 PlotDefC&     subPlot)
{
    int xAvail = masterPlot.plotUwidth - masterPlot.leftMargin - masterPlot.rightMargin;
    if (xAvail < 50)
        xAvail = masterPlot.plotUwidth;

    int yAvail = masterPlot.plotVheight - masterPlot.topMargin - masterPlot.bottomMargin;
    if (yAvail < 50)
        yAvail = masterPlot.plotVheight;

    double xStart = double(xAvail) * xOffset / 100.0;
    double xEnd = xStart + double(xAvail) * xWidth / 100.0;

    double yStart = double(yAvail) * yOffset / 100.0;
    double yEnd = yStart + double(yAvail) * yWidth / 100.0;

    uMin = masterPlot.leftMargin + int(xStart);
    uMax = masterPlot.leftMargin + int(xEnd);

    vMin = masterPlot.bottomMargin + int(yStart);
    vMax = masterPlot.bottomMargin + int(yEnd);

    subPlot.plotUOffset = uMin;
    subPlot.plotUwidth  = uMax - uMin;
    subPlot.plotVOffset = vMin;
    subPlot.plotVheight = vMax - vMin;
    subPlot.AdjustMargins();

    // reset vMin/vMax to be relative from top
    int tmp = vMax;
    vMax = masterPlot.plotVheight - vMin;
    vMin = masterPlot.plotVheight - tmp;
}

bool PSC_PlotLayout::WithinSubPlot(int uChk, int vChk)
{
    return (uChk >= uMin) && (uChk <= uMax) && (vChk >= vMin) && (vChk <= vMax);
};

bool PSC_PlotLayout::GetSublotUV(int& uChk,
                                 int& vChk)
{
    if (WithinSubPlot(uChk, vChk))
    {
        uChk -= uMin;
        vChk -= vMin;
        return true;
    }

    return false;
}

PD_Composite :: PD_Composite() : PlotDefC("Composite")
{
    subPlots.SetResizable(4);

    autoLayout  = true;
    nlayoutRows = 2;
    nlayoutCols = 2;

    InitCommon();
    SetDefault();
}

PD_Composite :: PD_Composite(const PD_Composite& a) : PlotDefC(a)
{
    InitCommon();
    LocalCopy(a);
}

PD_Composite :: ~PD_Composite()
{
}

void PD_Composite::InitCommon()
{
    rowSpacing.AllocAndSetSize(maxLayoutRowCol);
    colSpacing.AllocAndSetSize(maxLayoutRowCol);

    useCustomLayout.AllocAndSetSize(maxCustomLayout);
    customLayout.AllocAndSetSize(maxCustomLayout);
}

PD_Composite& PD_Composite::operator= (const PD_Composite& a)
{
    if (&a != this)
    {
        PlotDefC::operator =(a);
        LocalCopy(a);
    }
    return *this;
}


void  PD_Composite::LocalCopy(const PD_Composite& a)
{
    subPlots = a.subPlots;
    pageLayout = a.pageLayout;

    nlayoutRows = a.nlayoutRows;
    nlayoutCols = a.nlayoutCols;
    rowSpacing  = a.rowSpacing;
    colSpacing  = a.colSpacing;

    useCustomLayout = a.useCustomLayout;
    customLayout    = a.customLayout;
}


void PD_Composite::SetDefault()
{
    PlotDefC::SetDefault();

    rightMargin         = 50;
    leftMargin          = 50;
    topMargin           = 50;
    bottomMargin        = 50;

    autoLayout          = true;
    showLayout          = false;
    layoutPen           = 9;

    useCustomLayout.FillToAlloc(false);
    useCustomLayout[0] = true;
    customLayout.FillToAlloc(PSC_PlotLayout());

    rowSpacing.FillToAlloc(floor(100.0 / double(nlayoutRows)));
    colSpacing.FillToAlloc(floor(100.0 / double(nlayoutCols)));

    lastCursorPlot = -1;
    lastPlotSelected = -1;
    annoSelected = false;
}


void PD_Composite::AddSubplot(PlotDefC* newSubPlot)
{
    subPlots.AddTo(newSubPlot);
    newSubPlot->masterPlot = this;
}

void PD_Composite::RemoveSubplot(PlotDefC* currentSubPlot)
{
    subPlots.DeleteFrom(currentSubPlot);
    currentSubPlot->ResetToMaster();
}


void PD_Composite::DrawPlotObjects()
{
    for (int i = 0; i < nplotsToDisplay; i++)
    {
        PlotDefC& nextSubPlot = *(subPlots[i]);
        pageLayout[i].SetSubplotLocation(*this, nextSubPlot);

        if (nextSubPlot.autoUpdateRequired)
        {
            nextSubPlot.autoUpdateRequired = false;
            nextSubPlot.SetAxesAutoLimits();
            nextSubPlot.SetViewFromAxes();
            nextSubPlot.ResetView();
        }

        nextSubPlot.SetRubberbandOKLimits();
        nextSubPlot.SetupDataView();
        nextSubPlot.DrawAllObjects();
    }

}

void PD_Composite::ResetView()
{
    PlotDefC::ResetView();
}

void PD_Composite::ViewTop()
{
    if (lastPlotSelected >= 0)
        subPlots.GetRef(lastPlotSelected).ViewTop();
}

void PD_Composite::ViewPrev()
{
    if (lastPlotSelected >= 0)
        subPlots.GetRef(lastPlotSelected).ViewPrev();
}

PC_ViewOpRec PD_Composite::GetPlotViewOps()
{
    PC_ViewOpRec SP2D;
    SP2D.xOp = vo_na;
    SP2D.yOp = vo_na;
    SP2D.zOp = vo_na;
    return SP2D;
}

void PD_Composite::SetViewFromAxes()
{
    if (lastPlotSelected < 0)
    {
        Limit3D initialView;
        initialView.minLim.cX = 0.0;
        initialView.maxLim.cX = 100.0;
        initialView.minLim.cY = 0.0;
        initialView.maxLim.cY = 100.0;
        initialView.minLim.cZ = nullReal;
        initialView.maxLim.cZ = nullReal;

        // put it on stack
        plotViews.InitStack(initialView);
    }
    else
        subPlots.GetRef(lastPlotSelected).SetViewFromAxes();

}


void PD_Composite::SetAxesFromView()
{
    if (lastPlotSelected >= 0)
        subPlots.GetRef(lastPlotSelected).SetAxesFromView();
}

void PD_Composite::RestoreSetAxes()
{
    if (lastPlotSelected >= 0)
        subPlots.GetRef(lastPlotSelected).RestoreSetAxes();
}


void PD_Composite::SetAxesAutoLimits()
{
    if (lastPlotSelected >= 0)
        subPlots.GetRef(lastPlotSelected).SetAxesAutoLimits();
}

Coord3D  PD_Composite::GetNewViewCenter()
{
    return Coord3D(0.5);
}


bool PD_Composite::SetAxesFromExtents()
{
    if (lastPlotSelected >= 0)
        return subPlots.GetRef(lastPlotSelected).SetAxesFromExtents();

    return false;
}


void PD_Composite::DoStatusChk()
{
    PlotDefC::DoStatusChk();

    if (autoLayout)
    {
        double totalRows = 0.0;
        for (int i = 0; i < nlayoutRows; i++)
        {
            totalRows += rowSpacing[i];
            if (rowSpacing[i] < 5.0)
                SetObjErrMsg("each row must be greater than 5%");
        }
        if ((totalRows < 5.0) || (totalRows > 100.0))
            SetObjErrMsg("total rows must be greater >= 5% and <= 100.0");

        double totalCols = 0.0;
        for (int i = 0; i < nlayoutCols; i++)
        {
            totalCols += colSpacing[i];
            if (colSpacing[i] < 5.0)
                SetObjErrMsg("each colmust be greater than 5%");
        }
        if ((totalCols < 5.0) || (totalCols > 100.0))
            SetObjErrMsg("total cols must be greater >= 5% and <= 100.0");

        pageLayout.AllocAndSetSize(nlayoutRows * nlayoutCols);
        int nextIndex = 0;
        double ybottom = 100.0;
        for (int i = 0; i < nlayoutRows; i++)
        {
            double dy = rowSpacing[i];
            ybottom -= dy;
            double xleft = 0.0;
            for (int j = 0; j < nlayoutCols; j++)
            {
                PSC_PlotLayout& currLayout = pageLayout[nextIndex];
                double dx = colSpacing[j];
                currLayout.xOffset = xleft;
                currLayout.yOffset = ybottom;
                currLayout.xWidth = dx;
                currLayout.yWidth = dy;

                xleft += dx;
                nextIndex++;
            }
        }
    }
    else
    {
        pageLayout.Alloc(maxCustomLayout);
        for (int i = 0; i < maxCustomLayout ; i++)
            if (useCustomLayout[i])
            {
                if (!customLayout[i].LayoutOK())
                    SetObjErrMsg("bad fre-form layout definition");
                pageLayout += customLayout[i];
            }

        if (pageLayout.IsEmpty())
            SetObjErrMsg("no fre-form page layouts defined");

        if (StatusNotOK())
            return;
    }

    nplotsToDisplay = subPlots.Size();
    // anything okay if no plots
    if (nplotsToDisplay == 0)
        return;

    int nlayout = pageLayout.Size();
    for (int i = 0; i < nplotsToDisplay; i++)
    {
        if (i < nlayout)
            subPlots.GetRef(i).masterPlot = this;
//      else
//          subPlots.GetRef(i).ResetToMaster();
    }

    if (nlayout < nplotsToDisplay)
        nplotsToDisplay = nlayout;

    for (int i = 0; i < nplotsToDisplay; i++)
        if (!pageLayout[i].LayoutOK())
        {
            SetObjErrMsg("bad page layout definition");
            return;
        }
}



void PD_Composite::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    lastPlotSelected = -1;

    PlotDefC::CalcOutput(calcType);

    for (int i = 0; i < nplotsToDisplay; i++)
        pageLayout[i].SetSubplotLocation(*this, subPlots.GetRef(i));

}


//  ********************************************

//  cursor movement processing -- virtual from PlotDefC
void  PD_Composite::ProcessCursorMove(int xpos,
                                      int ypos)
{
    int prevLast = lastCursorPlot;
    if (prevLast >= nplotsToDisplay)
    {
        prevLast = -1;
        lastPlotSelected = -1;
    }

    lastCursorPlot = -1;
    for (int i = 0; i < nplotsToDisplay; i++)
    {
        if (pageLayout[i].GetSublotUV(xpos, ypos))
        {
            PlotDefC& currSub = subPlots.GetRef(i);
            if (prevLast < 0)
            {
                currSub.PointerEnter(xpos, ypos);
            }
            else if (prevLast != i)
            {
                subPlots.GetRef(prevLast).PointerLeave();
                currSub.PointerEnter(xpos, ypos);
            }

            currSub.ProcessCursorMove(xpos, ypos);
            lastCursorPlot = i;
            break;
        }
    }

    if ((lastCursorPlot < 0) && (prevLast >= 0))
        subPlots.GetRef(prevLast).PointerLeave();
}



bool PD_Composite::GetSubplotUV(int& xpos, int& ypos)
{
    return (lastCursorPlot >= 0) && pageLayout[lastCursorPlot].GetSublotUV(xpos, ypos);
}


void  PD_Composite::PointerSelect(int   xpos,
                                  int   ypos,
                                  bool  shifted)
{
    // comp plot anno takes priority for select
    annoSelected = false;
    lastPlotSelected = -1;

    for (int i = 0; i < annoObjList.Size(); i++)
    {
        AnnoObjC& currObj = annoObjList.GetRef(i);
        if (currObj.doPlot)
        {
            PC_ObjLimitRect& currRect = currObj.rectLimits;
            if (currRect.NotNull() && currRect.InRect(xpos, ypos))
            {
                annoSelected = true;
                break;
            }
        }
    }

    if ((!annoSelected) && GetSubplotUV(xpos, ypos))
    {
        subPlots.GetRef(lastCursorPlot).PointerSelect(xpos, ypos, shifted);
        lastPlotSelected = lastCursorPlot;
        axesHaveBeenSet = subPlots.GetRef(lastCursorPlot).axesHaveBeenSet;
    }
    else
        PlotDefC::PointerSelect(xpos, ypos, shifted);
}

void PD_Composite::PointerDrag(int     xpos,
                               int     ypos,
                               bool    shifted)
{
    if ((!annoSelected) && GetSubplotUV(xpos, ypos))
        subPlots.GetRef(lastCursorPlot).PointerDrag(xpos, ypos, shifted);
    else
        PlotDefC::PointerDrag(xpos, ypos, shifted);
}

void PD_Composite::PointerRelease(int  xpos,
                                  int  ypos,
                                   bool shifted)
{
    if ((!annoSelected) && GetSubplotUV(xpos, ypos))
    {
        subPlots.GetRef(lastCursorPlot).PointerRelease(xpos, ypos, shifted);
        lastPlotSelected = lastCursorPlot;
        axesHaveBeenSet = subPlots.GetRef(lastCursorPlot).axesHaveBeenSet;
    }
    else
        PlotDefC::PointerRelease(xpos, ypos, shifted);
    annoSelected = false;
}

PC_CallbackMenu* PD_Composite::GetCallbackMenu()
{
    if (lastCursorPlot < 0)
        return PlotDefC::GetCallbackMenu();

    return subPlots.GetRef(lastCursorPlot).GetCallbackMenu();
}

bool PD_Composite::ProcessRubberbandRegion(const PC_ObjLimitRect& rubberbandRegion)
{
    int dx, dy;
    rubberbandRegion.GetWidthHeight(dx, dy);
    return false;
}


// ********************************************** key processing

void PD_Composite::ProcessSelectionKeys(SelectionKeys inKey)
{
    if (lastCursorPlot >= 0)
        subPlots.GetRef(lastCursorPlot).ProcessSelectionKeys(inKey);
}

// update subplots after file read
void PD_Composite::UpdateFuncObjRefs()
{
    // base class for pen
    FuncObjC::UpdateFuncObjRefs();

    // all subplots
    FuncObjRef plotRef;
    for (int i = 0; i < subPlots.Size(); i++)
    {
        plotRef.objRef = subPlots[i];
        PerformUpdate(plotRef);
        subPlots[i] = static_cast<PlotDefC*>(plotRef.objRef);
    }
}


bool PD_Composite::UsesAsInput(const FuncObjC* inputObj) const
{
    if (FuncObjC::UsesAsInput(inputObj))
        return true;

    for (int i = 0; i < subPlots.Size(); i++)
        if (inputObj == subPlots[i])
            return true;

    return false;
}

