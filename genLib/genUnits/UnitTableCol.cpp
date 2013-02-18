///////////////////////////////////////////////////////////////////////////////////
//
// UnitTableCol.cpp
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
//      Platform independent and Windows specific (derived 
//  from genCtrl and genGrid) classes for performing
//  unit conversions and for entering/displaying
//  dimensioned numeric data in dialogs and grids.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genUnits/UnitTableCol.h>


UnitTableCol::UnitTableCol()
{
    tableContainsUserUnits = true;
    selectedTableCol = 0;
}

UnitTableCol::UnitTableCol(UnitType  uType)
    : UnitIndex(uType)
{
    tableContainsUserUnits = true;
    selectedTableCol = 0;
}

UnitTableCol::UnitTableCol(UnitIndex  uIndex)
    : UnitIndex(uIndex)
{
    tableContainsUserUnits = true;
    selectedTableCol = 0;
}

UnitTableCol::UnitTableCol(UnitType uType, int tableCol, bool tableIsUserData)
    : UnitIndex(uType)
{
    tableContainsUserUnits = tableIsUserData;
    selectedTableCol = tableCol;
}

UnitTableCol::UnitTableCol(const UnitTableCol& a)
    : UnitIndex(a),
    tableContainsUserUnits(a.tableContainsUserUnits),
    selectedTableCol(a.selectedTableCol)
{
}

UnitTableCol& UnitTableCol::operator= (const UnitTableCol& a)
{
    if (this != &a)
    {
        UnitIndex::operator = (a);
        tableContainsUserUnits = a.tableContainsUserUnits;
        selectedTableCol = a.selectedTableCol;
    }
    return *this;
}

bool UnitTableCol::TableColOK(const DC_TableData& inTbl) const
{
    return (inTbl.dataTable.Size() > selectedTableCol) && (selectedTableCol >= 0);
}

const SC_DoubleArray& UnitTableCol::GetTableCol(const DC_TableData& inTbl) const
{
    return inTbl.dataTable[selectedTableCol];
}

double UnitTableCol::GetMetricVal(const DC_TableData& inTbl, int row) const
{
    const double& dVal = inTbl.dataTable[selectedTableCol][row];
    if (tableContainsUserUnits)
        return UserToMetric(dVal);
    return dVal;
}

double UnitTableCol::GetUserVal(const DC_TableData& inTbl, int row) const
{
    const double& dVal = inTbl.dataTable[selectedTableCol][row];
    if (tableContainsUserUnits)
        return dVal;
    return MetricToUser(dVal);
}

