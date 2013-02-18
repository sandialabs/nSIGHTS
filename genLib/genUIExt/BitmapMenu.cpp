///////////////////////////////////////////////////////////////////////////////////
//
// BitmapMenu.cpp
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

#include "GenLibRes.h"

#include <genClass/U_Str.h>

#include "BitmapMenu.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CBitmapMenu::CBitmapMenu()
{
    iSpawnItem = 0;
    pSpawnItem = NULL;

    iImageItem = 0;
    pImageItem = NULL;

    szImage = CSize(20,20);

    NONCLIENTMETRICS ncm;
    memset(&ncm, 0, sizeof(ncm));
    ncm.cbSize = sizeof(ncm);

    ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, (PVOID) &ncm, 0);

    m_hMenuFont = ::CreateFontIndirect(&ncm.lfMenuFont);
    ASSERT(m_hMenuFont != NULL);

    COLORMAP cMap[3] = {
        { RGB(128,128,128), GetSysColor(COLOR_3DSHADOW) },
        { RGB(192,192,192), GetSysColor(COLOR_3DFACE) },
        { RGB(255,255,255), GetSysColor(COLOR_3DHILIGHT) }
    };
    CBitmap bmp;
    VERIFY( bmp.LoadMappedBitmap(IDB_MENUCHK, 0, cMap, 3) );
    VERIFY( ilOther.Create(19, 19, ILC_COLOR4|ILC_MASK, 1, 0) );
    VERIFY( ilOther.Add(&bmp, GetSysColor(COLOR_3DFACE)) != -1 );
    bmp.DeleteObject();
}

CBitmapMenu::~CBitmapMenu()
{
    if (iSpawnItem > 0) {
        TRACE1("CBitmapMenu::Deleting %d spawn item.\n", iSpawnItem);
        for (int t = 0; t < iSpawnItem; t++)
            if (pSpawnItem[t])
                delete pSpawnItem[t];

        GlobalFree((HGLOBAL) pSpawnItem);
    }
    if (iImageItem > 0) {
        TRACE1("CBitmapMenu::Deleting %d images item.\n", iImageItem);
        GlobalFree((HGLOBAL) pImageItem);
    }
    if (m_hMenuFont) {
        ::DeleteObject((HGDIOBJ)m_hMenuFont);
    }
}


bool CBitmapMenu::ExcludeItem(int nItemID)
{
    return false;
}

bool CBitmapMenu::AddToolBarResource(unsigned int resId)
{
    HRSRC hRsrc = ::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(resId), RT_TOOLBAR);
    if (hRsrc == NULL)
        return false;

    HGLOBAL hGlb = ::LoadResource(AfxGetResourceHandle(), hRsrc);
    if (hGlb == NULL)
        return false;

    ToolBarData* pTBData = (ToolBarData*) ::LockResource(hGlb);
    if (pTBData == NULL)
        return false;

    ASSERT(pTBData->wVersion == 1);

    CBitmap bmp;
    bmp.LoadBitmap(resId);
    int nBmpItems = ilList.Add(&bmp, RGB(192,192,192));
    bmp.DeleteObject();

    WORD* pItem = (WORD*)(pTBData+1);

    for (int i=0; i<pTBData->wItemCount; i++, pItem++) {
        if (*pItem != ID_SEPARATOR) {
            AddImageItem(nBmpItems++, (WORD) *pItem);
        }
    }

    return true;
}

bool CBitmapMenu::LoadToolBarResource(unsigned int resId)
{
    HRSRC hRsrc = ::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(resId), RT_TOOLBAR);
    if (hRsrc == NULL)
        return false;

    HGLOBAL hGlb = ::LoadResource(AfxGetResourceHandle(), hRsrc);
    if (hGlb == NULL)
        return false;

    ToolBarData* pTBData = (ToolBarData*) ::LockResource(hGlb);
    if (pTBData == NULL)
        return false;

    ASSERT(pTBData->wVersion == 1);

    szImage.cx = int(pTBData->wWidth);
    szImage.cy = int(pTBData->wHeight);

    if (ilList.Create(szImage.cx, szImage.cy, ILC_COLOR4|ILC_MASK, pTBData->wItemCount, 0) == false) {
        return false;
    }

    ilList.SetBkColor(GetSysColor(COLOR_3DFACE));

    CBitmap bmp;
    bmp.LoadBitmap(resId);
    ilList.Add(&bmp, RGB(192,192,192));
    bmp.DeleteObject();

    WORD* pItem = (WORD*)(pTBData+1);
    int nBmpItems = 0;
    for (int i=0; i<pTBData->wItemCount; i++, pItem++) {
        if (*pItem != ID_SEPARATOR) {
            AddImageItem(nBmpItems++, (WORD) *pItem);
        }
    }
    return true;
}

