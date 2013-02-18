///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadXYSimResults.cpp
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
//      Read XY Simulation results from file
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <nsDFO/DFO_ReadXYSimResults.h>

DFO_ReadXYSimResults :: DFO_ReadXYSimResults() :
    FuncObjC("ReadXYSimResults")
{
    InitCommon();
}

DFO_ReadXYSimResults::DFO_ReadXYSimResults(const DFO_ReadXYSimResults& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ReadXYSimResults::InitCommon()
{
    xySimResultsDO.xyResults = &xySimResults;
    fileNameDO.SetTypeLabel("XY sim results");
    AddOutPort(xySimResultsDO);
    AddOutPort(fileNameDO);

    tableDO.tableData = &tableData;
    AddOutPort(tableDO);

    DoStatusChk();
}

DFO_ReadXYSimResults& DFO_ReadXYSimResults::operator= (const DFO_ReadXYSimResults& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ReadXYSimResults::LocalCopy(const DFO_ReadXYSimResults& a)
{
    CopyString(xySimResultsIO.fileName, a.xySimResultsIO.fileName, stdFileStrLen);
}

void  DFO_ReadXYSimResults:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    if (!xySimResultsDO.DataOK())
        SetObjErrMsg("No data read");
}

void DFO_ReadXYSimResults:: CalcOutput(FOcalcType  calcType)
{

    if ((calcType == foc_Full) || (calcType == foc_Apply))
    {
        xySimResults.DeAlloc();
        objStatus = os_OK;

        if (!CheckFileExists(xySimResultsIO.fileName))
        {
            SetObjErrMsg("File does not exist ??");
            return;
        }

        tableData.ClearAll();

        if (!xySimResultsIO.ReadSimResults(xySimResults))
        {
            SetObjErrMsg("Error opening file (is it read-only) ??");
            return;
        }

        MakeCaseTable();
        SetDefaultFileID(xySimResultsIO.fileName);
        fileNameDO.SetFileValueLabel(xySimResultsIO.fileName);
    }

    DoStatusChk();
    if (StatusNotOK())
    {
        xySimResults.DeAlloc();
        tableData.ClearAll();
        return;
    }
}

void DFO_ReadXYSimResults::MakeCaseTable()
{
    SC_StringArray& colDesc = xySimResults[0].caseValShortDesc;
    int nCol = colDesc.Size();
    int nRows = xySimResults.GetnCases();

    if (!tableData.Alloc(nCol, nRows, 80))
    {
        SetObjErrMsg("Table allocation problem");
        return;
    }

    //  set column headers
    for (int i = 0; i < nCol; i++)
        tableData.columnDesc += colDesc[i];

    // and data
    for (int i = 0; i < xySimResults.Size(); i++)
    {
        XYSimRunResults& currRun = xySimResults[i];
        for (int j = 0; j < currRun.Size(); j++)
        {
            SC_DoubleArray& caseVals = currRun[j].caseValues;
            for (int k = 0; k < nCol; k++)
                tableData.dataTable[k] += caseVals[k];
        }
    }

    tableData.SetID("case values");
    tableData.dataTable.SetSize(nCol);
}

