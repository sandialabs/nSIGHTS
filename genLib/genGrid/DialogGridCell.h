///////////////////////////////////////////////////////////////////////////////////
//
// DialogGridCell.h
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

#if !defined(AFX_DialogGridCell_H__5980ED4E_E130_11D3_BCCC_00104B4C74FD__INCLUDED_)
#define AFX_DialogGridCell_H__5980ED4E_E130_11D3_BCCC_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GridCell.h"

class CDialogGridCell : public CGridCell
{
public:
    enum Alignment {
        alignLeft,
        alignRight,
        alignCenter
    };

    CDialogGridCell(const char* szText = NULL, Alignment alignment = alignLeft);
    ~CDialogGridCell();

    CDialogGridCell(const CDialogGridCell &rhs);
    const CDialogGridCell& operator=(const CDialogGridCell& rhs);

    void SetAlignment(Alignment alignment);

    virtual void DrawUnconstrained(CDC* pDC, int x, int y) const;
    virtual void DrawConstrained(CDC* pDC, int x, int y, int cx, int cy) const;

    const char* GetText();
    void SetText(const char* szText);

    CSize GetExtent(CDC* pDC) const;

    virtual void AddMenuItems(CMenu* pMenu);

    virtual CWnd* CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar);
    virtual bool CanEdit();
    virtual bool CanClose(CWnd* pWnd);
    virtual void EndEdit(CWnd* pWnd);

protected:
    Alignment m_alignment;
    CString m_sText;
};

inline void CDialogGridCell::SetAlignment(CDialogGridCell::Alignment alignment)
{
    m_alignment = alignment;
}

#endif // AFX_DialogGridCell_H__5980ED4E_E130_11D3_BCCC_00104B4C74FD__INCLUDED_

