///////////////////////////////////////////////////////////////////////////////////
//
// PFO_2DXYDataGL.cpp
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
//    Implements code to draw objPFO functional objects on a plot.
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genClass/U_Str.h>

#include <genPlotClassPS/PC_ReportPS.h>

#include <genPFOGL/GL_Symbol.h>

#include <objPFOGL/PFO_2DXYDataGL.h>


PFO_2DXYDataGL :: PFO_2DXYDataGL(PlotDefC& assPlt) :
                            PFO_2DXYData(assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
PFO_2DXYDataGL::PFO_2DXYDataGL(const PFO_2DXYDataGL& a, PlotDefC& assPlt) :
                            PFO_2DXYData(a, assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
void  PFO_2DXYDataGL :: InitCommon(PlotDefC& assPlt)
{
    cursorReport = new PC_ReportPS(*(dynamic_cast <PD_2D*> (&assPlt)));
    secondReport = new PC_ReportPS(*(dynamic_cast <PD_2D*> (&assPlt)));
    reportOK     = true;
}


PFO_2DXYDataGL::~PFO_2DXYDataGL()
{
    delete secondReport;
}

PFO_2DXYDataGL& PFO_2DXYDataGL::operator= (const PFO_2DXYDataGL& a)
{
    if (&a != this)
        PFO_2DXYData::FullCopy(a);
    return *this;
}


void  PFO_2DXYDataGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    cursorTrackData.Init();
    GL_Symbol currSeries;

    SC_IntArray dataIndexes;
    int indxStart = 0;

    for ( int i = 0; i < maxXYSeries; i++)
        if (xySeries[i].OKtoPlot())
        {
            const DC_XYData& currXY = *xySeries[i].xyDataDC;
            currSeries.DrawSeriesSpec(xySeries[i], currXY.xData, currXY.yData);

            if (cursorReport->showReport)
            {
                dataIndexes.AllocAndSetSize(currXY.xData.Size());
                for (int j = 0; j < dataIndexes.Size(); j++)
                    dataIndexes[j] = indxStart++;
                cursorTrackData.AddSeriesDataToTrack(currXY.xData, currXY.yData, dataIndexes);
            }
        }

    CloseDrawObject();
}

void PFO_2DXYDataGL::ReportCursorData(const Coord3D& cursorPos)
{
    if ((!doPlot) || (!cursorReport->showReport) || (objStatus != os_OK))
    {
        //  unmanage if set up
        SetReportTitles();
        return;
    }

    int dataIndx;
    if (!cursorTrackData.GetClosestIndex(cursorPos.cX, cursorPos.cY, dataIndx))
        return;

    if (!cursorReport->reportInitialized)
    {
        cursorReport->ReportInit("X:");
        secondReport->ReportInit("Indx:");
        SetReportTitles();

        cursorReport->ReportLabelSetup(0, "X:");
        cursorReport->ReportLabelSetup(1, "Y:");

        secondReport->ReportLabelSetup(0, "Indx");
        secondReport->ReportLabelSetup(1, "ID");
    }

    const SC_RealFormat& xForm = static_cast<PD_2D*>(assocPlot)->GetXFormat();
    const SC_RealFormat& yForm = static_cast<PD_2D*>(assocPlot)->GetYFormat();

    char intStr[40];
    char realStr[40];

    for (int i = 0; i < maxXYSeries; i++)
        if (xySeries[i].OKtoPlot())
        {
            const DC_XYData& currXY = *xySeries[i].xyDataDC;
            if (dataIndx < currXY.Size())
            {
                xForm.RealToString(currXY.xData[dataIndx], realStr, 40);
                cursorReport->ReportUpdate(0, realStr);

                IntToString(dataIndx + 1, intStr, 40);  // RR change to 1 based indexes
                secondReport->ReportUpdate(0, intStr);

                yForm.RealToString(currXY.yData[dataIndx], realStr, 40);
                cursorReport->ReportUpdate(1, realStr);

                secondReport->ReportUpdate(1, currXY.GetID());
                return;
            }
            dataIndx -= currXY.Size();
        }

}


