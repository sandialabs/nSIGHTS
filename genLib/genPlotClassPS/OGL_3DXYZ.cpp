///////////////////////////////////////////////////////////////////////////////////
//
// OGL_3DXYZ.cpp
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
//      adds coordinate transforms for log axes.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <math.h>

#include <genPlotClass/PD_3DXYZ.h>

#include <genPlotClassPS/OGL_3DXYZ.h>

OGL3DXYZ::OGL3DXYZ(PD_3DXYZ& inPlotDef) :  OGL3DBase(inPlotDef), plot3DXYZDef(inPlotDef)
{
}

void OGL3DXYZ::ViewSetup()
{
    SetCurrentView();

    doLogX = (!plot3DXYZDef.xAxis.axesAreLinear);
    doLogY = (!plot3DXYZDef.yAxis.axesAreLinear);
    doLogZ = (!plot3DXYZDef.zAxis.axesAreLinear);

    //  set spans
    normMin = currView.viewLimits.minLim;
    normMax = currView.viewLimits.maxLim;
    TransformForAxes(normMin);
    TransformForAxes(normMax);

    normSpan = normMax - normMin;
    normMid = (normMax + normMin) / 2.0;

    // set mid & mult for VRML
    zMult = 1.0;
    yMult = 1.0;
    zMid  = normMid.cZ;
    yMid  = normMid.cY;

    //  set spans
    xOrthoSpan = 1.0;
    yOrthoSpan = plot3DXYZDef.xyaxesLengthRatio;
    zOrthoSpan = plot3DXYZDef.xzaxesLengthRatio;

    // normalize
    NormalizeSpans();
}





void OGL3DXYZ::VRMLTransformCoord(Coord3D& inCoord)
{
    inCoord.cZ = zMid + (inCoord.cZ - zMid) * zMult;
    inCoord.cY = yMid + (inCoord.cY - yMid) * yMult;
    OGLBase::VRMLTransformCoord(inCoord);
}


bool OGL3DXYZ::TransformForAxes(Coord3D& inCoord) const
{
    bool transOK = true;
    if (doLogX)
    {
        if (inCoord.cX < logAxesMin)
            transOK = false;
        inCoord.cX = log10(inCoord.cX);
    }

    if (doLogY)
    {
        if (inCoord.cY < logAxesMin)
            transOK = false;
        inCoord.cY = log10(inCoord.cY);
    }

    if (doLogZ)
    {
        if (inCoord.cZ < logAxesMin)
            transOK = false;
        inCoord.cZ = log10(inCoord.cZ);
    }

    return transOK;
}


double  OGL3DXYZ::TransformX(const double& xVal)
{
    if (doLogX)
        return log10(xVal);
    return xVal;
}

double OGL3DXYZ::TransformY(const double& yVal)
{
    if (doLogY)
        return log10(yVal);
    return yVal;
}

double OGL3DXYZ::TransformZ(const double& zVal)
{
    if (doLogZ)
        return log10(zVal);
    return zVal;
}









