///////////////////////////////////////////////////////////////////////////////////
//
// ListHtmlView.h
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
//      Platform specific implementation of genListClass codes
//  adds Windows specific code for window creation and use of IE HTML renderer.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

// This is part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.
//
// ListHtmlView.h : interface of the CListingHtmlView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTHTMLVIEW_H__47FF4085_CE1B_11D0_BEB6_00C04FC99F83__INCLUDED_)
#define AFX_LISTHTMLVIEW_H__47FF4085_CE1B_11D0_BEB6_00C04FC99F83__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WebBrowserPrint.h"

class CListingHtmlView : public CHtmlView
{
protected: // create from serialization only
    CListingHtmlView();
    DECLARE_DYNCREATE(CListingHtmlView)

    // Attributes
public:

    IWebBrowser2* GetBrowserApp();
    LPDISPATCH GetDocumentX();

    CString m_sHeader;
    CString m_sFooter;
    bool m_bDocumentComplete;

    CWebBrowserPrint m_wbprint;

    // Operations
public:
    void InitPrint();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CListingHtmlView)
public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    virtual void OnDocumentComplete(LPCTSTR lpszURL);
    //}}AFX_VIRTUAL
    void OnTitleChange(LPCTSTR lpszText);
    void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
        LPCTSTR lpszTargetFrameName, CByteArray& baPostedData,
        LPCTSTR lpszHeaders, BOOL* pbCancel);

    // Implementation
public:
    virtual ~CListingHtmlView();
    #ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
    #endif

    protected:
    void Print(CString& sHeader, CString& sFooter, bool bPreview = false);

    void Print_AtlernateMethod(CString& sHeader, CString& sFooter);

    // Generated message map functions
    protected:
    //{{AFX_MSG(CListingHtmlView)
    afx_msg void OnViewFontsLargest();
    afx_msg void OnViewFontsLarge();
    afx_msg void OnViewFontsMedium();
    afx_msg void OnViewFontsSmall();
    afx_msg void OnViewFontsSmallest();
    afx_msg void OnFileOpen();
    afx_msg void OnViewStop();
    afx_msg void OnViewRefresh();
    afx_msg void OnPrintPreview();
    afx_msg void OnPrint();
    afx_msg void OnPrintSetup();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTHTMLVIEW_H__47FF4085_CE1B_11D0_BEB6_00C04FC99F83__INCLUDED_)

