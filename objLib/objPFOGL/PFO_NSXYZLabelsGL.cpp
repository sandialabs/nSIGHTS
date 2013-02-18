///////////////////////////////////////////////////////////////////////////////////
//
// PFO_NSXYZLabelsGL.cpp
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
#include <genClass/U_Real.h>

#include <genPFOGL/GL_Base.h>
#include <genPFOGL/GL_3DText.h>

#include <objPFOGL/PFO_NSXYZLabelsGL.h>

PFO_NSXYZLabelsGL :: PFO_NSXYZLabelsGL(PlotDefC& assPlt) :
                    PFO_NSXYZLabels(assPlt),
                    OGLBaseObj(this)

{
}
PFO_NSXYZLabelsGL::PFO_NSXYZLabelsGL(const PFO_NSXYZLabelsGL& a, PlotDefC& assPlt) :
                    PFO_NSXYZLabels(a, assPlt),
                    OGLBaseObj(this)
{
}
PFO_NSXYZLabelsGL::~PFO_NSXYZLabelsGL()
{
}

PFO_NSXYZLabelsGL& PFO_NSXYZLabelsGL::operator= (const PFO_NSXYZLabelsGL& a)
{
    if (&a != this)
        PFO_NSXYZLabels::FullCopy(a);
    return *this;
}

void  PFO_NSXYZLabelsGL :: DrawPlotObject()
{
    if (!InitDrawObject())
        return;

    const PC_View& currView = objectBase->GetCurrentView();

    bool labelsAreVertical = (labelAttitude == laVertical);
    if (labelAttitude == laAuto)
        labelsAreVertical = (fabs(currView.elevation) < 45.0);

    double rotAng = rotationAngle;
    if (autoLabelRotation)
        rotAng = -currView.azimuth;

    GL_3DText textObj(labelFont);
    textObj.SetAlign(horizontalPos, verticalPos);
    textObj.SetPlaneRot(labelsAreVertical, rotAng);
    textObj.SetColor(labelPen);

    if (!clipToAxes)
        textObj.ClearClipPlanes();

    SC_CoordArray& coords = labelsObj->dataCoords;
    SC_StringArray& labels = labelsObj->dataLabels;

    Coord3D pointCoord;
    for (int i = 0; i < labels.Size(); i++ )
        if (!coords[i].CoordIsNull())
        {
            pointCoord = coords[i];
            pointCoord += offsetCoord;
            textObj.DrawText(pointCoord, labels[i]);
        }

    if (!clipToAxes)
        textObj.SetClipPlanes();

    CloseDrawObject();

}

