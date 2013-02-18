///////////////////////////////////////////////////////////////////////////////////
//
// DC_RangeSimResults.h
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

#ifndef DC_RANGESIMRESULTS_H
#define DC_RANGESIMRESULTS_H

#include <genClass/SC_StringArray.h>

#include <objClass/DC_DataCoord.h>
#include <objClass/DC_GridData.h>
#include <objClass/DC_CubeData.h>

// results for a single case
class RangeSimOutput {
    public:
        enum                {outputIDLen = 80};
        char                outputID[outputIDLen];

        SC_DoubleArray      gridCubeData;

    public:
                            RangeSimOutput();
                            RangeSimOutput(const RangeSimOutput& a);
                            ~RangeSimOutput();

        RangeSimOutput&     operator=(const RangeSimOutput& a);

    private:
        void                LocalCopy(const RangeSimOutput& a);
};


class RangeSimRunResults : public T_SC_Array<RangeSimOutput> {
    public:
        enum                {runIDLen = 40};
        enum                {fullIDLen = runIDLen + RangeSimOutput::outputIDLen + 2};

        char                runID[runIDLen];
        bool                runIsCube;

        DC_DataCoord        xData;
        DC_DataCoord        yData;
        DC_DataCoord        zData;

    public:
                            RangeSimRunResults();
                            RangeSimRunResults(const RangeSimRunResults& a);
                            ~RangeSimRunResults();
        RangeSimRunResults& operator=(const RangeSimRunResults& a);

        int                 GetnOutput() const {return Size();}
        void                GetOutputIDs(SC_StringArray&  outputIDs)  const;

        bool                GetCubeData(int          index,
                                        DC_CubeData& cubeData)        const;
        bool                GetGridData(int          index,
                                        DC_GridData& gridData)        const;


    private:
        void                LocalCopy(const RangeSimRunResults& a);
        void                MakeID(char* idStr, int index, int idLen) const;
};


class DC_RangeSimResults : public T_SC_Array<RangeSimRunResults> {
    public:
        bool                rangeIsCube;

    public:
                            DC_RangeSimResults();
                            DC_RangeSimResults(const DC_RangeSimResults& a);
                            ~DC_RangeSimResults();

        DC_RangeSimResults& operator=(const DC_RangeSimResults& a);

        inline bool         IsCubes() const {return rangeIsCube;}

        int                 GetnRuns() const {return Size();}
        int                 GetNOutput() const;
        int                 GetnCubes() const;
        int                 GetnGrids() const;

        void                GetRunIDs(SC_StringArray&  runIDs) const;
        void                GetMenuStrings(SC_StringArray&  outputIDs) const;

    private:
        void                LocalCopy(const DC_RangeSimResults& a);

};

#endif // !DC_RANGESIMRESULTS_H

