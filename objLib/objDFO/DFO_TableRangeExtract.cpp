///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableRangeExtract.cpp
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
//      Extract rows from a table based upon values in a single column
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/DO_TableData.h>

#include <objDFO/DFO_TableRangeExtract.h>


DFO_TableRangeExtract :: DFO_TableRangeExtract(): 
    DFO_RangeExtract("Extract Table Rows"), 
    selectedRowIndexesDO(true, true),
    rejectedRowIndexesDO(true, true)
{
    InitCommon();
}

DFO_TableRangeExtract::DFO_TableRangeExtract(const DFO_TableRangeExtract& a) :
    DFO_RangeExtract(a),
    DSC_TableColSel(a),
    selectedRowIndexesDO(true, true),
    rejectedRowIndexesDO(true, true)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableRangeExtract :: ~DFO_TableRangeExtract ()
{
}

void DFO_TableRangeExtract::InitCommon()
{
    DSC_TableColSel::AddPort(*this);
    passTableDO.tableData = &passTableData;
    AddOutPort(passTableDO);
    AddOutPort(selectedRowIndexesDO);
    AddOutPort(rejectedRowIndexesDO);
    DoStatusChk();
    selectedRowIndexesDO.Reset("Selected rows");
    rejectedRowIndexesDO.Reset("Rejected rows");
}

DFO_TableRangeExtract& DFO_TableRangeExtract::operator= (const DFO_TableRangeExtract& a)
{
    if (&a != this)
    {
        DFO_RangeExtract::operator=(a);
        DSC_TableColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableRangeExtract::LocalCopy(const DFO_TableRangeExtract& a)
{
}


void  DFO_TableRangeExtract:: DoStatusChk()
{
    DFO_RangeExtract::DoStatusChk();
    DSC_TableColSel::DoCheck(*this);
}


void DFO_TableRangeExtract:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    int nCol = inputData->GetNCols();
    int maxRow = selectedColData->Size();

    int npass = 0;
    for (int i = 0; i < maxRow; i++)
        if (ValueInRange((*selectedColData)[i]))
            npass++;

    if (npass == 0)
    {
        ApplyWarningMsg(calcType,"No rows pass conditions");
        passTableData.ClearAll();

        selectedRowIndexesDO.SetIndexType(true);
        selectedRowIndexesDO.indxArray.SetEmpty();
        selectedRowIndexesDO.SetIndexValue(0);

        rejectedRowIndexesDO.SetIndexType(maxRow < 2);
        rejectedRowIndexesDO.indxArray.AllocAndIndex(maxRow);
        rejectedRowIndexesDO.SetIndexValue(0);
    }
    else
    {
        if (!passTableData.Alloc(nCol, npass))
        {
            SetObjErrMsg("table Allocation ??");
            return;
        }
        passTableData.columnDesc = inputData->columnDesc;

        SC_IntArray selectedRows(npass), rejectedRows(maxRow - npass);
        selectedRows.SetEmpty();
        rejectedRows.SetEmpty();

        int firstSel = -1;
        int firstRej = -1;
        InitStatusVals();
        for (int i = 0; i < maxRow; i++)
            if (ValueInRange((*selectedColData)[i]))
            {
                if (firstSel < 0)
                    firstSel = i;
                selectedRows += i;
                if (inputData->rowDesc.IsNotEmpty())
                    passTableData.rowDesc += inputData->rowDesc[i];
                for (int j = 0; j < nCol; j++)
                    passTableData.dataTable[j] += inputData->dataTable[j][i];
            }
            else
            {
                if (firstRej < 0)
                    firstRej = i;
                rejectedRows += i;
            }

        passTableData.SetSize(nCol, selectedRows.Size());
        passTableData.SetID("Extracted table");


        selectedRowIndexesDO.SetIndexType(selectedRows.Size() < 2);
        selectedRowIndexesDO.SetIndexArray(selectedRows);
        selectedRowIndexesDO.SetIndexValue(firstSel);

        rejectedRowIndexesDO.SetIndexType(rejectedRows.Size() < 2);
        rejectedRowIndexesDO.SetIndexArray(rejectedRows);
        rejectedRowIndexesDO.SetIndexValue(firstRej);
    }
}

