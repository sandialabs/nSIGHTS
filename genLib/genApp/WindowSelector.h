///////////////////////////////////////////////////////////////////////////////////
//
// WindowSelector.h
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
// DESCRIPTION: Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//              Implements WindowSelector UI control
//
///////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINDOWSELECTOR_H__6B0C6EA1_897D_11D4_BD06_00104B4C74FD__INCLUDED_)
#define AFX_WINDOWSELECTOR_H__6B0C6EA1_897D_11D4_BD06_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WindowSelector.h : header file
//
#include <genClassPS/PS_NCHitTest.h>

/////////////////////////////////////////////////////////////////////////////
// WindowSelector window

typedef void (*SelectCallback)(int);
typedef void (*PlotsCallback)();
typedef void (*TilePlotsCallback)(int);


class SelectorList : public CListCtrl
{
    // Generated message map functions
protected:
    //{{AFX_MSG(WindowSelector)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

class WindowSelector : public CWnd
{
    // Construction
public:
    WindowSelector();
    virtual ~WindowSelector();

    void            Init(SelectCallback    cbSelect = NULL,
                         PlotsCallback     minPlotsCallback = NULL,
                         TilePlotsCallback tilePlotsCallback = NULL);

    bool            Switch();
    bool            IsEnabled();

    void            RemoveAll();
    void            AddItem(int nItem, const CString& sMenu);
    void            Redraw();
    void            SelectItem(int nItem);
    int             GetSelectedID();
    bool            IsPinned();
    bool            IsProcessingSelectionCallback();

    void            EnableTileCascade(bool bEnable);

    void            SetAlpha(int nAlpha);

protected:
    void            ToggleTransparency(int nAlpha);
protected:
    CToolBar        m_wndToolBar;
    SelectorList    m_list;
    bool            m_bInit;
    bool            m_bEnable;
    bool            m_bNotify;
    bool            m_bProcessingCallback;

    bool            m_bOwnerCreated;
    CWnd            m_wndOwnerWindow;

    bool            m_bButtonPinned;
    CRect           m_rcButton;
    CBitmap         m_bmpPinned;
    CBitmap         m_bmpUnpinned;

    SelectCallback  m_cbSelect;
    PlotsCallback   m_minPlotsCallback;
    TilePlotsCallback m_tilePlotsCallback;

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(WindowSelector)
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //}}AFX_VIRTUAL


    // Generated message map functions
protected:
    //{{AFX_MSG(WindowSelector)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnClose();
    afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
    afx_msg void OnNcPaint();
    afx_msg BOOL OnNcActivate(BOOL bActive);
    afx_msg NCHITRETVAL OnNcHitTest(CPoint point);
    //}}AFX_MSG
    afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnMinimizeAllPlots();
    afx_msg void OnTileAllPlotsHorz();
    afx_msg void OnTileAllPlotsVert();
    afx_msg void OnCascadeAllPlots();
    afx_msg void OnToggleTransparency();
    DECLARE_MESSAGE_MAP()
};


extern WindowSelector windowSelector;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDOWSELECTOR_H__6B0C6EA1_897D_11D4_BD06_00104B4C74FD__INCLUDED_)

