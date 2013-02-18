///////////////////////////////////////////////////////////////////////////////////
//
// DSC_FandChiConfidence.cpp
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
//      Minor master/slave support classes for extracting individual results
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <osDataClass/DSC_FandChiConfidence.h>
#include <genClass/U_Real.h>
#include <genClass/U_Str.h>

DSC_FandChiConfidence::DSC_FandChiConfidence()
{
    distribution = dF;
}

DSC_FandChiConfidence::DSC_FandChiConfidence(const DSC_FandChiConfidence& a)
{
    LocalCopy(a);
}

DSC_FandChiConfidence::~DSC_FandChiConfidence()
{
}

DSC_FandChiConfidence& DSC_FandChiConfidence::operator= (const DSC_FandChiConfidence& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void  DSC_FandChiConfidence::LocalCopy(const DSC_FandChiConfidence& a)
{
    distribution = a.distribution;
}

bool DSC_FandChiConfidence::CalcQF(const SC_DoubleArray& f,
                                   const int             v1,
                                   const int             v2,
                                         SC_DoubleArray& conf,
                                        char*             errMsg,
                                        int           msgLen)
{
    if ((v1 <= 0) || (v2 <= 0))
    {
        CopyString(errMsg, "CalcQF:  v1 and v2 must be greater than zero", msgLen);
        return false;
    }

    // double math added - FB Case 79
    double dv1 = double(v1);
    double dv2 = double(v2);
    double dv12 = dv1 / 2.0;
    double dv22 = dv2 / 2.0;

    // based on the mathematical relationship to the incomplete beta function
    // see Abramowitz, and Stegun, Handbook of Mathematical Functions, vol. 55, Chapters 6, 7 and 26
    for (int i = 0; i < f.Size(); i++)
    {
        double x = dv2/(dv2 + dv1 * f[i]);
        conf[i] = BetaI(dv22, dv12, x);
    }
    return true;
}

bool DSC_FandChiConfidence::CalcQChiSquared(const SC_DoubleArray& chiSqr,
                                            const int             v,
                                                  SC_DoubleArray& conf,
                                                  char*           errMsg,
                                                  int             msgLen)
{
    if (v <= 0)
    {
        CopyString(errMsg, "CalcQChiSquared:  v must be greater than zero", msgLen);
        return false;
    }

    // double math added - FB Case 79
    double v2 = double(v) / 2.0;
    // based on the mathematical relationship to the gamma function
    // see Abramowitz, and Stegun, Handbook of Mathematical Functions, vol. 55, Chapters 6, 7 and 26
    for (int i = 0; i < chiSqr.Size(); i++)
    {
        conf[i] = GammaQ(v2, chiSqr[i]/2.0);
    }
    return true;
}

