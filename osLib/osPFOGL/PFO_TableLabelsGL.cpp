///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TableLabelsGL.cpp
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
//      Implements code to draw osPFO functional objects on a plot.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genClass/U_Real.h>

#include <genPFOGL/PC_Label.h>
#include <genPFOGL/GL_Base.h>

#include <osPFOGL/PFO_TableLabelsGL.h>


PFO_TableLabelsGL :: PFO_TableLabelsGL(PlotDefC& assPlt) :
                            PFO_TableLabels(assPlt),
                            OGLBaseObj(this)
{
}
PFO_TableLabelsGL::PFO_TableLabelsGL(const PFO_TableLabelsGL& a, PlotDefC& assPlt) :
                            PFO_TableLabels(a, assPlt),
                            OGLBaseObj(this)
{
}


PFO_TableLabelsGL::~PFO_TableLabelsGL()
{
}

PFO_TableLabelsGL& PFO_TableLabelsGL::operator= (const PFO_TableLabelsGL& a)
{
    if (&a != this)
        PFO_TableLabels::FullCopy(a);
    return *this;
}

void  PFO_TableLabelsGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    Coord3D labCoord;

    GL_Base::SetColor(labelPen);

    int nCols = tableLabelsDC->GetNCols();
    int nRows = tableLabelsDC->GetNRows();

    OGL2DBase& currPlot = static_cast<OGL2DBase&>(*objectBase);
    PC_Label lab2D(currPlot);

    for (int i = 0; i < nCols; i++)
    {
        for (int j = 0; j < nRows; j++)
        {
            if (((i == j) && !IncDiag())
             || ((i != j) && (matrixPlotOption == mpoDiag)))
                continue;

            if ((i > j) && !IncLower())
                continue;

            if ((i < j) && !IncUpper())
                continue;

            labCoord.cX = i;
            labCoord.cY = j;
            labCoord.cZ = 0.0;

            lab2D.labelPos = labCoord;
            if (!currPlot.TransformForXY(lab2D.labelPos))
                continue;

            if (pixelOffset != 0)
                lab2D.OffsetLabelPos(labelFont, pixelOffset, horizontalPos, verticalPos);

            double value = tableLabelsDC->dataTable[i][j];
            if (RealIsNull(value))
                continue;

            char label[80];
            convFormat.RealToString(tableLabelsDC->dataTable[i][j], label, 80);

            if (clipToAxes)
                lab2D.PrintClippedLabel(labelFont, label, horizontalPos, verticalPos);
            else
                lab2D.PrintUnclippedLabel(labelFont, label, horizontalPos, verticalPos);

        }
    }

    CloseDrawObject();
}

