///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TableHistogramGL.cpp
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

#include <genPFOGL/GL_Histogram.h>

#include <objPFOGL/PFO_TableHistogramGL.h>


PFO_TableHistogramGL :: PFO_TableHistogramGL(PlotDefC& assPlt) :
                            PFO_TableHistogram(assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
PFO_TableHistogramGL::PFO_TableHistogramGL(const PFO_TableHistogramGL& a, PlotDefC& assPlt) :
                            PFO_TableHistogram(a, assPlt),
                            OGLBaseObj(this)
{
    InitCommon(assPlt);
}
void  PFO_TableHistogramGL :: InitCommon(PlotDefC& assPlt)
{
    reportOK     = false;
}


PFO_TableHistogramGL::~PFO_TableHistogramGL()
{
}

PFO_TableHistogramGL& PFO_TableHistogramGL::operator= (const PFO_TableHistogramGL& a)
{
    if (&a != this)
        PFO_TableHistogram::operator=(a);
    return *this;
}

void  PFO_TableHistogramGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    int tempX = yDataIndx;
    int tempY = xDataIndx;
    if (positionIsValue)
    {
        tempX = xDataIndx;
        tempY = yDataIndx;
    }

    const SC_DoubleArray& yData = tableDataDC->dataTable[tempY];
    const SC_DoubleArray& xData = tableDataDC->dataTable[tempX];

    const SC_DoubleArray* hxData = &yData;
    if (orientationX)
        hxData = &xData;

    if (IsPolygon() && DoingHardCopy())
    {
        int glpNum = yData.Size() * 4;
        if (plotEdges)
            glpNum *= 2;
#ifdef _GLPRHARDCOPY_
        glpBegin(GLP_3D_COLOR, glpNum);
#endif //_GLPRHARDCOPY
        objectBase->SetHardCopyParameters();
    }

    if (IsLine())
        GL_Base::StdHardCopyBlockStart(yData.Size() * 2);

    GL_Histogram currHist;
    currHist.SetHistogramProperties(*this, *hxData, 1, plotLayer);

    for (int i = 0; i < yData.Size(); i++)
    {
        Point2D plotPt(xData[i], yData[i]);
        if (!positionIsValue)
            if (orientationX)
                plotPt.pX = double(i + 1) + indexOffset;
            else
                plotPt.pY = double(i + 1) + indexOffset;

        currHist.Draw2DHistogram(plotPt, histogramPen);
    }

    GL_Base::HardCopyBlockEnd();

    CloseDrawObject();
}

