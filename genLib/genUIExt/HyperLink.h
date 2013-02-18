///////////////////////////////////////////////////////////////////////////////////
//
// HyperLink.h
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

#if !defined(HYPERLINK_H)
#define HYPERLINK_H

class CHyperLink : public CStatic
{
public:
            CHyperLink();
    virtual ~CHyperLink();

public:
    void        SetURL(CString strURL);
    CString     GetURL() const;

    void        SetColours(COLORREF crLinkColour, COLORREF crVisitedColour,
                           COLORREF crHoverColour = -1);
    COLORREF    GetLinkColour() const;
    COLORREF    GetVisitedColour() const;
    COLORREF    GetHoverColour() const;

    void    SetVisited(bool bVisited = true);
    bool    GetVisited() const;

    void    SetLinkCursor(HCURSOR hCursor);
    HCURSOR GetLinkCursor() const;

    void    SetUnderline(bool bUnderline = true);
    bool    GetUnderline() const;

    void    SetAutoSize(bool bAutoSize = true);
    bool    GetAutoSize() const;

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CHyperLink)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
protected:
    HINSTANCE GotoURL(LPCTSTR szUrl, int nShowCmd);
    void ReportError(int nError);
    LONG GetRegKey(HKEY hKey, LPCTSTR subkey, LPTSTR szRetData, int retStrLen);
    void PositionWindow();
    void SetDefaultCursor();

// Protected attributes
protected:
    COLORREF    m_crLinkColour;
    COLORREF    m_crVisitedColour;      // Hyperlink colours
    COLORREF    m_crHoverColour;        // Hover colour
    bool        m_bOverControl;         // cursor over control?
    bool        m_bVisited;             // Has it been visited?
    bool        m_bUnderline;           // underline hyperlink?
    bool        m_bAdjustToFit;         // Adjust window size to fit text?
    CString     m_strURL;               // hyperlink URL
    CFont       m_Font;                 // Underline font if necessary
    HCURSOR     m_hLinkCursor;          // Cursor for hyperlink
    CToolTipCtrl m_toolTip;             // The tooltip

    // Generated message map functions
protected:
    //{{AFX_MSG(CHyperLink)
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    //}}AFX_MSG
    afx_msg void OnClicked();
    DECLARE_MESSAGE_MAP()
};


#endif //HYPERLINK_H

