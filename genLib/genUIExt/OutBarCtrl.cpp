///////////////////////////////////////////////////////////////////////////////////
//
// OutBarCtrl.cpp
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

// OutBarCtrl.cpp : implementation file
//

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include "OutBarCtrl.h"
#include "GroupEdit.h"

#include <GenLibRes.h>
#include <genUIExt/BitmapMenu.h>


const char* sz_ID_UIX_SMALLICON = "Small icons";
const char* sz_ID_UIX_LARGEICON = "Large Icons";
const char* sz_ID_UIX_REMOVEITEM    = "Remove";
const char* sz_ID_UIX_RENAMEITEM    = "Rename";

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutBarCtrl

IMPLEMENT_DYNCREATE(COutBarCtrl, CWnd)

COutBarCtrl::COutBarCtrl()
{
    crBackGroundColor = GetSysColor(COLOR_BACKGROUND);
    crBackGroundColor1 = GetSysColor(COLOR_BACKGROUND);
    crTextColor = GetSysColor(COLOR_WINDOW);
    crHilightTextColor = GetSysColor(COLOR_MENUTEXT);
    crHilightColor = GetSysColor(COLOR_WINDOW);
    cr3dFace = GetSysColor(COLOR_3DFACE);
    crLightBorder = GetSysColor(COLOR_3DLIGHT);
    crHilightBorder = GetSysColor(COLOR_3DHILIGHT);
    crShadowBorder = GetSysColor(COLOR_3DSHADOW);
    crDkShadowBorder = GetSysColor(COLOR_3DDKSHADOW);

    pBlackPen = new CPen(PS_SOLID, 1, RGB(0,0,0));

    //iFolderHeight = 22;
    iFolderHeight = 18;

    dwFlags = fDragItems|fEditGroups|fEditItems|fRemoveGroups|fRemoveItems|fAddGroups|fAnimation|fSelHighlight;

    iSelFolder = 1;

    iLastFolderHighlighted = -1;
    iLastSelectedFolder = -1;

    pLargeImageList = NULL;
    pSmallImageList = NULL;

    iFirstItem = 0;
/*
    xSmallIconLabelOffset = 4;
    yLargeIconLabelOffset = 3;

    ySmallIconSpacing = 10;
    yLargeIconSpacing = 8;

    xLeftMargin = 5;
    yTopMargin = 5;
*/
    xSmallIconLabelOffset = 3;
    yLargeIconLabelOffset = 2;

    ySmallIconSpacing = 9;
    yLargeIconSpacing = 7;

    xLeftMargin = 4;
    yTopMargin = 4;

    bUpArrow = bDownArrow = false;

    hHandCursor = AfxGetApp()->LoadCursor(IDC_HANDCUR);
    rcUpArrow = rcDownArrow = CRect(0,0,0,0);

    bUpPressed = bDowpaCalcssed = false;

    bLooping = false;

    iLastItemHighlighted = -1;
    bPressedHighlight = false;

    iLastDragItemDraw = -1;

    lAnimationTickCount = 10;

    hDragCursor = AfxGetApp()->LoadCursor(IDC_DRAGGING);
    hNoDragCursor = AfxGetApp()->LoadCursor(IDC_NODRAGGING);

    iLastSel = -1;
    iSelAnimCount = 0;

    iSelAnimTiming = 0;

    clrBtnFace   = ::GetSysColor(COLOR_BTNFACE);
    clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
    clrBtnHilite = ::GetSysColor(COLOR_BTNHIGHLIGHT);
    clrDisableText = ::GetSysColor(COLOR_BTNSHADOW);
    m_pWndNotify = NULL;
}

COutBarCtrl::~COutBarCtrl()
{
    for (int t = 0; t < arFolder.GetSize(); t++) {
        if ( arFolder.GetAt(t) != NULL ) {
            delete arFolder.GetAt(t);
        }
    }
    arFolder.RemoveAll();

    delete pBlackPen;
}


BEGIN_MESSAGE_MAP(COutBarCtrl, CWnd)
    //{{AFX_MSG_MAP(COutBarCtrl)
    ON_WM_CREATE()
    ON_WM_TIMER()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_SETCURSOR()
    ON_WM_SIZE()
    ON_WM_RBUTTONDOWN()
    ON_COMMAND(ID_UIX_LARGEICON, OnUixLargeicon)
    ON_UPDATE_COMMAND_UI(ID_UIX_LARGEICON, OnUpdateUixLargeicon)
    ON_COMMAND(ID_UIX_SMALLICON, OnUixSmallicon)
    ON_UPDATE_COMMAND_UI(ID_UIX_SMALLICON, OnUpdateUixSmallicon)
    ON_COMMAND(ID_UIX_REMOVEITEM, OnUixRemoveitem)
    ON_UPDATE_COMMAND_UI(ID_UIX_REMOVEITEM, OnUpdateUixRemoveitem)
    ON_COMMAND(ID_UIX_RENAMEITEM, OnUixRenameitem)
    ON_UPDATE_COMMAND_UI(ID_UIX_RENAMEITEM, OnUpdateUixRenameitem)
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_OUTBAR_NOTIFY, OnEndLabelEdit)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COutBarCtrl message handlers

LRESULT COutBarCtrl::OnEndLabelEdit(WPARAM wParam, LPARAM lParam)
{
    if (wParam == NM_OB_ONGROUPENDEDIT) {
        CGroupEdit* pEdit = (CGroupEdit*)lParam;
        OUTBAR_INFO ob;
        ob.index = pEdit->iIndex;
        ob.cText = pEdit->text;

        if (GetNotifyWnd()->SendMessage(WM_OUTBAR_NOTIFY, NM_OB_ONGROUPENDEDIT, (LPARAM) &ob)) {
            SetFolderText(pEdit->iIndex, pEdit->text);
            CRect rc;
            GetFolderRect(pEdit->iIndex, rc);
            InvalidateRect(rc, false);
        }
    }
    if (wParam == NM_OB_ONLABELENDEDIT) {
        CGroupEdit* pEdit = (CGroupEdit*)lParam;
        OUTBAR_INFO ob;
        ob.index = pEdit->iIndex;
        ob.cText = pEdit->text;

        if (GetNotifyWnd()->SendMessage(WM_OUTBAR_NOTIFY, NM_OB_ONLABELENDEDIT, (LPARAM) &ob)) {
            SetItemText(pEdit->iIndex, pEdit->text);
            CRect rc;
            GetInsideRect(rc);
            InvalidateRect(rc, true);
        }
    }
    return 0;
}

void COutBarCtrl::SetFolderText(int index, const char* text)
{
    ASSERT(index >= 0 && index < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(index);
    AllocAndCopyString(pbf->cName, text);
}

void COutBarCtrl::SetItemText(int index, const char* text)
{
    ASSERT(iSelFolder >= 0 && iSelFolder < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(iSelFolder);
    CBarItem* pi = pbf->arItems.GetAt(index);
    AllocAndCopyString(pi->cItem, text);
}

BOOL COutBarCtrl::Create(DWORD dwStyle, const RECT & rect, CWnd* pParentWnd, UINT nID, DWORD dwFlag)
{
    dwFlags = dwFlag;
    return CWnd::CreateEx(WS_EX_STATICEDGE, NULL, NULL, dwStyle|WS_CHILD, rect, pParentWnd, nID);
}

bool COutBarCtrl::IsSmallIconView() const
{
    return dwFlags & fSmallIcon;
}

void COutBarCtrl::SetSmallIconView(bool bSet)
{
    iFirstItem = 0;

    if (bSet) {
        if (!IsSmallIconView()) {
            dwFlags |= fSmallIcon;
            CRect rc;
            GetInsideRect(rc);
            InvalidateRect(rc, false);
        }
    } else {
        if (IsSmallIconView()) {
            dwFlags &= ~fSmallIcon;
            CRect rc;
            GetInsideRect(rc);
            InvalidateRect(rc, false);
        }
    }
}

DWORD COutBarCtrl::GetFlag() const
{
    return dwFlags;
}

void COutBarCtrl::ModifyFlag(DWORD & dwRemove, DWORD & dwAdd, UINT uRedraw)
{
    if (dwRemove)
        dwFlags &= ~(dwRemove);
    if (dwAdd)
        dwFlags |= dwAdd;

    if (GetSafeHwnd()) {
        if (uRedraw != 0)
            SetWindowPos(0,0,0,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOMOVE|uRedraw);
        else
            Invalidate();
    }
}

int COutBarCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

void COutBarCtrl::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1) {
        CPoint pt(GetMessagePos());

        ScreenToClient(&pt);

        CRect rc;
        GetClientRect(&rc);
        if (!(rc.PtInRect(pt))) {
            HighlightFolder(-1);
            HighlightItem(-1);
            KillTimer(1);
        }
    } else if (nIDEvent == 3 && iLastSel >= 0) {
        iSelAnimCount ++;
        if (iSelAnimCount > 10) iSelAnimCount = -1;
        if (iSelAnimCount == 0)
            DrawAnimItem(-1, 1, iLastSel);
        if (iSelAnimCount == 1)
            DrawAnimItem(0, 0, iLastSel);
        if (iSelAnimCount == 2)
            DrawAnimItem(1, 1, iLastSel);
        if (iSelAnimCount == 3)
            DrawAnimItem(0, 0, iLastSel);
    }
    CWnd::OnTimer(nIDEvent);
}

