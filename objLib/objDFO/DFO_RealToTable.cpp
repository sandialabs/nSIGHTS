///////////////////////////////////////////////////////////////////////////////////
//
// DFO_RealToTable.cpp
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

#include <objDFO/DFO_RealToTable.h>

DFO_RealToTable::RealData::RealData() :
    doReal(false)
{
    SetToNull(columnID);
}
DFO_RealToTable::RealData::RealData(const RealData& a)
{
    LocalCopy(a);
}
DFO_RealToTable::RealData& DFO_RealToTable::RealData::operator= (const RealData& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}
void DFO_RealToTable::RealData::LocalCopy(const RealData& a)
{
    doReal     = a.doReal;
    realObjRef = a.realObjRef;
    CopyString(columnID, a.columnID, maxColumnID);
}

DFO_RealToTable :: DFO_RealToTable()
    : FuncObjC("Add Real To Table")
{
    InitCommon();
}

DFO_RealToTable::DFO_RealToTable(const DFO_RealToTable& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_RealToTable :: ~DFO_RealToTable ()
{
}

void DFO_RealToTable::InitCommon()
{
    realTableDO.tableData = &realTable;
    AddOutPort(realTableDO);

    currRowVals.Alloc(maxTableCol);
    currColIDs.Alloc(maxTableCol);

    for (int i = 0; i < maxTableCol; i++)
        AddUseFlagInPort(columnData[i].realObjRef, typeid(DO_Real), columnData[i].doReal);

    DoStatusChk();
}

DFO_RealToTable& DFO_RealToTable::operator= (const DFO_RealToTable& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  DFO_RealToTable::LocalCopy(const DFO_RealToTable& a)
{
    for (int i = 0; i < maxTableCol; i++)
        columnData[i] = a.columnData[i];
}


void DFO_RealToTable::SetDefault()
{
    for (int i = 0; i < maxTableCol; i++)
        columnData[i].doReal = false;
}

void  DFO_RealToTable:: DoStatusChk()
{
    // base class check
    FuncObjC::DoStatusChk();

    // check input objects
    ntableCol = 0;
    currRowVals.SetEmpty();
    currColIDs.SetEmpty();
    for (int i = 0; i < maxTableCol; i++)
        if (columnData[i].doReal)
        {
            if (!CheckInputFO(columnData[i].realObjRef, "Real"))
                return;

            if (StringLength(columnData[i].columnID) == 0)
            {
                SetObjErrMsg("no ID for column");
                return;
            }

            currRowVals += static_cast<DO_Real*>(GetInPortData(columnData[i].realObjRef))->realVal;
            currColIDs += columnData[i].columnID;
            ntableCol++;
        }

    if (ntableCol == 0)
    {
        SetObjErrMsg("no data to put in table");
        realTable.ClearData();
        return;
    }

    // size change -- realloc and reset
    if ((ntableCol != realTable.GetNCols()) && (!realTable.ResizableAlloc(ntableCol, 50, maxColumnID)))
        SetObjErrMsg("table allocation problem ??");
}


void DFO_RealToTable:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
    {
        realTable.ClearData();
        return;
    }

#ifndef ADCONSOLEAPP
    //  don't recalc on full calc -- existing table is OK for mView
    // need to recalculate for mViewX on read though
    if (calcType == foc_Full)
        return;
#endif

    if (calcType == foc_Apply)
    {
        realTable.ClearData();
        realTable.SetID("Created table");
    }

    for (int i = 0; i < ntableCol; i++)
    {
        realTable.SetTitle(i, currColIDs[i]);
        realTable.dataTable[i] += currRowVals[i];
    }

    char tempStr[80];
    ConcatInt(realTable.GetNRows(), "A", tempStr, 4, 80);
    realTable.rowDesc += tempStr;

}


