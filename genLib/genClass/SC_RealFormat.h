///////////////////////////////////////////////////////////////////////////////////
//
// SC_RealFormat.h
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

#ifndef SC_REALFORMAT_H
#define SC_REALFORMAT_H

#ifdef _WIN32
#include <afx.h>
#endif



//  ***************************************************  real formatting
//  numeric conversion -- follows spirit of pascal code from GTFM

enum NumConvFormat {ncf_General, ncf_Decimal, ncf_Scientific,
                    ncf_Scientific2, // added with sub/super support
                    ncf_Exponent,    // power 10 in super format
                    ncf_Exponent2,
                    ncf_DecimalSpace};  // decimal 0 with space between each group of 3 (e.g. 1 000 000)

class SC_RealFormat {
public:
    NumConvFormat   format;
    int             digitsPrecision;

    static bool         european;  // if true dec delimiter is ,

public:
    // all permutations on overloaded constructors
    SC_RealFormat() {format = ncf_General; digitsPrecision = 5;}
    SC_RealFormat(NumConvFormat ncf) {format = ncf; digitsPrecision = 5;}
    SC_RealFormat(NumConvFormat ncf, int digP) {format = ncf; digitsPrecision = digP;}
    SC_RealFormat(int digP) {format = ncf_Decimal; digitsPrecision = digP;}
    ~SC_RealFormat(){};

    //  copy
    SC_RealFormat(const SC_RealFormat& a) {format = a.format; digitsPrecision = a.digitsPrecision;}
    SC_RealFormat&  operator= (const SC_RealFormat& a);

    // main conversion routine
    bool    RealToString(const double& rVal,
                         char          str[],
                         int           maxLen) const;

#ifdef _WIN32
    bool    RealToString(const double& rVal,
                         CString&      str) const;

#endif


    // access to equivalent C format strings
    char*   GetCformat() const;

private:
    static  bool StarString (char inStr[],
                             char outStr[],
                             int  maxLen);

    static void  SciConversion (char             inStr[],
                                const double&    rVal,
                                int              n_digits,
                                int              maxLen);

    static bool  FixedConversion (char             inStr[],
                                  const double&    rVal,
                                  int              n_digits,
                                  int              maxLen);

    static void  DefaultConversion (char            inStr[],
                                    const double&   rVal,
                                    int             maxLen);

    // post processes sci conversion
    static void  Sci2Conversion (char             inStr[],
                                 int              maxLen);

    static void  ExpConversion (char             inStr[],
                                int              maxLen);
    static void  Exp2Conversion (char             inStr[],
                                 int              maxLen);

    static bool  GetExponent(char       inStr[],
                             double&    mVal,
                             int&       eVal);

    static void  AddDecimalSpaces(char            inStr[],
                                  int             prec,
                                  int             maxLen);

};

//  derived classes just add new constructors for ease of use
class SC_SciFormat : public SC_RealFormat {

public:
    SC_SciFormat() {format = ncf_Scientific; digitsPrecision = 5;}
    SC_SciFormat(int digP) {format = ncf_Scientific; digitsPrecision = digP;}
};
class SC_DecFormat : public SC_RealFormat {

public:
    SC_DecFormat() {format = ncf_Decimal; digitsPrecision = 5;}
    SC_DecFormat(int digP) {format = ncf_Decimal; digitsPrecision = digP;}
};

#endif // SC_REALFORMAT_H

