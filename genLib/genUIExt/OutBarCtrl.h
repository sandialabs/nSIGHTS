///////////////////////////////////////////////////////////////////////////////////
//
// OutBarCtrl.h
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

#if !defined(AFX_OUTBARCTRL_H__28FA2CA4_11B7_11D2_8437_0000B43382FE__INCLUDED_)
#define AFX_OUTBARCTRL_H__28FA2CA4_11B7_11D2_8437_0000B43382FE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OutBarCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COutBarCtrl window

#define WM_OUTBAR_NOTIFY        WM_USER + 1
#define NM_OB_ITEMCLICK         1
#define NM_OB_ONLABELENDEDIT    2
#define NM_OB_ONGROUPENDEDIT    3
#define NM_OB_DRAGITEM          4
#define NM_FOLDERCHANGE         5

struct OUTBAR_INFO
{
    int             index;
    const char*     cText;
    int             iDragFrom;
    int             iDragTo;
};

class COutBarCtrl : public CWnd
{
// Construction
    DECLARE_DYNCREATE(COutBarCtrl)
public:
    COutBarCtrl();

    enum {
        fSmallIcon = 1 << 0,
        fLargeIcon = 1 << 1,
        fEditGroups = 1 << 2,
        fEditItems = 1 << 3,
        fRemoveGroups = 1 << 4,
        fRemoveItems = 1 << 5,
        fAddGroups = 1 << 6,
        fDragItems = 1 << 7,
        fAnimation = 1 << 8,
        fSelHighlight = 1 << 9
    };

    enum {
        ircIcon = 1,
        ircLabel = 2,
        ircAll = 3
    };

// Attributes
public:
    COLORREF    crBackGroundColor, crBackGroundColor1;
    COLORREF    crTextColor, crHilightTextColor, crHilightColor;
    COLORREF    cr3dFace, crLightBorder, crHilightBorder, crShadowBorder, crDkShadowBorder;
    int         iFolderHeight;

    int         xSmallIconLabelOffset, yLargeIconLabelOffset;
    int         ySmallIconSpacing, yLargeIconSpacing;

    int         xLeftMargin, yTopMargin;
    bool        bUpArrow, bDownArrow, bUpPressed, bDowpaCalcssed;
    CRect       rcUpArrow, rcDownArrow;
    bool        bLooping;

    int         iHitInternal1, iHitInternal2;

    long        lAnimationTickCount;

    int         iLastSel, iSelAnimTiming;
    int         iSelAnimCount;

    DWORD       dwFlags;

    int         iLastFolderHighlighted;
    int         iLastSelectedFolder;
    int         iFirstItem;

    int         iLastItemHighlighted;
    bool        bPressedHighlight;

    int         iLastDragItemDraw, iLastDragItemDrawType;

    CWnd* m_pWndNotify;

    class CBarItem
    {
    public:
        CBarItem(const char* name, int image, DWORD exData);
        virtual  ~CBarItem();
        int         iImageIndex;
        char*       cItem;
        DWORD       dwData;
        bool        bEnable;
    };

    class CBarFolder
    {
    public:
        CBarFolder(const char* szName, DWORD exData);
        virtual  ~CBarFolder();
        int GetItemCount();
        CBarFolder& InsertItem(const char* text, int image = 0, int index = -1, DWORD exData = 0);
        char*           cName;
        DWORD           dwData;
        CImageList*     pLargeImageList;
        CImageList*     pSmallImageList;
        CArray<CBarItem*,CBarItem*> arItems;
        CWnd*           pChild;
    };

    CArray<CBarFolder*,CBarFolder*> arFolder;

    int iSelFolder;

    CImageList*     pLargeImageList;
    CImageList*     pSmallImageList;

    HCURSOR         hHandCursor;
    HCURSOR         hDragCursor;
    HCURSOR         hNoDragCursor;

    CPen*           pBlackPen;

