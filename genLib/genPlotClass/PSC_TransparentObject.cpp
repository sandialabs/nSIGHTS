///////////////////////////////////////////////////////////////////////////////////
//
// PSC_TransparentObject.cpp
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
//      Platform independent base classes for plot definitions and plot functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>


#include <genPlotClass/C_PlotObj.h>

#include <genPlotClass/PSC_TransparentObject.h>

PSC_TransparentObject :: PSC_TransparentObject(PlotObjBaseC& inObj)
:baseObj(inObj)
{
    transGroup = 0;
}


PSC_TransparentObject::PSC_TransparentObject(const PSC_TransparentObject& a)
:baseObj(a.baseObj)
{
    GenAppInternalError("PSC_TransparentObject::copy ctor");
};

PSC_TransparentObject::~PSC_TransparentObject()
{
}

PSC_TransparentObject& PSC_TransparentObject::operator= (const PSC_TransparentObject& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void PSC_TransparentObject::LocalCopy(const PSC_TransparentObject& a)
{
    transGroup = a.transGroup;
}

bool PSC_TransparentObject::SupportsTransparency() const
{
    return false;
}

int PSC_TransparentObject::GetnTransObjects() const
{
    return 0;
}

void PSC_TransparentObject::SetupForGetCoord()
{
    transLimits = baseObj.assocPlot->GetCurrentViewLimits();
}

Coord3D PSC_TransparentObject::GetTransObjectCoord(int objIndex)
{
    GenAppInternalError("PSC_TransparentObject::GetObjectCoord");
    return Coord3D();
}


Coord3D PSC_TransparentObject::GetTransPolyCentroid(const SC_CoordArray& inPoly)
{
    if (!transLimits.WithinLimits(inPoly))
        return Coord3D();
    return inPoly.CalcPolyCentroid();
}

void PSC_TransparentObject::DrawTransObject(int objIndex)
{
    GenAppInternalError("PSC_TransparentObject::DrawTransObject");
}


