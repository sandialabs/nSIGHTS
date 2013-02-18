///////////////////////////////////////////////////////////////////////////////////
//
// InfoBar.h
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
//      Windows specific code for MFC user-interface extension or enhancement classes.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_INFOBAR_H__C789E26C_DA4B_11D2_BF44_006008085F93__INCLUDED_)
#define AFX_INFOBAR_H__C789E26C_DA4B_11D2_BF44_006008085F93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InfoBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInfoBar window

class CInfoBar : public CControlBar
{
public:
    CInfoBar();
    virtual ~CInfoBar();

public:
    void SetTextColor(COLORREF crNew);
    void SetBackgroundColor(COLORREF cr);
    BOOL SetTextFont(LPCTSTR lpFontName);
    bool SetBitmap(UINT nResID);
    bool SetIcon(HICON hIcon);
    bool SetIcon(UINT nResID);
    void SetText(LPCTSTR lpszNew);

    int GetHeight(int cx);

    // Generated message map functions
protected:
    int     m_cxAvailable;
    CFont   m_font;
    CString m_sCaption;
    CBitmap m_bm;
    HICON   m_hIcon;
    bool    m_bPictureSet;

protected:
    COLORREF    m_crBackgroundColor;
    COLORREF    m_crTextColor;
    CSize       m_sizeBitmap;

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CInfoBar)
    public:
    virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
    virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
    //}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CInfoBar)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    //}}AFX_MSG
    afx_msg LRESULT OnSizeParent(WPARAM, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
};

enum MessageStatus
{
    msNone,
    msInformation,
    msWarning,
    msError,
};

class CInfoBarStatus : public CInfoBar
{
public:
    CInfoBarStatus();
    virtual ~CInfoBarStatus();

    BOOL SetStatus(MessageStatus messageStatus, LPCTSTR szText = NULL);
    void SetValueRange(LPCTSTR szFrom = NULL, LPCTSTR szTo = NULL);

protected:
    MessageStatus m_messageStatus;
    CString m_sFrom;
    CString m_sTo;

protected:
    //{{AFX_MSG(CInfoBarStratus)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFOBAR_H__C789E26C_DA4B_11D2_BF44_006008085F93__INCLUDED_)