    COLORREF clrBtnFace;
    COLORREF clrBtnShadow;
    COLORREF clrBtnHilite;
    COLORREF clrDisableText;

// Operations
public:
    enum { htNothing = -1, htFolder, htItem, htUpScroll, htDownScroll};

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(COutBarCtrl)
    //}}AFX_VIRTUAL

// Implementation
public:
    void DrawAnimItem(int xoffset, int yoffset, int index);
    void SetAnimSelHighlight(int iTime);
    DWORD GetFolderData(int iFolder = -1);
    CWnd* GetFolderChild(int iFolder = -1);
    CBarFolder* AddFolderBar(const char* szFolder, CWnd* pSon, DWORD exData = 0);
    CString GetItemText(int index);
    void SetAnimationTickCount(const long value) { lAnimationTickCount = value; };
    long GetAnimationTickCount() { return lAnimationTickCount; };

    void AnimateFolderScroll(int iFrom, int iTo);
    int GetDragItemRect(int index, CRect &rect);
    void DrawDragArrow(CDC* pDC, int iFrom, int iTo);
    void SetItemImage(int index, int iImage);
    void SetItemData(int index, DWORD dwData);
    int  GetItemImage(int index) const;
    DWORD GetItemData(int index) const;
    bool IsValidItem(int index) const;
    void RemoveItem(int index);
    void SetItemText(int index, const char* text);
    void StartItemEdit(int index);
    void SetFolderText(int index, const char* text);
    void StartGroupEdit(int index);
    void GetLabelRect(int iFolder, int iIndex, CRect &rect);
    void GetIconRect(int iFolder, int iIndex, CRect &rect);
    void HighlightItem(int index, bool bPressed = false);
    void GetVisibleRange(int iFolder, int &first, int &last);
    void DrawItem(CDC* pDC, int iFolder, CRect rc, int index, bool bOnlyImage = false);
    CImageList* GetFolderImageList(int index, bool bSmall) const;
    CSize GetItemSize(int iFolder, int index, int type);
    void PaintItems(CDC* pDC, int iFolder, CRect rc);
    CImageList* GetImageList(CImageList* pImageList, int nImageList);
    CImageList* SetFolderImageList(int folder, CImageList* pImageList, int nImageList);
    CImageList* SetImageList(CImageList* pImageList, int nImageList);
    int GetCountPerPage() const;
    void RemoveFolder(int index);
    int GetSelFolder() const;
    int GetFolderCount() const;
    void SetSelFolder(int index);
    int GetItemCount() const;
    int InsertItem(int folder, int nindex, const char* text, int image = -1, DWORD exData = 0);
    void HighlightFolder(int index);
    int HitTestEx(const CPoint &point, int &index);
    void GetInsideRect(CRect &rect) const;
    CBarFolder* AddFolder(const char* cFolderName, DWORD exData = 0);
    void GetItemRect(int iFolder, int iIndex, CRect &rect);
    bool GetFolderRect(int iIndex, CRect &rect) const;
    void ModifyFlag(DWORD &dwRemove, DWORD &dwAdd, UINT uRedraw = 0);
    DWORD GetFlag() const;
    void SetSmallIconView(bool bSet);
    bool IsSmallIconView() const;
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwFlag = fDragItems|fEditGroups|fEditItems|fRemoveGroups|fRemoveItems|fAddGroups|fAnimation|fSelHighlight);
    virtual ~COutBarCtrl();

    CString GetFolderText(int iFolder = -1);
    bool EnableItem(int iFolder, int index, bool bEnable = true);

    void SetNotifyWnd(CWnd* pWnd);
    CWnd* GetNotifyWnd();
    // Generated message map functions
protected:
    void DrawFolder(CDC* pDC, int iIdx, CRect rect, bool bSelected);
    //{{AFX_MSG(COutBarCtrl)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnUixLargeicon();
    afx_msg void OnUpdateUixLargeicon(CCmdUI* pCmdUI);
    afx_msg void OnUixSmallicon();
    afx_msg void OnUpdateUixSmallicon(CCmdUI* pCmdUI);
    afx_msg void OnUixRemoveitem();
    afx_msg void OnUpdateUixRemoveitem(CCmdUI* pCmdUI);
    afx_msg void OnUixRenameitem();
    afx_msg void OnUpdateUixRenameitem(CCmdUI* pCmdUI);
    //}}AFX_MSG
    afx_msg LRESULT OnEndLabelEdit(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
protected:
    void DrawDisabledItem(CDC& dc, const CRect& rc) const;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTBARCTRL_H__28FA2CA4_11B7_11D2_8437_0000B43382FE__INCLUDED_)

