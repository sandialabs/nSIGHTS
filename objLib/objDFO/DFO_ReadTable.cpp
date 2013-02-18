///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadTable.cpp
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
//      Reads table data from ASCII file in several formats
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <objDFO/DFO_ReadTable.h>

DFO_ReadTable :: DFO_ReadTable() :
    FuncObjC("Read Table"),
    tableFileFormat(tffBasic),
    readColumnIDs(false),
    readRowIDs(false),
    setNullsToZero(false),
    ignoreComments(false)
{
    averageMultipleColumns = false;
    multipleColumnCount = 10;

    SetToNull(tableFname);
    CopyString(commentChars, "!#*", maxCommentChars);
    InitCommon();
}

DFO_ReadTable::DFO_ReadTable(const DFO_ReadTable& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ReadTable::InitCommon()
{
    xyDataArrayDO.xyDataArray = &xyTableData;
    tableDO.tableData = &tableData;
    fileNameDO.SetTypeLabel("table file");
    AddOutPort(tableDO);
    AddOutPort(xyDataArrayDO);
    AddOutPort(fileNameDO);
    mpiFiles += NodeFile(tableFname, "table file");


    DoStatusChk();
}

DFO_ReadTable& DFO_ReadTable::operator= (const DFO_ReadTable& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ReadTable::LocalCopy(const DFO_ReadTable& a)
{
    CopyString(tableFname, a.tableFname, stdFileStrLen);
    tableFileFormat = a.tableFileFormat;
    readColumnIDs   = a.readColumnIDs;
    readRowIDs      = a.readRowIDs;

    setNullsToZero  = a.setNullsToZero;
    ignoreComments  = a.ignoreComments;

    averageMultipleColumns = a.averageMultipleColumns;
    multipleColumnCount = a.multipleColumnCount;

    CopyString(commentChars, a.commentChars, maxCommentChars);
    mpiFiles.CopyFrom(a.mpiFiles);
}


void  DFO_ReadTable:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    if (ignoreComments)
    {
        FullTrim(commentChars);
        FillBlanks(commentChars);
    }
    if (!tableDO.DataOK())
        SetObjErrMsg("No data read");

}

void DFO_ReadTable:: CalcOutput(FOcalcType  calcType)
{
    if ((calcType == foc_Full) || (calcType == foc_Apply))
    {
        xyTableData.DeAlloc();
        tableData.Alloc(0, 0);
        objStatus = os_OK;
        mpiFiles.StdCheckAndUpdate(*this);
        if (StatusNotOK())
            return;

        if (!ReadTable())
            return;

        if ((tableData.GetNCols() > 1) && (tableData.GetNRows() > 0))
        {
            xyTableData.AllocAndSetSize(tableData.GetNCols() - 1);
            for (int i = 1; i < tableData.GetNCols(); i++)
            {
                xyTableData[i - 1].CreateFrom(tableData.dataTable[0], tableData.dataTable[i]);
                xyTableData[i - 1].SetID(tableData.columnDesc[i]);
            }
        }

        SetDefaultFileID(tableFname);
        tableData.SetID(GetID());
        fileNameDO.SetFileValueLabel(tableFname);
    }

    DoStatusChk();
    if (StatusNotOK())
        tableData.Alloc(0, 0);
}

