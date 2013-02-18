///////////////////////////////////////////////////////////////////////////////////
//
// SC_ColorSpec.cpp
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
// DESCRIPTION: Generic support classes: defines color by RGB or HSV components.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>

#include <genClass/U_Real.h>

#include <genClass/SC_ColorSpec.h>


SC_ColorSpec::SC_ColorSpec(const double& RHv, const double& GSv, const double& BVv)
{
    RH = RHv;
    GS = GSv;
    BV = BVv;
}

SC_ColorSpec::SC_ColorSpec(const SC_ColorSpec& a)
{
    RH = a.RH;
    GS = a.GS;
    BV = a.BV;
}

SC_ColorSpec& SC_ColorSpec::operator= (const SC_ColorSpec& a)
{
    if (&a != this)
        {
            RH = a.RH;
            GS = a.GS;
            BV = a.BV;
        }
    return *this;
}

bool SC_ColorSpec::operator== (const SC_ColorSpec& a) const
{
    static const double colorEps = 0.001;
    return (fabs(RH - a.RH) < colorEps) && (fabs(GS - a.GS) < colorEps) && (fabs(BV - a.BV) < colorEps);
}

bool SC_ColorSpec::operator!= (const SC_ColorSpec& a) const
{
    return ! operator==(a);
}

void SC_ColorSpec::Blend(const SC_ColorSpec& a,
                         const SC_ColorSpec& b,
                         const double&       tVal)
{
    if (tVal <= 0.0)
        {
            SC_ColorSpec::operator=(a);
            return;
        }

    if (tVal >= 1.0)
        {
            SC_ColorSpec::operator=(b);
            return;
        }

    double sVal = 1.0 - tVal;
    RH = sVal * a.RH + tVal * b.RH;
    GS = sVal * a.GS + tVal * b.GS;
    BV = sVal * a.BV + tVal * b.BV;
}

void SC_ColorSpec::ToRGB()
{
    double  h = RH * 6.0;
    int     i = int(floor(h));
    double  f;

    if ( i == 6)
        {
            i = 0;
            f = 0.0;
        }
    else
        f = h - i;

    double  p1 = BV * (1.0 - GS);
    double  p2 = BV * (1.0 - (GS * f));
    double  p3 = BV * (1.0 - (GS * (1.0 - f)));

    switch (i) {
    case 0: RH = BV; GS = p3; BV = p1; break;
    case 1: RH = p2; GS = BV; BV = p1; break;
    case 2: RH = p1; GS = BV; BV = p3; break;
    case 3: RH = p1; GS = p2; break;
    case 4: RH = p3; GS = p1; break;
    case 5: RH = BV; GS = p1; BV = p2; break;
    default: GenAppInternalError("SC_ColorSpec::toRGB");
    }
}

void SC_ColorSpec::ToHSV()
{
    double  v = RH > GS ? RH : GS;
    v = v > BV ? v : BV;

    double  m = RH < GS ? RH : GS;
    m = m < BV ? m : BV;

    double s = 0;
    double h = 0;
    if (v != 0)
        s = (v - m) / v;

    if (s != 0)
        {
            double dv = v - m;
            double r1 = (v - RH) / dv;
            double g1 = (v - GS) / dv;
            double b1 = (v - BV) / dv;


            if (v == RH)
                if (m == GS)
                    h = 5 + b1;
                else
                    h = 1 - g1;

            if ( v == GS)
                if (m == BV)
                    h = 1 + r1;
                else
                    h = 3 - b1;
            else
                if ( m == RH )
                    h = 3 + g1;
                else
                    h = 5 - r1;
        }

    RH = h / 6.0;
    GS = s;
    BV = v;
}

