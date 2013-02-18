///////////////////////////////////////////////////////////////////////////////////
//
// C_AnnoObj.cpp
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
//      PlotObjC derived object that is plotted in 2D annotation coordinate system. 
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genPlotClass/C_AnnoObj.h>

const double  AnnoObjC::maxAnnoCoord = 100.0;  // 100.0

void AnnoObjC::ForceToAnnoLimits(Point2D&  inPoint)
{
    if ( inPoint.pX < 0.0)
        inPoint.pX = 0.0;
    else
        if (inPoint.pX > AnnoObjC::maxAnnoCoord)
            inPoint.pX = AnnoObjC::maxAnnoCoord;

    if (inPoint.pY < 0.0)
        inPoint.pY = 0.0;
    else
        if ( inPoint.pY > AnnoObjC::maxAnnoCoord)
            inPoint.pY = AnnoObjC::maxAnnoCoord;
}


AnnoObjC :: AnnoObjC(const char* legendType, PlotDefC& assPlt)
    : PlotObjBaseC(legendType, assPlt),
      PSC_TransparentObject(static_cast<PlotObjBaseC&>(*this))
{
    InitCommon();
    SetDefault();
}

AnnoObjC::AnnoObjC(const AnnoObjC& a, PlotDefC& assPlt)
    : PlotObjBaseC(a, assPlt), PSC_AnnoSpec(a),
      PSC_TransparentObject(static_cast<PlotObjBaseC&>(*this))
{
    InitCommon();
    LocalCopy(a);
}

AnnoObjC :: ~AnnoObjC ()
{
    assocPlot->annoObjList.DeleteFrom(this);
}

void AnnoObjC::InitCommon()
{
    assocPlot->annoObjList.AddTo(this);
}

AnnoObjC& AnnoObjC::operator= (const AnnoObjC& a)
{
    if (&a != this)
    {
        PlotObjBaseC::operator=(a);
        PSC_AnnoSpec::operator =(a);
        LocalCopy(a);
    }
    return *this;
}



void  AnnoObjC::LocalCopy(const AnnoObjC& a)
{
}

void AnnoObjC::SetDefault()
{
    PSC_AnnoSpec::SetDefault();
}

void  AnnoObjC:: DoStatusChk()
{
    PlotObjBaseC::DoStatusChk();
}

void AnnoObjC::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();   // may be duplicate -- required because some anno objects dont Calcoutput
    if (legendTitle[0] != '\0')
        SetDefaultID(legendTitle);
}

void  AnnoObjC::SetRectOffset(const Point2D& rectUL)
{
    ulRectOffset.pX = rectUL.pX - ulLocation.pX;
    ulRectOffset.pY = rectUL.pY - ulLocation.pY;
}

 void AnnoObjC::UpdateULlocation(const Point2D& newrectUL)
 {
    ulLocation.pX = newrectUL.pX - ulRectOffset.pX;
    ulLocation.pY = newrectUL.pY - ulRectOffset.pY;
    UpdateULUI();
 }


bool AnnoObjC::SupportsTransparency() const
{
    return true;
}

int AnnoObjC::GetnTransObjects() const
{
    GenAppInternalError("AnnoObjC::GetnTransObjects");
    return 0;
}

Coord3D AnnoObjC::GetTransObjectCoord(int objIndex)
{
    GenAppInternalError("AnnoObjC::GetTransObjectCoord");
    return Coord3D();
}

