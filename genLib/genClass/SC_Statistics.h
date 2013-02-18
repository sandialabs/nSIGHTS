///////////////////////////////////////////////////////////////////////////////////
//
// SC_Statistics.h
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
// DESCRIPTION: Generic support classes: performs univariate statistics calculations
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef SC_STATISTICS_H
#define SC_STATISTICS_H

#include <genClass/SC_DoubleArray.h>
#include <genClass/T_SC_Array.h>

class SC_Statistics {
public:
    enum            {maxStatisticType = 21};        // including nSights specific
    enum            {maxNormStatisticType = 13};    // usual

    enum StatisticOp {
        //    std univariate stats
        soSum            = 0,
        soMin            = 1,
        soMax            = 2,
        soMean           = 3,
        soVar            = 4,
        soStdDev         = 5,
        soRange          = 6,
        soN              = 7,
        soNnull          = 8,
        soNnegZero       = 9,       // added for SyView 1.2 - Mar06
        soPercentNonNull = 10,
        soMedian         = 11,
        soMode           = 12,

        // nSights additions
        soMinAbs         = 13,
        soMinPos         = 14,
        soMinIndx        = 15,
        soMaxIndx        = 16,
        soMinAbsIndx     = 17,
        soMinPosIndx     = 18,

        //some late additions
        soMeanGeo        = 19,  //same as log-arithmetic
        soMeanHarm       = 20,
    };


    static const  char*   opIDs[maxStatisticType];

    double                realResults[maxStatisticType];
    double                intResults[maxStatisticType];  // only soMinIndx to soMinPosIndx
    bool                  resultsOK;

private:
    int                   nOK;
    int                   nPositive;  //for goemetric and harmonic mean

public:
    SC_Statistics();
    SC_Statistics(const SC_Statistics& a);

    SC_Statistics&      operator=(const SC_Statistics& a);

    void                CalcStatistics(const SC_DoubleArray& data,
                                       bool logData = false,
                                       bool baseTen = true);
    // if log data then only +ve values are processed
    // and sum, stdDev, and var are based on log10 data
    // if baseTen = false then natural log  (added by NED)

    // extra calc because sorting required
    // array is sorted on exit
    // assumes CalcStatistics was already called
    void                CalcMedian(SC_DoubleArray& data);
    void                CalcMode(const SC_DoubleArray& data);

    static bool         IsInteger(int indx);

    // to access results after CalcStatistics has run
    //this type of access might be useful in the future, but
    //I will comment out for now
    /*
      const double&       GetSum() {return realResults[soSum];}
      const double&       GetMin() {return realResults[soMin];}
      const double&       GetMax() {return realResults[soMax];}
      const double&       GetMean() {return realResults[soMean];}
      const double&       GetVar() {return realResults[soVar];}
      const double&       GetStdDev() {return realResults[soStdDev];}
      const double&       GetRange() {return realResults[soRange];}
      const double&       GetN() {return realResults[soN];}
      const double&       GetNnull() {return realResults[soNnull];}
      const double&       GetNnegZero() {return realResults[soNnegZero];}
      const double&       GetPercentNonNull() {return realResults[soPercentNonNull];}
      const double&       GetMedian() {return realResults[soMedian];}
      const double&       GetMode() {return realResults[soMode];}

      const double&       GetMinAbs() {return realResults[soMinAbs];}
      const double&       GetMinPos() {return realResults[soMinPos];}
      const double&       GetMinIndx() {return realResults[soMinIndx];}
      const double&       GetMaxIndx() {return realResults[soMaxIndx];}
      const double&       GetMinAbsIndx() {return realResults[soMinAbsIndx];}
      const double&       GetMinPosIndx() {return realResults[soMinPosIndx];}
    */



protected:
    void                Init();

private:
    void                LocalCopy(const SC_Statistics& a);

};


typedef T_SC_Array<SC_Statistics> SC_StatisticsArray;

#endif // SC_STATISTICS_H

