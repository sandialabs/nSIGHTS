///////////////////////////////////////////////////////////////////////////////////
//
// SC_Random.h
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

#ifndef SC_RANDOM_H
#define SC_RANDOM_H

#include <genClass/SC_IntArray.h>

// 2 algorithms:
//      ALGORITHM AS 183 APPL. STATIST. (1982) VOL. 31, NO. 2
//      B. A. WICHMANN AND I. D. HILL

class SC_DoubleArray;

class SC_Random {
public:
    enum    Algorithm {raWichmannHill};
private:
    Algorithm   algorithm;

    //      WichmannHill parameters
    int         rn_ix;
    int         rn_iy;
    int         rn_iz;


    SC_IntArray     ma;
    int             inext;
    int             inextp;
    bool        initialized;

public:
    SC_Random(Algorithm     inAlg = raWichmannHill); // default for compatibility
    SC_Random(const SC_Random& a);

    SC_Random&  operator= (const SC_Random& a);

    double      Random();  // returns a random number rectangularly distributed between 0 and 1.

    void        SetupRandom(int iseed);

    void        RandomMix(SC_IntArray& arr);  // generates mixing array

    void        RandomMix(SC_DoubleArray& arr);  // mixes randomly

private:
    void        SetupWH(int iseed);
    void        LocalCopy(const SC_Random& a);

};

#endif // SC_RANDOM_H

