///////////////////////////////////////////////////////////////////////////////////
//
// InfoBar.cpp
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

#include "InfoBar.h"

#include <AfxPriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CStaticCmdUI : public CCmdUI
{
public:
    virtual void SetText(LPCTSTR lpszText);
};

void CStaticCmdUI::SetText(LPCTSTR lpszText)
{
    CInfoBar* pInfoBar = (CInfoBar*)m_pOther;
    ASSERT(pInfoBar != NULL);

    pInfoBar->SetText(lpszText);
}


/////////////////////////////////////////////////////////////////////////////
// CInfoBar

CInfoBar::CInfoBar()
{
    m_cyBottomBorder = 4;
    m_cyTopBorder = 4;  // l&r default to 6; t&b was 1
    m_sCaption = "Default";                 // must have some text for CalcFixedLayout
    m_cxAvailable = 0;
    m_crBackgroundColor = ::GetSysColor(COLOR_3DSHADOW); //Gray
    m_crTextColor = RGB(255,255,255); // White
    m_hIcon = NULL;
    m_sizeBitmap.cx = 0;
    m_bPictureSet = false;
}

CInfoBar::~CInfoBar()
{
}

BEGIN_MESSAGE_MAP(CInfoBar, CControlBar)
    //{{AFX_MSG_MAP(CInfoBar)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInfoBar message handlers

int CInfoBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CControlBar::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Create a font twice as tall as the system status bar font
    NONCLIENTMETRICS metrics;
    metrics.cbSize = sizeof(metrics);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &metrics, 0);

    metrics.lfStatusFont.lfHeight = int(1.5 * metrics.lfStatusFont.lfHeight);

    VERIFY(m_font.CreateFontIndirect(&metrics.lfStatusFont));
    return 0;
}

BOOL CInfoBar::OnEraseBkgnd(CDC* pDC)
{
    CRect rect;
    GetClientRect(rect);
    pDC->FillSolidRect(rect, ::GetSysColor(COLOR_BTNFACE) );

    rect.top += 2; rect.bottom -= 3;

    pDC->FillSolidRect(rect, m_crBackgroundColor);

    return CControlBar::OnEraseBkgnd(pDC);
}

void CInfoBar::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    CRect r;
    GetClientRect(r);
    CalcInsideRect(r, TRUE);

    r.InflateRect(0, 2);

    CFont* pOldFont = dc.SelectObject(&m_font);

    dc.SetTextColor(m_crTextColor);
    UINT nFormat = DT_EDITCONTROL|DT_EXTERNALLEADING|DT_LEFT|DT_WORDBREAK|DT_END_ELLIPSIS;
    dc.DrawText(m_sCaption, -1, r, nFormat);

    if (m_bPictureSet) {
        if (m_hIcon == NULL)
            dc.DrawState(CPoint(r.right - m_sizeBitmap.cx,(r.bottom - m_sizeBitmap.cy)/2),
                         m_sizeBitmap, &m_bm, DST_BITMAP);
        else
            dc.DrawState(CPoint(r.right - m_sizeBitmap.cx,(r.bottom - m_sizeBitmap.cy)/2),
                         m_sizeBitmap, m_hIcon, DST_ICON, HBRUSH(NULL) );
    }


    if (pOldFont != NULL)
        dc.SelectObject(pOldFont);
}

LRESULT CInfoBar::OnSizeParent(WPARAM, LPARAM lParam)
{
    AFX_SIZEPARENTPARAMS* lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;

    // keep track of the available width for use by CalcFixedLayout later
    m_cxAvailable = lpLayout->rect.right - lpLayout->rect.left;

    return CControlBar::OnSizeParent(0, lParam);
}

// Overridden virtual helper methods

void CInfoBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
    CStaticCmdUI state;
    state.m_pOther = this;
    state.m_nIndexMax = 1;      // there's only one thing to update
    state.m_nID = AFX_IDW_STATUS_BAR;

    // allow the statusbar itself to have update handlers
    if (CWnd::OnCmdMsg(state.m_nID, CN_UPDATE_COMMAND_UI, &state, NULL))
        return;

    // allow target (owner) to handle the remaining updates
    state.DoUpdate(pTarget, FALSE);
}

CSize CInfoBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
    // Get border size (values will be negative)
    CRect rBorder;
    rBorder.SetRectEmpty();
    CalcInsideRect(rBorder, bHorz);

    // Based on the available width minus the border area,
    // calculate the necessary Y value to hold the text
    CRect rCalc(0, 0, m_cxAvailable - (-rBorder.Width()), 0);
    CClientDC dc(this);
    CFont* pOldFont = dc.SelectObject(&m_font);

    UINT nFormat = DT_CALCRECT|DT_EDITCONTROL|DT_EXTERNALLEADING|DT_LEFT|DT_WORDBREAK;
    dc.DrawText(m_sCaption, -1, rCalc, nFormat);

    if (pOldFont != NULL)
        dc.SelectObject(pOldFont);

    // The Y value is the sum of the calculated height from DrawText,
    // plus the top and bottom border.
    CSize size;
    size.cx = 32767;
    size.cy = rCalc.Height();
    size.cy += (-rBorder.Height());

    return size;
}

