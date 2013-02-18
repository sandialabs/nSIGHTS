///////////////////////////////////////////////////////////////////////////////////
//
// TreeCtrlEx.cpp
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

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include "TreeCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx

CTreeCtrlEx::CTreeCtrlEx()
{
    m_brushBack.CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
    m_brushNormal.CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
    m_brushGrey.CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
    m_clrHilightText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
}

CTreeCtrlEx::~CTreeCtrlEx()
{
}

BEGIN_MESSAGE_MAP(CTreeCtrlEx, CTreeCtrl)
    //{{AFX_MSG_MAP(TreeBaseC)
    ON_WM_CREATE()
    ON_MESSAGE(WM_LBUTTONDBLCLK, OnLButtonDblClk)
    ON_MESSAGE(WM_LBUTTONDOWN, OnLButtonClk)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CTreeCtrlEx::OnCreate(LPCREATESTRUCT pCStruct)
{
    if (CTreeCtrl::OnCreate(pCStruct) == -1) {
        return -1;
    }
#ifdef _DEBUG
    if ((pCStruct->style & TVS_INFOTIP) == 0 || (pCStruct->style & TVS_NOTOOLTIPS) != 0) {
        TRACE("You should set TVS_INFOTIP and unset TVS_NOTOOLTIPS for proper titletip functionality\r\n");
    }
#endif
    return 0;
}

// In the case of dialog created from resources PreSubclassWindow
// is the only chance to check style
void CTreeCtrlEx::PreSubclassWindow()
{
    CTreeCtrl::PreSubclassWindow();
    DWORD dwStyle = GetStyle();
#ifdef _DEBUG
    if ((dwStyle & TVS_INFOTIP) == 0 || (dwStyle & TVS_NOTOOLTIPS) != 0) {
        TRACE("You should set TVS_INFOTIP and unset TVS_NOTOOLTIPS for proper titletip functionality\r\n");
    }
#endif
}

BOOL CTreeCtrlEx::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    if (message != WM_NOTIFY) {
        return CTreeCtrl::OnChildNotify(message, wParam, lParam, pResult);
    }
    LPNMHDR pNmhdr = (LPNMHDR)lParam;
    switch (pNmhdr->code) {
    case TVN_GETINFOTIP:
    {
        LPNMTVGETINFOTIP lpGetInfoTip = (LPNMTVGETINFOTIP)lParam;

        CRect rect;
        GetItemRect(lpGetInfoTip->hItem, rect, TRUE);
        CRect rcClient;
        GetClientRect(rcClient);
        //TODO: find the obstruction in a better way (ie. also for the 2nd line)
        if (rcClient.right < rect.right) {
            CString sText = GetItemText(lpGetInfoTip->hItem);
            CString sOutText = GetOutText(lpGetInfoTip->hItem);
            CString sToolTip;
            if (sOutText.GetLength() > 0) {
                sToolTip.Format("%s\n%s", sText, sOutText);
            } else {
                sToolTip = sText;
            }
            lpGetInfoTip->cchTextMax;
            CopyString(lpGetInfoTip->pszText, sToolTip, lpGetInfoTip->cchTextMax);
        }

        // Place ToolTip on the correct place
        CWnd* pWnd = GetToolTips();
        ClientToScreen(rect);
        pWnd->SetWindowPos(NULL, rect.left, rect.top, rect.Width()+4, rect.Height()-1, SWP_NOACTIVATE);

        return true;
    }
    case NM_CUSTOMDRAW:
    {
        LPNMTVCUSTOMDRAW pCustomDraw = (LPNMTVCUSTOMDRAW)lParam;
        switch (pCustomDraw->nmcd.dwDrawStage) {
        case CDDS_PREPAINT:
            // Need to process this case and set pResult to CDRF_NOTIFYITEMDRAW,
            // otherwise parent will never receive CDDS_ITEMPREPAINT notification.
            *pResult = CDRF_NOTIFYITEMDRAW;
            return true;

        case CDDS_ITEMPREPAINT:
            if (pCustomDraw->iLevel >= 1) {
                // only want to do custom drawing on level 1 and more
                DWORD dwStyle = GetStyle();
                if (dwStyle & TVS_NONEVENHEIGHT) {
                    *pResult = CDRF_NOTIFYPOSTPAINT;
                } else {
                    *pResult = CDRF_DODEFAULT;
                }
                return true;
            } else {
                *pResult = CDRF_DODEFAULT;
                return true;
            }
            break;
        case CDDS_ITEMPOSTPAINT:
            {
                *pResult = CDRF_DODEFAULT;
                HTREEITEM hItem = HTREEITEM(pCustomDraw->nmcd.dwItemSpec);
                CString sOutText = GetOutText(hItem);
                if (sOutText.GetLength() > 0) {
                    CDC dc;
                    dc.Attach(pCustomDraw->nmcd.hdc);
                    DrawItem(dc, hItem, pCustomDraw->clrText, sOutText, pCustomDraw->nmcd.uItemState);
                    dc.Detach();
                }
                return true;
            }
        default:
            break;
        }
        break;
    }
    break;

    }
    return CTreeCtrl::OnChildNotify(message, wParam, lParam, pResult);
}

