///////////////////////////////////////////////////////////////////////////////////
//
// GridCellRange.h
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

#if !defined(AFX_GRIDCELLRANGE_H__5980ED49_E130_11D3_BCCC_00104B4C74FD__INCLUDED_)
#define AFX_GRIDCELLRANGE_H__5980ED49_E130_11D3_BCCC_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GridCellIndices.h"

class CGridCellRange
{
public:
    CGridCellRange(int nMinRow = -1, int nMinCol = -1, int nMaxRow = -1, int nMaxCol = -1) :
        m_nMinRow(nMinRow), m_nMinCol(nMinCol),
        m_nMaxRow(nMaxRow), m_nMaxCol(nMaxCol)
    {}
    CGridCellRange(CGridCellIndices min, CGridCellIndices max) :
        m_nMinRow(min.m_nRow), m_nMinCol(min.m_nCol),
        m_nMaxRow(max.m_nRow), m_nMaxCol(max.m_nCol)
    {}
    void Set(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol);
    void Set(const CGridCellIndices& min, const CGridCellIndices& max);
    virtual ~CGridCellRange() {}

    bool IsValid() const;
    int InRange(int nRow, int nCol) const;
    int InRange(const CGridCellIndices& cellIndices) const;
    int IsSingleCell() const;

    CGridCellIndices GetTopLeft() const;
    CGridCellRange Intersect(const CGridCellRange& ci) const;
    CGridCellRange Union(const CGridCellRange& ci) const;

    int  GetMinRow() const;
    void SetMinRow(int nMinRow);
    int  GetMinCol() const;
    void SetMinCol(int nMinCol);
    int  GetMaxRow() const;
    void SetMaxRow(int nMaxRow);
    int  GetMaxCol() const;
    void SetMaxCol(int nMaxCol);

    int operator==(const CGridCellRange& cr) const;
    int operator!=(const CGridCellRange& cr) const;

protected:
    int m_nMinRow;
    int m_nMinCol;
    int m_nMaxRow;
    int m_nMaxCol;
};


inline void CGridCellRange::Set(int nMinRow, int nMinCol, int nMaxRow, int nMaxCol)
{
    m_nMinRow = nMinRow;
    m_nMinCol = nMinCol;
    m_nMaxRow = nMaxRow;
    m_nMaxCol = nMaxCol;
}

inline void CGridCellRange::Set(const CGridCellIndices& min, const CGridCellIndices& max)
{
    m_nMinRow = min.m_nRow;
    m_nMinCol = min.m_nCol;
    m_nMaxRow = max.m_nRow;
    m_nMaxCol = max.m_nCol;
}

inline bool CGridCellRange::IsValid() const
{
    return m_nMinRow != -1 && m_nMinCol != -1 && m_nMaxRow != -1 && m_nMaxCol != -1 &&
        m_nMinRow <= m_nMaxRow && m_nMinCol <= m_nMaxCol;
}

inline int CGridCellRange::InRange(int nRow, int nCol) const
{
    return nRow >= m_nMinRow && nRow <= m_nMaxRow && nCol >= m_nMinCol && nCol <= m_nMaxCol;
}

inline int CGridCellRange::IsSingleCell() const
{
    return m_nMinRow == m_nMaxRow && m_nMinCol == m_nMaxCol && IsValid();
}

inline int CGridCellRange::InRange(const CGridCellIndices& cellIndices) const
{
    return InRange(cellIndices.m_nRow, cellIndices.m_nCol);
}

inline CGridCellIndices CGridCellRange::GetTopLeft() const
{
    return CGridCellIndices(m_nMinRow, m_nMinCol);
}

inline CGridCellRange CGridCellRange::Intersect(const CGridCellRange& cr) const
{
    return CGridCellRange(max(m_nMinRow,cr.m_nMinRow),
                          max(m_nMinCol,cr.m_nMinCol),
                          min(m_nMaxRow,cr.m_nMaxRow),
                          min(m_nMaxCol,cr.m_nMaxCol));
}

inline int CGridCellRange::GetMinRow() const
{
    return m_nMinRow;
}
inline void CGridCellRange::SetMinRow(int nMinRow)
{
    m_nMinRow = nMinRow;
}
inline int CGridCellRange::GetMinCol() const
{
    return m_nMinCol;
}
inline void CGridCellRange::SetMinCol(int nMinCol)
{
    m_nMinCol = nMinCol;
}
inline int CGridCellRange::GetMaxRow() const
{
    return m_nMaxRow;
}
inline void CGridCellRange::SetMaxRow(int nMaxRow)
{
    m_nMaxRow = nMaxRow;
}
inline int CGridCellRange::GetMaxCol() const
{
    return m_nMaxCol;
}
inline void CGridCellRange::SetMaxCol(int nMaxCol)
{
    m_nMaxCol = nMaxCol;
}

#endif // AFX_GRIDCELLRANGE_H__5980ED49_E130_11D3_BCCC_00104B4C74FD__INCLUDED_

