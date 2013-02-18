///////////////////////////////////////////////////////////////////////////////////
//
// PSC_ExtrusionSpec.cpp
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
//      how extrusion is plotted on 3D plot
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genPlotClass/PSC_ExtrusionSpec.h>

PSC_ExtrusionSpec :: PSC_ExtrusionSpec()
{
    capEnds             = true;
    adjustForScaling    = false;

}

PSC_ExtrusionSpec::PSC_ExtrusionSpec(const PSC_ExtrusionSpec& a)
{
    LocalCopy(a);
}

PSC_ExtrusionSpec& PSC_ExtrusionSpec::operator= (const PSC_ExtrusionSpec& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  PSC_ExtrusionSpec::LocalCopy(const PSC_ExtrusionSpec& a)
{
    capEnds             = a.capEnds;
    adjustForScaling    = a.adjustForScaling;
}

PSC_TubeExtrusionSpec :: PSC_TubeExtrusionSpec()
{
    nextrusionPoints    = 6;
    usePixelSizing      = true;
    extrusionPixelSize  = 6;
    extrusionRealSize   = 1.0;
}

PSC_TubeExtrusionSpec::PSC_TubeExtrusionSpec(const PSC_TubeExtrusionSpec& a) : PSC_ExtrusionSpec(a)
{
    LocalCopy(a);
}

PSC_TubeExtrusionSpec& PSC_TubeExtrusionSpec::operator= (const PSC_TubeExtrusionSpec& a)
{
    if (&a != this)
    {
        PSC_ExtrusionSpec::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PSC_TubeExtrusionSpec::LocalCopy(const PSC_TubeExtrusionSpec& a)
{
    nextrusionPoints    = a.nextrusionPoints;
    usePixelSizing      = a.usePixelSizing;
    extrusionPixelSize  = a.extrusionPixelSize;
    extrusionRealSize   = a.extrusionRealSize;
}

PSC_PolygonExtrusionSpec :: PSC_PolygonExtrusionSpec()
{
}

PSC_PolygonExtrusionSpec::PSC_PolygonExtrusionSpec(const PSC_PolygonExtrusionSpec& a) : PSC_ExtrusionSpec(a)
{
    LocalCopy(a);
}

PSC_PolygonExtrusionSpec& PSC_PolygonExtrusionSpec::operator= (const PSC_PolygonExtrusionSpec& a)
{
    if (&a != this)
    {
        PSC_ExtrusionSpec::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  PSC_PolygonExtrusionSpec::LocalCopy(const PSC_PolygonExtrusionSpec& a)
{
    extrusionShape    = a.extrusionShape;
}

