///////////////////////////////////////////////////////////////////////////////////
//
// CheckGridCell.cpp
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
// DESCRIPTION: Windows specific code for implementing basic spreadsheet type
//              data entry/display.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include "CheckGridCell.h"

#include "CheckEditWnd.h"
#include "GridCtrlBase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static HANDLE AFXAPI LoadResourceBitmap(HINSTANCE hInstance, LPCTSTR lpString);
static DWORD AFXAPI GetNumDIBColorEntries(LPBITMAPINFO pBmpInfo);


HANDLE CCheckGridCell::m_hBitmapCheck = 0;
UINT   CCheckGridCell::m_nBitmapResourceId = IDB_CHECK_95;


CCheckGridCell::CCheckGridCell(bool bValue) :
    m_bValue(bValue)
{
    LoadCheckBitmap();
}

CCheckGridCell::~CCheckGridCell()
{
}

// copy constructor
CCheckGridCell::CCheckGridCell(const CCheckGridCell& rhs) :
    CGridCell(rhs)
{
    m_bValue = rhs.m_bValue;
}

// assignment operator
const CCheckGridCell& CCheckGridCell::operator=(const CCheckGridCell& rhs)
{
    if (&rhs == this) {
        return *this;
    }
    CGridCell::operator=(rhs);
    m_bValue = rhs.m_bValue;
    return *this;
}

void CCheckGridCell::DrawUnconstrained(CDC* pDC, int x, int y) const
{
    //pDC->TextOut(x, y, m_bValue, StringLength(m_bValue));
}

void CCheckGridCell::DrawConstrained(CDC* pDC, int x, int y, int cx, int cy) const
{
    CRect rect(x, y, x + cx, y + cy);

    CBrush br(pDC->GetTextColor());
    CBrush* pOldBrush = pDC->SelectObject(&br);

    ASSERT(m_hBitmapCheck != 0);

    // the checkboxes are arranges as follows in the bitmap:
    // 0) unchecked
    // 1) checked
    // 2) unchecked with scope (grey backgorund)
    // 3) checked with scope (grey background)
    // 4) indetermined

    // compute offset in 3dctrl bitmap
    int offset = m_bValue ? 1 : 0;

    HANDLE hBitmap = m_hBitmapCheck;

    // Bitmap Info, width and height
    LPSTR lpDIB = (LPSTR)GlobalLock(hBitmap);

    BITMAPINFO* pBMI = (BITMAPINFO *)lpDIB;

    // Look at color table size, and work out where image bits start
    DWORD dwColorTableSize;
    DWORD dwColors = GetNumDIBColorEntries((LPBITMAPINFO)lpDIB);
    dwColorTableSize = dwColors * sizeof(RGBQUAD);
    LPSTR pBits = lpDIB + sizeof(BITMAPINFOHEADER) + dwColorTableSize;

    int dx = 13;
    int x1 = rect.left;
    int y1 = rect.top;
    if (rect.Width()-dx > 0) {
        x1 += (rect.Width()-dx)/2;
    }
    if (rect.Height()-dx > 0) {
        y1 += (rect.Height()-dx)/2;
    }
    int dx2 = min(dx, cx);
    int dy2 = min(dx, cy);

    ::StretchDIBits(pDC->GetSafeHdc(),
                    x1, y1,
                    dx2, dy2,
                    dx*offset, 0,
                    dx, dx,
                    pBits,                    // Pointer to bits
                    pBMI,                     // BITMAPINFO
                    DIB_RGB_COLORS,           // Options
                    SRCCOPY);                 // Raster operator code (ROP)

    ::GlobalUnlock(hBitmap);
    pDC->DrawEdge(CRect(x1, y1, x1+dx2, y1+dy2), EDGE_SUNKEN, BF_ADJUST);
    pDC->SelectObject(pOldBrush);
}

bool CCheckGridCell::GetValue()
{
    return m_bValue;
}

void CCheckGridCell::SetValue(bool bValue)
{
    m_bValue = bValue;
}

CSize CCheckGridCell::GetExtent(CDC* pDC) const
{
    CSize sz(15, 15);
    return sz;
}

CWnd* CCheckGridCell::CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar)
{
    CCheckEditWnd* pEdit = new CCheckEditWnd(pParent, rect, 0, nID, nRow, nCol, nFirstChar, this);
    return pEdit;
}

bool CCheckGridCell::CanEdit()
{
    return CGridCell::CanEdit();
}

bool CCheckGridCell::CanClose(CWnd *pWnd)
{
    ASSERT( pWnd != NULL );
    CCheckEditWnd* pEditWnd = static_cast<CCheckEditWnd*>(pWnd);
    return pEditWnd->CanClose();
}

