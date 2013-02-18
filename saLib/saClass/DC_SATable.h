///////////////////////////////////////////////////////////////////////////////////
//
// DC_SATable.h
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

#ifndef DC_SATABLE_H
#define DC_SATABLE_H

#include <genClass/C_Common.h>

#include <genClass/DC_TableData.h>
#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_StringArray.h>

class DC_SATable {

    friend class IO_SATable;

    public:
        enum            {tableIDLen = 80};
        char            tableID[tableIDLen];

        enum    SATableComponent    {saVariable, saVector, saSteps};


        // QA & traceability
        char            originalFile[stdFileStrLen];
        enum            {procStepLen = 80};
        SC_StringArray  processingSteps;

        //  data
        SC_DoubleCube   saData;  // [page][col][row]
                                       // page - steps
                                       // col - variables
                                       // row - vectors

        enum            {varNameLen = 80};
        SC_StringArray  variableNames;
        SC_DoubleArray  stepValues;

        //  used to show indexes in original file
        SC_IntArray     variableIndexes;
        SC_IntArray     stepIndexes;
        SC_IntArray     vectorIndexes;

    public:
                    DC_SATable();
                    DC_SATable(const DC_SATable& a);
                    ~DC_SATable();

        DC_SATable&  operator=(const DC_SATable& a);


        bool        Alloc(int nStep, int nVar, int nVector);
                    // allocates AND sets all to size

        bool        Alloc(const DC_SATable& a); // allocs to match

        int         GetNType(SATableComponent   tableComp) const;

        void        GetTypeIDs(SATableComponent tableComp,
                               SC_StringArray& typeIDs) const;

        void        SetVariableName(int varIndex, const char* varText);
        const char* GetVariableName(int varIndex) const {return variableNames[varIndex];}

        void        GetVariableIDs(SC_StringArray& varIDs) const;
        void        GetStepIDs(SC_StringArray& stepIDs) const;

        int         GetNVariable() const;
        int         GetNVector() const;
        int         GetNStep() const;

        void        ClearData();    // fills cube and step with 0.0 to alloc size

                    // ID related
        void        SetID(const char id[]);
        void        GetID(char id[], int  maxIDlen);
        const char* GetID() const {return tableID;}

        const SC_DoubleArray&   GetVariableVector(int stepIndex, int varIndex) const;  // ref
        SC_DoubleArray&         GetVariableVector(int stepIndex, int varIndex);  // ref

        void        GetVariableVector(int stepIndex, int varIndex,
                               SC_DoubleArray& varData) const; // copy

        void        GetVectorVariables(int stepIndex, int vectorIndex,
                               SC_DoubleArray& vectorData) const;  // copy

        double      GetStepVal(int stepIndex) const {return stepValues[stepIndex];}

        void        SetInitialIndexes();

        void        CopyBasicInfo(const DC_SATable& a);

    private:
        void        DeAlloc();      // deallocation used by destructor and alloc
        void        LocalCopy(const DC_SATable& a);
};

#endif // !DC_SATABLE_H

