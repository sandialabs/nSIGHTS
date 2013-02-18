///////////////////////////////////////////////////////////////////////////////////
//
// ComboEditWnd.h
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
//              Edit control for combo box
//
///////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMBOEDITWND_H__5980ED50_E130_11D3_BCCC_00104B4C74FD__INCLUDED_)
#define AFX_COMBOEDITWND_H__5980ED50_E130_11D3_BCCC_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComboEditWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#define IDC_COMBOEDIT 1001

/////////////////////////////////////////////////////////////////////////////
// CComboEdit window

class CComboEdit : public CEdit
{
    // Construction
public:
    CComboEdit();

    // Attributes
public:

    // Operations
public:

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CComboEdit)
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~CComboEdit();

    // Generated message map functions
protected:
    //{{AFX_MSG(CComboEdit)
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};


#include "ExtendedComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// CComboEditWnd window

class CComboEditWnd : public CExtendedComboBox
{
    friend class CComboEdit;

    // Construction
public:
    CComboEditWnd(CWnd* pParent,         // parent
                  CRect& rect,           // dimensions & location
                  DWORD dwStyle,         // window/combobox style
                  UINT nID,              // control ID
                  int nRow, int nColumn, // row and column
                  CStringArray& Items,   // Items in list
                  CString sInitText,     // initial selection
                  UINT nFirstChar);      // first character to pass to control


    // Attributes
public:
    CComboEdit m_edit;  // subclassed edit control

    // Operations
public:
    virtual bool CanClose();
    virtual void EndEdit();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CComboEditWnd)
protected:
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~CComboEditWnd();

protected:

    // Generated message map functions
protected:
    //{{AFX_MSG(CComboEditWnd)
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSelchange();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    private:
    int     m_nNumLines;
    CString m_sInitText;
    int     m_nRow;
    int     m_nCol;
    UINT    m_nLastChar;
    bool    m_bExitOnArrows;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_COMBOEDITWND_H__5980ED50_E130_11D3_BCCC_00104B4C74FD__INCLUDED_

