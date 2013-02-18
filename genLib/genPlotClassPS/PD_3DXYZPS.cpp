///////////////////////////////////////////////////////////////////////////////////
//
// PD_3DXYZPS.cpp
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
//      merges OGL_3DXYZ with PD_3DXYZ.  Implements axes drawing routines.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include <genClassPS/PS_Color.h>
#include <genPlotClass/C_PlotObj.h>

#include <genPlotClassPS/PC_Platform3DPS.h>
#include <genPlotClassPS/PC_ReportPS.h>

#include <genPlotClassPS/PD_3DXYZPS.h>


//  derived for plot type specific implementation of Init and Reset

class PC_Platform3DXYZPS : public PC_Platform3DPS {
    private:
        PD_3DXYZPS& xyz3DplotDef;
    public:

                        PC_Platform3DXYZPS(PD_3DXYZPS& assPlt, OGL3DBase& ogl3DObj)
                            : xyz3DplotDef(assPlt), PC_Platform3DPS("XYZ Plot", assPlt, ogl3DObj) {};
                        ~PC_Platform3DXYZPS(){};

        //  virtuals from PC_Platform
        void            InitGraphicsWindow();
        void            ResetGraphicsWindow();
};


//  class specific derivation of Init and Reset

void PC_Platform3DXYZPS:: InitGraphicsWindow()
{
    //  base class initialization
    PC_Platform3DPS :: InitGraphicsWindow();

    SetFrameIcon(IDI_3DPLOT);

    //  3D spatial callbacks
    //  window resize
//    oglObject.AddCallback(XmNresizeCallback, ResizeCB, &xyz3DplotDef);

    // platform init complete
    initComplete = true;
}

void PC_Platform3DXYZPS:: ResetGraphicsWindow()
{
    //  base class reset
    PC_Platform3DPS :: ResetGraphicsWindow();

    xyz3DplotDef.ResetSize();

}


PD_3DXYZPS :: PD_3DXYZPS() : OGL3DXYZ(*(static_cast<PD_3DXYZ*>(this)))
{
    InitCommon();
}

PD_3DXYZPS :: PD_3DXYZPS(const PD_3DXYZPS& a) : OGL3DXYZ(*(static_cast<PD_3DXYZ*>(this))), PD_3DXYZ(a)
{
    InitCommon();
}

PD_3DXYZPS :: ~PD_3DXYZPS ()
{
}

void PD_3DXYZPS::InitCommon()
{
    //  create and initialize all PS data
    platformData = new PC_Platform3DXYZPS(*this, *this);
}

PD_3DXYZPS& PD_3DXYZPS::operator= (const PD_3DXYZPS& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}

void  PD_3DXYZPS::FullCopy(const PD_3DXYZPS& a)
{
    PD_3DXYZ::FullCopy(a);
    LocalCopy(a);
}

void  PD_3DXYZPS::LocalCopy(const PD_3DXYZPS& a)
{
    //  no properties yet
}

OGLBase* PD_3DXYZPS::GetOGLBase()
{
    return this;
}


void  PD_3DXYZPS::InitRenderer()
{
    //  switch context
    if (currOGLBase)
        currOGLBase->MakeCurrent();

    // set clear color
    SetClearColor(defaultPenSet->GetColor(plotBackgroundPen));
    Clear();
}

void PD_3DXYZPS::StartLineSection()
{
    SetLineAntiAlias();
}
void PD_3DXYZPS::EndLineSection()
{
    DisableLineAntiAlias();
}

void  PD_3DXYZPS::SetupDataView()
{
    InitView();
}

void  PD_3DXYZPS::SetupAnnoView()
{
    InitAnnoView();
}

void  PD_3DXYZPS::CloseRenderer()
{
    Flush();
}

void PD_3DXYZPS::BasicFullDraw()
{
    if (!PlotDefIsStereo())
    {
        PlotDefC::BasicFullDraw();
        return;
    }

    if (!currOGLBase->GLisStereo())
    {
        GenAppWarningMsg("PD_3DXYZPS", "no OpenGL stereo support in selected visual");
        PlotDefC::BasicFullDraw();
        return;
    }

    //  switch context
    if (currOGLBase)
        currOGLBase->MakeCurrent();

    // set clear color
    SetClearColor(defaultPenSet->GetColor(plotBackgroundPen));


    InitStereoView();

    SetStereoView(true);
    DrawAllObjects();

    SetStereoView(false);
    DrawAllObjects();

    CloseRenderer();
}


