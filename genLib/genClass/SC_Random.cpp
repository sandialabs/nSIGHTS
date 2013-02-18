///////////////////////////////////////////////////////////////////////////////////
//
// SC_Random.cpp
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

#include <cmath>

#include <genClass/SC_IntArray.h>
#include <genClass/SC_DoubleArray.h>
#include <genClass/U_Real.h>
#include <genClass/U_Msg.h>

#include <genClass/SC_Random.h>


SC_Random::SC_Random(Algorithm  inAlg)
{
    algorithm = inAlg;

    rn_ix = 23563;
    rn_iy = 11731;
    rn_iz = 5657;

    initialized = false;
}


SC_Random::SC_Random(const SC_Random& a)
{
    LocalCopy(a);
}

SC_Random& SC_Random::operator= (const SC_Random& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void SC_Random::LocalCopy(const SC_Random& a)
{
    rn_ix = a.rn_ix;
    rn_iy = a.rn_iy;
    rn_iz = a.rn_iz;

    ma              = a.ma;
    inext   = a.inext;
    inextp  = a.inextp;
    initialized = a.initialized;
}

double SC_Random::Random()
{
    if (algorithm == raWichmannHill)
        {
            //
            // Generate a random number rectangularly distributed between 0 and 1.
            //
            // ALGORITHM AS 183 APPL. STATIST. (1982) VOL. 31, NO. 2
            // B. A. WICHMANN AND I. D. HILL
            //
            // IX, IY, AND IZ SHOULD BE SET TO INTEGER VALUES BETWEEN
            // 1 AND 30000 BEFORE FIRST ENTRY.  THIS IS DONE IN PROCEDURE
            // setup_random, WHICH CONVERTS ISeed INTO IX, IY, AND IZ.
            //
            rn_ix = (171 * rn_ix) % 30269;
            rn_iy = (172 * rn_iy) % 30307;
            rn_iz = (170 * rn_iz) % 30323;

            return RealMod(double(rn_ix) / 30269.0 + double(rn_iy) / 30307.0 + double(rn_iz) / 30323.0, 1.0);
        }
    else
        GenAppInternalError("SC_Random::Random - unknown algorithm");
    return 0.0;
}

void SC_Random::SetupRandom(int iseed)
{
    if (algorithm == raWichmannHill)
        {
            SetupWH(iseed);
        }
    else
        GenAppInternalError("SC_Random::SetupRandom");
}


void SC_Random::SetupWH(int iseed)
{
    //
    // Convert a random seed into three seeds between 1 and 30000 as follows:
    //
    //    1. THE VALUE OF ISeed IS CONVERTED TO MOD 30000
    //    2. THE DIGITS OF ISeed ARE REVERSED AND COVERTED TO MOD 30000
    //    3. THE PRODUCT OF THE RESULTS OF (1) AND (2) IS CONVERTED TO MOD 30000
    //
    int idigit[10];
    //
    //    CHECK FOR NEGATIVE AND ZERO SEEDS
    //
    if (iseed > 1)
        {
            //
            //    PUT THE INDIVIDUAL DIGITS OF ISeed INTO THE VECTOR IDIGIT
            //
            int itemp = iseed;
            int ndigit = 0;
            while (itemp > 0)
                {
                    idigit[ndigit++] = itemp - (itemp / 10) * 10;
                    itemp /= 10;
                }
            //
            //    REVERSE THE DIGITS OF ISeed TO FORM A SECOND SEED
            //
            int iseed2 = 0;
            for (int i = 0; i < ndigit; i++)
                {
                    iseed2 += int(floor(Power(double(idigit[i] * 10), double(ndigit - i))));
                }
            //
            //    CONVERT ISeed AND ISeed2 TO INTEGERS FROM 1 TO 30000
            //    ALONG WITH THE RESULTANT PRODUCT OF THE CONVERSIONS
            //
            rn_ix = (iseed % 30000) + 1;
            rn_iy = (iseed2 % 30000) + 1;
            rn_iz = ((rn_ix * rn_iy) % 30000) + 1;
        }
    else
        {
            rn_ix = 1;
            rn_iy = 2;
            rn_iz = 3;
        }
}


void SC_Random::RandomMix(SC_IntArray& arr)
{
    int n = arr.Size();
    for (int j = 2; j <= n; j++)
        {
            int ir = n + 2 - j;
            double u = Random() * double(ir);

            int jj = int(u) + 1;
            if (jj > n)
                jj = n;

            int kk = arr[ir - 1];
            arr[ir - 1] = arr[jj - 1];
            arr[jj - 1] = kk;
        }
}

void SC_Random::RandomMix(SC_DoubleArray& arr)
{
    int nmix = arr.Size();
    SC_IntArray mix;
    mix.AllocAndIndex(nmix);
    RandomMix(mix);

    arr.Reorder(mix);
}

