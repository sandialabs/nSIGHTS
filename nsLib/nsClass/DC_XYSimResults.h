///////////////////////////////////////////////////////////////////////////////////
//
// DC_XYSimResults.h
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
//      File I/O for DC_XYSimResults.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_XYSIMRESULTS_H
#define DC_XYSIMRESULTS_H

#include <objClass/DC_XYDataArray.h>
#include <genClass/SC_StringArray.h>
#include <nsClass/DC_SequenceTimes.h>

// results for a single case
class XYSimCaseResults {
    public:
        enum                {caseIDLen = 80};
        char                caseID[caseIDLen];

        int                 caseIndex;       // added for cluster support to
                                             // combine multiple run results

        double              staticPressure;  // always output for each case
        SC_DoubleArray      caseValues;      // sim varying values -- may be empty

        DC_XYDataArray      caseData;        // the actual output

    public:
                            XYSimCaseResults();
                            XYSimCaseResults(const XYSimCaseResults& a);
                            ~XYSimCaseResults();

        XYSimCaseResults&   operator=(const XYSimCaseResults& a);
        int                 GetnXY() const {return caseData.Size();}

    private:
        void                LocalCopy(const XYSimCaseResults& a);
};


class XYSimRunResults : public T_SC_Array<XYSimCaseResults> {
    public:
        enum                {runIDLen = 40};
        char                runID[runIDLen];
        SC_StringArray      caseValDesc;         // descriptions for case varying values
        SC_StringArray      caseValShortDesc;    // abbreviated descriptions for case varying values
        DC_SequenceTimes    caseSeqData;         // sequences for run

    public:
                            XYSimRunResults();
                            XYSimRunResults(const XYSimRunResults& a);
                            ~XYSimRunResults();
        XYSimRunResults&    operator=(const XYSimRunResults& a);

        int                 GetnCases() const {return Size();}
        int                 GetnXY() const;
        int                 GetnCaseVals() const {return caseValDesc.Size();}
        void                GetCaseIDs(SC_StringArray&  caseIDs) const;

    private:
        void                LocalCopy(const XYSimRunResults& a);
};


class DC_XYSimResults : public T_SC_Array<XYSimRunResults> {


    public:
                            DC_XYSimResults();
                            DC_XYSimResults(const DC_XYSimResults& a);
                            ~DC_XYSimResults();

        DC_XYSimResults&    operator=(const DC_XYSimResults& a);

        int                 GetnRuns() const {return Size();}
        int                 GetnCases() const;
        int                 GetnXY() const;

        void                GetRunIDs(SC_StringArray&  runIDs) const;
        void                GetCaseIDs(SC_StringArray&  caseIDs) const;
        void                GetXYIDs(SC_StringArray&  xyIDs) const;

    private:
        void                LocalCopy(const DC_XYSimResults& a);

};

#endif // DC_XYSIMRESULTS_H