int CInfoBar::GetHeight(int cx)
{
    // Get border size (values will be negative)
    CRect rBorder;
    rBorder.SetRectEmpty();
    CalcInsideRect(rBorder, TRUE);

    // Based on the available width minus the border area,
    // calculate the necessary Y value to hold the text
    CRect rCalc(0, 0, cx - (-rBorder.Width()), 0);
    CClientDC dc(this);
    CFont* pOldFont = dc.SelectObject(&m_font);

    UINT nFormat = DT_CALCRECT|DT_EDITCONTROL|DT_EXTERNALLEADING|DT_LEFT|DT_WORDBREAK;
    dc.DrawText(m_sCaption, -1, rCalc, nFormat);

    if (pOldFont != NULL)
        dc.SelectObject(pOldFont);

    // The Y value is the sum of the calculated height from DrawText,
    // plus the top and bottom border.
    int cy = rCalc.Height();
    cy += (-rBorder.Height());

    return cy;
}

void CInfoBar::SetText(LPCTSTR lpszNew)
{
    ASSERT(lpszNew && AfxIsValidString(lpszNew));

    if (m_sCaption != lpszNew) {
        if (lpszNew == NULL) {
            m_sCaption = " "; // CalcFixedlayout needs some text
        } else {
            m_sCaption = lpszNew;
        }
        if (::IsWindow(m_hWnd)) {
            Invalidate();
        }
    }
}

bool CInfoBar::SetBitmap(UINT nResID)
{
    bool bRet = false;
    m_bm.DeleteObject();
    bRet = m_bm.LoadBitmap(nResID) != NULL;
    if (bRet) {
        BITMAP bitmap;
        m_bm.GetBitmap(&bitmap);
        m_sizeBitmap.cx = bitmap.bmWidth;
        m_sizeBitmap.cy = bitmap.bmHeight;
        m_bPictureSet = true;
    }
    if (::IsWindow(m_hWnd)) {
        Invalidate();
    }
    m_hIcon = NULL;
    return bRet;
}

bool CInfoBar::SetIcon(UINT nResID)
{
    m_sizeBitmap.cx = ::GetSystemMetrics(SM_CXSMICON);
    m_sizeBitmap.cy = ::GetSystemMetrics(SM_CYSMICON);
    m_hIcon = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(nResID));
    if (m_hIcon != NULL) {
        m_bPictureSet = true;
    }
    if (::IsWindow(m_hWnd)) {
        Invalidate();
    }
    return m_hIcon != NULL;
}

bool CInfoBar::SetIcon(HICON hIcon)
{
    m_sizeBitmap.cx = ::GetSystemMetrics(SM_CXSMICON);
    m_sizeBitmap.cy = ::GetSystemMetrics(SM_CYSMICON);
    m_hIcon = hIcon;
    m_bPictureSet = true;
    if (::IsWindow(m_hWnd)) {
        Invalidate();
    }
    return TRUE;
}

BOOL CInfoBar::SetTextFont(LPCTSTR lpFontName)
{
    BOOL bRet;
    m_font.DeleteObject();

    NONCLIENTMETRICS metrics;
    metrics.cbSize = sizeof(metrics);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &metrics, 0);

    metrics.lfStatusFont.lfHeight *= 2;
    CopyString(metrics.lfStatusFont.lfFaceName, lpFontName, 32);
    bRet = m_font.CreateFontIndirect(&metrics.lfStatusFont);
    if (::IsWindow(m_hWnd)) {
        Invalidate();
    }
    return bRet;
}

void CInfoBar::SetBackgroundColor(COLORREF crNew)
{
    m_crBackgroundColor = crNew;
    if (::IsWindow(m_hWnd)) {
        Invalidate();
    }
}

void CInfoBar::SetTextColor(COLORREF crNew)
{
    m_crTextColor = crNew;
    if (::IsWindow(m_hWnd)) {
        Invalidate();
    }
}



CInfoBarStatus::CInfoBarStatus()
{
    m_cyBottomBorder = 4;
    m_messageStatus = msNone;
}

CInfoBarStatus::~CInfoBarStatus()
{
}