void PD_3DXYZPS:: DrawPlotAxes()
{
    StartLineSection();

    SetLineSolid();

    xAxis.SetMajorIncs();
    xAxis.SetMinorIncs();
    yAxis.SetMajorIncs();
    yAxis.SetMinorIncs();
    zAxis.SetMajorIncs();
    zAxis.SetMinorIncs();

    SC_DoubleArray xMajorIncs, xMinorIncs, yMajorIncs, yMinorIncs, zMajorIncs, zMinorIncs;
    SC_DoubleArray xtranMajorIncs, ytranMajorIncs, ztranMajorIncs;

    xAxis.GetAxesMajorIncs(xMajorIncs);
    xAxis.GetAxesMinorIncs(xMinorIncs);

    yAxis.GetAxesMajorIncs(yMajorIncs);
    yAxis.GetAxesMinorIncs(yMinorIncs);

    zAxis.GetAxesMajorIncs(zMajorIncs);
    zAxis.GetAxesMinorIncs(zMinorIncs);

    TransformIncrements(!xAxis.axesAreLinear, xMajorIncs, xtranMajorIncs);
    TransformIncrements(!yAxis.axesAreLinear, yMajorIncs, ytranMajorIncs);
    TransformIncrements(!zAxis.axesAreLinear, zMajorIncs, ztranMajorIncs);

    Coord3D minLim, maxLim;
    GetTransformedLimits(minLim, maxLim);

    Limit2D axLimits;
    Coord3D offComp = GetPixelComponents(axesFormat.axesOffset);

    PC_AxesFormat xynUorig, xynVorig;
    SetAutoView(xynUorig, xynVorig);

    // XY
    axLimits.minLim = Point2D(minLim.cX, minLim.cY);
    axLimits.maxLim = Point2D(maxLim.cX, maxLim.cY);
    DrawAxesPlane(xtranMajorIncs, xMinorIncs, ytranMajorIncs, yMinorIncs, axLimits,
                  minLim.cZ - offComp.cZ, PC_3DAxesFormat::a3pXYn, doLogX, doLogY);
    DrawAxesPlane(xtranMajorIncs, xMinorIncs, ytranMajorIncs, yMinorIncs, axLimits,
                  maxLim.cZ + offComp.cZ, PC_3DAxesFormat::a3pXYp, doLogX, doLogY);

    // XZ
    axLimits.minLim = Point2D(minLim.cX, minLim.cZ);
    axLimits.maxLim = Point2D(maxLim.cX, maxLim.cZ);
    DrawAxesPlane(xtranMajorIncs, xMinorIncs, ztranMajorIncs, zMinorIncs, axLimits,
                  minLim.cY - offComp.cY, PC_3DAxesFormat::a3pXZn, doLogX, doLogZ);
    DrawAxesPlane(xtranMajorIncs, xMinorIncs, ztranMajorIncs, zMinorIncs, axLimits,
                  maxLim.cY + offComp.cY, PC_3DAxesFormat::a3pXZp, doLogX, doLogZ);

    // YZ
    axLimits.minLim = Point2D(minLim.cY, minLim.cZ);
    axLimits.maxLim = Point2D(maxLim.cY, maxLim.cZ);
    DrawAxesPlane(ytranMajorIncs, yMinorIncs, ztranMajorIncs, zMinorIncs, axLimits,
                  minLim.cX - offComp.cX, PC_3DAxesFormat::a3pYZn, doLogY, doLogZ);
    DrawAxesPlane(ytranMajorIncs, yMinorIncs, ztranMajorIncs, zMinorIncs, axLimits,
                  maxLim.cX + offComp.cX, PC_3DAxesFormat::a3pYZp, doLogY, doLogZ);

    EndLineSection();

    // reset auto changes
    axesFormat.axesUFormat[PC_3DAxesFormat::a3pXYn] = xynUorig;
    axesFormat.axesVFormat[PC_3DAxesFormat::a3pXYn] = xynVorig;

    // labels
    DrawAxesLabels(xMajorIncs, xtranMajorIncs, yMajorIncs, ytranMajorIncs, zMajorIncs, ztranMajorIncs,
                   xAxis.axisIncFormat, yAxis.axisIncFormat, zAxis.axisIncFormat);


    SetClipPlanes();
}


