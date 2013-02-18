///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CombineTables.cpp
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

#include <objDFO/DFO_CombineTables.h>

DFO_CombineTables::TableData::TableData() :
    doTableCol(false)
{
    tableColumnIndex = 0;
    columnID[0] = '\0';
}
DFO_CombineTables::TableData::TableData(const TableData& a)
{
    LocalCopy(a);
}
DFO_CombineTables::TableData& DFO_CombineTables::TableData::operator= (const TableData& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}
void DFO_CombineTables::TableData::LocalCopy(const TableData& a)
{
    doTableCol     = a.doTableCol;
    tableObjRef = a.tableObjRef;
    tableColumnIndex = a.tableColumnIndex;
    CopyString(columnID, a.columnID, maxColumnID);
}

DFO_CombineTables :: DFO_CombineTables()
    : FuncObjC("Combine Table Columns")
{
    InitCommon();
}

DFO_CombineTables::DFO_CombineTables(const DFO_CombineTables& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_CombineTables :: ~DFO_CombineTables ()
{
}

void DFO_CombineTables::InitCommon()
{
    outputTableDO.tableData = &outputTableDC;
    AddOutPort(outputTableDO);

    currColData.Alloc(maxTableCol);
    currColIDs.Alloc(maxTableCol);

    for (int i = 0; i < maxTableCol; i++)
        AddUseFlagInPort(columnData[i].tableObjRef, typeid(DO_TableData), columnData[i].doTableCol);

    DoStatusChk();
}

DFO_CombineTables& DFO_CombineTables::operator= (const DFO_CombineTables& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  DFO_CombineTables::LocalCopy(const DFO_CombineTables& a)
{
    for (int i = 0; i < maxTableCol; i++)
        columnData[i] = a.columnData[i];
}


void DFO_CombineTables::SetDefault()
{
    for (int i = 0; i < maxTableCol; i++)
        columnData[i].doTableCol = false;
}

void  DFO_CombineTables:: DoStatusChk()
{
    // base class check
    FuncObjC::DoStatusChk();

    // check input objects
    ntableCol = 0;
    firstColRowDesc = 0;
    currColData.SetEmpty();
    currColIDs.SetEmpty();
    int lastSize = -1;
    for (int i = 0; i < maxTableCol; i++)
    {
        if (columnData[i].doTableCol)
        {
            if (!CheckInputFO(columnData[i].tableObjRef, "Table"))
                return;

            if (StringLength(columnData[i].columnID) == 0)
            {
                SetObjErrMsg("no ID for column");
                return;
            }

            const DC_TableData* currTable = static_cast<DO_TableData*>(GetInPortData(columnData[i].tableObjRef))->tableData;
            if (currTable->dataTable.UpperBound() < columnData[i].tableColumnIndex)
            {
                SetObjErrMsg("table column index out-of-range");
                return;
            }

            SC_DoubleArray& colData = currTable->dataTable[columnData[i].tableColumnIndex];
            if ((lastSize >= 0) && (colData.Size() != lastSize))
            {
                SetObjErrMsg("table column size different than previous");
                return;
            }

            lastSize = colData.Size();

            if (currColData.IsEmpty())
                firstColRowDesc = &currTable->rowDesc;

            currColData += &colData;
            currColIDs += columnData[i].columnID;

            ntableCol++;
        }
    }

    if (ntableCol == 0)
    {
        SetObjErrMsg("no data to put in table");
        outputTableDC.ClearData();
        return;
    }

    outputTableDC.SetID("Combined table");

    // size change -- realloc and reset
    if ((ntableCol != outputTableDC.GetNCols()) && (!outputTableDC.ResizableAlloc(ntableCol, 50, maxColumnID)))
        SetObjErrMsg("table allocation problem ??");
}


void DFO_CombineTables:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
    {
        outputTableDC.ClearData();
        return;
    }
    outputTableDC.rowDesc = *firstColRowDesc;

    for (int i = 0; i < ntableCol; i++)
    {
        outputTableDC.SetTitle(i, currColIDs[i]);
        outputTableDC.dataTable[i] = *(currColData[i]);
    }
    outputTableDC.dataTable.SetSize(ntableCol);
    outputTableDC.SetID("Combined table");
}


