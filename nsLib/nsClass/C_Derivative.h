///////////////////////////////////////////////////////////////////////////////////
//
// C_Derivative.h
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

#ifndef C_DERIVATIVE_H
#define C_DERIVATIVE_H

#include <genClass/SC_SetupErr.h>

namespace Derivative {
        //  all usual combinations
        enum DerivativeType   {  dt_dY_dX,                // d(Y)/d(X)
                                 dt_dY_dlog10X,           // d(Y)/d(log10X)
                                 dt_dlog10Y_dlog10X,      // d(log10(Y))/d(log10(X))
                                 dt_dY_dlnX               // d(Y)/d(ln(X))
                                };

        enum DerivativeCalcType {   dctSimple,            // adjacent points
                                    dct2SlopeAverage,     // 3 point/2 slope average
                                    dctFixedWindow,       // fixed span window
                                    dctLogPctWindow,      // % log span window
                                    dctLinPctWindow,      // % log span window
                                    dctLogValWindow,      //  log span value    
                                    dctLinValWindow       //  lin span value    
                                };

        enum WindowCalcType     {   wctRegress,           // linear regress on all points in window
                                    wctClark,             // Clark 85 algorithm
                                    wctSimmons            // Simmons algorithm
                                };

        enum TimeMultType       {   tmtNone,              // output X is input X
                                    tmtTime,              // inputX * time
                                    tmtDeltaTime          // ??
                                };
};

using namespace Derivative;


class   DerivativeDescription {
    public:
        DerivativeType          derivativeType;
        DerivativeCalcType      derivativeCalcType;
        WindowCalcType          windowCalcType;     // for all but dctBetween, dctSimple
        TimeMultType            timeMultType;

        bool                    useSuperTime;       // calc deriv from super time before deriv
        int                     windowPtsSpan;      // for dctFixedWindow
        double                  windowPctSpan;      // for dctLogPct/LinWindow
        double                  windowValSpan;      // for dctLogVal/LinWindow

        bool                    absOutputY;         // always +ve output added v1

        double                  logEps;             // added v2 for double deriv noise fix
        double                  offsetY;

    public:
                                DerivativeDescription();
                                DerivativeDescription(const DerivativeDescription& a);
        virtual                 ~DerivativeDescription();
        DerivativeDescription&  operator= (const DerivativeDescription& a);

        bool                    DerivativeSetupOK(int      ninputPoints,
                                                  SC_SetupErr& errData);      // returns true if basic setup is OK

    private:
        void                    LocalCopy(const DerivativeDescription& a);  // implementation of copy

};


#endif // C_DERIVATIVE_H