void COutBarCtrl::DrawAnimItem(int xoffset, int yoffset, int index)
{
    CImageList* ima = GetFolderImageList(iSelFolder, IsSmallIconView());

    CRect rc, irc;
    GetInsideRect(irc);
    GetItemRect(iSelFolder, iLastSel, rc);

    if (iLastSel >= iFirstItem && irc.bottom > rc.bottom && irc.top < rc.top) {
        ASSERT(iSelFolder >= 0 && iSelFolder < GetFolderCount());
        CBarFolder* pbf = arFolder.GetAt(iSelFolder);

        ASSERT(index >= 0 && index < pbf->GetItemCount());
        CBarItem* pi = pbf->arItems.GetAt(index);

        ASSERT(pi && ima);

        CClientDC dc(this);

        if (IsSmallIconView()) {
            if (ima) {
                IMAGEINFO ii;
                ima->GetImageInfo(pi->iImageIndex, &ii);
                CSize szImage = CRect(ii.rcImage).Size();
                CPoint pt;
                pt.x = rc.left + 2;
                pt.y = rc.top + (rc.Height() - szImage.cy) / 2;

                CRect rcBck(pt.x-1, pt.y-1, pt.x + szImage.cx+2, pt.y + szImage.cy+2);
                dc.FillSolidRect(rcBck, crBackGroundColor);

                pt.x += xoffset;
                pt.y += yoffset;

                ima->Draw(&dc, pi->iImageIndex, pt, ILD_NORMAL);

            }
        } else {
            if (ima) {
                IMAGEINFO ii;
                ima->GetImageInfo(pi->iImageIndex, &ii);
                CSize szImage = CRect(ii.rcImage).Size();
                CPoint pt;
                pt.x = rc.left + (rc.Width() - szImage.cx) / 2;
                pt.y = rc.top;

                CRect rcBck(pt.x-1, pt.y-1, pt.x + szImage.cx+2, pt.y + szImage.cy+2);
                dc.FillSolidRect(rcBck, crBackGroundColor);

                pt.x += xoffset;
                pt.y += yoffset;

                ima->Draw(&dc, pi->iImageIndex, pt, ILD_NORMAL);
            }
        }
    }
}

void COutBarCtrl::OnPaint()
{
    CRect clientRc;
    GetClientRect(&clientRc);
    CPaintDC dc(this);
    CDC memDC;
    memDC.CreateCompatibleDC(&dc);
    CBitmap bmp;
    bmp.CreateCompatibleBitmap(&dc, clientRc.Width(), clientRc.Height());
    CBitmap* obmp = memDC.SelectObject(&bmp);

    CDC* pDC = &memDC;

    CRect rc;
    GetInsideRect(rc);
    if (!GetFolderChild())
        pDC->FillSolidRect(rc, crBackGroundColor);

    int max = (int) arFolder.GetSize();
    CRect frc;
    for (int t = 0; t < max; t++) {
        GetFolderRect(t, frc);
        DrawFolder(pDC, t, frc, false);
    }
    if (!GetFolderChild()) {
        int f,l;
        GetVisibleRange(iSelFolder, f,l);
        rcUpArrow.SetRect(0,0,GetSystemMetrics(SM_CXVSCROLL), GetSystemMetrics(SM_CXVSCROLL));
        rcDownArrow = rcUpArrow;
        rcUpArrow.OffsetRect(rc.right - 5 - GetSystemMetrics(SM_CXVSCROLL), rc.top +5);
        rcDownArrow.OffsetRect(rc.right - 5 - GetSystemMetrics(SM_CXVSCROLL), rc.bottom - 5 - GetSystemMetrics(SM_CXVSCROLL));

        PaintItems(pDC, iSelFolder, rc);

        if (f > 0) {
            if (bUpPressed)
                pDC->DrawFrameControl(rcUpArrow, DFC_SCROLL, DFCS_SCROLLUP|DFCS_PUSHED);
            else
                pDC->DrawFrameControl(rcUpArrow, DFC_SCROLL, DFCS_SCROLLUP);
            bUpArrow = true;
            pDC->ExcludeClipRect(rcUpArrow);
        } else {
            bUpArrow = false;
        }

        if (l < GetItemCount() - 1) {
            if (bDowpaCalcssed)
                pDC->DrawFrameControl(rcDownArrow, DFC_SCROLL, DFCS_SCROLLDOWN|DFCS_PUSHED);
            else
                pDC->DrawFrameControl(rcDownArrow, DFC_SCROLL, DFCS_SCROLLDOWN);
            bDownArrow = true;
            pDC->ExcludeClipRect(rcDownArrow);
        } else {
            bDownArrow = false;
        }

    }

    dc.BitBlt(clientRc.left, clientRc.top, clientRc.Width(), clientRc.Height(), &memDC, 0,0, SRCCOPY);

    memDC.SelectObject(obmp);

    if (iLastFolderHighlighted >= 0) {
        int i = iLastFolderHighlighted;
        iLastFolderHighlighted = -1;
        HighlightFolder(i);
    }
    if (iLastItemHighlighted >= 0) {
        int i = iLastItemHighlighted;
        iLastItemHighlighted = -1;
        HighlightItem(i, bPressedHighlight);
    }
}

BOOL COutBarCtrl::OnEraseBkgnd(CDC* pDC)
{
    return true;
}

bool COutBarCtrl::GetFolderRect(int iIndex, CRect& rect) const
{
    int max = (int) arFolder.GetSize();
    ASSERT(iIndex >= 0 && iIndex < max);

    if (iIndex >= 0 && iIndex < max) {
        CRect rc;
        GetClientRect(rc);
        if (iIndex > iSelFolder) {
            rect.SetRect(rc.left, rc.bottom - ((max - iIndex))* iFolderHeight, rc.right,
                rc.bottom - (max - iIndex - 1)* iFolderHeight);
        } else {
            rect.SetRect(rc.left, rc.top + iIndex* iFolderHeight - 1, rc.right,
                rc.top + (1 + iIndex)* iFolderHeight - 1);
        }
        return true;
    }
    return false;
}

void COutBarCtrl::GetItemRect(int iFolder, int iIndex, CRect& rect)
{
    CRect rc;
    GetInsideRect(rc);
    int top = rc.top;
    CSize sz(0,0);
    int y = 0;
    for (int t = 0; t < iIndex; t++) {
        sz = GetItemSize(iFolder, t, ircAll);
        top += sz.cy;

        if (IsSmallIconView())  {
            top += ySmallIconSpacing;
        } else {
            top += yLargeIconSpacing;
        }
        if (t == iFirstItem - 1) {
            y = top - rc.top;
        }
    }
    sz = GetItemSize(iFolder, iIndex, ircAll);
    rect.SetRect(rc.left, top, rc.left + sz.cx, top + sz.cy);

    rect.top -= y;
    rect.bottom -= y;

    rect.left += xLeftMargin;
    rect.right += xLeftMargin;
    rect.top  += yTopMargin;
    rect.bottom += yTopMargin;

    if (!IsSmallIconView()) {
        rect.left = rc.left;
        rect.right = rc.right;
    }
}

