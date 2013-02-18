///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TableColorSeriesGL.cpp
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

#include <genPFOGL/GL_Symbol.h>
#include <genPlotClassPS/PC_ReportPS.h>
#include <genClass/U_Str.h>

#include <objPFOGL/PFO_TableColorSeriesGL.h>


PFO_TableColorSeriesGL :: PFO_TableColorSeriesGL(PlotDefC& assPlt) :
                            PFO_TableColorSeries(assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
PFO_TableColorSeriesGL::PFO_TableColorSeriesGL(const PFO_TableColorSeriesGL& a, PlotDefC& assPlt) :
                            PFO_TableColorSeries(a, assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}


PFO_TableColorSeriesGL::~PFO_TableColorSeriesGL()
{
}

void  PFO_TableColorSeriesGL::InitCommon(PlotDefC& assPlt)
{
    if (!is3DObject)
    {
        cursorReport = new PC_ReportPS(*(dynamic_cast <PD_2D*> (&assPlt)));
        reportOK     = true;
    }
    else
    {
        reportOK      = false;
        usePolyOffset = true;
    }
}

PFO_TableColorSeriesGL& PFO_TableColorSeriesGL::operator= (const PFO_TableColorSeriesGL& a)
{
    if (&a != this)
        PFO_TableColorSeries::FullCopy(a);
    return *this;
}


void  PFO_TableColorSeriesGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    int maxCol = objTableData->GetNCols();
    bool isLessThanMaxCol = (xDataIndx < maxCol) && (yDataIndx < maxCol);
    if (is3DObject)
        isLessThanMaxCol = isLessThanMaxCol && (zDataIndx < maxCol);
    if (isLessThanMaxCol)
    {
        const SC_DoubleArray& xData = objTableData->dataTable[xDataIndx];
        const SC_DoubleArray& yData = objTableData->dataTable[yDataIndx];
        const SC_DoubleArray& zData = objTableData->dataTable[zDataIndx];

        // this can happen in wierd startup/paint cases
        // DoStatusChk called but CalcOutput not called
        // temporary kluge until a correct general approach is done
        // TODO - make general
        if (symbolColors.Size() != objTableData->dataTable[colorDataIndx].Size())
            CalcDataColors();

        nrows = xData.Size();

        SC_CoordArray xyzData(nrows);
        Coord3D pointCoord;

        GL_Symbol currSeries;
        currSeries.SetSymScale(pointSymSize);

        if (!is3DObject)
            cursorTrackData.Init();
        SC_IntArray dataIndexes;
        int indxStart = 0;

        for (int i = 0; i < colorMap->ncolorMap; i++)
        {
            int colorIndex = i;
            if (reversePlotOrder)
                colorIndex = colorMap->ncolorMap - i - 1;
            //  reset polygon
            xyzData.SetEmpty();
            //  add points for color
            for (int j = startRow; j < nrows; j++ )
            {
                if (symbolColors[j] != colorIndex)
                    continue;

                if (!is3DObject)
                    MapCoord(pointCoord, xData[j], yData[j], nullReal);
                else
                    MapCoord(pointCoord, xData[j], yData[j], zData[j]);

                xyzData += pointCoord;
            }

            if (!xyzData.IsEmpty())
                currSeries.DrawSymbols(xyzData, edgeSpec, plotLayer, polyOffset, colorMap->GetColor(colorIndex), pointSymbol);

            if ((!is3DObject) && (cursorReport->showReport))
            {
                dataIndexes.AllocAndSetSize(nrows);
                for (int j = 0; j < dataIndexes.Size(); j++)
                    dataIndexes[j] = indxStart++;
                cursorTrackData.AddSeriesDataToTrack(xData, yData, dataIndexes);
            }
        }
    }

    CloseDrawObject();

}

void PFO_TableColorSeriesGL::ReportCursorData(const Coord3D& cursorPos)
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
        cursorReport->ReportLabelSetup(0, "Desc: ");
        cursorReport->ReportLabelSetup(1, "Val:");
    }

    const SC_RealFormat& yForm = static_cast<PD_2D*>(assocPlot)->GetYFormat();

    for (int j = 0; j < nrows; j++ )
    {
        if (dataIndx < nrows)
        {
            char tempStr[40];
            IntToString(dataIndx, tempStr, 5, 40);

            const char* reportID = tempStr;
            if (objTableData->rowDesc.Size() > dataIndx)
                reportID = objTableData->rowDesc[dataIndx];

            cursorReport->ReportUpdate(0, reportID);

            yForm.RealToString(objTableData->dataTable[colorDataIndx][dataIndx], tempStr, 40);
            cursorReport->ReportUpdate(1, tempStr);
            return;
        }
            dataIndx -= nrows;
    }
}