void CBitmapMenu::AddImageItem(int idx, WORD cmd)
{
    if (iImageItem == 0)
        pImageItem = (ImageItem*)GlobalAlloc(GPTR, sizeof(ImageItem));
    else
        pImageItem = (ImageItem*)GlobalReAlloc((HGLOBAL) pImageItem, sizeof(ImageItem) * (iImageItem + 1), GMEM_MOVEABLE|GMEM_ZEROINIT);

    ASSERT(pImageItem);
    pImageItem[iImageItem].iCmd = (int) cmd;
    pImageItem[iImageItem].iImageIdx = idx;
    iImageItem ++;
}

void CBitmapMenu::RemapMenu(CMenu* pMenu)
{
    static int iRecurse = 0;
    iRecurse++;

    ASSERT(pMenu);
    int nItem = pMenu->GetMenuItemCount();
    while ((--nItem)>=0) {
        UINT nItemID = pMenu->GetMenuItemID(nItem);
        if (ExcludeItem(nItemID)) {
            continue;
        }
        if (nItemID == (UINT) -1) {
            CMenu* pops = pMenu->GetSubMenu(nItem);
            if (pops) {
                RemapMenu(pops);
            }
            if (iRecurse > 1) {
                CString cs;
                pMenu->GetMenuString(nItem, cs, MF_BYPOSITION);
                if (cs != "") {
                    SpawnItem* sp = AddSpawnItem(cs, -2);
                    pMenu->ModifyMenu(nItem, MF_BYPOSITION|MF_OWNERDRAW, (UINT) -1, (LPCTSTR)sp);
                }
            }
        } else {
            if (nItemID != 0) {
                UINT oldState = pMenu->GetMenuState(nItem, MF_BYPOSITION);
                if (!(oldState & MF_OWNERDRAW)) {
                    ASSERT(oldState != (UINT)-1);
                    CString cs;
                    pMenu->GetMenuString(nItem, cs, MF_BYPOSITION);
                    SpawnItem* sp = AddSpawnItem(cs, nItemID);
                    pMenu->ModifyMenu(nItem, MF_BYPOSITION|MF_OWNERDRAW|oldState, (LPARAM)nItemID, (LPCTSTR)sp);
                }
            }
        }
    }
    iRecurse--;
}

CBitmapMenu::SpawnItem* CBitmapMenu::AddSpawnItem(const char * txt, int cmd)
{
    if (iSpawnItem == 0)
        pSpawnItem = (SpawnItem**)GlobalAlloc(GPTR, sizeof(SpawnItem));
    else
        pSpawnItem = (SpawnItem**)GlobalReAlloc((HGLOBAL) pSpawnItem, sizeof(SpawnItem)*(iSpawnItem + 1), GMEM_MOVEABLE|GMEM_ZEROINIT);

    ASSERT(pSpawnItem);

    SpawnItem* p = new SpawnItem;
    ASSERT(p);
    pSpawnItem[iSpawnItem] = p;
    CopyString(p->cText, txt, 128);
    p->iCmd = cmd;

    if (cmd >= 0)
        p->iImageIdx = FindImageItem(cmd);
    else
        p->iImageIdx = -2;

    iSpawnItem ++;
    return p;
}

int CBitmapMenu::FindImageItem(int cmd)
{
    for (int t = 0; t < iImageItem; t++) {
        if (pImageItem[t].iCmd == cmd) {
            return pImageItem[t].iImageIdx;
        }
    }
    return -1;
}

