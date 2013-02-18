///////////////////////////////////////////////////////////////////////////////////
//
// C_Derivative.cpp
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
//      Definition of data required derivative calculation.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/C_Common.h>

#include <nsClass/C_Derivative.h>

DerivativeDescription::DerivativeDescription()
{
    derivativeType       = dt_dY_dlnX;
    derivativeCalcType   = dctLogPctWindow;
    windowCalcType       = wctClark;
    timeMultType         = tmtNone;

    useSuperTime        = false;
    windowPtsSpan       = 1;
    windowPctSpan       = 10.0;
    windowValSpan       = 0.1;

    absOutputY          = false;

    logEps              = 1.0E-15;  
    offsetY             = 1.0E-08;
}

DerivativeDescription::DerivativeDescription(const DerivativeDescription& a)
{
    LocalCopy(a);
}

DerivativeDescription::~DerivativeDescription()
{
    // empty for now
}

DerivativeDescription& DerivativeDescription::operator= (const DerivativeDescription& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

bool  DerivativeDescription::DerivativeSetupOK(int          ninputPoints,
                                               SC_SetupErr& errData)
{
    int minPoints = 10;
    switch (derivativeCalcType)
    {
        case Derivative::dctSimple  : {
            minPoints = 3;
            break;
        }
        case Derivative::dct2SlopeAverage: {
            minPoints = windowPtsSpan * 2 + 4;
            break;
        }
        case Derivative::dctFixedWindow : {
            minPoints = windowPtsSpan * 4;
            break;
        }
        default:break;
    }
    if (ninputPoints < minPoints)
        errData.SetConstantError("insufficient points in input");

    return errData.NoErrors();
}


void  DerivativeDescription::LocalCopy(const DerivativeDescription& a)
{
    derivativeType       = a.derivativeType;
    derivativeCalcType   = a.derivativeCalcType;
    windowCalcType       = a.windowCalcType;
    timeMultType         = a.timeMultType;

    useSuperTime         = a.useSuperTime;
    windowPtsSpan        = a.windowPtsSpan;
    windowPctSpan        = a.windowPctSpan;
    windowValSpan        = a.windowValSpan;

    absOutputY           = a.absOutputY;

    logEps               = a.logEps;
    offsetY              = a.offsetY;
}

