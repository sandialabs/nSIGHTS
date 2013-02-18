///////////////////////////////////////////////////////////////////////////////////
//
// PFO_GridColorPoint.cpp
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
//    Plots grids as symbols on a 2D or 3D plot.
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>

#include <genClass/SC_CoordArray.h>

#include <genClass/U_Str.h>


#include <objPFO/PFO_GridColorPoint.h>


PFO_GridColorPoint :: PFO_GridColorPoint(PlotDefC& assPlt) :
    PFO_GridColorBase("Grid Color Symbols", assPlt)
{
    reportOK     = false;
    pointSymbol  = ps_FCircle;
    pointSymSize = 6;
    edgeSpec.plotEdges  = false;
}

PFO_GridColorPoint::PFO_GridColorPoint(const PFO_GridColorPoint& a, PlotDefC& assPlt) :
    PFO_GridColorBase(a, assPlt)
{
    reportOK   = false;
    LocalCopy(a);
}

PFO_GridColorPoint :: ~PFO_GridColorPoint ()
{
}


PFO_GridColorPoint& PFO_GridColorPoint::operator= (const PFO_GridColorPoint& a)
{
    if (&a != this)
    {
        PFO_GridColorBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PFO_GridColorPoint::LocalCopy(const PFO_GridColorPoint& a)
{
    edgeSpec     = a.edgeSpec;
    pointSymbol  = a.pointSymbol;
    pointSymSize = a.pointSymSize;
}


