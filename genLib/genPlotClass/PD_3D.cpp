///////////////////////////////////////////////////////////////////////////////////
//
// PD_3D.cpp
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
//      defines basic 3D plot 
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotObj.h>
#include <genPlotClass/C_AnnoObj.h>

#include <genPlotClass/PD_3D.h>


PD_3D :: PD_3D(const char* dType)
    : PlotDefC(dType), transSpec(*this),
        fieldOfViewMS("Field of view"),
        eyeSeparationDivisorMS("Eye separation divisor")
{
    InitCommon();
    SetDefault();
}

PD_3D :: PD_3D(const PD_3D& a)
    : PlotDefC(a), transSpec(*this),
        fieldOfViewMS(a.fieldOfViewMS),
        eyeSeparationDivisorMS(a.eyeSeparationDivisorMS)
{
    InitCommon();
    LocalCopy(a);
}

PD_3D :: ~PD_3D()
{
}

void PD_3D::InitCommon()
{
    fieldOfViewMS.AddPort(*this);
    fieldOfViewCheckRef = GetLastInPortCheckRef();

    eyeSeparationDivisorMS.AddPort(*this);
    eyeSeparationDivisorCheckRef = GetLastInPortCheckRef();

    AddOutPort(elevationDO);
    AddOutPort(azimuthDO);
    AddOutPort(scaleDO);

    elevationDO.SetTypeLabel("Elevation");
    azimuthDO.SetTypeLabel("Azimuth");
    scaleDO.SetTypeLabel("Scale");

    plotProjection = tdp_Orthographic;
    fieldOfView = 40.0;
    useStereo3DIfAvailable = false;
    eyeSeparationDivisor   = 25.0;

    optimizeRedraw = false;

    fieldOfViewMS.enteredRealValue = 30.0;
    eyeSeparationDivisorMS.enteredRealValue = 75.0;
}

PD_3D& PD_3D::operator= (const PD_3D& a)
{
    if (&a != this)
        FullCopy(a);
    return *this;
}

void  PD_3D::FullCopy(const PD_3D& a)
{
    PlotDefC::FullCopy(a);
    LocalCopy(a);
}

void  PD_3D::LocalCopy(const PD_3D& a)
{
    plotProjection  = a.plotProjection ;
    useStereo3DIfAvailable = a.useStereo3DIfAvailable;
    fieldOfViewMS   = a.fieldOfViewMS;
    eyeSeparationDivisorMS = a.eyeSeparationDivisorMS;
    axesFormat      = a.axesFormat;
    axesLabel       = a.axesLabel ;
    vrmlOutData     = a.vrmlOutData;
    plotLighting    = a.plotLighting;
    transSpec       = a.transSpec;
    optimizeRedraw  = a.optimizeRedraw;
}

void  PD_3D::DoStatusChk()
{
    *fieldOfViewCheckRef = (plotProjection != tdp_Orthographic);
    *eyeSeparationDivisorCheckRef = (plotProjection != tdp_Orthographic) && useStereo3DIfAvailable;
    PlotDefC::DoStatusChk();

    if (*fieldOfViewCheckRef)
    {
        fieldOfViewMS.DoCheck("FOV master", *this);
        if (StatusOK())
            fieldOfView = DMin(DMax(fieldOfViewMS.GetCurrentValue(), 1.0), 85.0);
    }
    if (*eyeSeparationDivisorCheckRef)
    {
        eyeSeparationDivisorMS.DoCheck("Eye Sep master", *this);
        if (StatusOK())
            eyeSeparationDivisor = DMin(DMax(eyeSeparationDivisorMS.GetCurrentValue(), 5.0), 500.0);
    }
}



void PD_3D::SetDefault()
{
    PlotDefC::SetDefault();
    rightMargin   = 0;
    leftMargin    = 0;
    topMargin     = 0;
    bottomMargin  = 0;
}

void PD_3D::ChangeView(const PC_View&  newView)
{
    UpdateViewDO(newView);
    PlotDefC::ChangeView(newView);
}

void PD_3D::SetInitialView(const PC_View&  newView)
{
    UpdateViewDO(newView);
    PlotDefC::SetInitialView(newView);
}

void  PD_3D::UpdateViewDO(const PC_View& newView)
{
    elevationDO.InitLabelAndValue(newView.elevation);
    azimuthDO.InitLabelAndValue(newView.azimuth);
    scaleDO.InitLabelAndValue(newView.scale);
}

bool PD_3D::PlotDefIsStereo() const
{
    return useStereo3DIfAvailable && (plotProjection == tdp_Perspective);
}


void PD_3D::DrawPlotObjects()
{
    transOK = transSpec.TransparencySetup();
    // first solids
    for (int j = 0; j < plotObjList.Size(); j++)
    {
        if (!transSpec.objectIsTransparent[j])
        {
            PlotObjC& currObj = plotObjList.GetRef(j);
            if (currObj.doPlot && (!currObj.ContainsLinesOnly()))
                currObj.DrawPlotObject();
        }
    }

    StartLineSection();
    // now lines
    for (int j = 0; j < plotObjList.Size(); j++)
    {
        if (!transSpec.objectIsTransparent[j])
        {
            PlotObjC& currObj = plotObjList.GetRef(j);
            if (currObj.doPlot && (currObj.ContainsLinesOnly()))
                currObj.DrawPlotObject();
        }
    }
    EndLineSection();

    if (transOK)
    {
        StartTransparentSection();
        transSpec.DrawTransparentObjects();
        EndTransparentSection();
    }

}


void PD_3D::DrawAnnoObjects()
{
    for (int i = 0; i < annoObjList.Size(); i++)
    {
        if (!transSpec.annoIsTransparent[i])
        {
            AnnoObjC& currObj = annoObjList.GetRef(i);
            if (currObj.doPlot)
                currObj.DrawPlotObject();
        }
    }

    if (transOK)
    {
        StartTransparentSection();
        transSpec.DrawTransparentAnno();
        EndTransparentSection();
    }

}

