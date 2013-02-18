///////////////////////////////////////////////////////////////////////////////////
//
// SplitterWndEx.h
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

// SplitterWndEx.h: interface for the CSplitterWndEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLITTERWND_H__6D985468_F726_11D1_83B6_0000B43382FE__INCLUDED_)
#define AFX_SPLITTERWND_H__6D985468_F726_11D1_83B6_0000B43382FE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSplitterWndEx : public CSplitterWnd
{
public:
    CSplitterWndEx();
    virtual ~CSplitterWndEx();

    void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg);
    void OnInvertTracker(const CRect & rect);

public:
    virtual void RecalcLayout();
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
        AFX_CMDHANDLERINFO* pHandlerInfo);

protected:
    int m_upBorder;
    bool bWhiteLine;

protected:
    void GetInsideRect(CRect& rect) const;
    // Generated message map functions
    //{{AFX_MSG(CSplitterWndEx)
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnDestroy();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_SPLITTERWND_H__6D985468_F726_11D1_83B6_0000B43382FE__INCLUDED_)

