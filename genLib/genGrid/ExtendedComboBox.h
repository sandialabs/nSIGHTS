///////////////////////////////////////////////////////////////////////////////////
//
// ExtendedComboBox.h
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

#ifndef EXTENDEDCOMBOBOX_H
#define EDTENDEDCOMBOBOX_H

class CExtendedComboBox;

/////////////////////////////////////////////////////////////////////////////
// CListBoxInsideComboBox window

class CListBoxInsideComboBox : public CWnd
{
// Construction
public:
    CListBoxInsideComboBox();

// Attributes
public:
    CExtendedComboBox* m_pParent;
    void SetParent(CExtendedComboBox *);

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CListBoxInsideComboBox)
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CListBoxInsideComboBox();

    // Generated message map functions
protected:
    //{{AFX_MSG(CListBoxInsideComboBox)
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CExtendedComboBox window

class CExtendedComboBox : public CComboBox
{
// Construction
public:
    CExtendedComboBox();

// Attributes
public:
    // default implementation uses LSB of item data; override to get other behaviour
    virtual bool IsItemEnabled(UINT) const;
    void EnableItem(UINT nIndex, bool bSet = true);

protected:
    CString m_strSavedText;     // saves text between OnSelendok and OnRealSelEndOK calls
    CListBoxInsideComboBox m_listBox;

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CExtendedComboBox)
    public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    protected:
    virtual void PreSubclassWindow();
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CExtendedComboBox();

    // Generated message map functions
protected:
    //{{AFX_MSG(CExtendedComboBox)
    afx_msg int OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex);
    afx_msg void OnSelendok();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    //}}AFX_MSG

    LRESULT OnCtlColor(WPARAM,LPARAM lParam);
    afx_msg LRESULT OnRealSelEndOK(WPARAM,LPARAM);

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // EDTENDEDCOMBOBOX_H
