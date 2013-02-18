///////////////////////////////////////////////////////////////////////////////////
//
// C_PlotObj.cpp
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
//      derived from PlotObjBaseC.  Adds plot layering and
//  reporting support for 2D objects and polygon offset
//  support for 3D objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genPlotClass/PD_3D.h>

#include <genPlotClass/C_PlotObj.h>

PlotObjC :: PlotObjC(const char* pltType, PlotDefC& assPlt)
    : PlotObjBaseC(pltType, assPlt),
      PSC_TransparentObject(static_cast<PlotObjBaseC&>(*this))
{
    InitCommon();
}


PlotObjC :: PlotObjC(const PlotObjC& a, PlotDefC& assPlt)
    : PlotObjBaseC(a),
      PSC_TransparentObject(static_cast<PlotObjBaseC&>(*this))
{
    InitCommon();
    LocalCopy(a);
}


PlotObjC :: ~PlotObjC ()
{
    assocPlot->plotObjList.DeleteFrom(this);

    if ( cursorReport != 0)
        delete cursorReport;
}


void PlotObjC::InitCommon()
{
    reportOK                = false;
    activeOK                = false;
    isActive                = false;
    is3DObject              = (dynamic_cast <PD_3D*> (assocPlot) != 0);

    plotLayer               = 0;
    polyOffset              = 0.0;
    usePolyOffset           = false;
    recalcOnViewChange      = false;
    cursorReport            = 0;

    offsetCoord             = Coord3D(0.0, 0.0, 0.0);

    // add to list
    assocPlot->plotObjList.AddTo(this);
}

PlotObjC& PlotObjC::operator= (const PlotObjC& a)
{
    if (&a != this)
    {
        PlotObjBaseC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PlotObjC::LocalCopy(const PlotObjC& a)
{
    plotLayer   = a.plotLayer;
    polyOffset  = a.polyOffset;
    offsetCoord = a.offsetCoord;
    PSC_TransparentObject::operator =(a);
}

void  PlotObjC::DoStatusChk()
{
    PlotObjBaseC::DoStatusChk();
}

void PlotObjC::ReportCursorData(const Coord3D& cursorPos) {}

Limit3D PlotObjC::GetPlotObjLimits()
{
    Limit3D nullLimit;
    return nullLimit;
}

void PlotObjC::ProcessSelection(const Coord3D&                cursorPos,
                                      PlotDefC::SelectionKeys inKey)
{
}


PC_CallbackMenu* PlotObjC::GetCallbackMenu()
{
    return 0;
}


bool PlotObjC::ContainsLinesOnly()
{
    return false;
}