void CTreeCtrlEx::GetItemRect2(CDC& dc, HTREEITEM hItem, CRect& rc)
{
    GetItemRect(hItem, rc, TRUE);
    CString sOutText = GetOutText(hItem);
    if (sOutText.GetLength() > 0) {
        CSize sz = dc.GetTextExtent(sOutText);
        if (sz.cx > rc.Width())
            rc.right = rc.left + sz.cx + 2 + 4;
        else
            rc.right += 2;
    }
}

void CTreeCtrlEx::DrawItem(CDC& dc, HTREEITEM hItem, COLORREF clrText, const CString& sOutText, UINT uItemState)
{
    int nOldBkMode = dc.SetBkMode(TRANSPARENT);
    int nOldTextColor = dc.SetTextColor(clrText);

    CSize sz(0,0);
    if (sOutText.GetLength() > 0)
        sz = dc.GetTextExtent(sOutText);
    CRect rect;
    GetItemRect(hItem, rect, TRUE);

    CFont fontUnderlined;
    CFont* pOldFont = NULL;
    if (GetStyle() & TVS_TRACKSELECT && uItemState & CDIS_HOT) {
        CFont* pFont = GetFont();
        LOGFONT logFont;
        pFont->GetLogFont(&logFont);

        logFont.lfUnderline = TRUE;
        VERIFY(fontUnderlined.CreateFontIndirect(&logFont));
        pOldFont = dc.SelectObject(&fontUnderlined);
    }
    CRect rcFill = rect;
    if (sz.cx > rect.Width()) {
        rcFill.right = rcFill.left + sz.cx + 2 + 4; //?? Why 4 ?
    } else {
        rcFill.right += 2;
    }
    CString sText = GetItemText(hItem);
    if (uItemState & CDIS_FOCUS) {
        dc.FillRect(&rcFill, &m_brushBack);
        dc.DrawFocusRect(&rcFill);
        dc.TextOut(rect.left+2, rect.top, sText);
        if (sOutText.GetLength() > 0)
            dc.TextOut(rect.left+2, rect.top + rect.Height()/2-1, sOutText);
    } else if (uItemState & CDIS_SELECTED) {
        CWnd* pWnd = GetFocus();
        if (pWnd != this) {
            dc.FillRect(&rcFill, &m_brushGrey);
            dc.SetTextColor(COLOR_BTNFACE);
        } else
            dc.FillRect(&rcFill, &m_brushBack);
        dc.TextOut(rect.left+2, rect.top, sText);
        if (sOutText.GetLength() > 0)
            dc.TextOut(rect.left+2, rect.top + rect.Height()/2-1, sOutText);
    } else {
        dc.FillRect(&rcFill, &m_brushNormal);
        dc.TextOut(rect.left+2, rect.top, sText);
        if (sOutText.GetLength() > 0)
            dc.TextOut(rect.left+2, rect.top + rect.Height()/2-1, sOutText);
    }

    if (GetStyle() & TVS_TRACKSELECT && uItemState & CDIS_HOT) {
        dc.SelectObject(pOldFont);
    }
    dc.SetTextColor(nOldTextColor);
    dc.SetBkMode(nOldBkMode);
}