BEGIN_MESSAGE_MAP(CInfoBarStatus, CInfoBar)
    //{{AFX_MSG_MAP(CInfoBarStatus)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CInfoBarStatus::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CControlBar::OnCreate(lpCreateStruct) == -1)
        return -1;

    ModifyStyleEx(0, WS_EX_CLIENTEDGE);

    // Create a font twice as tall as the system status bar font
    NONCLIENTMETRICS metrics;
    metrics.cbSize = sizeof(metrics);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &metrics, 0);
    metrics.lfStatusFont.lfHeight = int(metrics.lfStatusFont.lfHeight);
    metrics.lfStatusFont.lfWeight = FW_BOLD;
    VERIFY(m_font.CreateFontIndirect(&metrics.lfStatusFont));
    return 0;
}

BOOL CInfoBarStatus::OnEraseBkgnd(CDC* pDC)
{
    CRect rect;
    GetClientRect(rect);
    pDC->FillSolidRect(rect, ::GetSysColor(COLOR_BTNFACE));

    pDC->FillSolidRect(rect, m_crBackgroundColor);

    return CControlBar::OnEraseBkgnd(pDC);
}

void CInfoBarStatus::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    CRect r;
    GetClientRect(r);
    CalcInsideRect(r, TRUE);

    r.InflateRect(0, 2);

    CFont* pOldFont = dc.SelectObject(&m_font);

    dc.SetTextColor(m_crTextColor);

    if (m_bPictureSet) {
        if (m_hIcon == NULL)
            dc.DrawState(CPoint(0,(r.bottom - m_sizeBitmap.cy)/2),
                         m_sizeBitmap, &m_bm, DST_BITMAP);
        else
            ::DrawIconEx(dc.m_hDC,
              0,(r.bottom - m_sizeBitmap.cy)/2,
              m_hIcon,          // HICON
              m_sizeBitmap.cx, m_sizeBitmap.cy, // cx,cy
              0,                // frame num N/A
              NULL,             // background brush
              DI_NORMAL);       // flags
    }

    const char* szText = "0.123456789012345";
    CSize size = dc.GetOutputTextExtent(szText);

    switch (m_messageStatus) {
    case msNone:
    case msInformation:
        dc.SetTextColor(m_crTextColor);
        break;
    case msWarning:
        dc.SetTextColor(RGB(255,255,0)); // yellow
        break;
    case msError:
        dc.SetTextColor(RGB(0x0,0x0,0x0)); // black
        break;
    }

    r.left += m_sizeBitmap.cx + 1;
    r.right -= 2 * size.cx;
    UINT nFormat = DT_EDITCONTROL|DT_EXTERNALLEADING|DT_LEFT|DT_WORDBREAK|DT_END_ELLIPSIS;
    dc.DrawText(m_sCaption, -1, r, nFormat);

    CRect r1 = r;
    CRect r2 = r;
    r1.left = r.right;
    r1.right = r1.left + size.cx;
    r2.left = r1.right + 2;
    r2.right = r2.left + size.cx;

    r1.InflateRect(0, 2);
    //dc.DrawEdge(r1, EDGE_RAISED, BF_RECT);
    dc.DrawEdge(r1, EDGE_SUNKEN, BF_RECT);
    r1.DeflateRect(2, 2);
    dc.SetTextColor(m_crTextColor);
    dc.DrawText(m_sFrom, r1, DT_RIGHT|DT_VCENTER);

    r2.InflateRect(0, 2);
    //dc.DrawEdge(r2, EDGE_RAISED, BF_RECT);
    dc.DrawEdge(r2, EDGE_SUNKEN, BF_RECT);
    r2.DeflateRect(2, 2);
    dc.SetTextColor(m_crTextColor);
    dc.DrawText(m_sTo, r2, DT_RIGHT|DT_VCENTER);

    if (pOldFont != NULL) {
        dc.SelectObject(pOldFont);
    }
}

BOOL CInfoBarStatus::SetStatus(MessageStatus messageStatus, LPCTSTR szText)
{
    HICON hIcon;
    m_messageStatus = messageStatus;
    switch (messageStatus) {
    case msNone:
        hIcon = NULL;
        break;
    case msInformation:
        hIcon = ::LoadIcon(NULL, IDI_INFORMATION);
        break;
    case msWarning:
        hIcon = ::LoadIcon(NULL, IDI_WARNING);
        break;
    case msError:
        hIcon = ::LoadIcon(NULL, IDI_ERROR);
        break;
    }
    if (szText == NULL) {
        m_sCaption = " "; // for height calculation in CalcFixelayout
    } else {
        m_sCaption = szText;
    }
    return CInfoBar::SetIcon(hIcon);
}

void CInfoBarStatus::SetValueRange(LPCTSTR szFrom, LPCTSTR szTo)
{
    m_sFrom = szFrom;
    m_sTo = szTo;
    if (::IsWindow(m_hWnd)) {
        Invalidate();
    }
}

