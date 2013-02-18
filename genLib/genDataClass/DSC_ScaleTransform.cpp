///////////////////////////////////////////////////////////////////////////////////
//
// DSC_ScaleTransform.cpp
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
// DESCRIPTION: common arithmetic and transform operations on real values
//
///////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <genClass/C_Common.h>
#include <genDataClass/DSC_ScaleTransform.h>

DSC_ScaleTransform :: DSC_ScaleTransform()
{
    scaleOp         = st_DmSpO;
    tranOp          = tt_None;
    offsetVal       = 0.0;
    scaleVal        = 1.0;
    scaleFirst      = true;
}

DSC_ScaleTransform::DSC_ScaleTransform(const DSC_ScaleTransform& a)
{
    LocalCopy(a);
}


DSC_ScaleTransform& DSC_ScaleTransform::operator= (const DSC_ScaleTransform& a)
{
    if (&a != this)
        {
            LocalCopy(a);
        }
    return *this;
}


void  DSC_ScaleTransform::LocalCopy(const DSC_ScaleTransform& a)
{
    scaleOp         = a.scaleOp;
    tranOp          = a.tranOp;
    offsetVal       = a.offsetVal;
    scaleVal        = a.scaleVal;
    scaleFirst      = a.scaleFirst;
}

void DSC_ScaleTransform:: DoTransform(double& tVal)
{
    if ((tranOp == tt_None) || RealIsNull(tVal))
        return;

    const double tranEps    = 1.0E-99;
    const double maxExp     = 150.0;
    const double maxInvLgt  = 90.0;
    const double maxSquare  = 1.0E+45;

    switch (tranOp)
        {
        case tt_Ln : {
            if (tVal < tranEps)
                tVal = nullReal;
            else
                tVal = log(tVal);
            break;
        }
        case tt_Lgt : {
            if (tVal < tranEps)
                tVal = nullReal;
            else
                tVal = log10(tVal);
            break;
        }

        case tt_Sqrt : {
            if (tVal < 0.0)
                tVal = nullReal;
            else
                tVal = sqrt(tVal);
            break;
        }
        case tt_Inv : {
            if (fabs(tVal) < tranEps)
                tVal = nullReal;
            else
                tVal = 1.0 / tVal;
            break;
        }
        case tt_Exp : {
            if (tVal > maxExp)
                tVal = nullReal;
            else
                tVal = exp(tVal);
            break;
        }
        case tt_pow10 : {
            if (tVal > maxInvLgt)
                tVal = nullReal;
            else
                tVal = pow(10.0, tVal);
            break;
        }
        case tt_Sqr : {
            if (fabs(tVal) > maxSquare)
                tVal = nullReal;
            else
                tVal = tVal * tVal;
            break;
        }
        case tt_Abs : {
            tVal = fabs(tVal);
            break;
        }
        }
}

void DSC_ScaleTransform:: DoScale(double& sVal)
{
    if (RealIsNull(sVal))
        return;

    switch (scaleOp)
        {
        case st_DmSpO : {
            sVal = sVal * scaleVal + offsetVal;
            break;
        }
        case st_DpOmS : {
            sVal = (sVal + offsetVal) * scaleVal;
            break;
        }
        case st_DdSpO : {
            sVal = sVal / scaleVal + offsetVal;
            break;
        }
        case st_DpOdS : {
            sVal = (sVal + offsetVal) / scaleVal;
            break;
        }
        }
}

void  DSC_ScaleTransform::DoScaleTransform(double& stVal)
{
    if (scaleFirst)
        {
            DoScale(stVal);
            DoTransform(stVal);
        }
    else
        {
            DoTransform(stVal);
            DoScale(stVal);
        }
}