void CTreeCtrlEx::DrawDisabledItem(CDC& dc, const CRect& rc, COLORREF crBackGroundColor)
{
    // create a monochrome memory DC
    CDC ddc;
    ddc.CreateCompatibleDC(0);
    CBitmap bmp;
    bmp.CreateCompatibleBitmap(&ddc, rc.Width(), rc.Height());
    CBitmap* pOldBmp = ddc.SelectObject(&bmp);

    COLORREF clrBtnFace = crBackGroundColor;
    COLORREF clrBtnHilight = ::GetSysColor(COLOR_BTNHILIGHT);
    COLORREF clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);

    // build a mask
    ddc.PatBlt(0, 0, rc.Width(), rc.Height(), WHITENESS);
    int nOldBkColor = dc.SetBkColor(clrBtnFace);
    ddc.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, rc.left, rc.top, SRCCOPY);
    dc.SetBkColor(clrBtnHilight);
    ddc.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, rc.left, rc.top, SRCPAINT);

    // Copy the image from dc into the memory DC
    // and draw it (grayed) back dc.
    dc.FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(), clrBtnFace);
    dc.SetBkColor(RGB(0, 0, 0));
    int nOldTextColor = dc.SetTextColor(RGB(255, 255, 255));
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

    dc.SetTextColor(nOldTextColor);
    dc.SetBkColor(nOldBkColor);
}

LRESULT CTreeCtrlEx::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
    short xPos = LOWORD(lParam);
    short yPos = HIWORD(lParam);
    TVHITTESTINFO HitTestInfo;
    HitTestInfo.pt.x = xPos;
    HitTestInfo.pt.y = yPos;
    HitTest(&HitTestInfo);
    if (HitTestInfo.flags == TVHT_ONITEMRIGHT) {
        if (IsInExtendedRect(HitTestInfo.hItem, CPoint(xPos, yPos))) {
            CRect rc;
            GetItemRect(HitTestInfo.hItem, &rc, TRUE);
            xPos = short(rc.left + rc.Width() / 2);
            lParam = MAKELPARAM(xPos, yPos);
        }
    }

    return DefWindowProc(WM_LBUTTONDBLCLK, wParam, lParam);
}

LRESULT CTreeCtrlEx::OnLButtonClk(WPARAM wParam, LPARAM lParam)
{
    short xPos = LOWORD(lParam);
    short yPos = HIWORD(lParam);
    TVHITTESTINFO HitTestInfo;
    HitTestInfo.pt.x = xPos;
    HitTestInfo.pt.y = yPos;
    HitTest(&HitTestInfo);
    bool bChangeExpansion = false;
    if (HitTestInfo.flags == TVHT_ONITEMRIGHT) {
        if (IsInExtendedRect(HitTestInfo.hItem, CPoint(xPos, yPos))) {
            CRect rc;
            GetItemRect(HitTestInfo.hItem, &rc, TRUE);
            xPos = short(rc.left + rc.Width()/2);
            lParam = MAKELPARAM(xPos, yPos);
            bChangeExpansion = true;
        }
    } else if (HitTestInfo.flags & TVHT_ONITEM) {
        bChangeExpansion = true;
    }

    LRESULT rc = DefWindowProc(WM_LBUTTONDOWN, wParam, lParam);

    if (bChangeExpansion && ItemHasChildren(HitTestInfo.hItem)) {
        // Toggle node on the click
        if (GetStyle() & TVS_TRACKSELECT) {
            //Expand(HitTestInfo.hItem, TVE_TOGGLE);
            //OnToggleNode(HitTestInfo.hItem);
        }
    }
    return rc;
}

bool CTreeCtrlEx::IsInExtendedRect(HTREEITEM hItem, CPoint pt)
{
    CString sText = GetItemText(hItem);
    CString sOutText = GetOutText(hItem);
    if (sOutText.GetLength() > sText.GetLength() ) {
        CRect rect;
        GetItemRect(hItem, &rect, TRUE);
        CPaintDC dc(this);
        CSize sz = dc.GetTextExtent(sOutText);
        rect.right = rect.left + sz.cx + 2 + 4; //?? Why 4 ?
        if (rect.PtInRect(pt)) {
            return true;
        }
    }
    return false;
}

// virtual function, here just for testing
CString CTreeCtrlEx::GetOutText(HTREEITEM hItem)
{
    DWORD_PTR dwData = GetItemData(hItem);
    CString sText;
    if (dwData != 0) {
        sText.Format("%ld", dwData);
    }
    return sText;
}

void CTreeCtrlEx::OnToggleNode(HTREEITEM hItem)
{
    // default implementation does nothing
}

