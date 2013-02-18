///////////////////////////////////////////////////////////////////////////////////
//
// PFO_VariableArrayGridLinesGL.cpp
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

#include <genPFOGL/GL_Line.h>

#include <genPFOGL/PC_GridLine.h>

#include <objPFOGL/PFO_VariableArrayGridLinesGL.h>


PFO_VariableArrayGridLinesGL :: PFO_VariableArrayGridLinesGL(PlotDefC& assPlt) :
                            PFO_VariableArrayGridLines(assPlt),
                            OGLBaseObj(this)
{
}
PFO_VariableArrayGridLinesGL::PFO_VariableArrayGridLinesGL(const PFO_VariableArrayGridLinesGL& a, PlotDefC& assPlt) :
                            PFO_VariableArrayGridLines(a, assPlt),
                            OGLBaseObj(this)
{
}


PFO_VariableArrayGridLinesGL::~PFO_VariableArrayGridLinesGL()
{
}

PFO_VariableArrayGridLinesGL& PFO_VariableArrayGridLinesGL::operator= (const PFO_VariableArrayGridLinesGL& a)
{
    if (&a != this)
        PFO_VariableArrayGridLines::operator=(a);

    return *this;
}


void  PFO_VariableArrayGridLinesGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    Limit3D currLimits = GetCurrentViewLimits();
    OGL2DBase& currPlot = static_cast<OGL2DBase&>(*objectBase);

    PC_GridLine currGrid(currPlot, arrayIsX);

    GL_Line::SetLineProperties(gridLineSpec);


    for (int i = 0; i < variableArrayDC->Size(); i++)
    {
        double varVal = (*variableArrayDC)[i].GetValue();
        if (!currGrid.CreateGridLine(varVal, currLimits))
            continue;

        currGrid.SetLabelPos(labelFormat.labelPosition / 100.0);
        currGrid.AdjustForAxes();

        char label[80];
        bool doLabel;
        switch (labelFormat.labelType) {
            case PSC_GridLineLabelFormat::ltNone: {
                doLabel = false;
                break;
            }
            case PSC_GridLineLabelFormat::ltText : {
                CopyString(label, (*variableArrayDC)[i].GetID(), 80);
                doLabel = IsNotNull(label);
                break;
            }
            case PSC_GridLineLabelFormat::ltValue : {
                valueFormat.RealToString(varVal, label, 80);
                doLabel = true;
                break;
            }
        }

        bool drawFullLine = true;
        if (doLabel)
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

    }

    CloseDrawObject();
}
