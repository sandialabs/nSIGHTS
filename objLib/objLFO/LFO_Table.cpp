///////////////////////////////////////////////////////////////////////////////////
//
// LFO_Table.cpp
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

#include <genClass/DO_TableData.h>

#include <objLFO/LFO_Table.h>

LFO_Table :: LFO_Table(ListDefC& assLst)
    : ListObjC("List Table", assLst)
{
    listTableID         = false;
    listColumnHeaders   = true;
    listRowDescriptions = true;
    tableColWidth       = 20;
    maxListWidth        = 128;
    tableDataFormat     = SC_SciFormat(5);

    InitCommon();
}

LFO_Table::LFO_Table(const LFO_Table& a, ListDefC& assLst) : ListObjC(a, assLst)
{
    LocalCopy(a);
    InitCommon();
}

LFO_Table :: ~LFO_Table ()
{
}

void LFO_Table::InitCommon()
{
    AddInPort(tableObjRef, typeid(DO_TableData));
    DoStatusChk();
}

LFO_Table& LFO_Table::operator= (const LFO_Table& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void LFO_Table::LocalCopy(const LFO_Table& a)
{
    tableObjRef         = a.tableObjRef;
    listTableID         = a.listTableID;
    listColumnHeaders   = a.listColumnHeaders;
    listRowDescriptions = a.listRowDescriptions;
    tableColWidth       = a.tableColWidth;
    maxListWidth        = a.maxListWidth;
    tableDataFormat     = a.tableDataFormat;
}


void LFO_Table::DoStatusChk()
{
    FuncObjC::DoStatusChk();
    CheckInputFO(tableObjRef, "Table");
}


void LFO_Table::CreateListing()
{
    const DC_TableData* inTableDC = static_cast<DO_TableData*>(GetInPortData(tableObjRef))->tableData;

    if (tableColWidth > listingLineLen)
        tableColWidth = listingLineLen - 1;

    char tempStr[80];
    if (listTableID)
    {
        MakeString(tempStr, "Table: ", inTableDC->GetID(), 80);
        TextStdTitle(tempStr);
    }

    int maxPageCols = maxListWidth / tableColWidth;
    if (maxPageCols < 2)
    {
        maxPageCols = 2;
        maxListWidth = 50;
        tableColWidth = 20;
    }

    SC_IntArray colWidth(maxPageCols, tableColWidth);
    SC_BoolArray leftJust(maxPageCols, false);
    leftJust[0] = listRowDescriptions;

    int dataColStart = 0;
    if (listRowDescriptions)
    {
        dataColStart = 1;
        if (inTableDC->rowDesc.IsEmpty())
            colWidth[0] = 5;
        else
            colWidth[0] = inTableDC->rowDesc.GetMaxStringLen();
    }
    TableSetup(colWidth, leftJust, 0);


    int stCol = 0;
    int maxTableCols = inTableDC->GetNCols();
    while (stCol < maxTableCols)
    {
        int ncolsWrite = maxPageCols - dataColStart;
        if (stCol + ncolsWrite > maxTableCols)
            ncolsWrite = maxTableCols - stCol;

        if (listColumnHeaders)
        {
            for (int j = 0; j < ncolsWrite; j++)
                SetTableCol(j + dataColStart, inTableDC->GetTitle(stCol + j));
            AddNextLine();
            AddBlankLine();
        }

        for (int i = 0; i <  inTableDC->GetNRows(); i++)
        {
            if (listRowDescriptions)
            {
                if (inTableDC->rowDesc.Size() > i)
                {
                    SetTableCol(0, inTableDC->rowDesc[i]);
                }
                else
                {
                    IntToString(i, tempStr, 3, 80);
                    SetTableCol(0, tempStr);
                }
            }

            for (int j = 0; j < ncolsWrite; j++)
                SetTableReal(j + dataColStart, inTableDC->dataTable[stCol + j][i], tableDataFormat);
            AddNextLine();
        }

        stCol +=  ncolsWrite;
        AddBlankLine();
    }

}


