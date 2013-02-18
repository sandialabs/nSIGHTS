///////////////////////////////////////////////////////////////////////////////////
//
// DO_CalcParReal.h
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
//      OsLib supports sampling and processing/plotting of optimization output
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DO_CALCPARREAL_H
#define DO_CALCPARREAL_H

#include <genClass/DO_Real.h>

// class for bundling DO_Real derived type as calculated parameter

class DO_CalcParReal : public DO_Real {
    private:
        static char* typeDesc;
    public:
        enum        {descLen = 40};
        char        shortID[descLen];
        char        unitText[descLen];
        bool        isLinear;
        bool        valueSet;

    public:
                    DO_CalcParReal(const char* fullID, const char* shID);
                    ~DO_CalcParReal(){};

        bool        DataOK();
        char*       GetType() const {return typeDesc;}

        void        SetCalcValue(const double& calcVal);
        void        SetCalcUnits(const char* calcUnits);

        void        CalcInit();

    private:
                        DO_CalcParReal() {}
                        DO_CalcParReal(const DO_CalcParReal& a) {}
        DO_CalcParReal& operator= (const DO_CalcParReal& a) {return *this;}
};


#endif // !DO_CALCPARREAL_H

