///////////////////////////////////////////////////////////////////////////////////
//
// PSC_AnnoSpec.cpp
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
//      elements common to most annotation objects (legend frame, pen, font)
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <genPlotClass/PSC_AnnoSpec.h>

PSC_AnnoSpec :: PSC_AnnoSpec()
{
    SetDefault();
}

PSC_AnnoSpec::PSC_AnnoSpec(const PSC_AnnoSpec& a)
{
    LocalCopy(a);
}

PSC_AnnoSpec :: ~PSC_AnnoSpec ()
{
}

PSC_AnnoSpec& PSC_AnnoSpec::operator= (const PSC_AnnoSpec& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  PSC_AnnoSpec::LocalCopy(const PSC_AnnoSpec& a)
{
    ulLocation.pX    = a.ulLocation.pX     ;         //  location of UL corner in 0 - 100 XY
    ulLocation.pY    = a.ulLocation.pY     ;        //  location of UL corner in 0 - 100 XY

    CopyString(legendTitle, a.legendTitle, legendTitleLen);
    frameLegend      = a.frameLegend       ;
    frameThick       = a.frameThick        ;
    opaqueBackground = a.opaqueBackground  ;
    legendPen        = a.legendPen         ;
    legendFont       = a.legendFont        ;
}

void PSC_AnnoSpec::SetDefault()
{
    ulLocation.pX    = 5.0;         //  location of UL corner in 0 - 100 XY
    ulLocation.pY    = 80.0;        //  location of UL corner in 0 - 100 XY

    legendTitle[0]   = '\0';
    frameLegend      = false;
    frameThick       = 2;
    opaqueBackground = false;
    legendPen        = 6;
}

