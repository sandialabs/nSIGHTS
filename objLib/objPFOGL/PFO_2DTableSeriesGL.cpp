///////////////////////////////////////////////////////////////////////////////////
//
// PFO_2DTableSeriesGL.cpp
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
#include <genPFOGL/GL_Polygon.h>

#include <objPFOGL/PFO_2DTableSeriesGL.h>


PFO_2DTableSeriesGL :: PFO_2DTableSeriesGL(PlotDefC& assPlt) :
                            PFO_2DTableSeries(assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
PFO_2DTableSeriesGL::PFO_2DTableSeriesGL(const PFO_2DTableSeriesGL& a, PlotDefC& assPlt) :
                            PFO_2DTableSeries(a, assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
void  PFO_2DTableSeriesGL :: InitCommon(PlotDefC& assPlt)
{
    if (!is3DObject)
    {
        cursorReport = new PC_ReportPS(*(dynamic_cast <PD_2D*> (&assPlt)));
        reportOK     = true;
    }
}


PFO_2DTableSeriesGL::~PFO_2DTableSeriesGL()
{
}

PFO_2DTableSeriesGL& PFO_2DTableSeriesGL::operator= (const PFO_2DTableSeriesGL& a)
{
    if (&a != this)
        PFO_2DTableSeries::FullCopy(a);
    return *this;
}

void  PFO_2DTableSeriesGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    int maxCol = objTableData->GetNCols();
    if (xDataIndx >= maxCol)
        return;

    GL_Symbol currSeries;
    cursorTrackData.Init();
    SC_IntArray dataIndexes;
    int indxStart = 0;
    int lasti = -1;
    for ( int i = 0; i < maxTableSeries; i++)
        if (seriesData[i].doSeries && (seriesData[i].tableColIndx < maxCol))
        {
            SC_DoubleArray& xData = objTableData->dataTable[xDataIndx];
            SC_DoubleArray& yData = objTableData->dataTable[seriesData[i].tableColIndx];
            currSeries.DrawSeriesSpec(seriesData[i], xData, yData, startRow);

            if (shadeBetween)
            {
                if (lasti >= 0)
                {
                    GL_Polygon  rectObject;
                    rectObject.SetPolygonFilled(0.0);
                    GL_Base::SetColor(seriesData[i].seriesPen);

                    SC_DoubleArray& yLastData = objTableData->dataTable[seriesData[lasti].tableColIndx];
                    for (int j = 0; j < (xData.Size() - 1); j++)
                    {
                        //shade between lines, even if previous line is not there
                        double yLastDataJ = yLastData[j];
                        double yLastDataJ1 = yLastData[j+1];
                        if (RealIsNull(yLastDataJ))
                        {
                            for (int k = lasti; k >= 0; k--)
                            {
                                if (!seriesData[k].doSeries)
                                    continue;
                                yLastDataJ = objTableData->dataTable[seriesData[k].tableColIndx][j];
                                if (!RealIsNull(yLastDataJ))
                                    break;
                            }
                        }
                        if (RealIsNull(yLastDataJ1))
                        {
                            for (int k = lasti; k >= 0; k--)
                            {
                                if (!seriesData[k].doSeries)
                                    continue;
                                yLastDataJ1 = objTableData->dataTable[seriesData[k].tableColIndx][j+1];
                                if (!RealIsNull(yLastDataJ1))
                                    break;
                            }
                        }
#ifdef _GLPRHARDCOPY_
                        if (DoingHardCopy())
                        {
                            glpBegin(GLP_3D_COLOR, 4);
                            objectBase->SetHardCopyParameters();
                        }
#endif

                        SC_PointArray rectVertexes(4);
                        rectVertexes.SetEmpty();
                        rectVertexes += Point2D(xData[j], yLastDataJ);
                        rectVertexes += Point2D(xData[j+1], yLastDataJ1);
                        rectVertexes += Point2D(xData[j+1], yData[j+1]);
                        rectVertexes += Point2D(xData[j], yData[j]);

                        rectObject.DrawPolygon(rectVertexes);

                        GL_Base::HardCopyBlockEnd();

                    }
                }
                lasti = i;
            }

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

void PFO_2DTableSeriesGL::ReportCursorData(const Coord3D& cursorPos)
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

    for ( int i = 0; i < maxTableSeries; i++)
        if (seriesData[i].doSeries)
        {
            if (dataIndx < objTableData->dataTable[xDataIndx].Size())
            {
                char tempStr[40];
                if (!objTableData->rowDesc.IsEmpty())
                    CopyString(tempStr, objTableData->rowDesc[dataIndx], 40);
                else
                    IntToString(dataIndx, tempStr, 5, 40);

                cursorReport->ReportUpdate(0, tempStr);

                yForm.RealToString(objTableData->dataTable[seriesData[i].tableColIndx][dataIndx], tempStr, 40);
                cursorReport->ReportUpdate(1, tempStr);
                return;
            }
            dataIndx -= objTableData->dataTable[xDataIndx].Size();
        }
}