void CBitmapMenu::DrawItem(LPDRAWITEMSTRUCT lp)
{
    if (lp->CtlType == ODT_MENU) {
        UINT id = lp->itemID;
        UINT state = lp->itemState;
        bool bEnab = !(state & ODS_DISABLED);
        bool bSelect = (state & ODS_SELECTED) ? true : false;
        bool bChecked = (state & ODS_CHECKED) ? true : false;

        SpawnItem* pItem = (SpawnItem*)lp->itemData;
        if (pItem) {
            CDC* pDC = CDC::FromHandle(lp->hDC);
            CFont* pft = CFont::FromHandle(m_hMenuFont);
            CFont* of = pDC->SelectObject(pft);
            CRect rc = lp->rcItem;
            CRect rcImage(rc), rcText(rc);
            rcImage.right = rcImage.left + rc.Height();
            rcImage.bottom = rc.bottom;
            rcText.left += rcImage.right + 1;

            int obk = pDC->SetBkMode(TRANSPARENT);

            COLORREF ocr;
            if (bSelect) {
                if (pItem->iImageIdx >= 0 || (state & ODS_CHECKED))
                    pDC->FillSolidRect(rcText, GetSysColor(COLOR_HIGHLIGHT));
                else
                    pDC->FillSolidRect(rc, GetSysColor(COLOR_HIGHLIGHT));

                ocr = pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
            } else {
                if (pItem->iImageIdx >= 0 || (state & ODS_CHECKED))
                    pDC->FillSolidRect(rcText, GetSysColor(COLOR_MENU));
                else
                    pDC->FillSolidRect(rc, GetSysColor(COLOR_MENU));

                ocr = pDC->SetTextColor(GetSysColor(COLOR_MENUTEXT));
            }

            if (pItem->iImageIdx >= 0) {
                int ay = (rcImage.Height() - szImage.cy) / 2;
                int ax = (rcImage.Width()  - szImage.cx) / 2;

                if (bSelect && bEnab)
                    pDC->Draw3dRect(rcImage,GetSysColor(COLOR_3DHILIGHT),GetSysColor(COLOR_3DSHADOW));
                else
                    pDC->Draw3dRect(rcImage,GetSysColor(COLOR_MENU),GetSysColor(COLOR_MENU));

                ilList.Draw(pDC, pItem->iImageIdx, CPoint(rcImage.left + ax, rcImage.top + ay), ILD_NORMAL);
                if (!bEnab) {
                    CRect strictImgRc(rcImage.left + ax, rcImage.top + ay, rcImage.left + ax + szImage.cx, rcImage.top + ay + szImage.cy);
                    DrawDisabledButton(*pDC, strictImgRc);
                }
            } else {
                if (bChecked) {
                    int ay = (rcImage.Height() - szImage.cy) / 2;
                    int ax = (rcImage.Width()  - szImage.cx) / 2;

                    ilOther.Draw(pDC, 0, CPoint(rcImage.left + ax, rcImage.top + ay - 2), ILD_NORMAL);
                }
            }


            CString cs = pItem->cText, cs1;
            CSize sz;
            sz = pDC->GetTextExtent(cs);
            int ay1 = (rcText.Height() - sz.cy) / 2;
            rcText.top += ay1;
            rcText.left += 2;
            rcText.right -= 15;

            int tf = cs.Find('\t');
            if (tf >= 0) {
                cs1 = cs.Right(cs.GetLength() - tf - 1);
                cs = cs.Left(tf);
                if (!bEnab) {
                    if (!bSelect) {
                        CRect rcText1(rcText);
                        rcText1.InflateRect(-1,-1);
                        pDC->SetTextColor(GetSysColor(COLOR_3DHILIGHT));
                        pDC->DrawText(cs, rcText1, DT_VCENTER|DT_LEFT);
                        pDC->DrawText(cs1, rcText1, DT_VCENTER|DT_RIGHT);
                        pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
                        pDC->DrawText(cs, rcText, DT_VCENTER|DT_LEFT);
                        pDC->DrawText(cs1, rcText, DT_VCENTER|DT_RIGHT);
                    } else {
                        pDC->SetTextColor(GetSysColor(COLOR_MENU));
                        pDC->DrawText(cs, rcText, DT_VCENTER|DT_LEFT);
                        pDC->DrawText(cs1, rcText, DT_VCENTER|DT_RIGHT);
                    }
                } else {
                    pDC->DrawText(cs, rcText, DT_VCENTER|DT_LEFT);
                    pDC->DrawText(cs1, rcText, DT_VCENTER|DT_RIGHT);
                }
            } else {
                if (!bEnab) {
                    if (!bSelect) {
                        CRect rcText1(rcText);
                        rcText1.InflateRect(-1,-1);
                        pDC->SetTextColor(GetSysColor(COLOR_3DHILIGHT));
                        pDC->DrawText(cs, rcText1, DT_VCENTER|DT_LEFT|DT_EXPANDTABS);
                        pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
                        pDC->DrawText(cs, rcText, DT_VCENTER|DT_LEFT|DT_EXPANDTABS);
                    } else {
                        pDC->SetTextColor(GetSysColor(COLOR_MENU));
                        pDC->DrawText(cs, rcText, DT_VCENTER|DT_LEFT|DT_EXPANDTABS);
                    }
                } else {
                    pDC->DrawText(cs, rcText, DT_VCENTER|DT_LEFT|DT_EXPANDTABS);
                }
            }

            pDC->SelectObject(of);
            pDC->SetTextColor(ocr);
            pDC->SetBkMode(obk);
        }
    }
}

