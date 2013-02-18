///////////////////////////////////////////////////////////////////////////////////
//
// SizingControlBar.h
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

/////////////////////////////////////////////////////////////////////////
//
// CSizingControlBar            Version 2.43
//
// Created: Jan 24, 1998        Last Modified: August 03, 2000
//
// See the official site at www.datamekanix.com for documentation and
// the latest news.
//
/////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998-2000 by Cristi Posea. All rights reserved.
//
// This code is free for personal and commercial use, providing this
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments.
//
// You must obtain the author's consent before you can include this code
// in a software library.
//
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc. to
// cristi@datamekanix.com or post them at the message board at the site.
/////////////////////////////////////////////////////////////////////////

#if !defined(__SIZINGCONTROLBAR_H__)
#define __SIZINGCONTROLBAR_H__

#include <afxpriv.h>    // for CDockContext
#include <afxtempl.h>   // for CTypedPtrArray

#include <genClassPS/PS_NCHitTest.h>

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#if defined(_SCB_MINIFRAME_CAPTION) && !defined(_SCB_REPLACE_MINIFRAME)
    #error "_SCB_MINIFRAME_CAPTION requires _SCB_REPLACE_MINIFRAME"
#endif

/////////////////////////////////////////////////////////////////////////
// CSCBDockBar dummy class for access to protected members

class CSCBDockBar : public CDockBar
{
    friend class CSizingControlBar;
};

/////////////////////////////////////////////////////////////////////////
// CSizingControlBar control bar styles

#define SCBS_EDGELEFT       0x00000001
#define SCBS_EDGERIGHT      0x00000002
#define SCBS_EDGETOP        0x00000004
#define SCBS_EDGEBOTTOM     0x00000008
#define SCBS_EDGEALL        0x0000000F
#define SCBS_SHOWEDGES      0x00000010
#define SCBS_SIZECHILD      0x00000020

/////////////////////////////////////////////////////////////////////////
// CSizingControlBar control bar


class CSizingControlBar;
typedef CTypedPtrArray <CPtrArray, CSizingControlBar*> CSCBArray;

class CSizingControlBar : public CControlBar
{
    DECLARE_DYNAMIC(CSizingControlBar);

// Construction
public:
    CSizingControlBar();

    virtual BOOL Create(LPCTSTR lpszWindowName, CWnd* pParentWnd,
        CSize sizeDefault, BOOL bHasGripper,
        UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP);
    virtual BOOL Create(LPCTSTR lpszWindowName, CWnd* pParentWnd,
        UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP);

// Attributes
public:
    const BOOL IsFloating() const;
    const BOOL IsHorzDocked() const;
    const BOOL IsVertDocked() const;
    const BOOL IsSideTracking() const;
    const BOOL GetSCBStyle() const {return m_dwSCBStyle;}

// Operations
public:
#if defined(_SCB_REPLACE_MINIFRAME) && !defined(_SCB_MINIFRAME_CAPTION)
    void EnableDocking(DWORD dwDockStyle);
#endif
    virtual void LoadState(LPCTSTR lpszProfileName);
    virtual void SaveState(LPCTSTR lpszProfileName);
    static void GlobalLoadState(CFrameWnd* pFrame, LPCTSTR lpszProfileName);
    static void GlobalSaveState(CFrameWnd* pFrame, LPCTSTR lpszProfileName);
    void SetSCBStyle(DWORD dwSCBStyle)
        {m_dwSCBStyle = (dwSCBStyle & ~SCBS_EDGEALL);}

// Overridables
    virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

// Overrides
public:
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSizingControlBar)
    public:
    virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
    virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CSizingControlBar();

protected:
    // implementation helpers
    UINT GetEdgeHTCode(int nEdge);
    BOOL GetEdgeRect(CRect rcWnd, UINT nHitTest, CRect& rcEdge);
    virtual void StartTracking(UINT nHitTest, CPoint point);
    virtual void StopTracking();
    virtual void OnTrackUpdateSize(CPoint& point);
    virtual void OnTrackInvertTracker();
    virtual void NcPaintGripper(CDC* pDC, CRect rcClient);
    virtual void NcCalcClient(LPRECT pRc, UINT nDockBarID);

    virtual void AlignControlBars();
    void GetRowInfo(int& nFirst, int& nLast, int& nThis);
    void GetRowSizingBars(CSCBArray& arrSCBars);
    void GetRowSizingBars(CSCBArray& arrSCBars, int& nThis);
    BOOL NegotiateSpace(int nLengthTotal, BOOL bHorz);

protected:
    DWORD   m_dwSCBStyle;
    UINT    m_htEdge;

    CSize   m_szHorz;
    CSize   m_szVert;
    CSize   m_szFloat;
    CSize   m_szMinHorz;
    CSize   m_szMinVert;
    CSize   m_szMinFloat;
    int     m_nTrackPosMin;
    int     m_nTrackPosMax;
    int     m_nTrackPosOld;
    int     m_nTrackEdgeOfs;
    BOOL    m_bTracking;
    BOOL    m_bKeepSize;
    BOOL    m_bParentSizing;
    BOOL    m_bDragShowContent;
    UINT    m_nDockBarID;
    int     m_cxEdge;

// Generated message map functions
protected:
    //{{AFX_MSG(CSizingControlBar)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnNcPaint();
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
    afx_msg NCHITRETVAL OnNcHitTest(CPoint point);
    afx_msg void OnCaptureChanged(CWnd *pWnd);
    afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
    afx_msg void OnPaint();
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

#ifdef _SCB_REPLACE_MINIFRAME
    friend class CSCBMiniDockFrameWnd;
#endif //_SCB_REPLACE_MINIFRAME
};

#ifdef _SCB_REPLACE_MINIFRAME
#ifndef _SCB_MINIFRAME_CAPTION
/////////////////////////////////////////////////////////////////////////
// CSCBDockContext dockcontext

class CSCBDockContext : public CDockContext
{
public:
// Construction
    CSCBDockContext(CControlBar* pBar) : CDockContext(pBar) {}

// Drag Operations
    virtual void StartDrag(CPoint pt);
};
#endif //_SCB_MINIFRAME_CAPTION

/////////////////////////////////////////////////////////////////////////
// CSCBMiniDockFrameWnd miniframe

class CSCBMiniDockFrameWnd : public CMiniDockFrameWnd
{
    DECLARE_DYNCREATE(CSCBMiniDockFrameWnd)

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSCBMiniDockFrameWnd)
    public:
    virtual BOOL Create(CWnd* pParent, DWORD dwBarStyle);
    //}}AFX_VIRTUAL

// Implementation
public:
    CSizingControlBar* GetSizingControlBar();

    //{{AFX_MSG(CSCBMiniDockFrameWnd)
    afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
    afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
#endif //_SCB_REPLACE_MINIFRAME

#endif // !defined(__SIZINGCONTROLBAR_H__)

