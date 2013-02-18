///////////////////////////////////////////////////////////////////////////////////
//
// ProjectTreeView.h
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

#if !defined(PROJECTTREEVIEW_H)
#define PROJECTTREEVIEW_H


class MenuRootC;

class CCreateContextProject : public CCreateContext
{
public:
    CCreateContextProject();
    CCreateContextProject(const CCreateContext& cc) ;
    MenuRootC* m_pProjectRoot;
};

inline CCreateContextProject::CCreateContextProject() :
    CCreateContext(),
    m_pProjectRoot(NULL)
{
}

inline CCreateContextProject::CCreateContextProject(const CCreateContext& cc)
{
    m_pNewViewClass = cc.m_pNewViewClass;
    m_pCurrentDoc = cc.m_pCurrentDoc;
    m_pNewDocTemplate = cc.m_pNewDocTemplate;

    m_pLastView = cc.m_pLastView;
    m_pCurrentFrame = cc.m_pCurrentFrame;
}

class CProjectTreeView : public CView
{
    DECLARE_DYNCREATE(CProjectTreeView)
protected:
    CProjectTreeView();

public:
    MenuRootC* projectRoot;

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CProjectTreeView)
    public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
    protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnInitialUpdate(); // called first time after construct
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CProjectTreeView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
public:
    //{{AFX_MSG(CProjectTreeView)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


#endif // PROJECTTREEVIEW_H

