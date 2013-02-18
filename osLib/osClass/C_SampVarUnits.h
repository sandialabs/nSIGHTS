///////////////////////////////////////////////////////////////////////////////////
//
// C_SampVarUnits.h
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
//      variable to be sampled.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_SAMPVARUNITS_H
#define C_SAMPVARUNITS_H

#include <osClass/C_SampVar.h>
#include <genUnits/C_GenUnits.h>


class UnitListObj;

// adds unit index reference to the base sample class
class SampVarUnits  :  public SampVar {

    public:
        UnitIndex&              unitIndex;

    public:
                                SampVarUnits(bool& isActiveInit, int corrGroupInit,
                                             const char* varIDInit, UnitIndex& unitIndexInit, bool defIsLinear = true);
                                SampVarUnits(bool& isActiveInit, int corrGroupInit,
                                             const char* varShortIDInit, const char* varLongIDInit,
                                             UnitIndex& unitIndexInit, bool defIsLinear = true);

        SampVarUnits&           operator=(const SampVarUnits& a);
                                // for convenience
        double                  MetricToUser(const double& metricVal) {return unitIndex.MetricToUser(metricVal);}
        double                  UserToMetric(const double& userVal) {return unitIndex.UserToMetric(userVal);}

        //  does unit conversion with special override for Poisson distribution
        virtual double          GetMean() const;  // virtual for override in units
        virtual double          GetMedian() const;  // virtual for override in units
        virtual double          GetStdDev() const;  // virtual for override in units
        virtual double          GetLogMean() const;  // virtual for override in units
        virtual double          GetLogStdDev() const;  // virtual for override in units
        virtual double          GetFinalValue(int sampleNum) const;
        virtual double          GetCurrentValue(int sampleNum) const;  // base class gets internal sampled value
        virtual double          GetMinVarValue() const;
        virtual double          GetMaxVarValue() const;
        //  to process units correctly when using secondary samples
        virtual   double        GetCurrentValue(const double& userValue) const;    // base class gets internal sampled value

        virtual void            MakeUnitString(char unitStr[], int maxLen);

        void                    AddToListing(UnitListObj& listObj) const;

    private:
                                // copy ctor not allowed
                                SampVarUnits(const SampVarUnits& a);

        double                  GetMetricValue(const double& inVal) const;

};


#endif // !C_SAMPVARUNITS_H

