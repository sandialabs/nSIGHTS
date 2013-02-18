///////////////////////////////////////////////////////////////////////////////////
//
// PSC_3DSeriesSpec.cpp
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

#include <genPlotClass/PSC_3DSeriesSpec.h>

PSC_3DSeriesSpec::PSC_3DSeriesSpec() :
    plotLineType(plt_Solid),
    plotLineAsTube(false),
    plotSymEdges(false),
    plotTubeEdges(false)
{
    adjustForScaling = true;
    nextrusionPoints = 8;
}

PSC_3DSeriesSpec::PSC_3DSeriesSpec(const PSC_3DSeriesSpec& a) :
    PSC_SeriesSpec(a), PSC_EdgeSpec(a), PSC_TubeExtrusionSpec(a)
{
    LocalCopy(a);
}

PSC_3DSeriesSpec&  PSC_3DSeriesSpec::operator= (const PSC_3DSeriesSpec& a)
{
    if (&a != this)
    {
        PSC_SeriesSpec::operator=(a);
        PSC_EdgeSpec::operator=(a);
        PSC_TubeExtrusionSpec::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void  PSC_3DSeriesSpec::LocalCopy(const PSC_3DSeriesSpec& a)
{
    plotLineType    = a.plotLineType;
    plotLineAsTube  = a.plotLineAsTube;
    plotSymEdges    = a.plotSymEdges;
    plotTubeEdges   = a.plotTubeEdges;
}


bool PSC_3DSeriesSpec::IsSolid()
{
    return PSC_SeriesSpec::IsSolid() || plotLineAsTube;
}

