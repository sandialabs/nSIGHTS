///////////////////////////////////////////////////////////////////////////////////
//
// DC_UncertSimResults.cpp
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

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/SC_RealFormat.h>

#include <osClass/DC_UncertSimResults.h>

UncertRunResults::UncertRunResults()
{
    SetToNull(runID);
    multipleRunScenarioIndex = -1;
}

UncertRunResults::UncertRunResults(const UncertRunResults& a)
{
    LocalCopy(a);
}

UncertRunResults::~UncertRunResults()
{
}

UncertRunResults& UncertRunResults::operator=(const UncertRunResults& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void UncertRunResults::LocalCopy(const UncertRunResults& a)
{
    CopyString(runID, a.runID, runIDLen);
    multipleRunVariables = a.multipleRunVariables;
    resultTables = a.resultTables;

    multipleRunScenarioIndex = a.multipleRunScenarioIndex;
}


void UncertRunResults::DeAlloc()
{
    resultTables.DeAlloc();
}

void UncertRunResults::GetTableIDs(SC_StringArray&  resultIDs) const
{
    resultTables.GetIDs(resultIDs);
};


DC_UncertSimResults::DC_UncertSimResults() : T_SC_Array<UncertRunResults>()
{
    CopyString(resultID, "result", resultIDLen);
    isMultipleRun = false;
    multipleRunVariableIDs.SetStringLen(80);
    multipleRunVariableIDs.SetResizable(16);
}

DC_UncertSimResults::DC_UncertSimResults(const DC_UncertSimResults& a) :
    T_SC_Array<UncertRunResults>(a)
{
    LocalCopy(a);
}

DC_UncertSimResults::~DC_UncertSimResults()
{
}

DC_UncertSimResults& DC_UncertSimResults::operator=(const DC_UncertSimResults& a)
{
    if (&a != this)
    {
        T_SC_Array<UncertRunResults>::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void DC_UncertSimResults::LocalCopy(const DC_UncertSimResults& a)
{
    CopyString(resultID, a.resultID, resultIDLen);

    isMultipleRun = a.isMultipleRun;
    multipleRunVariableIDs = a.multipleRunVariableIDs;
}

int DC_UncertSimResults::GetnTables() const
{
    int ntables = 0;
    for (int i = 0; i < Size(); i++)
        ntables += (*this)[i].GetnTables();
    return ntables;
}

void  DC_UncertSimResults::GetRunIDs(SC_StringArray&  runIDs) const
{
    runIDs.ForceMinStringLen(UncertRunResults::runIDLen);
    runIDs.Alloc(Size());
    for (int i = 0; i < Size(); i++)
        runIDs += (*this)[i].runID;
};

void DC_UncertSimResults::GetTableIDs(SC_StringArray&  tableIDs) const
{
    static const int maxStrLen = 120;

    tableIDs.ForceMinStringLen(maxStrLen);
    tableIDs.Alloc(GetnTables());

    char uncertTypeStr[maxStrLen];
    if (uncertType == utFOSM)
        CopyString(uncertTypeStr, "FOSM", maxStrLen);
    else if (uncertType == utPEM)
        CopyString(uncertTypeStr, "PEM", maxStrLen);
    else
        CopyString(uncertTypeStr, "unknown", maxStrLen);

    char tempStr[maxStrLen];

    for (int i = 0; i < Size(); i++)
    {
        UncertRunResults& currRun = (*this)[i];
        DC_TableArray& resultTable = currRun.resultTables;

        for (int j = 0; j < resultTable.Size(); j++)
        {
            CopyString(tempStr, currRun.runID, maxStrLen);
            ConcatString(tempStr, "\t", maxStrLen);
            ConcatString(tempStr, resultTable[j].tableID, maxStrLen);
            ConcatString(tempStr, "\t", maxStrLen);
            ConcatString(tempStr, uncertTypeStr, maxStrLen);
            tableIDs += tempStr;
        }
    }
}

DC_TableData* DC_UncertSimResults::GetTable(int index)
{
    for (int i = 0; i < Size(); i++)
    {
        UncertRunResults& currRun = (*this)[i];
        DC_TableArray& resultTable = currRun.resultTables;
        if (index < resultTable.Size())
            return &(resultTable[index]);
        index -= resultTable.Size();
    }

    return 0;
}

void DC_UncertSimResults::GetMultipleRunTable(DC_TableData& mrunTable)
{
    if ((!isMultipleRun) || IsEmpty() || multipleRunVariableIDs.IsEmpty())
    {
        mrunTable.ClearAll();
        return;
    }

    int ncol = multipleRunVariableIDs.Size();
    mrunTable.Alloc(ncol, Size(), multipleRunVariableIDs.GetStringLen());

    mrunTable.columnDesc =  multipleRunVariableIDs;
    mrunTable.SetSizeToAlloc();

    for (int i = 0; i < Size(); i++)
    {
        const UncertRunResults& currRun = (*this)[i];
        mrunTable.rowDesc.SetString(currRun.runID, i);

        for (int j = 0; j < ncol; j++)
            mrunTable.dataTable[j][i] = currRun.multipleRunVariables[j];
    }

    mrunTable.SetID(resultID);

}



