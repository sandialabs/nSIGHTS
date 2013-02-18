///////////////////////////////////////////////////////////////////////////////////
//
// PFO_2DMultTablesGL.cpp
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
#include <genClass/SC_RealFormat.h>
#include <genPlotClass/PD_2D.h>

#include <genPFOGL/GL_Symbol.h>

#include <genPlotClassPS/PC_ReportPS.h>

#include <objPFOGL/PFO_2DMultTablesGL.h>


PFO_2DMultTablesGL :: PFO_2DMultTablesGL(PlotDefC& assPlt) :
                            PFO_2DMultTables(assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
PFO_2DMultTablesGL::PFO_2DMultTablesGL(const PFO_2DMultTablesGL& a, PlotDefC& assPlt) :
                            PFO_2DMultTables(a, assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
void  PFO_2DMultTablesGL :: InitCommon(PlotDefC& assPlt)
{
    cursorReport = new PC_ReportPS(*(dynamic_cast <PD_2D*> (&assPlt)));
    reportOK     = true;
}


PFO_2DMultTablesGL::~PFO_2DMultTablesGL()
{
}

PFO_2DMultTablesGL& PFO_2DMultTablesGL::operator= (const PFO_2DMultTablesGL& a)
{
    if (&a != this)
        PFO_2DMultTables::FullCopy(a);
    return *this;
}


void  PFO_2DMultTablesGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    cursorTrackData.Init();
    GL_Symbol currSeriesGL;

    SC_IntArray dataIndexes;
    int indxStart = 0;

    for ( int i = 0; i < maxTableSeries; i++)
    {
        TableSeries& currSeries = tableSeries[i];

        if (currSeries.OKtoPlot())
        {
            const SC_DoubleMatrix& currData = currSeries.tableDataDC->dataTable;
            int maxCol = currSeries.tableDataDC->GetNCols();
            if ((currSeries.xColumn < maxCol) && (currSeries.yColumn < maxCol))
            {
                SC_DoubleArray& xData = currData[currSeries.xColumn];
                SC_DoubleArray& yData = currData[currSeries.yColumn];

                currSeriesGL.DrawSeriesSpec(tableSeries[i], xData, yData);

                if (cursorReport->showReport)
                {
                    dataIndexes.AllocAndSetSize(xData.Size());
                    for (int j = 0; j < dataIndexes.Size(); j++)
                        dataIndexes[j] = indxStart++;
                    cursorTrackData.AddSeriesDataToTrack(xData, yData, dataIndexes);
                }
            }
        }
    }

    CloseDrawObject();
}

void PFO_2DMultTablesGL::ReportCursorData(const Coord3D& cursorPos)
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
        cursorReport->ReportInit("#/X:");
        cursorReport->ReportTitleSetup(GetID());
        cursorReport->ReportLabelSetup(0, "#/X:");
        cursorReport->ReportLabelSetup(1, "#/Y:");
    }

    const SC_RealFormat& xForm = static_cast<PD_2D*>(assocPlot)->GetXFormat();
    const SC_RealFormat& yForm = static_cast<PD_2D*>(assocPlot)->GetYFormat();

    char intStr[40];
    char realStr[40];
    char outStr[80];
    for (int i = 0; i < maxTableSeries; i++)
        if (tableSeries[i].OKtoPlot())
        {
            const SC_DoubleMatrix& currData = tableSeries[i].tableDataDC->dataTable;
            const SC_DoubleArray& xData = currData[tableSeries[i].xColumn];
            if (dataIndx < xData.Size())
            {
                IntToString(dataIndx, intStr, 40);
                xForm.RealToString(xData[dataIndx], realStr, 40);
                MakeString(outStr, intStr, "/", realStr, 80);
                cursorReport->ReportUpdate(0, outStr);

                IntToString(i, intStr, 40);
                yForm.RealToString(currData[tableSeries[i].yColumn][dataIndx], realStr, 40);
                MakeString(outStr, intStr, "/", realStr, 80);
                cursorReport->ReportUpdate(1, outStr);
                return;
            }
            dataIndx -= xData.Size();
        }

}

