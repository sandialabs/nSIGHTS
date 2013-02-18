///////////////////////////////////////////////////////////////////////////////////
//
// DC_RangeSimResults.cpp
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

#include <osClass/DC_RangeSimResults.h>

RangeSimOutput::RangeSimOutput()
{
    outputID[0] = '\0';
}

RangeSimOutput::RangeSimOutput(const RangeSimOutput& a)
{
    LocalCopy(a);
}

RangeSimOutput::~RangeSimOutput()
{
}

RangeSimOutput& RangeSimOutput::operator=(const RangeSimOutput& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void RangeSimOutput::LocalCopy(const RangeSimOutput& a)
{
    CopyString(outputID, a.outputID, outputIDLen);
    gridCubeData = a.gridCubeData;
}


RangeSimRunResults::RangeSimRunResults() : T_SC_Array<RangeSimOutput>()
{
    runID[0] = '\0';
    runIsCube = false;
}

RangeSimRunResults::RangeSimRunResults(const RangeSimRunResults& a) :
    T_SC_Array<RangeSimOutput>(a)
{
    LocalCopy(a);
}

RangeSimRunResults::~RangeSimRunResults()
{
}

RangeSimRunResults& RangeSimRunResults::operator=(const RangeSimRunResults& a)
{
    if (&a != this)
    {
        T_SC_Array<RangeSimOutput>::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void RangeSimRunResults::LocalCopy(const RangeSimRunResults& a)
{
    CopyString(runID, a.runID, runIDLen);

    runIsCube = a.runIsCube;
    xData     = a.xData;
    yData     = a.yData;
    zData     = a.zData;
}

void RangeSimRunResults::GetOutputIDs(SC_StringArray&  outputIDs) const
{
    outputIDs.DeAlloc();
    outputIDs.SetStringLen(RangeSimOutput::outputIDLen);
    outputIDs.Alloc(Size());
    for (int i = 0; i < Size(); i++)
        outputIDs += (*this)[i].outputID;
};

void RangeSimRunResults::MakeID(char* idStr, int index, int idLen) const
{
    CopyString(idStr, runID, idLen);
    ConcatString(idStr, "::", idLen);
    ConcatString(idStr, (*this)[index].outputID, idLen);
}

bool RangeSimRunResults::GetCubeData(int          index,
                                     DC_CubeData& cubeData) const
{
    if (!runIsCube)
        return false;

    if (!cubeData.Alloc(xData.Size(), yData.Size(), zData.Size()))
        return false;


    char tempStr[fullIDLen];
    MakeID(tempStr, index, fullIDLen);
    cubeData.SetID(tempStr);
    cubeData.xData = xData;
    cubeData.yData = yData;
    cubeData.zData = zData;

    SC_DoubleArray& cData = (*this)[index].gridCubeData;
    for ( int i = 0; i < cubeData.Size(); i++)
        cubeData[i] = cData[i];

    return true;
}

bool RangeSimRunResults::GetGridData(int          index,
                                     DC_GridData& gridData) const
{
    if (runIsCube)
        return false;

    if (!gridData.Alloc(xData.Size(), yData.Size()))
        return false;


    char tempStr[fullIDLen];
    MakeID(tempStr, index, fullIDLen);
    gridData.SetID(tempStr);
    gridData.xData = xData;
    gridData.yData = yData;

    SC_DoubleArray& gData = (*this)[index].gridCubeData;
    for (int i = 0; i < gridData.Size(); i++)
        gridData[i] = gData[i];

    return true;


}

DC_RangeSimResults::DC_RangeSimResults() : T_SC_Array<RangeSimRunResults>()
{
}

DC_RangeSimResults::DC_RangeSimResults(const DC_RangeSimResults& a) :
    T_SC_Array<RangeSimRunResults>(a)
{
    LocalCopy(a);
}

DC_RangeSimResults::~DC_RangeSimResults()
{
}

DC_RangeSimResults& DC_RangeSimResults::operator=(const DC_RangeSimResults& a)
{
    if (&a != this)
    {
        T_SC_Array<RangeSimRunResults>::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void DC_RangeSimResults::LocalCopy(const DC_RangeSimResults& a)
{
}

void  DC_RangeSimResults::GetRunIDs(SC_StringArray&  runIDs) const
{
    runIDs.ForceMinStringLen(RangeSimRunResults::runIDLen);
    runIDs.Alloc(Size());
    for (int i = 0; i < Size(); i++)
        runIDs += (*this)[i].runID;
};


int DC_RangeSimResults::GetNOutput() const
{
    int nData = 0;
    for (int i = 0; i < Size(); i++)
        nData += (*this)[i].Size();
    return nData;
}


int DC_RangeSimResults::GetnCubes() const
{
    if (!rangeIsCube)
        return 0;
    return GetNOutput();
}

int DC_RangeSimResults::GetnGrids() const
{
    if (rangeIsCube)
        return 0;
    return GetNOutput();
}


void DC_RangeSimResults::GetMenuStrings(SC_StringArray&  outputIDs) const
{
    outputIDs.ForceMinStringLen(RangeSimRunResults::fullIDLen);
    outputIDs.Alloc(GetNOutput());
    char tempStr[RangeSimRunResults::fullIDLen];

    for (int i = 0; i < Size(); i++)
    {
        RangeSimRunResults& currRun = (*this)[i];
        for (int j = 0; j < currRun.Size(); j++)
        {
            CopyString(tempStr, currRun.runID, RangeSimRunResults::fullIDLen);
            ConcatString(tempStr, " \t", RangeSimRunResults::fullIDLen);
            ConcatString(tempStr, currRun[j].outputID, RangeSimRunResults::fullIDLen);
            outputIDs += tempStr;
        }
    }
}

