///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableColumnSort.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <objClass/DC_Curve.h>

#include <objDFO/DFO_TableColumnSort.h>

DFO_TableColumnSort :: DFO_TableColumnSort() :
    FuncObjC("Sort Table by Column")
{
    sortAscending = true;
    InitCommon();
}

DFO_TableColumnSort::DFO_TableColumnSort(const DFO_TableColumnSort& a) :
    FuncObjC(a),
    DSC_TableColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableColumnSort :: ~DFO_TableColumnSort ()
{
}

void DFO_TableColumnSort::InitCommon()
{
    sortedTableDO.tableData = &sortedTableDC;

    DSC_TableColSel::AddPort(*this);
    AddOutPort(sortedTableDO);
    AddOutPort(sortColumnIDDO);

    sortColumnIDDO.Reset("Sort Column ID");

    DoStatusChk();
}

DFO_TableColumnSort& DFO_TableColumnSort::operator= (const DFO_TableColumnSort& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        DSC_TableColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableColumnSort::LocalCopy(const DFO_TableColumnSort& a)
{
    sortAscending = a.sortAscending;
}


void  DFO_TableColumnSort:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    DSC_TableColSel::DoCheck(*this);
    if (StatusNotOK())
    {
        sortedTableDO.tableData = 0;
        sortedTableDC.ClearData();
    }

}

void DFO_TableColumnSort:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    sortedTableDC = *inputData; 
    sortedTableDO.tableData = &sortedTableDC;

    sortColumnIDDO.SetValueLabel(sortedTableDC.GetTitle(selectedCol));

    SC_IntArray sortIndexes;
    sortIndexes.AllocAndIndex(sortedTableDC.GetNRows());
    sortedTableDC.dataTable[selectedCol].Sort(sortAscending, sortIndexes);

    const SC_StringArray& inRowDesc = inputData->rowDesc;
    for (int j = 0; j < sortedTableDC.GetNRows(); j++)
    {
        int inputRow = sortIndexes[j];
        if (inputRow < inRowDesc.Size())
            sortedTableDC.rowDesc.SetString(inRowDesc[inputRow], j);
        else
        {
            char tempStr[80];
            ConcatInt(inputRow, "I", tempStr, 4, 80);
            sortedTableDC.rowDesc.SetString(tempStr, j);
        }
    }

    for (int i = 0; i < sortedTableDC.GetNCols(); i++)
    {
        if (i == selectedCol)
            continue;

        const SC_DoubleArray& dataIn = inputData->dataTable[i];
        SC_DoubleArray& dataOut = sortedTableDC.dataTable[i];
        for (int j = 0; j < sortedTableDC.GetNRows(); j++)
            dataOut[j] = dataIn[sortIndexes[j]];
    }

    sortedTableDC.SetID("Sorted table");

}