void CCheckGridCell::EndEdit(CWnd *pWnd)
{
    ASSERT(pWnd != NULL);
    CCheckEditWnd* pEditWnd = static_cast<CCheckEditWnd*>(pWnd);
    pEditWnd->EndEdit();
}

void CCheckGridCell::AddMenuItems(CMenu *pMenu)
{
}

void CCheckGridCell::OnCellSelected(CWnd* pParent, int nRow, int nCol)
{
    m_bValue = !m_bValue;
    static_cast<CGridCtrlBase*>(pParent)->InvalidateCell(nRow, nCol);
    pParent->UpdateWindow();
}

COLORREF CCheckGridCell::GetColorValue(COLORREF rgb)
{
    COLORREF refF = ::GetSysColor(COLOR_WINDOWFRAME);
    COLORREF refB = ::GetSysColor(COLOR_WINDOW);

    switch (rgb) {
    case RGB(255, 0, 0):
        return refB;

    case RGB(0, 255, 0):
        return refF;

    case RGB(192, 192, 192):
        return ::GetSysColor(COLOR_BTNFACE);

    case RGB(0, 0, 0):
        return ::GetSysColor(COLOR_WINDOWTEXT);

    default:
        return rgb;
    }
}

bool CCheckGridCell::LoadCheckBitmap()
{
    if (m_hBitmapCheck != 0) {
        return true;
    }

    HINSTANCE hInst = AfxGetInstanceHandle();
    m_hBitmapCheck = LoadResourceBitmap(hInst, MAKEINTRESOURCE(m_nBitmapResourceId));
    if (m_hBitmapCheck == 0) {
        return false;
    }

    HANDLE hBitmap = m_hBitmapCheck;
    LPSTR lpDIB = (LPSTR)GlobalLock(hBitmap);
    DWORD dwColors = GetNumDIBColorEntries((LPBITMAPINFO)lpDIB);
    for (DWORD i = 0; i < dwColors; i++) {
        RGBQUAD* pColor = &((LPBITMAPINFO)lpDIB)->bmiColors[i];
        COLORREF rgbOld = RGB(pColor->rgbRed, pColor->rgbGreen, pColor->rgbBlue);
        COLORREF rgbNew = GetColorValue(rgbOld);

        // replace color
        if (rgbOld != rgbNew) {
            pColor->rgbGreen = GetGValue(rgbNew);
            pColor->rgbBlue = GetBValue(rgbNew);
            pColor->rgbRed = GetRValue(rgbNew);
        }
    }

    GlobalUnlock(hBitmap);

    return true;
}

HANDLE AFXAPI LoadResourceBitmap(HINSTANCE hInstance, LPCTSTR lpString)
{
    HANDLE hBitmapFinal = NULL;
    HRSRC hRsrc = FindResource(hInstance, lpString, RT_BITMAP);
    if (hRsrc != NULL) {
        // On Win32s, LoadResource does not return a true global memory handle. This
        // causes CreateDIBitmap to fail. To work around this problem, we must use GlobalAlloc
        // to create a global memory handle.

        HGLOBAL hTemp = LoadResource(hInstance, hRsrc);
        DWORD dwSize = SizeofResource(hInstance, hRsrc);
        LPVOID lpRes = LockResource(hTemp);

        HGLOBAL hGlobal = GlobalAlloc(GHND, dwSize);
        LPVOID lpNew = GlobalLock(hGlobal);
        memcpy(lpNew, lpRes, (size_t) dwSize);
        UnlockResource(hTemp);
        FreeResource(hTemp);

        LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)lpNew;

        GlobalUnlock(hGlobal);
        hBitmapFinal = (HANDLE) hGlobal;
    }

    return hBitmapFinal;
}

static DWORD AFXAPI GetNumDIBColorEntries(LPBITMAPINFO pBmpInfo)
{
    ASSERT(pBmpInfo);

    BITMAPINFOHEADER* pBIH = &(pBmpInfo->bmiHeader);
    BITMAPCOREHEADER* pBCH = (BITMAPCOREHEADER*) pBIH;

    // Start off by assuming the color table size from
    // the bit-per-pixel field.
    int iBitCount = pBIH->biBitCount;

    DWORD dwColors;
    switch (iBitCount) {
    case 1:
        dwColors = 2;
        break;
    case 4:
        dwColors = 16;
        break;
    case 8:
        dwColors = 256;
        break;
    default:
        dwColors = 0;
        break;
    }

    // the color table length is determined by the
    // biClrUsed field if the value in the field is nonzero.
    if (pBIH->biClrUsed != 0) {
        dwColors = pBIH->biClrUsed;
    }

    return dwColors;
}

