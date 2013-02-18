///////////////////////////////////////////////////////////////////////////////////
//
// TitlePopupMenu.cpp
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

#include "TitlePopupMenu.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


TitlePopupMenu::TitlePopupMenu() :
    m_hFont(NULL)
{
}

TitlePopupMenu::~TitlePopupMenu()
{
    if (m_hFont != NULL) {
        DeleteObject(m_hFont);
    }
}

void TitlePopupMenu::AddMenuTitle(LPSTR szTitle)
{
    // separator
    InsertMenu(0, MF_BYPOSITION|MF_SEPARATOR);

    InsertMenu(0, MF_BYPOSITION|MF_STRING|MF_DISABLED |MF_OWNERDRAW, 1, szTitle);

    m_sTitle = szTitle;
    ASSERT(m_hFont == NULL);
    m_hFont = CreatePopupMenuTitleFont();
}

HFONT TitlePopupMenu::CreatePopupMenuTitleFont()
{
    HFONT hFont = HFONT(::GetStockObject(ANSI_VAR_FONT));
    if (hFont != NULL) {
        LOGFONT lf;
        if (::GetObject(hFont, sizeof(LOGFONT), &lf)) {
            lf.lfWeight = FW_BOLD;
            hFont = CreateFontIndirect(&lf);
        }
    }
    return hFont;
}

void TitlePopupMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{

    if (m_hFont != NULL) {
        CClientDC dc(CWnd::GetDesktopWindow());
        CFont* pFontOld = dc.SelectObject(CFont::FromHandle(m_hFont));

        LPCSTR szMenuTitle = LPCSTR(lpMeasureItemStruct->itemData);
        CSize size = dc.GetTextExtent(szMenuTitle);

        dc.SelectObject(pFontOld);

        size.cx += GetSystemMetrics(SM_CXMENUCHECK); // add in the left margin for the menu item

        lpMeasureItemStruct->itemWidth = size.cx;
        lpMeasureItemStruct->itemHeight = size.cy;
    }
}

void TitlePopupMenu::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

    // create the font we will use for the title
    if (m_hFont != NULL) {
        HBRUSH hBrushBackgound = CreateSolidBrush(::GetSysColor(COLOR_MENU));
        FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, hBrushBackgound);
        DeleteObject(hBrushBackgound);

        int nMode = SetBkMode(lpDrawItemStruct->hDC, TRANSPARENT);
        COLORREF clrTextOld = SetTextColor(lpDrawItemStruct->hDC, ::GetSysColor(COLOR_MENUTEXT));
        HFONT pFontOld = HFONT(SelectObject(lpDrawItemStruct->hDC, m_hFont));

        lpDrawItemStruct->rcItem.left += GetSystemMetrics(SM_CXMENUCHECK)+8;

        //LPCSTR szMenuTitle = LPCSTR(lpDrawItemStruct->itemData);
        LPCSTR szMenuTitle = m_sTitle;
        TRACE("Menu Title: %s\r\n", szMenuTitle);
        DrawText(lpDrawItemStruct->hDC, szMenuTitle, -1, &lpDrawItemStruct->rcItem, DT_SINGLELINE|DT_VCENTER|DT_LEFT);

        SelectObject(lpDrawItemStruct->hDC, pFontOld);
        SetBkMode(lpDrawItemStruct->hDC, nMode);
        SetTextColor(lpDrawItemStruct->hDC, clrTextOld);
    }
}