void COutBarCtrl::DrawFolder(CDC* pDC, int iIdx, CRect rect, bool bSelected)
{
    CBarFolder* pbf = arFolder.GetAt(iIdx);

    if (!bSelected) {
        CPen pn1(PS_SOLID, 1, crBackGroundColor1);
        CPen* op = pDC->SelectObject(&pn1);
        pDC->MoveTo(rect.left, rect.top);
        pDC->LineTo(rect.right, rect.top);
        pDC->SelectObject(op);

        rect.top++;

        pDC->Draw3dRect(rect, crHilightBorder, crBackGroundColor1);
        rect.InflateRect(-1,-1);
        pDC->FillSolidRect(rect, cr3dFace);
        int obk = pDC->SetBkMode(TRANSPARENT);
        CFont* of = pDC->SelectObject(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
        pDC->DrawText(CString(pbf->cName), rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        pDC->SetBkMode(obk);
        pDC->SelectObject(of);
    } else {
        CPen pn1(PS_SOLID, 1, crBackGroundColor1);
        CPen* op = pDC->SelectObject(&pn1);
        pDC->MoveTo(rect.left+1, rect.top);
        pDC->LineTo(rect.right, rect.top);
        pDC->SelectObject(op);

        rect.top++;

        pDC->Draw3dRect(rect, crDkShadowBorder, crHilightBorder);
        rect.InflateRect(-1,-1);
        pDC->Draw3dRect(rect, crBackGroundColor1, cr3dFace);
        rect.InflateRect(-1,-1);

        pDC->FillSolidRect(rect, cr3dFace);

        int obk = pDC->SetBkMode(TRANSPARENT);
        CFont* of = pDC->SelectObject(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
        pDC->DrawText(CString(pbf->cName), rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        pDC->SetBkMode(obk);
        pDC->SelectObject(of);
    }
}

COutBarCtrl::CBarFolder* COutBarCtrl::AddFolder(const char* cFolderName, DWORD exData)
{
    CBarFolder* pbf = new CBarFolder(cFolderName, exData);
    ASSERT(pbf);
    arFolder.Add(pbf);
    return pbf;
}

COutBarCtrl::CBarFolder::CBarFolder(const char* name, DWORD exData)
{
    cName = NULL;
    dwData = exData;

    AllocAndCopyString(cName, name);

    pLargeImageList = NULL;
    pSmallImageList = NULL;
    pChild = NULL;
}

COutBarCtrl::CBarFolder::~CBarFolder()
{
    if (cName)
        delete [] cName;
    for (int t = 0; t < arItems.GetSize(); t++) {
        if (arItems.GetAt(t)) {
            delete  arItems.GetAt(t);
        }
    }

    arItems.RemoveAll();
}

void COutBarCtrl::GetInsideRect(CRect & rect) const
{
    GetClientRect(rect);
    if (arFolder.GetSize() > 0) {
        int max = (int) arFolder.GetSize();
        rect.top += iFolderHeight* (iSelFolder + 1) - 1;//+ 2;
        rect.bottom -= (max - iSelFolder - 1)*iFolderHeight;
    }
}

void COutBarCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
    int index, ht = HitTestEx(point, index);

    if (ht != htFolder && iLastFolderHighlighted >= 0)
        HighlightFolder(-1);
    if (ht != htItem  && iLastItemHighlighted >= 0)
        HighlightItem(-1);

    if (ht == htFolder) {
        HighlightFolder(index);
        SetTimer(1,100,NULL);
    } else if (ht == htItem) {
        HighlightItem(index);
        SetTimer(1,100,NULL);
    }

    CWnd::OnMouseMove(nFlags, point);
}

int COutBarCtrl::HitTestEx(const CPoint & point, int &index)
{
    if (bUpArrow && rcUpArrow.PtInRect(point))
        return htUpScroll;
    if (bDownArrow && rcDownArrow.PtInRect(point))
        return htDownScroll;

    int max = (int) arFolder.GetSize();
    CRect rc;
    for ( int t = 0; t < max; t++) {
        GetFolderRect(t, rc);
        if (rc.PtInRect(point))
        {
            index = t;
            return htFolder;
        }
    }
    GetInsideRect(rc);
    CBarFolder* pbf = arFolder.GetAt(iSelFolder);
    max = pbf->GetItemCount();
    for (int t = iFirstItem; t < max; t++) {
        CBarItem* pi = pbf->arItems.GetAt(t);
        CRect itemRc;
        if (!IsSmallIconView())  {
            GetIconRect(iSelFolder, t, itemRc);
            if (itemRc.PtInRect(point)) {
                index = t;
                return htItem;
            } else if (itemRc.top > rc.bottom) {
                break;
            }
            GetLabelRect(iSelFolder, t, itemRc);
            itemRc.top -= yLargeIconLabelOffset;

            if (itemRc.PtInRect(point)) {
                index = t;
                return htItem;
            } else if (itemRc.top > rc.bottom) {
                break;
            }
        } else {
            GetItemRect(iSelFolder, t, itemRc);
            if (itemRc.PtInRect(point)) {
                index = t;
                return htItem;
            } else if (itemRc.top > rc.bottom) {
                break;
            }
        }
    }
    return htNothing;
}

void COutBarCtrl::HighlightFolder(int index)
{
    CWnd* pf = GetFocus();
    if (pf != NULL && pf != this && IsChild(pf))
        return;

    if (iLastFolderHighlighted == index)
        return;

    if (iLastFolderHighlighted >= 0) {
        CRect rc;
        if (GetFolderRect(iLastFolderHighlighted, rc)) {
            CClientDC dc(this);
            CPen pn(PS_SOLID, 1, crBackGroundColor1);//crShadowBorder);
            CPen* op = dc.SelectObject(&pn);
            dc.MoveTo(rc.left, rc.bottom-1);
            dc.LineTo(rc.right-1, rc.bottom-1);
            dc.LineTo(rc.right-1, rc.top);
            CPen pn1(PS_SOLID, 1, cr3dFace);
            dc.SelectObject(&pn1);
            dc.MoveTo(rc.left+1, rc.bottom-2);
            dc.LineTo(rc.right-2, rc.bottom-2);
            dc.LineTo(rc.right-2, rc.top+1);
            dc.SelectObject(op);
        }
    }
    iLastFolderHighlighted = index;
    if (iLastFolderHighlighted >= 0) {
        CRect rc;
        if (GetFolderRect(iLastFolderHighlighted, rc)) {
            CClientDC dc(this);
            CPen pn(PS_SOLID, 1, crDkShadowBorder);
            CPen* op = dc.SelectObject(&pn);
            dc.MoveTo(rc.left, rc.bottom-1);
            dc.LineTo(rc.right-1, rc.bottom-1);
            dc.LineTo(rc.right-1, rc.top);
            CPen pn1(PS_SOLID, 1, crBackGroundColor1);
            dc.SelectObject(&pn1);
            dc.MoveTo(rc.left+1, rc.bottom-2);
            dc.LineTo(rc.right-2, rc.bottom-2);
            dc.LineTo(rc.right-2, rc.top+1);
            dc.SelectObject(op);
        }
    }
}

void COutBarCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (GetFocus() != this)
        SetFocus();

    int index, ht = HitTestEx(point, index);
    iLastDragItemDrawType = -1;

    CRect inRc;
    GetInsideRect(inRc);

    if (ht == htFolder) {
        bool bHigh = true;
        CRect rc;
        GetFolderRect(index, rc);

        if (::GetCapture() == NULL) {
            SetCapture();
            ASSERT(this == GetCapture());
            CClientDC dc(this);
            DrawFolder(&dc, index, rc, true);
            AfxLockTempMaps();
            for (;;) {
                MSG msg;
                VERIFY(::GetMessage(&msg, NULL, 0, 0));

                if (CWnd::GetCapture() != this) break;

                switch (msg.message)
                {
                case WM_MOUSEMOVE:
                    {
                        CPoint pt(msg.lParam);
                        int idx, ht1 = HitTestEx(pt, idx);
                        if (ht1 == htFolder && idx == index) {
                            if (!bHigh) {
                                DrawFolder(&dc, index, rc, true);
                                bHigh = true;
                            }
                        } else {
                            if (bHigh) {
                                DrawFolder(&dc, index, rc, false);
                                bHigh = false;
                            }
                        }
                    }
                    break;

                case WM_LBUTTONUP:
                    {
                        if (bHigh) {
                            DrawFolder(&dc, index, rc, false);
                            bHigh = false;
                        }
                        CPoint pt(msg.lParam);
                        int idx, ht1 = HitTestEx(pt, idx);
                        if (ht1 == htFolder && idx != iSelFolder) {
                            SetSelFolder(idx);
                        }
                    }

                    goto ExitLoop2;

                case WM_KEYDOWN:
                    if (msg.wParam != VK_ESCAPE)
                        break;

                default:
                    DispatchMessage(&msg);
                    break;
                }
            }

        ExitLoop2:
            ReleaseCapture();
            AfxUnlockTempMaps(FALSE);
        }
        if (bHigh)
            InvalidateRect(rc, false);
    } else {
        iLastSelectedFolder = -1;
    }

    if (ht == htItem) {
        iLastDragItemDraw = -1;
        bool bHigh = true;
        bool bDragging = false;
        CRect rc;
        GetItemRect(iSelFolder, index, rc);

        // Disabled item
        CBarFolder* pbf = arFolder.GetAt(iSelFolder);
        CBarItem* item = pbf->arItems.GetAt(index);
        if (!item->bEnable) {
            return;
        }

        HCURSOR hCur = GetCursor();

        if (::GetCapture() == NULL) {
            SetCapture();
            ASSERT(this == GetCapture());
            CClientDC dc(this);
            HighlightItem(index, true);
            AfxLockTempMaps();
            for (;;) {
                MSG msg;
                VERIFY(::GetMessage(&msg, NULL, 0, 0));

                if (CWnd::GetCapture() != this) break;

                switch (msg.message) {
                case WM_MOUSEMOVE:
                    {
                        CPoint pt(msg.lParam);
                        int idx, ht1 = HitTestEx(pt, idx);
                        if (bDragging) {
                            if (ht1 == htItem)  {
                                DrawDragArrow(&dc, index, idx);
                                SetCursor(hDragCursor);
                                hCur = hDragCursor;
                            } else {
                                CRect rcItem;
                                GetItemRect(iSelFolder, GetItemCount() - 1, rcItem);
                                if (pt.y > rcItem.bottom && pt.y < inRc.bottom) {
                                    DrawDragArrow(&dc, index, GetItemCount());
                                    SetCursor(hDragCursor);
                                    hCur = hDragCursor;
                                } else {
                                    DrawDragArrow(&dc, index, -1);
                                    SetCursor(hNoDragCursor);
                                    hCur = hNoDragCursor;
                                }
                            }
                        } else {
                            if (ht1 == htItem && idx == index) {
                                if (!bHigh) {
                                    HighlightItem(index, true);
                                    bHigh = true;
                                    bPressedHighlight = true;
                                }
                            } else {
                                if (ht1 == htItem) {
                                    if (bHigh) {
                                        HighlightItem(index, false);
                                        bHigh = false;
                                        bPressedHighlight = false;
                                    }
                                } else {
                                    if (dwFlags&fDragItems) {
                                        HighlightItem(index, true);
                                        bHigh = true;
                                        bDragging = true;

                                        SetCursor(hDragCursor);
                                        hCur = hDragCursor;

                                        bPressedHighlight = true;
                                    }
                                }
                            }
                        }
                    }
                    break;

                case WM_SETCURSOR:
                    SetCursor(hCur);
                    break;

                case WM_LBUTTONUP:
                    {
                        if (bHigh) {
                            HighlightItem(-1);
                            bHigh = false;
                        }
                        CPoint pt(msg.lParam);
                        int idx, ht1 = HitTestEx(pt, idx);
                        if (!bDragging) {
                            if (ht1 == htItem && idx == index)  {
                                if (iSelAnimTiming > 0 && index != iLastSel && iLastSel >= 0) {
                                    DrawAnimItem(0, 0, iLastSel);
                                }

                                if (dwFlags & fSelHighlight && iLastSel >= 0) {
                                    CRect rc;
                                    GetIconRect(iSelFolder, iLastSel, rc);
                                    rc.InflateRect(1,1);
                                    InvalidateRect(rc);
                                }

                                iLastSel = index;

                                if (dwFlags & fSelHighlight && iLastSel >= 0) {
                                    CRect rc;
                                    GetIconRect(iSelFolder, iLastSel, rc);
                                    rc.InflateRect(1,1);
                                    InvalidateRect(rc);
                                }

                                GetNotifyWnd()->SendMessage(WM_OUTBAR_NOTIFY, NM_OB_ITEMCLICK, idx);
                            }
                        } else {
                            if (ht1 == htItem) {
                                if (idx != index) {
                                    OUTBAR_INFO ob;
                                    ob.iDragFrom = iSelFolder;
                                    ob.iDragTo = index;
                                    if (GetNotifyWnd()->SendMessage(WM_OUTBAR_NOTIFY, NM_OB_DRAGITEM, (LPARAM) &ob)) {
                                        CBarFolder* pbf = arFolder.GetAt(iSelFolder);
                                        CBarItem* piFrom = pbf->arItems.GetAt(index);
                                        pbf->arItems.RemoveAt(index);
                                        if (idx > index) idx --;
                                        pbf->arItems.InsertAt(idx, piFrom);
                                    }
                                }
                            } else {
                                CRect rcItem;
                                GetItemRect(iSelFolder, GetItemCount() - 1, rcItem);
                                if (pt.y > rcItem.bottom && pt.y < inRc.bottom) {
                                    OUTBAR_INFO ob;
                                    ob.iDragFrom = iSelFolder;
                                    ob.iDragTo = index;
                                    if (GetNotifyWnd()->SendMessage(WM_OUTBAR_NOTIFY, NM_OB_DRAGITEM, (LPARAM) &ob)) {
                                        CBarFolder* pbf = arFolder.GetAt(iSelFolder);
                                        CBarItem* piFrom = pbf->arItems.GetAt(index);
                                        pbf->arItems.RemoveAt(index);
                                        pbf->arItems.Add(piFrom);
                                    }
                                }
                            }
                        }
                    }

                    goto ExitLoop4;

                default:
                    DispatchMessage(&msg);
                    break;
                }
            }

        ExitLoop4:
            ReleaseCapture();
            AfxUnlockTempMaps(FALSE);
            if (bDragging) {
                Invalidate();
            }
        }
        bPressedHighlight = false;
        //if (bHigh)
        //    InvalidateRect(rc, false);
        //updates entire folder - to ensure step is highlighted permanently
        //done to highlight step for Singen
        InvalidateRect(inRc, false);

    } else {
        iLastItemHighlighted = -1;
    }

    if (ht == htDownScroll)
    {
        bLooping = true;
        bool bHigh = true;

        if (::GetCapture() == NULL) {
            SetCapture();
            ASSERT(this == GetCapture());
            CClientDC dc(this);
            dc.DrawFrameControl(rcDownArrow, DFC_SCROLL, DFCS_SCROLLDOWN|DFCS_PUSHED);
            SetTimer(2,300,NULL);
            AfxLockTempMaps();
            for (;;) {
                MSG msg;
                VERIFY(::GetMessage(&msg, NULL, 0, 0));

                if (CWnd::GetCapture() != this) break;

                switch (msg.message) {
                case WM_MOUSEMOVE:
                    {
                        CPoint pt(msg.lParam);
                        if (rcDownArrow.PtInRect(pt)) {
                            if (bHigh == false) {
                                dc.DrawFrameControl(rcDownArrow, DFC_SCROLL, DFCS_SCROLLDOWN|DFCS_PUSHED);
                                bHigh = true;
                                bDownArrow = true;
                                bDowpaCalcssed = true;
                            }
                        } else {
                            if (bHigh == true) {
                                dc.DrawFrameControl(rcDownArrow, DFC_SCROLL, DFCS_SCROLLDOWN);
                                bHigh = false;
                                bDownArrow = false;
                                bDowpaCalcssed = false;
                            }
                        }
                    }
                    break;

                case WM_LBUTTONUP:
                    {
                        if (bHigh) {
                            dc.DrawFrameControl(rcDownArrow, DFC_SCROLL, DFCS_SCROLLDOWN);
                            bHigh = false;
                        }
                        bDownArrow = false;
                        CPoint pt(msg.lParam);
                        if (rcDownArrow.PtInRect(pt)) {
                            CRect itrc;
                            GetItemRect(iSelFolder, GetItemCount() - 1, itrc);
                            CRect crc;
                            GetInsideRect(crc);
                            if (itrc.bottom > crc.bottom)
                            {
                                iFirstItem ++;
                                InvalidateRect(crc, true);
                            }
                        }
                    }
                    goto ExitLoop3;


                case WM_TIMER:
                    {
                        if (msg.wParam == 2) {
                            if (bHigh) {
                                CPoint pt(msg.pt);
                                ScreenToClient(&pt);
                                if (rcDownArrow.PtInRect(pt)) {
                                    bDowpaCalcssed = true;
                                    CRect itrc;
                                    GetItemRect(iSelFolder, GetItemCount() - 1, itrc);
                                    CRect crc;
                                    GetInsideRect(crc);
                                    if (itrc.bottom > crc.bottom) {
                                        iFirstItem ++;
                                        InvalidateRect(crc, true);
                                    }
                                    else goto ExitLoop3;
                                }
                                else bDowpaCalcssed = false;
                            }
                        }
                        break;
                    }

                case WM_KEYDOWN:
                    if (msg.wParam != VK_ESCAPE)
                        break;

                default:
                    DispatchMessage(&msg);
                    break;
                }
            }

        ExitLoop3:
            KillTimer(2);
            ReleaseCapture();
            AfxUnlockTempMaps(FALSE);
            bLooping = false;
            bDowpaCalcssed = false;
            bDownArrow = false;
            CRect crc;
            GetInsideRect(crc);
            InvalidateRect(crc, true);
        }
    }

    if (ht == htUpScroll) {
        bLooping = true;
        bool bHigh = true;

        if (::GetCapture() == NULL) {
            SetCapture();
            ASSERT(this == GetCapture());
            CClientDC dc(this);
            dc.DrawFrameControl(rcUpArrow, DFC_SCROLL, DFCS_SCROLLUP|DFCS_PUSHED);
            SetTimer(2,300,NULL);
            AfxLockTempMaps();
            for (;;) {
                MSG msg;
                VERIFY(::GetMessage(&msg, NULL, 0, 0));

                if (CWnd::GetCapture() != this)
                    break;

                switch (msg.message) {
                case WM_MOUSEMOVE:
                    {
                        CPoint pt(msg.lParam);
                        if (rcUpArrow.PtInRect(pt)) {
                            if (bHigh == false) {
                                dc.DrawFrameControl(rcUpArrow, DFC_SCROLL, DFCS_SCROLLUP|DFCS_PUSHED);
                                bHigh = true;
                                bUpArrow = true;
                                bUpPressed = true;
                            }
                        } else {
                            if (bHigh == true) {
                                dc.DrawFrameControl(rcUpArrow, DFC_SCROLL, DFCS_SCROLLUP);
                                bHigh = false;
                                bUpArrow = false;
                                bUpPressed = false;
                            }
                        }
                    }
                    break;

                case WM_LBUTTONUP:
                    {
                        if (bHigh) {
                            dc.DrawFrameControl(rcUpArrow, DFC_SCROLL, DFCS_SCROLLUP);
                            bHigh = false;
                        }
                        bUpArrow = false;
                        CPoint pt(msg.lParam);
                        if (rcUpArrow.PtInRect(pt)) {
                            if (iFirstItem > 0 ) {
                                iFirstItem --;
                                CRect crc;
                                GetInsideRect(crc);
                                InvalidateRect(crc, true);
                            }
                        }
                    }

                    goto ExitLoop;

                case WM_TIMER:
                    {
                        if (msg.wParam == 2) {
                            if (bHigh) {
                                CPoint pt(msg.pt);
                                ScreenToClient(&pt);
                                if (rcUpArrow.PtInRect(pt)) {
                                    bUpPressed = true;
                                    if (iFirstItem > 0) {
                                        iFirstItem --;
                                        CRect crc;
                                        GetInsideRect(crc);
                                        InvalidateRect(crc, true);
                                    } else
                                        goto ExitLoop;
                                } else
                                    bUpPressed = false;
                            }
                        }
                        break;
                    }
                case WM_KEYDOWN:
                    if (msg.wParam != VK_ESCAPE)
                        break;

                default:
                    DispatchMessage(&msg);
                    break;
                }
            }

        ExitLoop:
            KillTimer(2);
            ReleaseCapture();
            AfxUnlockTempMaps(FALSE);
            bLooping = false;
            bUpPressed = false;
            bUpArrow = false;
            CRect crc;
            GetInsideRect(crc);
            InvalidateRect(crc, true);
        }
    }

    CWnd::OnLButtonDown(nFlags, point);
}


int COutBarCtrl::InsertItem(int folder, int index, const char* text, int image, DWORD exData)
{
    ASSERT(folder >= 0 && folder < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(folder);
    pbf->InsertItem(text, image, index, exData);
    return pbf->GetItemCount();
}

int COutBarCtrl::GetItemCount() const
{
    ASSERT(iSelFolder >= 0 && iSelFolder < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(iSelFolder);
    return  pbf->GetItemCount();
}

void COutBarCtrl::SetSelFolder(int index)
{
    ASSERT(index >= 0 && index < GetFolderCount());
//  if (index == iSelFolder) return;

    CWnd* pc = GetFolderChild();
    if (pc)
        pc->ShowWindow(SW_HIDE);

    if (index != iSelFolder) {
        if (dwFlags & fAnimation && lAnimationTickCount >= 0) {
            AnimateFolderScroll(iSelFolder, index);
        }
    }

    iSelFolder = index;
    iFirstItem = 0;
    iLastSel = -1;

    pc = GetFolderChild();
    if (pc) {
        CRect rc;
        GetInsideRect(rc);
        pc->MoveWindow(rc);
        pc->ShowWindow(SW_SHOW);
    }

    GetNotifyWnd()->SendMessage(WM_OUTBAR_NOTIFY, NM_FOLDERCHANGE, (LPARAM)iSelFolder);

    Invalidate();
}

int COutBarCtrl::GetFolderCount() const
{
    return (int) arFolder.GetSize();
}

int COutBarCtrl::GetSelFolder() const
{
    return iSelFolder;
}

void COutBarCtrl::RemoveFolder(int index)
{
    ASSERT(index >= 0 && index < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(index);
    delete pbf;
    arFolder.RemoveAt(index);
    if (iSelFolder >= index)
        iSelFolder = index - 1;
    if (iSelFolder < 0 && GetFolderCount() > 0)
        iSelFolder = 0;
    Invalidate();
}

COutBarCtrl::CBarFolder& COutBarCtrl::CBarFolder::InsertItem(const char* text, int image, int index, DWORD exData)
{
    if (index < 0 || index > GetItemCount()) {
        index = GetItemCount();
    }

    int c = GetItemCount();
    CBarItem* pbf = new CBarItem(text, image, exData);
    ASSERT(pbf);
    if (index < GetItemCount())
        arItems.InsertAt(index, pbf);
    else
        arItems.Add(pbf);

    return *this;
}

int COutBarCtrl::GetCountPerPage() const
{
    return 0;
}

CImageList* COutBarCtrl::SetImageList(CImageList* pImageList, int nImageList)
{
    CImageList* o = NULL;
    if (nImageList == fSmallIcon) {
        o = pSmallImageList;
        pSmallImageList = pImageList;
    } else if (nImageList == fLargeIcon) {
        o = pLargeImageList;
        pLargeImageList = pImageList;
    }
    return o;
}

CImageList* COutBarCtrl::GetImageList(CImageList* pImageList, int nImageList)
{
    if (nImageList == fSmallIcon)
        return pSmallImageList;
    else if (nImageList == fLargeIcon)
        return pLargeImageList;
    return NULL;
}

CImageList* COutBarCtrl::SetFolderImageList(int folder, CImageList* pImageList, int nImageList)
{
    ASSERT(folder >= 0 && folder < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(folder);
    CImageList* o = NULL;
    if (nImageList == fSmallIcon) {
        o = pbf->pSmallImageList;
        pbf->pSmallImageList = pImageList;
    } else if (nImageList == fLargeIcon) {
        o = pbf->pLargeImageList;
        pbf->pLargeImageList = pImageList;
    }
    return o;
}


COutBarCtrl::CBarItem::CBarItem(const char* name, int image, DWORD exData)
{
    cItem = NULL;
    iImageIndex = image;
    dwData = exData;
    bEnable = true;
    AllocAndCopyString(cItem, name);
}

COutBarCtrl::CBarItem::~CBarItem()
{
    if (cItem) {
        delete [] cItem;
    }
}

int COutBarCtrl::CBarFolder::GetItemCount()
{
    return (int) arItems.GetSize();
}

void COutBarCtrl::PaintItems(CDC* pDC, int iFolder, CRect rc)
{
    ASSERT(iFolder >= 0 && iFolder < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(iFolder);
    int max = pbf->GetItemCount();
    CRect irc;
    GetInsideRect(irc);
    int isdc = pDC->SaveDC();
    CRgn itemRegion;
    itemRegion.CreateRectRgnIndirect(&irc);
    pDC->SelectClipRgn(&itemRegion);
    itemRegion.DeleteObject();
    for (int t = iFirstItem; t < max; t++) {
        CBarItem* pi = pbf->arItems.GetAt(t);
        CRect itemRc;
        GetItemRect(iFolder, t, itemRc);
        if (itemRc.top > rc.bottom) {
            break;
        } else {
            DrawItem(pDC, iFolder, itemRc, t);
        }
    }
    pDC->RestoreDC(isdc);
}


CSize COutBarCtrl::GetItemSize(int iFolder, int index, int type)
{
    ASSERT(iFolder >= 0 && iFolder < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(iFolder);

    ASSERT(index >= 0 && index < pbf->GetItemCount());
    CBarItem* pi = pbf->arItems.GetAt(index);

    CSize szImage(0,0);
    CSize szLabel(0,0);
    CSize szAll(0,0);

    if (pi->iImageIndex >= 0) {
        if (type != ircLabel) {
            CImageList* il = GetFolderImageList(iFolder, IsSmallIconView());
            ASSERT(il);
            if (il) {
                IMAGEINFO ii;
                il->GetImageInfo(pi->iImageIndex, &ii);
                szImage = CRect(ii.rcImage).Size();
            }
        }
    }
    if (pi->cItem) {
        if (type != ircIcon) {
            CClientDC dc(this);
            CFont* oft = (CFont*)dc.SelectObject(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
            if (IsSmallIconView()) {
                szLabel = dc.GetTextExtent(pi->cItem, StringLength(pi->cItem));
            } else {
                CRect rc;
                GetInsideRect(rc);
                rc.bottom = rc.top;
                dc.DrawText(pi->cItem, StringLength(pi->cItem), rc, DT_CALCRECT|DT_CENTER|DT_WORDBREAK);
                szLabel = rc.Size();
            }
            dc.SelectObject(oft);
        }
    }
    if (IsSmallIconView()) {
        if (type == ircIcon)
            szAll = szImage;
        else if (type == ircLabel)
            szAll = szLabel;
        else if (type == ircAll)
            szAll = CSize(szImage.cx + szLabel.cx + xSmallIconLabelOffset,
                          szImage.cy > szLabel.cy ? szImage.cy : szLabel.cy);
    } else {
        if (type == ircIcon)
            szAll = szImage;
        else if (type == ircLabel)
            szAll = szLabel;
        else if (type == ircAll)  {
            szAll = CSize(szImage.cx > szLabel.cx ? szImage.cx : szLabel.cx,
                          szLabel.cy + szImage.cy + yLargeIconLabelOffset + yLargeIconSpacing);
        }
    }
    return szAll;
}

CImageList* COutBarCtrl::GetFolderImageList(int index, bool bSmall) const
{
    ASSERT(index >= 0 && index < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(index);
    if (bSmall) {
        if (pbf->pSmallImageList)
            return pbf->pSmallImageList;
        else
            return pSmallImageList;
    }
    if (pbf->pLargeImageList)
        return pbf->pLargeImageList;
    else
        return pLargeImageList;
}

void COutBarCtrl::DrawItem(CDC* pDC, int iFolder, CRect rc, int index, bool bOnlyImage)
{
    CImageList* ima = GetFolderImageList(iFolder, IsSmallIconView());

    ASSERT(iFolder >= 0 && iFolder < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(iFolder);

    ASSERT(index >= 0 && index < pbf->GetItemCount());
    CBarItem* pi = pbf->arItems.GetAt(index);

    ASSERT(pi && ima);

    CFont* oft = (CFont*) pDC->SelectObject(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));

    //COLORREF ocr = pDC->SetTextColor(crTextColor);
    //int obk = pDC->SetBkMode(TRANSPARENT);
    COLORREF ncr, ocr, nbcr, obcr;
    int nbk, obk;
    if (dwFlags & fSelHighlight && index == iLastSel && iLastSel >= 0)
    {
        ncr = crHilightTextColor;
        nbk = OPAQUE;
        nbcr = crHilightColor;
    }
    else
    {
        nbk = TRANSPARENT;
        ncr = crTextColor;
        nbcr = crBackGroundColor;
    }
    ocr = pDC->SetTextColor(ncr);
    obcr = pDC->SetBkColor(nbcr);
    obk = pDC->SetBkMode(nbk);

    if (IsSmallIconView()) {
        if (ima) {
            IMAGEINFO ii;
            ima->GetImageInfo(pi->iImageIndex, &ii);
            CSize szImage = CRect(ii.rcImage).Size();
            CPoint pt;
            pt.x = rc.left + 2;
            pt.y = rc.top + (rc.Height() - szImage.cy) / 2;
            ima->Draw(pDC, pi->iImageIndex, pt, ILD_NORMAL);
            if (!bOnlyImage) {
                CRect rect = rc;
                rect.left += szImage.cx + xSmallIconLabelOffset;
                if (pi->bEnable) {
                    pDC->TextOut(rect.left, rect.top, CString(pi->cItem));
                } else {
                    COLORREF ocr = pDC->SetTextColor(RGB(0, 0, 0));
                    pDC->TextOut(rect.left, rect.top, CString(pi->cItem));
                    pDC->SetTextColor(ocr);
                }
            }
            if (!pi->bEnable) {
                //rc.right += 5; // bulgarian constant
                DrawDisabledItem(*pDC, rc);
            }
        }
    } else {
        if (ima) {
            IMAGEINFO ii;
            ima->GetImageInfo(pi->iImageIndex, &ii);
            CSize szImage = CRect(ii.rcImage).Size();
            CPoint pt;
            pt.x = rc.left + (rc.Width() - szImage.cx) / 2;
            pt.y = rc.top;// + (rc.Height() - szImage.cy) / 2;
            ima->Draw(pDC, pi->iImageIndex, pt, ILD_NORMAL);
            if (!bOnlyImage) {
                CRect rect = rc;
                rect.top += szImage.cy + yLargeIconLabelOffset;
                if (pi->bEnable) {
                    pDC->DrawText(pi->cItem, StringLength(pi->cItem), rect, DT_CENTER|DT_WORDBREAK);
                } else {
                    COLORREF ocr = pDC->SetTextColor(RGB(0, 0, 0));
                    pDC->DrawText(pi->cItem, StringLength(pi->cItem), rect, DT_CENTER|DT_WORDBREAK);
                    pDC->SetTextColor(ocr);
                }
            }
            if (!pi->bEnable) {
                DrawDisabledItem(*pDC, rc);
            }
        }
    }

    if (dwFlags & fSelHighlight && iLastSel == index && iLastSel >= 0) {
        CRect rc;
        GetIconRect(iSelFolder, iLastSel, rc);
        rc.InflateRect(1,1);
        pDC->Draw3dRect(rc, crDkShadowBorder, cr3dFace);
    }

    pDC->SetTextColor(ocr);
    pDC->SelectObject(oft);
    pDC->SetBkMode(obk);
    pDC->SetBkColor(obcr);

}


BOOL COutBarCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    CPoint pt(GetMessagePos());
    ScreenToClient(&pt);
    int index;
    int ht = HitTestEx(pt, index);
    if (ht == htFolder) {
        SetCursor(hHandCursor);
        return true;
    }

    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void COutBarCtrl::GetVisibleRange(int iFolder, int & first, int & last)
{
    first = iFirstItem;
    CRect rc;
    GetInsideRect(rc);

    CBarFolder* pbf = arFolder.GetAt(iFolder);
    int max = pbf->GetItemCount(), t;
    for (t = iFirstItem; t < max; t++)
    {
        CBarItem* pi = pbf->arItems.GetAt(t);
        CRect itemRc;
        GetItemRect(iFolder, t, itemRc);
        if (itemRc.bottom > rc.bottom)
        {
            last = t - 1;
            break;
        }
        else last = t;
    }
}

void COutBarCtrl::OnSize(UINT nType, int cx, int cy)
{
    bUpArrow = bDownArrow = false;

    CWnd::OnSize(nType, cx, cy);

    int max = GetFolderCount();
    CRect rc;
    GetInsideRect(rc);

    for ( int t = 0; t < max; t++) {
        CBarFolder* pbf = arFolder.GetAt(t);
        CWnd* pWnd = GetFolderChild(t);
        if (pWnd != NULL) {
            pWnd->SetWindowPos(0, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER);
        }
    }
}

void COutBarCtrl::HighlightItem(int index, bool bPressed)
{
    CWnd* pf = GetFocus();
    if (pf != NULL && pf != this && IsChild(pf)) {
        return;
    }

    static bool bOldPressed = false;
    if (iLastItemHighlighted == index && bOldPressed == bPressed) {
        return;
    }
    if (index != -1) {
        CBarFolder* pbf = arFolder.GetAt(iSelFolder);
        CBarItem* item = pbf->arItems.GetAt(index);
        if (!item->bEnable) {
            return;
        }
    }

    bOldPressed = bPressed;

    CClientDC dc(this);

    CRect irc;
    GetInsideRect(irc);
    int isdc = dc.SaveDC();
    CRgn itemRegion;
    itemRegion.CreateRectRgnIndirect(&irc);
    dc.SelectClipRgn(&itemRegion);
    itemRegion.DeleteObject();

    if (iLastItemHighlighted >= 0 && IsValidItem(iLastItemHighlighted)) {
        CRect rc;
        GetIconRect(iSelFolder, iLastItemHighlighted, rc);
        rc.InflateRect(1,1);
        dc.Draw3dRect(rc, crBackGroundColor, crBackGroundColor);
    }
    if (iSelAnimTiming > 0 && index == iLastSel) {
        iLastItemHighlighted = -1;
        return;
    }
    if (dwFlags & fSelHighlight && iLastSel == index) {
        iLastItemHighlighted = -1;
        return;
    }

    iLastItemHighlighted = index;
    if (iLastItemHighlighted >= 0 && IsValidItem(iLastItemHighlighted)) {
        CRect rc;
        GetIconRect(iSelFolder, iLastItemHighlighted, rc);
        rc.InflateRect(1,1);
        if (bPressed)
            dc.Draw3dRect(rc, crDkShadowBorder, cr3dFace);
        else
            dc.Draw3dRect(rc, cr3dFace, crDkShadowBorder);
    }
    dc.RestoreDC(isdc);
}

void COutBarCtrl::GetIconRect(int iFolder, int iIndex, CRect & rect)
{
    CRect rc;
    GetInsideRect(rc);
    int top = rc.top;
    CSize sz(0,0);
    int y = 0;
    int spacing = IsSmallIconView() ? ySmallIconSpacing : yLargeIconSpacing;

    for (int t = 0; t < iIndex; t++) {
        sz = GetItemSize(iFolder, t, ircAll);
        top += sz.cy;
        top += spacing;
        if (t == iFirstItem - 1)
            y = top - rc.top;
    }
    top += spacing;

    sz = GetItemSize(iFolder, iIndex, ircIcon);
    if (IsSmallIconView()) {
        rect.SetRect(rc.left, top, rc.left + sz.cx, top + sz.cy);
        rect.left += xLeftMargin + 2;
        rect.right += xLeftMargin + 2;
        rect.top -= yTopMargin;
        rect.bottom -= yTopMargin;

        rect.top -= y;
        rect.bottom -= y;
    } else {
        rect.SetRect(rc.left + (rc.Width() - sz.cx)/2, top, rc.left + (rc.Width() - sz.cx)/2 + sz.cx, top + sz.cy);
        rect.top -= y + 3;
        rect.bottom -= y + 2;
    }
}

void COutBarCtrl::GetLabelRect(int iFolder, int iIndex, CRect & rect)
{
    CRect rc;
    GetInsideRect(rc);
    int top = rc.top;
    CSize sz(0,0);
    int y = 0;
    int spacing = IsSmallIconView() ? ySmallIconSpacing : yLargeIconSpacing;

    for (int t = 0; t < iIndex; t++) {
        sz = GetItemSize(iFolder, t, ircAll);
        top += sz.cy;
        top += spacing;
        if (t == iFirstItem - 1) y = top - rc.top;
    }
    top += spacing;

    sz = GetItemSize(iFolder, iIndex, ircAll);

    CSize szLabel = GetItemSize(iFolder, iIndex, ircLabel);

    if (IsSmallIconView()) {
        rect.SetRect(rc.left, top, rc.left + sz.cx, top + sz.cy);
        rect.left += xLeftMargin + 2;
        rect.right += xLeftMargin + 2;
        rect.top  += yTopMargin - 5;
        rect.bottom += yTopMargin - 5;
    } else {
        rect.SetRect(rc.left + (rc.Width() - sz.cx)/2, top, rc.left + (rc.Width() - sz.cx)/2 + sz.cx, top + sz.cy);
        rect.top -= y + 3;
        rect.bottom -= y + 2;

        rect.bottom -=  yLargeIconSpacing;
        rect.top    = rect.bottom - szLabel.cy;
    }
}


void COutBarCtrl::StartGroupEdit(int index)
{
    CGroupEdit* pEdit = new CGroupEdit;
    pEdit->iIndex = index;
    CRect rc;
    GetFolderRect(index, rc);
    rc.InflateRect(-2,-2);
    pEdit->Create(WS_CHILD|WS_VISIBLE|ES_CENTER|ES_MULTILINE,rc,this,1);
    pEdit->msgSend = NM_OB_ONGROUPENDEDIT;
    pEdit->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
    ASSERT(index >= 0 && index < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(index);
    pEdit->SetWindowText(pbf->cName);
    pEdit->SetFocus();
}

void COutBarCtrl::StartItemEdit(int index)
{
    ASSERT(iSelFolder >= 0 && iSelFolder < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(iSelFolder);
    ASSERT(index >= 0 && index < pbf->GetItemCount());
    CBarItem* pi = pbf->arItems.GetAt(index);

    CGroupEdit* pEdit = new CGroupEdit;
    pEdit->iIndex = index;
    CRect rc, crc;
    GetLabelRect(iSelFolder, index, rc);
    GetInsideRect(crc);

    DWORD dwFlag = WS_CHILD|WS_VISIBLE|WS_BORDER;

    if (!IsSmallIconView()) {
        rc.left = crc.left + 5;
        rc.right = crc.right - 5;
        rc.bottom += 5;
        dwFlag |= ES_CENTER|ES_MULTILINE;
    } else {
        CRect irc;
        GetIconRect(iSelFolder, index, irc);
        dwFlag |= ES_AUTOHSCROLL;
        rc.OffsetRect(irc.Width()+1, -6);
        pEdit->bNoDown = true;
    }

    pEdit->Create(dwFlag, rc, this, 1);
    pEdit->msgSend = NM_OB_ONLABELENDEDIT;

    pEdit->SetWindowText(pi->cItem);
    pEdit->SetFocus();

}

void COutBarCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
    iHitInternal1 = HitTestEx(point, iHitInternal2);

    CBitmapMenu cMenu;
    cMenu.CreatePopupMenu();

    cMenu.AppendMenu(MF_STRING, ID_UIX_SMALLICON, sz_ID_UIX_SMALLICON);
    cMenu.AppendMenu(MF_STRING, ID_UIX_LARGEICON, sz_ID_UIX_LARGEICON);

    if (iHitInternal1 == htItem) {
        if (dwFlags & fRemoveItems || dwFlags & fEditItems) {
            cMenu.AppendMenu(MF_SEPARATOR);
            if (dwFlags & fRemoveItems) {
                cMenu.AppendMenu(MF_STRING, ID_UIX_REMOVEITEM, sz_ID_UIX_REMOVEITEM);
            }
            if (dwFlags & fEditItems) {
                cMenu.AppendMenu(MF_STRING, ID_UIX_RENAMEITEM, sz_ID_UIX_RENAMEITEM);
            }
        }
    } else if (iHitInternal1 == htFolder) {
        if ( dwFlags&fRemoveGroups || dwFlags&fEditGroups) {
            cMenu.AppendMenu(MF_SEPARATOR);
            if (dwFlags & fRemoveGroups) {
                cMenu.AppendMenu(MF_STRING, ID_UIX_REMOVEITEM, sz_ID_UIX_REMOVEITEM);
            }
            if (dwFlags & fEditGroups) {
                cMenu.AppendMenu(MF_STRING, ID_UIX_RENAMEITEM, sz_ID_UIX_RENAMEITEM);
            }
        }
    }

    cMenu.LoadToolBarResource(IDR_OUTBAR);
    cMenu.RemapMenu(&cMenu);
    cMenu.EnableMenuItems(&cMenu, this);

    CPoint pt(point);
    ClientToScreen(&pt);
    cMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, pt.x, pt.y, this);

    cMenu.DestroyMenu();

    CWnd::OnRButtonDown(nFlags, point);
}

void COutBarCtrl::OnUixLargeicon()
{
    SetSmallIconView(false);
    Invalidate();
}

void COutBarCtrl::OnUpdateUixLargeicon(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsSmallIconView());
}

void COutBarCtrl::OnUixSmallicon()
{
    SetSmallIconView(true);
    Invalidate();
}

void COutBarCtrl::OnUpdateUixSmallicon(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(!IsSmallIconView());
}

void COutBarCtrl::OnUixRemoveitem()
{
    if (iHitInternal1 == htFolder) {
        RemoveFolder(iHitInternal2);
    } else if (iHitInternal1 == htItem) {
        RemoveItem(iHitInternal2);
    }
}

void COutBarCtrl::OnUpdateUixRemoveitem(CCmdUI* pCmdUI)
{

}

void COutBarCtrl::OnUixRenameitem()
{
    if (iHitInternal1 == htFolder) {
        StartGroupEdit(iHitInternal2);
    } else if (iHitInternal1 == htItem) {
        StartItemEdit(iHitInternal2);
    }
}

void COutBarCtrl::OnUpdateUixRenameitem(CCmdUI* pCmdUI)
{
}

void COutBarCtrl::RemoveItem(int index)
{
    ASSERT(iSelFolder >= 0 && iSelFolder < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(iSelFolder);
    ASSERT(index >= 0 && index < pbf->GetItemCount());
    if (IsValidItem(index)) {
        CBarItem* i = pbf->arItems.GetAt(index);
        delete i;

        pbf->arItems.RemoveAt(index);
        CRect rc;
        GetInsideRect(rc);
        InvalidateRect(rc);
    }
}

bool COutBarCtrl::IsValidItem(int index) const
{
    ASSERT(iSelFolder >= 0 && iSelFolder < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(iSelFolder);
    return (index >= 0 && index < pbf->GetItemCount());
}

DWORD COutBarCtrl::GetItemData(int index) const
{
    ASSERT(iSelFolder >= 0 && iSelFolder < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(iSelFolder);
    if (IsValidItem(index)) {
        CBarItem* i = pbf->arItems.GetAt(index);
        return i->dwData;
    }
    return 0;
}

int COutBarCtrl::GetItemImage(int index) const
{
    ASSERT(iSelFolder >= 0 && iSelFolder < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(iSelFolder);
    if (IsValidItem(index)) {
        CBarItem* i = pbf->arItems.GetAt(index);
        return i->iImageIndex;
    }
    return 0;
}

void COutBarCtrl::SetItemData(int index, DWORD dwData)
{
    ASSERT(iSelFolder >= 0 && iSelFolder < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(iSelFolder);
    if (IsValidItem(index)) {
        CBarItem* i = pbf->arItems.GetAt(index);
        i->dwData = dwData;
    }
}

void COutBarCtrl::SetItemImage(int index, int iImage)
{
    ASSERT(iSelFolder >= 0 && iSelFolder < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(iSelFolder);
    if (IsValidItem(index)) {
        CBarItem* i = pbf->arItems.GetAt(index);
        i->iImageIndex = iImage;
    }
}

void COutBarCtrl::DrawDragArrow(CDC* pDC, int iFrom, int iTo)
{
    if (iTo == iLastDragItemDraw) return;
    CRect rc1;
    if (iLastDragItemDraw >= 0) {
        GetDragItemRect(iLastDragItemDraw, rc1);
        if (iLastDragItemDrawType == 0) {
            rc1.bottom = rc1.top + 5;
        } else if (iLastDragItemDrawType == 1) {
            rc1.top -= 4;
            rc1.bottom = rc1.top + 9;
        } else if (iLastDragItemDrawType == 2) {
            rc1.top -= 4;
            rc1.bottom = rc1.top + 5;
        }
        InvalidateRect(rc1, true);
        UpdateWindow();
    }
    iLastDragItemDraw = iTo;

    if (iLastDragItemDraw >= 0) {
        iLastDragItemDrawType = GetDragItemRect(iLastDragItemDraw, rc1);

        CPen* op = pDC->SelectObject(pBlackPen);

        pDC->MoveTo(rc1.left, rc1.top);
        pDC->LineTo(rc1.right, rc1.top);

        if (iLastDragItemDrawType != 2) {
            pDC->MoveTo(rc1.left, rc1.top+1);
            pDC->LineTo(rc1.left + 7, rc1.top+1);
            pDC->MoveTo(rc1.left, rc1.top+2);
            pDC->LineTo(rc1.left + 5, rc1.top+2);
            pDC->MoveTo(rc1.left, rc1.top+3);
            pDC->LineTo(rc1.left + 3, rc1.top+3);
            pDC->MoveTo(rc1.left, rc1.top+4);
            pDC->LineTo(rc1.left + 1, rc1.top+4);

            pDC->MoveTo(rc1.right-1, rc1.top+1);
            pDC->LineTo(rc1.right - 7, rc1.top+1);
            pDC->MoveTo(rc1.right-1, rc1.top+2);
            pDC->LineTo(rc1.right - 5, rc1.top+2);
            pDC->MoveTo(rc1.right-1, rc1.top+3);
            pDC->LineTo(rc1.right - 3, rc1.top+3);
        }
        if (iLastDragItemDrawType != 0) {
            pDC->MoveTo(rc1.left, rc1.top-1);
            pDC->LineTo(rc1.left + 7, rc1.top-1);
            pDC->MoveTo(rc1.left, rc1.top-2);
            pDC->LineTo(rc1.left + 5, rc1.top-2);
            pDC->MoveTo(rc1.left, rc1.top-3);
            pDC->LineTo(rc1.left + 3, rc1.top-3);
            pDC->MoveTo(rc1.left, rc1.top-4);
            pDC->LineTo(rc1.left + 1, rc1.top-4);

            pDC->MoveTo(rc1.right-1, rc1.top-1);
            pDC->LineTo(rc1.right - 7, rc1.top-1);
            pDC->MoveTo(rc1.right-1, rc1.top-2);
            pDC->LineTo(rc1.right - 5, rc1.top-2);
            pDC->MoveTo(rc1.right-1, rc1.top-3);
            pDC->LineTo(rc1.right - 3, rc1.top-3);
            pDC->MoveTo(rc1.right-1, rc1.top-4);
            pDC->LineTo(rc1.right - 1, rc1.top-4);
        }

        pDC->SelectObject(op);
    }
}

int COutBarCtrl::GetDragItemRect(int index, CRect & rect)
{
    CRect rc, crc;
    GetInsideRect(crc);
    crc.InflateRect(-2, 0);

    GetItemRect(iSelFolder, index < GetItemCount() ? index : index - 1, rc);
    int line = 0;
    if (index < GetItemCount()) {
        line = rc.top - 4;
    } else {
        line = rc.bottom + 6;
    }
    int tpe;
    if (index == 0) {
        rect.SetRect(crc.left, line+2, crc.right, line + 7);
        tpe = 0;
    } else if (index < GetItemCount()) {
        rect.SetRect(crc.left, line - 9, crc.right, line);
        tpe = 1;
        if (IsSmallIconView()) {
            rect.top += 8;
            rect.bottom += 8;
        }
    } else {
        rect.SetRect(crc.left, line, crc.right, line + 5);
        tpe = 2;
    }
    return tpe;
}


void COutBarCtrl::AnimateFolderScroll(int iFrom, int iTo)
{
    ASSERT(iFrom >= 0 && iFrom < GetFolderCount());
    ASSERT(iTo >= 0 && iTo < GetFolderCount());

    CRect rc, rc1, frc, frc1;
    GetInsideRect(rc);
    rc1.SetRect(0,0,rc.Width(),rc.Height());
    GetFolderRect(iTo, frc);
    frc1 = frc;

    CClientDC dc(this);
    CDC memDC;
    memDC.CreateCompatibleDC(&dc);
    CBitmap bmpFrom, bmpTo;
    bmpFrom.CreateCompatibleBitmap(&dc, rc1.Width(), rc1.Height() + frc.Height()* 2);
    bmpTo.CreateCompatibleBitmap(&dc, rc1.Width(), rc1.Height() + frc.Height()* 2);
    CDC* pDC = &memDC;

    CWnd* pWnd1 = GetFolderChild(iFrom);
    CWnd* pWnd2 = GetFolderChild(iTo);

    CBitmap* obmp = pDC->SelectObject(&bmpFrom);
    if (iTo > iFrom) {
        rc1.bottom += frc.Height()* 2;
        pDC->FillSolidRect(rc1, crBackGroundColor);
        if (pWnd1) {
            BOOL bPrev = pWnd1->ShowWindow(SW_SHOW);
            pWnd1->SendMessage(WM_ERASEBKGND, (WPARAM) pDC->GetSafeHdc(), (LPARAM) 0);
            pWnd1->SendMessage(WM_PAINT, (WPARAM) pDC->GetSafeHdc(), (LPARAM) 0);
            if (!bPrev) pWnd1->ShowWindow(SW_HIDE);
        } else {
            PaintItems(pDC, iFrom, rc1);
        }

        pDC->SelectObject(&bmpTo);
        frc.SetRect(0,0,frc.Width(),frc.Height());
        rc1.SetRect(0,frc.Height(),rc.Width(), rc.Height()+frc.Height()* 2);
        pDC->FillSolidRect(rc1, crBackGroundColor);

        if (pWnd2) {
            CPoint ovpt = pDC->SetViewportOrg(0, frc.Height());
            BOOL bPrev = pWnd2->ShowWindow(SW_SHOW);
            pWnd2->SendMessage(WM_ERASEBKGND, (WPARAM) pDC->GetSafeHdc(), (LPARAM) 0);
            pWnd2->SendMessage(WM_PAINT, (WPARAM) pDC->GetSafeHdc(), (LPARAM) 0);
            if (!bPrev)
                pWnd2->ShowWindow(SW_HIDE);
            pDC->SetViewportOrg(ovpt);
        } else {
            PaintItems(pDC, iTo, rc1);
        }
        DrawFolder(pDC, iTo, frc, false);
    } else {
        frc.SetRect(0,0,frc.Width(),frc.Height());
        rc1.top += frc.Height();
        rc1.bottom += frc.Height()* 2;
        pDC->FillSolidRect(rc1, crBackGroundColor);
        if (pWnd1)  {
            CPoint ovpt = pDC->SetViewportOrg(0, frc.Height());
            BOOL bPrev = pWnd1->ShowWindow(SW_SHOW);
            pWnd1->SendMessage(WM_ERASEBKGND, (WPARAM) pDC->GetSafeHdc(), (LPARAM) 0);
            pWnd1->SendMessage(WM_PAINT, (WPARAM) pDC->GetSafeHdc(), (LPARAM) 0);
            if (!bPrev) pWnd1->ShowWindow(SW_HIDE);
            pDC->SetViewportOrg(ovpt);
        } else {
            PaintItems(pDC, iFrom, rc1);
        }

        DrawFolder(pDC, iFrom, frc, false);

        pDC->SelectObject(&bmpTo);
        rc1.SetRect(0,0,rc.Width(), rc.Height() + frc.Height()* 2);
        pDC->FillSolidRect(rc1, crBackGroundColor);

        if (pWnd2 != NULL) {
            BOOL bPrev = pWnd2->ShowWindow(SW_SHOW);
            pWnd2->SendMessage(WM_ERASEBKGND, (WPARAM) pDC->GetSafeHdc(), (LPARAM) 0);
            pWnd2->SendMessage(WM_PAINT, (WPARAM) pDC->GetSafeHdc(), (LPARAM) 0);
            if (!bPrev) pWnd2->ShowWindow(SW_HIDE);
        } else {
            PaintItems(pDC, iTo, rc1);
        }
    }

    if (iTo > iFrom) {
        CRect rcFrom, rcTo;
        GetFolderRect(iFrom, rcFrom);
        GetFolderRect(iTo, rcTo);
        int fh = rcFrom.Height();
        for (int y = rcTo.top - fh; y > rcFrom.bottom; y -= fh) {
            pDC->SelectObject(&bmpFrom);
            dc.BitBlt(rc.left, rcFrom.bottom + 1, rc.Width(), y - rcFrom.bottom - 1, pDC, 0,fh, SRCCOPY);
            pDC->SelectObject(&bmpTo);
            dc.BitBlt(rc.left, y, rc.Width(), rc.bottom - y + fh, pDC, 0,0, SRCCOPY);
            Sleep(lAnimationTickCount);
        }
    } else {
        CRect rcFrom, rcTo;
        GetFolderRect(iFrom, rcFrom);
        int fh = rcFrom.Height();
        rcTo.SetRect(rc.left, rc.bottom, rc.right, rc.bottom - fh);
        for (int y = rcFrom.top + 1; y < rcTo.top - fh; y += fh) {
            pDC->SelectObject(&bmpTo);
            dc.BitBlt(rc.left, rcFrom.top, rc.Width(), y - rcFrom.top - 1, pDC, 0, fh*2, SRCCOPY);
            pDC->SelectObject(&bmpFrom);
            dc.BitBlt(rc.left, y, rc.Width(), rc.bottom - y, pDC, 0,0, SRCCOPY);
            Sleep(lAnimationTickCount);
        }
    }

    pDC->SelectObject(obmp);
}

CString COutBarCtrl::GetItemText(int index)
{
    CString item;

    ASSERT(iSelFolder >= 0 && iSelFolder < GetFolderCount());
    CBarFolder* pbf = arFolder.GetAt(iSelFolder);
    if (IsValidItem(index)) {
        CBarItem* pi = pbf->arItems.GetAt(index);
        if (pi->cItem)
            item = pi->cItem;
    }
    return item;
}

COutBarCtrl::CBarFolder* COutBarCtrl::AddFolderBar( const char* szFolder, CWnd* pSon, DWORD exData )
{
    CBarFolder* pbf = new CBarFolder(szFolder, exData);
    ASSERT(pbf);
    pbf->pChild = pSon;
    arFolder.Add(pbf);
    return pbf;
}

CWnd* COutBarCtrl::GetFolderChild(int iFolder)
{
    if (GetFolderCount()) {
        if (iFolder < 0) {
            iFolder = iSelFolder;
            if (iFolder >= GetFolderCount()) {
                iFolder = GetFolderCount()-1;
            }
        }

        CBarFolder* pbf = arFolder.GetAt(iFolder);
        return pbf->pChild;
    }
    return NULL;
}

DWORD COutBarCtrl::GetFolderData(int iFolder)
{
    if (iFolder < 0) {
        iFolder = iSelFolder;
    }
    CBarFolder* pbf = arFolder.GetAt(iFolder);
    return pbf->dwData;
}

void COutBarCtrl::SetAnimSelHighlight(int iTime)
{
    if (iTime <= 0)
        KillTimer(3);
    else
        SetTimer(3, iTime, NULL);
    iSelAnimTiming = iTime;
}

CString COutBarCtrl::GetFolderText(int iFolder)
{
    if (iFolder < 0) {
       iFolder = iSelFolder;
    }
    CBarFolder* pbf = arFolder.GetAt(iFolder);
    return CString(pbf->cName);
}

bool COutBarCtrl::EnableItem(int iFolder, int index, bool bEnable)
{
    if (iFolder < 0) {
       iFolder = iSelFolder;
    }
    CBarFolder* pbf = arFolder.GetAt(iFolder);
    ASSERT(iFolder >= 0 && iFolder < GetFolderCount());

    CBarItem* item = pbf->arItems.GetAt(index);
    bool bPreviousEnable = item->bEnable;
    item->bEnable = bEnable;
    return bPreviousEnable;
}

void COutBarCtrl::DrawDisabledItem(CDC& dc, const CRect& rc) const
{
    // create a monochrome memory DC
    CDC ddc;
    ddc.CreateCompatibleDC(0);
    CBitmap bmp;
    bmp.CreateCompatibleBitmap(&ddc, rc.Width(), rc.Height());
    CBitmap* pOldBmp = ddc.SelectObject(&bmp);

    COLORREF clrBtnFace = crBackGroundColor; //::GetSysColor(COLOR_BTNFACE);
    COLORREF clrBtnHilight = ::GetSysColor(COLOR_BTNHILIGHT);
    COLORREF clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);

    // build a mask
    ddc.PatBlt(0, 0, rc.Width(), rc.Height(), WHITENESS);
    dc.SetBkColor(clrBtnFace);
    ddc.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, rc.left, rc.top, SRCCOPY);
    dc.SetBkColor(clrBtnHilight);
    ddc.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, rc.left, rc.top, SRCPAINT);

    // Copy the image from dc into the memory DC
    // and draw it (grayed) back dc.
    dc.FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(), clrBtnFace);
    dc.SetBkColor(RGB(0, 0, 0));
    dc.SetTextColor(RGB(255, 255, 255));
    CBrush brHilight;
    brHilight.CreateSolidBrush(clrBtnHilight);
    CBrush brShadow;
    brShadow.CreateSolidBrush(clrBtnShadow);
    CBrush* pOldBrush = dc.SelectObject(&brHilight);
    dc.BitBlt(rc.left+1, rc.top+1, rc.Width(), rc.Height(), &ddc, 0, 0, 0x00E20746L);
    dc.SelectObject(&brShadow);
    dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &ddc, 0, 0, 0x00E20746L);

    // reset DCs
    dc.SelectObject(pOldBrush);
    ddc.SelectObject(pOldBmp);
    ddc.DeleteDC();

    bmp.DeleteObject();
}

void COutBarCtrl::SetNotifyWnd(CWnd* pWnd)
{
    m_pWndNotify = pWnd;
}

CWnd* COutBarCtrl::GetNotifyWnd()
{
    if (m_pWndNotify != NULL) {
        return m_pWndNotify;
    }
    return GetOwner();
}