void CBitmapMenu::MeasureItem(LPMEASUREITEMSTRUCT lpm)
{
    if (lpm->CtlType == ODT_MENU) {
        UINT id = lpm->itemID;

        SpawnItem* pItem = (SpawnItem *)lpm->itemData;
        if (pItem) {
            CString cs = pItem->cText;
            if (cs != "") {
                CWnd* pWnd = AfxGetMainWnd();
                ASSERT(pWnd != NULL);
                CClientDC dc(pWnd);
                CFont* pft = CFont::FromHandle(m_hMenuFont);
                CFont* of = dc.SelectObject(pft);
                CRect rc(0,0,0,0);
                // determine rc
                dc.DrawText(cs, rc, DT_CALCRECT|DT_EXPANDTABS|DT_VCENTER|DT_LEFT);
                lpm->itemHeight = szImage.cy + 5;
                lpm->itemWidth  = 2 + szImage.cx + 2 + rc.Width() + 10;
                dc.SelectObject(of);
            } else {
                lpm->itemHeight = szImage.cy + 5;
                lpm->itemWidth  = 100;
            }
        }
    }
}

void CBitmapMenu::DrawDisabledButton( CDC & dc, const CRect & rc ) const
{
    // create a monochrome memory DC
    CDC ddc;
    ddc.CreateCompatibleDC(0);
    CBitmap bmp;
    bmp.CreateCompatibleBitmap(&ddc, rc.Width(), rc.Height());
    CBitmap * pOldBmp = ddc.SelectObject(&bmp);

    COLORREF m_clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
    COLORREF m_clrBtnHilight = ::GetSysColor(COLOR_BTNHILIGHT);
    COLORREF m_clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);

    // build a mask
    ddc.PatBlt(0, 0, rc.Width(), rc.Height(), WHITENESS);
    dc.SetBkColor(m_clrBtnFace);
    ddc.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, rc.left, rc.top, SRCCOPY);
    dc.SetBkColor(m_clrBtnHilight);
    ddc.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, rc.left, rc.top, SRCPAINT);

    // Copy the image from the toolbar into the memory DC
    // and draw it (grayed) back into the toolbar.
    dc.FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(), m_clrBtnFace);
    dc.SetBkColor(RGB(0, 0, 0));
    dc.SetTextColor(RGB(255, 255, 255));
    CBrush brShadow, brHilight;
    brHilight.CreateSolidBrush(m_clrBtnHilight);
    brShadow.CreateSolidBrush(m_clrBtnShadow);
    CBrush * pOldBrush = dc.SelectObject(&brHilight);
    dc.BitBlt(rc.left+1, rc.top+1, rc.Width(), rc.Height(), &ddc, 0, 0, 0x00E20746L);
    dc.SelectObject(&brShadow);
    dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &ddc, 0, 0, 0x00E20746L);

    // reset DCs
    dc.SelectObject(pOldBrush);
    ddc.SelectObject(pOldBmp);
    ddc.DeleteDC();

    bmp.DeleteObject();
}

void CBitmapMenu::EnableMenuItems(CMenu * pMenu, CWnd * pParent)
{
    ASSERT(pMenu);
    ASSERT(pParent);

    int nItem = pMenu->GetMenuItemCount();
    CCmdUI state;
    state.m_pMenu = pMenu;
    state.m_nIndex = nItem-1;
    state.m_nIndexMax = nItem;

    while ((--nItem)>=0) {
        UINT itemId = pMenu->GetMenuItemID(nItem);
        if (itemId == (UINT) -1) {
            CMenu *pops = pMenu->GetSubMenu(nItem);
            if (pops)
                EnableMenuItems(pops, pParent);
        } else {
            if (itemId != 0) {
                state.m_nID = itemId;
                pParent->OnCmdMsg(itemId, CN_UPDATE_COMMAND_UI, &state, NULL);
                state.DoUpdate(pParent, true);
            }
        }
        state.m_nIndex = nItem-1;
    }
}