void  PD_3DXYZPS::SetClipPlanes()
{
    double planeEqn[4];

    Coord3D minLim, maxLim;
    GetTransformedLimits(minLim, maxLim);

    //  left of X = xAxisMin
    planeEqn[1] = 0.0;
    planeEqn[2] = 0.0;
    if (minLim.cX < maxLim.cX)
    {
        planeEqn[0] = 1.0;
        planeEqn[3] = -minLim.cX;
        SetClipPlane(0, planeEqn);

        //  right of X = xAxisMax
        planeEqn[0] = -1.0;
        planeEqn[3] = maxLim.cX;
        SetClipPlane(1, planeEqn);
    }
    else
    {
        planeEqn[0] = -1.0;
        planeEqn[3] = minLim.cX;
        SetClipPlane(0, planeEqn);

        //  right of X = xAxisMax
        planeEqn[0] = 1.0;
        planeEqn[3] = -maxLim.cX;
        SetClipPlane(1, planeEqn);
    }

    planeEqn[0] = 0.0;
    if (minLim.cY < maxLim.cY)
    {
        //  below Y = yAxisMin
        planeEqn[1] = 1.0;
        planeEqn[3] = -minLim.cY;
        SetClipPlane(2, planeEqn);

        //  above Y = yAxisMax
        planeEqn[1] = -1.0;
        planeEqn[3] = maxLim.cY;
        SetClipPlane(3, planeEqn);
    }
    else
    {
        //  below Y = yAxisMin
        planeEqn[1] = -1.0;
        planeEqn[3] = minLim.cY;
        SetClipPlane(2, planeEqn);

        //  above Y = yAxisMax
        planeEqn[1] = 1.0;
        planeEqn[3] = -maxLim.cY;
        SetClipPlane(3, planeEqn);
    }


    //  below Z = zAxisMin
    planeEqn[1] = 0.0;
    if (minLim.cZ < maxLim.cZ)
    {
        planeEqn[2] = 1.0;
        planeEqn[3] = -minLim.cZ;
        SetClipPlane(4, planeEqn);

        //  above Z = zAxisMax
        planeEqn[2] = -1.0;
        planeEqn[3] = maxLim.cZ;
        SetClipPlane(5, planeEqn);
    }
    else
    {
        planeEqn[2] = -1.0;
        planeEqn[3] = minLim.cZ;
        SetClipPlane(4, planeEqn);

        //  above Z = zAxisMax
        planeEqn[2] = 1.0;
        planeEqn[3] = -maxLim.cZ;
        SetClipPlane(5, planeEqn);
    }
}

void PD_3DXYZPS::ResetSize()
{
    if (IsMasterPlot())
        AdjustMargins();
    static_cast<PC_PlatformPS*>(platformData)->DoResize();
}

void PD_3DXYZPS::ResetView()
{
    if (!platformData->initComplete)
        return;

    static_cast<PC_Platform3DXYZPS*>(platformData)->UpdateViewSettings();

    CWaitCursor w;
    PD_3DXYZ::ResetView();
}


void PD_3DXYZPS::SetTransparencyFactor(const double& tranFactor)
{
    SetTransFactor(tranFactor);
}

void PD_3DXYZPS::StartTransparentSection()
{
    StartTransparency();
}
void PD_3DXYZPS::EndTransparentSection()
{
    EndTransparency();
}


Coord3D PD_3DXYZPS::GetNormalizedCoord(const Coord3D& inCoord,
                                       const Coord3D& offsetCoord)
{
    Coord3D tempCoord(inCoord);
    if (!TransformForAxes(tempCoord))
        return Coord3D();
    tempCoord += offsetCoord;
    return OGL3DBase::GetNormalizedCoord(tempCoord);
}


