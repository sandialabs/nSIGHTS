///////////////////////////////////////////////////////////////////////////////////
//
// PFO_3DTableSeriesGL.cpp
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

#include <objPFOGL/PFO_3DTableSeriesGL.h>


PFO_3DTableSeriesGL :: PFO_3DTableSeriesGL(PlotDefC& assPlt) :
                            PFO_3DTableSeries(assPlt),
                            OGLBaseObj(this)
{
}
PFO_3DTableSeriesGL::PFO_3DTableSeriesGL(const PFO_3DTableSeriesGL& a, PlotDefC& assPlt) :
                            PFO_3DTableSeries(a, assPlt),
                            OGLBaseObj(this)
{
}


PFO_3DTableSeriesGL::~PFO_3DTableSeriesGL()
{
}

PFO_3DTableSeriesGL& PFO_3DTableSeriesGL::operator= (const PFO_3DTableSeriesGL& a)
{
    if (&a != this)
        PFO_3DTableSeries::FullCopy(a);
    return *this;
}


void  PFO_3DTableSeriesGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    int maxCol = objTableData->GetNCols();
    if ((xDataIndx < maxCol) && (yDataIndx < maxCol) && (zDataIndx < maxCol))
    {
        SC_DoubleArray& xData = objTableData->dataTable[xDataIndx];
        SC_DoubleArray& yData = objTableData->dataTable[yDataIndx];
        SC_DoubleArray& zData = objTableData->dataTable[zDataIndx];

        int nrows = xData.Size() - startRow;
        SC_CoordArray xyzData(nrows);
        for (int i = startRow; i < xData.Size(); i++)
            xyzData[i - startRow] = Coord3D(xData[i], yData[i], zData[i]);

        GL_Symbol currSeries;
        currSeries.DrawSeriesSpec(xyzSeries, xyzData, polyOffset, 0);
    }

    CloseDrawObject();
}

