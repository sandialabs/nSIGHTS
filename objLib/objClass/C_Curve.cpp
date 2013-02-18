///////////////////////////////////////////////////////////////////////////////////
//
// C_Curve.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/C_Common.h>

#include <objClass/C_Curve.h>



CurveDescription::CurveDescription()
{
    curveType               = Curve::ctLinear;
    splineNaturalSlope      = true;
    fixedSplineStartSlope   = 0.0;
    fixedSplineEndSlope     = 0.0;
    splineTension           = 1.0;
    polyOrder               = 1;
}

CurveDescription::CurveDescription(const CurveDescription& a)
{
    LocalCopy(a);
}

CurveDescription::~CurveDescription()
{
    // empty for now
}

CurveDescription& CurveDescription::operator= (const CurveDescription& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

bool  CurveDescription::CurveSetupOK(int nInputPoints,    // number of data points used in constructing curve
                                         SC_SetupErr& errData)
{
    int minPoints = 1;
    switch(curveType)
    {
        case Curve::ctCubicSpline: {
                minPoints = 3;
                break;
            }
        case Curve::ctPolynomial: {
                minPoints = polyOrder + 1;
                break;
            }
        case Curve::ctLinear:
        case Curve::ctStepMid:
        case Curve::ctStepFull:
                break;
        default:
        {
            GenAppInternalError("CurveDescription::CurveSetupOK");
        }
    }

    if (nInputPoints < minPoints)
    {
        errData.SetConstantError("insufficient data to create curve");
        return false;
    }
    return true;
}


void  CurveDescription::LocalCopy(const CurveDescription& a)
{
   curveType             = a.curveType            ;
   splineNaturalSlope    = a.splineNaturalSlope   ;
   fixedSplineStartSlope = a.fixedSplineStartSlope;
   fixedSplineEndSlope   = a.fixedSplineEndSlope  ;
   splineTension         = a.splineTension        ;
   polyOrder             = a.polyOrder            ;
}

