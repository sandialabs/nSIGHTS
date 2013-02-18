///////////////////////////////////////////////////////////////////////////////////
//
// PFO_2DXYDataArrayGL.cpp
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

#include <objPFOGL/PFO_2DXYDataArrayGL.h>


PFO_2DXYDataArrayGL :: PFO_2DXYDataArrayGL(PlotDefC& assPlt) :
                            PFO_2DXYDataArray(assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
PFO_2DXYDataArrayGL::PFO_2DXYDataArrayGL(const PFO_2DXYDataArrayGL& a, PlotDefC& assPlt) :
                            PFO_2DXYDataArray(a, assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
void  PFO_2DXYDataArrayGL :: InitCommon(PlotDefC& assPlt)
{
    if (!is3DObject)
    {
        cursorReport = new PC_ReportPS(*(dynamic_cast <PD_2D*> (&assPlt)));
        reportOK     = true;
    }
}


PFO_2DXYDataArrayGL::~PFO_2DXYDataArrayGL()
{
}

PFO_2DXYDataArrayGL& PFO_2DXYDataArrayGL::operator= (const PFO_2DXYDataArrayGL& a)
{
    if (&a != this)
        PFO_2DXYDataArray::FullCopy(a);
    return *this;
}

void  PFO_2DXYDataArrayGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    int maxArray = xyDataArrayDC->Size();

    GL_Symbol currSeries;
    cursorTrackData.Init();
    SC_IntArray dataIndexes;
    int indxStart = 0;
    for ( int i = 0; i < maxXYSeries; i++)
        if (seriesData[i].PlotSeries(maxArray))
        {
            int serIndx = seriesData[i].xyArrayIndx;
            const SC_DoubleArray& xData = (*xyDataArrayDC)[serIndx].xData;
            const SC_DoubleArray& yData = (*xyDataArrayDC)[serIndx].yData;
            currSeries.DrawSeriesSpec(seriesData[i], xData, yData);

            if (cursorReport->showReport)
            {
                dataIndexes.AllocAndSetSize(xData.Size());
                for (int j = 0; j < dataIndexes.Size(); j++)
                    dataIndexes[j] = indxStart++;
                cursorTrackData.AddSeriesDataToTrack(xData, yData, dataIndexes);
            }
        }

    CloseDrawObject();
}

void PFO_2DXYDataArrayGL::ReportCursorData(const Coord3D& cursorPos)
{
    if ((! doPlot) || (!cursorReport->showReport) || (objStatus != os_OK))
    {
        //  unmanage if set up
        cursorReport->ReportTitleSetup(GetID());
        return;
    }


    int dataIndx;
    if (!cursorTrackData.GetClosestIndex(cursorPos.cX, cursorPos.cY, dataIndx))
        return;

    if (!cursorReport->reportInitialized)
    {
        cursorReport->ReportInit("Desc:");
        cursorReport->ReportTitleSetup(GetID());
        cursorReport->ReportLabelSetup(0, "X:");
        cursorReport->ReportLabelSetup(1, "Y:");
    }

    const SC_RealFormat& yForm = static_cast<PD_2D*>(assocPlot)->GetYFormat();
    const SC_RealFormat& xForm = static_cast<PD_2D*>(assocPlot)->GetXFormat();

    int maxArray = xyDataArrayDC->Size();
    for ( int i = 0; i < maxXYSeries; i++)
        if (seriesData[i].PlotSeries(maxArray))
        {
            const DC_XYData& currXY = (*xyDataArrayDC)[seriesData[i].xyArrayIndx];

            if (dataIndx < currXY.Size())
            {
                char tempStr[40];
                xForm.RealToString(currXY.xData[dataIndx], tempStr, 40);
                cursorReport->ReportUpdate(0, tempStr);

                yForm.RealToString(currXY.yData[dataIndx], tempStr, 40);
                cursorReport->ReportUpdate(1, tempStr);
                return;
            }
            dataIndx -= currXY.Size();
        }
}

