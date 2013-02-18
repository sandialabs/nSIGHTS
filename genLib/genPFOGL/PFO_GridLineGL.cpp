///////////////////////////////////////////////////////////////////////////////////
//
// PFO_GridLineGL.cpp
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
//      Implements renderer dependent code to actually draw genPFO functional
// objects on a plot.  Is platform independent but
// relies on platform dependent code in genPlotClassPS.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include <genPFOGL/GL_Line.h>
#include <genPFOGL/PC_GridLine.h>

#include <genPFOGL/PFO_GridLineGL.h>


PFO_GridLineGL :: PFO_GridLineGL(PlotDefC& assPlt) :
                            PFO_GridLine(assPlt),
                            OGLBaseObj(this)
{
}
PFO_GridLineGL::PFO_GridLineGL(const PFO_GridLineGL& a, PlotDefC& assPlt) :
                            PFO_GridLine(a, assPlt),
                            OGLBaseObj(this)
{
}


PFO_GridLineGL::~PFO_GridLineGL()
{
}

PFO_GridLineGL& PFO_GridLineGL::operator= (const PFO_GridLineGL& a)
{
    if (&a != this)
        PFO_GridLine::operator=(a);

    return *this;
}


void  PFO_GridLineGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    Limit3D currLimits = GetCurrentViewLimits();
    OGL2DBase& currPlot = static_cast<OGL2DBase&>(*objectBase);

    PC_GridLine currGrid(currPlot, gridIsX);

    if (!currGrid.CreateGridLine(gVal, currLimits))
        return;

    currGrid.SetLabelPos(labelFormat.labelPosition / 100.0);

    currGrid.AdjustForAxes();

    GL_Base::SetColor(gridLinePen);
    GL_Line::SetLine(gridLineThickness);
    GL_Line::SetLineType(gridLineType);


    char label[maxLabelLen];
    bool doLabel;
    switch (labelFormat.labelType) {
        case PSC_GridLineLabelFormat::ltNone: {
            doLabel = false;
            break;
        }
        case PSC_GridLineLabelFormat::ltText : {
            CopyString(label, gridLabel, maxLabelLen);
            doLabel = IsNotNull(label);
            break;
        }
        case PSC_GridLineLabelFormat::ltValue : {
            valueFormat.RealToString(gVal, label, maxLabelLen);
            doLabel = true;
            break;
        }
    }


    bool drawFullLine = true;
    if (doLabel )
    {
        currGrid.OffsetLabelPos(labelFont, labelFormat.labelOffset, labelFormat.halign, labelFormat.valign);

        if (labelFormat.blankLabel)
        {
            drawFullLine = false;
            currGrid.DrawBlankedLine(labelFont, label, labelFormat.halign, labelFormat.valign);
        }

        currGrid.PrintUnclippedLabel(labelFont, label, labelFormat.halign, labelFormat.valign);
    }

    if (drawFullLine)
        currGrid.DrawGridLine();

    CloseDrawObject();
}

