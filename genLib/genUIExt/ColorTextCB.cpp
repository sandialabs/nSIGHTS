///////////////////////////////////////////////////////////////////////////////////
//
// ColorTextCB.cpp
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

#include "ColorTextCB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CColorTextComboBox::CColorTextComboBox()
{
}


CColorTextComboBox::~CColorTextComboBox()
{
}

BEGIN_MESSAGE_MAP(CColorTextComboBox, CComboBox)
    //{{AFX_MSG_MAP(CColorTextComboBox)
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorTextComboBox message handlers

int CColorTextComboBox::OnCreate(LPCREATESTRUCT pCStruct)
{
    if (CComboBox::OnCreate(pCStruct) == -1) {
        return -1;
    }
    ASSERT((pCStruct->style & (CBS_HASSTRINGS|CBS_OWNERDRAWFIXED)) != 0);
    return 0;
}

// In the case of dialog created from resources PreSubclassWindow
// is the only chance to check style
void CColorTextComboBox::PreSubclassWindow()
{
    CComboBox::PreSubclassWindow();
    ASSERT((GetStyle() & (CBS_HASSTRINGS|CBS_OWNERDRAWFIXED)) != 0);
}

// MeasureItem isn't called, when owner-draw combo box is created
// from resources
void CColorTextComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
    lpMIS->itemHeight = 2 + 12 + 2;
}

COLORREF CColorTextComboBox::GetSelectedColorValue()
{
    int iItem = GetCurSel();
    if (iItem == CB_ERR) {
        return RGB(0, 0, 0);
    }
    return (COLORREF) GetItemData(iItem);
}

void CColorTextComboBox::DrawItem(LPDRAWITEMSTRUCT pDIStruct)
{
    static CString sColor;

    CRect rItemRect = pDIStruct->rcItem;
    CRect rBlockRect = rItemRect;
    CRect rTextRect = rBlockRect;

    COLORREF clrNormal = ::GetSysColor(COLOR_WINDOW);
    COLORREF clrSelected = ::GetSysColor(COLOR_HIGHLIGHT);
    COLORREF clrText = ::GetSysColor(COLOR_WINDOWTEXT);

    CDC dcContext;
    if (!dcContext.Attach(pDIStruct->hDC)) {
        return;
    }

    int iFourthWidth = rBlockRect.Width() / 4;
    CBrush brushFrame;
    brushFrame.CreateStockObject(BLACK_BRUSH);

    int iState = pDIStruct->itemState;
    if (iState & ODS_SELECTED) {
        dcContext.SetTextColor(0x00FFFFFF & ~(clrText));
        dcContext.SetBkColor(clrSelected);
        dcContext.FillSolidRect(&rBlockRect, clrSelected);
    } else {
        dcContext.SetTextColor(clrText);
        dcContext.SetBkColor(clrNormal);
        dcContext.FillSolidRect(&rBlockRect, clrNormal);
    }
    if (iState & ODS_FOCUS) {
        dcContext.DrawFocusRect(&rItemRect);
    }

    //  Calculate Text Area
    //
    rTextRect.left += (iFourthWidth + 2);
    rTextRect.top += 2;

    //  Calculate Color Block Area
    //
    rBlockRect.DeflateRect(CSize(2, 2));
    rBlockRect.right = iFourthWidth;

    //  Draw Color Text And Block
    //
    int iItem = pDIStruct->itemID;
    if (iItem != -1) {
        GetLBText(iItem, sColor);
        COLORREF clrColor;
        if (iState & ODS_DISABLED) {
            clrColor = ::GetSysColor(COLOR_INACTIVECAPTIONTEXT);
            dcContext.SetTextColor(clrColor);
        } else {
            clrColor = (COLORREF) GetItemData(iItem);
        }

        dcContext.SetBkMode(TRANSPARENT);
        dcContext.TextOut(rTextRect.left, rTextRect.top, sColor);
        dcContext.FillSolidRect(&rBlockRect, clrColor);
        dcContext.FrameRect(&rBlockRect, &brushFrame);
    }
    dcContext.Detach();
}


int CColorTextComboBox::AddColor(PCSTR szName, COLORREF clrColor)
{
    int iItem = InsertString(-1, szName);
    if ( iItem != LB_ERR ) {
        if (SetItemData(iItem, clrColor) != CB_ERR && iItem == 0) {
            SetCurSel(0);
        }
    }
    return iItem;
}


