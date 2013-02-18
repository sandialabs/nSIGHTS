///////////////////////////////////////////////////////////////////////////////////
//
// DC_SATable.cpp
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

#include <genClass/C_Common.h>
#include <genClass/SC_RealFormat.h>

#include <genClass/U_Str.h>

#include <genClass/U_Msg.h>

#include <saClass/DC_SATable.h>

DC_SATable :: DC_SATable()
{
    SetToNull(tableID);
    SetToNull(originalFile);
    processingSteps.SetResizable();
}

DC_SATable::DC_SATable(const DC_SATable& a)
{
    LocalCopy(a);
}


DC_SATable&  DC_SATable::operator=(const DC_SATable& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }

    return *this;
}

void DC_SATable::CopyBasicInfo(const DC_SATable& a)
{
    processingSteps = a.processingSteps;
    stepValues      = a.stepValues;
    CopyString(tableID, a.tableID, tableIDLen);
    CopyString(originalFile, a.originalFile, stdFileStrLen);
}

void DC_SATable::LocalCopy(const DC_SATable& a)
{
    CopyBasicInfo(a);

    saData  = a.saData;
    variableNames   = a.variableNames;
    variableIndexes = a.variableIndexes;
    stepIndexes     = a.stepIndexes;
    vectorIndexes   = a.vectorIndexes;
}


DC_SATable :: ~DC_SATable()
{
}

void DC_SATable::DeAlloc()
{
    saData.DeAlloc();
    variableNames.DeAlloc();
    stepValues.DeAlloc();
    processingSteps.DeAlloc();
    variableIndexes.DeAlloc();
    stepIndexes.DeAlloc();
    vectorIndexes.DeAlloc();

    SetToNull(tableID);
    SetToNull(originalFile);
}


bool DC_SATable::Alloc(int nStep, int nVar, int nVector)
{
    try
    {
        saData.CubeAllocAndSetSize(nStep, nVar, nVector);
        stepValues.AllocAndFill(nStep, 0.0);

        variableIndexes.AllocAndFill(nVar, -1);
        stepIndexes.AllocAndFill(nStep, -1);
        vectorIndexes.AllocAndFill(nVector, -1);

        //  set char lens for descs
        variableNames.ForceMinStringLen(varNameLen);
        variableNames.AllocAndSetSize(nVar);

        processingSteps.ForceMinStringLen(procStepLen);
        processingSteps.SetResizable(8);
    }
    catch (SC_Array::AllocationError)
    {
        DeAlloc();
        return false;
    }

    return true;
}


bool DC_SATable::Alloc(const DC_SATable& a)
{
    return Alloc(a.GetNStep(), a.GetNVariable(), a.GetNVector());
}

void DC_SATable::SetInitialIndexes()
{
    variableIndexes.AllocAndIndex(GetNVariable());
    stepIndexes.AllocAndIndex(GetNStep());
    vectorIndexes.AllocAndIndex(GetNVector());
}

int DC_SATable::GetNType(SATableComponent   tableComp) const
{
    switch (tableComp) {
        case saVariable : {
            return GetNVariable();
        }
        case saVector : {
            return GetNVector();
        }
        case saSteps : {
            return GetNStep();
        }
    }
    return -1;
}

void DC_SATable::GetTypeIDs(SATableComponent    tableComp,
                                   SC_StringArray& typeIDs) const
{
    switch (tableComp) {
        case saVariable : {
            GetVariableIDs(typeIDs);
            break;
        }
        case saVector : {
            typeIDs.AllocAndSetSize(GetNVector());
            typeIDs.SetIndexed("Obs#", 1);
            break;
        }
        case saSteps : {
            GetStepIDs(typeIDs);
            break;
        }
    }
}


void DC_SATable::GetVariableIDs(SC_StringArray& varIDs) const
{
    varIDs = variableNames;
}

void DC_SATable::GetStepIDs(SC_StringArray& stepIDs) const
{
    stepIDs.Alloc(GetNStep());
    SC_DecFormat rForm(3);
    for (int i = 0; i < GetNStep(); i++)
    {
        char outStr[20];
        rForm.RealToString(stepValues[i], outStr, 20);
        stepIDs += outStr;
    }
}
void DC_SATable::SetVariableName(int varIndex, const char* varText)
{
    variableNames.SetString(varText, varIndex);
}

int DC_SATable::GetNVariable() const
{
    if (saData.IsEmpty())
        return 0;
    return saData[0].Size();
}

int  DC_SATable::GetNVector() const
{
    if (saData.IsEmpty() || saData[0].IsEmpty())
        return 0;
    return saData[0][0].Size();
}

int DC_SATable::GetNStep() const
{
    return saData.Size();
}

void DC_SATable::ClearData()
{
    stepValues.FillToAlloc(0.0);
    for (int i = 0; i < saData.AllocSize(); i++)
    {
        SC_DoubleMatrix& currStep = saData[i];
        for (int j = 0; j < currStep.AllocSize(); j++)
            currStep[j].FillToAlloc(0.0);
    }
}

void DC_SATable::SetID(const char id[])
{
    CopyString(tableID, id, tableIDLen);
}

void DC_SATable::GetID(char id[],
                      int  maxIDlen)
{
    CopyString(id, tableID, maxIDlen);
}


const SC_DoubleArray& DC_SATable::GetVariableVector(int stepIndex, int varIndex) const
{
    return saData[stepIndex][varIndex];
}

SC_DoubleArray& DC_SATable::GetVariableVector(int stepIndex, int varIndex)
{
    return saData[stepIndex][varIndex];
}

void DC_SATable::GetVariableVector(int stepIndex, int varIndex,
                                   SC_DoubleArray& varData) const
{
    varData = saData[stepIndex][varIndex];
}

void DC_SATable::GetVectorVariables(int stepIndex, int vectorIndex,
                                   SC_DoubleArray& vectorData) const
{
    int nvec = GetNVector();
    vectorData.AllocAndSetSize(nvec);
    for (int i = 0; i < nvec; i++)
        vectorData[i] = saData[stepIndex][i][vectorIndex];
}

