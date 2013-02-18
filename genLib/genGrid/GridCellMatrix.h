///////////////////////////////////////////////////////////////////////////////////
//
// GridCellMatrix.h
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
// DESCRIPTION: Windows specific code for implementing basic spreadsheet type
//              data entry/display.
//
///////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCELLMATRIX_H__5980ED4F_E130_11D3_BCCC_00104B4C74FD__INCLUDED_)
#define AFX_GRIDCELLMATRIX_H__5980ED4F_E130_11D3_BCCC_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GridCell.h"

class CGridCellArray : public CArray<CGridCell*, CGridCell*>
{
public:
    CGridCellArray(int nItems);

    void DeleteAll();
};

class CGridCellMatrix
{
public:
    CGridCellMatrix(int nRows, int nCols);
    virtual ~CGridCellMatrix();

    void DeleteAll();
    void AddRow();
    void AddColumn();

    void DeleteRow(int nRow);
    void DeleteColumn(int nCol);

    void InsertRowAt(int nRow);
    void InsertColumnAt(int nCol);

    void SetCell(int nRow, int nCol, CGridCell* pCell);
    CGridCell* GetCell(int nRow, int nCol);

    int GetRowCount();
    int GetColCount();

    void SetRowCount(int nRows);
    void SetColCount(int nCols);

protected:
    int m_nRows;
    int m_nCols;

    CArray<CGridCellArray*,CGridCellArray*> m_aCellColumn;
};

inline int CGridCellMatrix::GetRowCount()
{
    return m_nRows;
}
inline int CGridCellMatrix::GetColCount()
{
    return m_nCols;
}

#endif // AFX_GRIDCELLMATRIX_H__5980ED4F_E130_11D3_BCCC_00104B4C74FD__INCLUDED_

