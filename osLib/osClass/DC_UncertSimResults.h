///////////////////////////////////////////////////////////////////////////////////
//
// DC_UncertSimResults.h
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

#ifndef DC_UNCERTSIMRESULTS_H
#define DC_UNCERTSIMRESULTS_H

#include <genClass/DC_TableData.h>
#include <genClass/SC_StringArray.h>
#include <genClass/SC_BoolArray.h>

#include <genClass/DC_TableArray.h>

// results for a FOSM or PEM uncertainty
class UncertRunResults  {
    public:
        enum                        {runIDLen = 40};
        char                        runID[runIDLen];

        // scenario support - added Feb 04
        SC_DoubleArray              multipleRunVariables;
        int                         multipleRunScenarioIndex; // for MPI support

        DC_TableArray               resultTables;

    public:
                            UncertRunResults();
                            UncertRunResults(const UncertRunResults& a);
                            ~UncertRunResults();
        UncertRunResults&   operator=(const UncertRunResults& a);

        int                 GetnTables() const {return resultTables.Size();}

        void                GetTableIDs(SC_StringArray&  resultIDs) const;

        void                DeAlloc();

    private:
        void                LocalCopy(const UncertRunResults& a);
};




class DC_UncertSimResults : public T_SC_Array<UncertRunResults> {

    public:
        enum                {resultIDLen = 80};
        char                resultID[resultIDLen];

        enum                UncertType {utFOSM, utPEM};
        UncertType          uncertType;

        bool                isMultipleRun;
        SC_StringArray      multipleRunVariableIDs;

    public:
                            DC_UncertSimResults();
                            DC_UncertSimResults(const DC_UncertSimResults& a);
                            ~DC_UncertSimResults();

        DC_UncertSimResults&    operator=(const DC_UncertSimResults& a);

        inline bool         IsMultiple() const {return isMultipleRun;}
        int                 GetnRuns() const {return Size();}
        int                 GetnTables() const;

        void                GetRunIDs(SC_StringArray&  runIDs) const;
        void                GetTableIDs(SC_StringArray&  tableIDs) const;

        DC_TableData*       GetTable(int index);

        void                GetMultipleRunTable(DC_TableData& mrunTable);

    private:
        void                LocalCopy(const DC_UncertSimResults& a);

};

#endif // !DC_UNCERTSIMRESULTS_H

