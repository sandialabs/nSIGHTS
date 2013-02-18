///////////////////////////////////////////////////////////////////////////////////
//
// PD_2D.cpp
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
//      defines basic 2D plot 
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <genPlotClass/C_PlotObj.h>

#include <genPlotClass/PD_2D.h>


PD_2D :: PD_2D(const char* dType) : PlotDefC(dType)
{
    InitCommon();
    SetDefault();
}

PD_2D :: PD_2D(const PD_2D& a) : PlotDefC(a)
{
    InitCommon();
    LocalCopy(a);
}

PD_2D :: ~PD_2D()
{
//  delete cursorReport;
}

void PD_2D::InitCommon()
{
    cursorReport = 0;
    cursorIsInsidePlot = false;
}

PD_2D& PD_2D::operator= (const PD_2D& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}

void  PD_2D::FullCopy(const PD_2D& a)
{
    PlotDefC::FullCopy(a);
    LocalCopy(a);
}

void  PD_2D::LocalCopy(const PD_2D& a)
{
    showReportArea      = a.showReportArea;
    plotAnnotation      = a.plotAnnotation;
}


void PD_2D::SetDefault()
{
    PlotDefC::SetDefault();

    rightMargin         = 50;
    leftMargin          = 75;
    topMargin           = 50;
    bottomMargin        = 50;
    showReportArea      = true;
}


void PD_2D::DrawPlotObjects()
{
    for (int i = 0; i < maxPlotLayers; i++ )
    {
        int j;
        // first solids
        for (j = 0; j < plotObjList.Size(); j++)
        {
            PlotObjC& currObj = plotObjList.GetRef(j);
            if ((currObj.plotLayer == i) && currObj.doPlot && (!currObj.ContainsLinesOnly()))
                currObj.DrawPlotObject();
        }

        StartLineSection();
        // now liness
        for (j = 0; j < plotObjList.Size(); j++)
        {
            PlotObjC& currObj = plotObjList.GetRef(j);
            if ((currObj.plotLayer == i) && currObj.doPlot && (currObj.ContainsLinesOnly()))
                currObj.DrawPlotObject();
        }
        EndLineSection();
    }

}

void PD_2D::CalcOutput(FOcalcType  calcType)
{
    PlotDefC::CalcOutput(calcType);
    if (objStatus != os_OK)
        return;

    //  init cursor report
    if (!cursorReport->reportInitialized)
    {
        cursorReport->ReportInit("X:");
        cursorReport->ReportLabelSetup(0, "X:");
        cursorReport->ReportLabelSetup(1, "Y:");
        cursorReport->showReport = true;
        cursorReport->ReportTitleSetup("Cursor Position");
    }
}


//  ********************************************

//  cursor movement processing -- virtual from PlotDefC
void  PD_2D::ProcessCursorMove(int xpos,
                               int ypos)
{
/*
    char xStr[50];
    char yStr[50];
    IntToString(xpos, xStr, 50);
    IntToString(ypos, yStr, 50);
    cursorReport->ReportUpdate(0, xStr);
    cursorReport->ReportUpdate(1, yStr);
 */

    // convert to user
    Point2D userPt;
    if (!PixelXYtoUserXY(xpos, ypos, userPt))
        return;

    Limit2D currLim = GetCurrentViewLimits();

    //  in plot space
    cursorIsInsidePlot = currLim.WithinLimits(userPt);
    char xStr[50];
    char yStr[50];
    if (!cursorIsInsidePlot)
    {
        CopyString(xStr, " ", 50);
        CopyString(yStr, " ", 50);
    }
    else
    {
        xFormat.RealToString(userPt.pX, xStr, 20);
        yFormat.RealToString(userPt.pY, yStr, 20);
    }
    cursorReport->ReportUpdate(0, xStr);
    cursorReport->ReportUpdate(1, yStr);

    cursorCoord = userPt;

    if (cursorCoord.CoordIsNull() || (! cursorIsInsidePlot))
        return;

    Coord3D processedCoord = cursorCoord;
    TransformCursor(processedCoord);

    for (int i = 0; i < plotObjList.Size(); i++)
        plotObjList[i]->ReportCursorData(processedCoord);
}

bool PD_2D::ProcessRubberbandRegion(const PC_ObjLimitRect& rubberbandRegion)
{
    int dx, dy;
    rubberbandRegion.GetWidthHeight(dx, dy);

    //  skip tiny zooms
    if ((dx  < 3) || (dy <  3))
        return false;

    Point2D stAxes, endAxes;

    if (PixelXYtoUserXY(rubberbandRegion.ulXpos, rubberbandRegion.lrYpos, stAxes) &&
        PixelXYtoUserXY(rubberbandRegion.lrXpos, rubberbandRegion.ulYpos, endAxes))
    {
        PC_View newView;
        newView.viewLimits.minLim = Coord3D(stAxes);
        newView.viewLimits.maxLim = Coord3D(endAxes);
        ChangeView(newView);
        return true;
    }

    return false;
}


// ********************************************** key processing

void PD_2D::ProcessSelectionKeys(SelectionKeys inKey)
{
    if (cursorCoord.CoordIsNull() || (! cursorIsInsidePlot))
        return;

    Coord3D processedCoord = cursorCoord;
    TransformCursor(processedCoord);
    for (int i = 0; i < plotObjList.Size(); i++)
        plotObjList[i]->ProcessSelection(processedCoord, inKey);
}

