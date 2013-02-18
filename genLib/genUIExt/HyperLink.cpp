///////////////////////////////////////////////////////////////////////////////////
//
// HyperLink.cpp
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

#include "HyperLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int TOOLTIP_ID = 1;

/////////////////////////////////////////////////////////////////////////////
// CHyperLink

CHyperLink::CHyperLink()
{
    m_hLinkCursor       = NULL;                 // No cursor as yet
    m_crLinkColour      = RGB(  0,   0, 238);   // Blue
    m_crVisitedColour   = RGB( 85,  26, 139);   // Purple
    m_crHoverColour     = ::GetSysColor(COLOR_3DHILIGHT);
    m_bOverControl      = false;                // Cursor not yet over control
    m_bVisited          = false;                // Hasn't been visited yet.
    m_bUnderline        = true;                 // Underline the link?
    m_bAdjustToFit      = true;                 // Resize the window to fit the text?
}

CHyperLink::~CHyperLink()
{
    m_Font.DeleteObject();
}

BEGIN_MESSAGE_MAP(CHyperLink, CStatic)
    //{{AFX_MSG_MAP(CHyperLink)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_SETCURSOR()
    ON_WM_MOUSEMOVE()
    ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
    ON_WM_KILLFOCUS()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHyperLink message handlers

BOOL CHyperLink::PreTranslateMessage(MSG* pMsg)
{
    m_toolTip.RelayEvent(pMsg);
    return CStatic::PreTranslateMessage(pMsg);
}

void CHyperLink::OnClicked()
{
    m_bOverControl = false;
    int result = (int)GotoURL(m_strURL, SW_SHOW);
    m_bVisited = (result > HINSTANCE_ERROR);
    if (!m_bVisited) {
        MessageBeep(MB_ICONEXCLAMATION);     // Unable to follow link
        ReportError(result);
    } else {
        SetVisited();                        // Repaint to show visited colour
    }
}

HBRUSH CHyperLink::CtlColor(CDC* pDC, UINT nCtlColor)
{
    ASSERT(nCtlColor == CTLCOLOR_STATIC);

    if (m_bOverControl)
      pDC->SetTextColor(m_crHoverColour);
    else if (m_bVisited)
      pDC->SetTextColor(m_crVisitedColour);
    else
      pDC->SetTextColor(m_crLinkColour);

    // transparent text.
    pDC->SetBkMode(TRANSPARENT);
    return (HBRUSH)GetStockObject(NULL_BRUSH);
}

void CHyperLink::OnMouseMove(UINT nFlags, CPoint point)
{
    CStatic::OnMouseMove(nFlags, point);

    if (m_bOverControl) { // Cursor is currently over control
        CRect rect;
        GetClientRect(rect);

        if (!rect.PtInRect(point)) {
            m_bOverControl = false;
            ReleaseCapture();
            RedrawWindow();
            return;
        }
    } else { // Cursor has just moved over control
        m_bOverControl = true;
        RedrawWindow();
        SetCapture();
    }
}

BOOL CHyperLink::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
    if (m_hLinkCursor) {
        ::SetCursor(m_hLinkCursor);
        return TRUE;
    }
    return FALSE;
}

void CHyperLink::OnKillFocus(CWnd* pNewWnd)
{
    CStatic::OnKillFocus(pNewWnd);

    m_bOverControl = false;
    Invalidate();
}

void CHyperLink::PreSubclassWindow()
{
    // We want to get mouse clicks via STN_CLICKED
    DWORD dwStyle = GetStyle();
    ::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

    // Set the URL as the window text
    if (m_strURL.IsEmpty())
        GetWindowText(m_strURL);

    // Check that the window text isn't empty. If it is, set it as the URL.
    CString strWndText;
    GetWindowText(strWndText);
    if (strWndText.IsEmpty()) {
        ASSERT(!m_strURL.IsEmpty());    // Window and URL both NULL
        SetWindowText(m_strURL);
    }

    // Create the font
    LOGFONT lf;
    GetFont()->GetLogFont(&lf);
    lf.lfUnderline = (BYTE) m_bUnderline;
    m_Font.CreateFontIndirect(&lf);
    SetFont(&m_Font);

    PositionWindow();        // Adjust size of window to fit URL if necessary
    SetDefaultCursor();      // Try and load up a "hand" cursor

    // Create the tooltip
    CRect rect;
    GetClientRect(rect);
    m_toolTip.Create(this);
    m_toolTip.AddTool(this, m_strURL, rect, TOOLTIP_ID);

    CStatic::PreSubclassWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CHyperLink operations

void CHyperLink::SetURL(CString strURL)
{
    m_strURL = strURL;

    if (::IsWindow(GetSafeHwnd())) {
        PositionWindow();
        m_toolTip.UpdateTipText(strURL, this, TOOLTIP_ID);
    }
}

CString CHyperLink::GetURL() const
{
    return m_strURL;
}

void CHyperLink::SetColours(COLORREF crLinkColour, COLORREF crVisitedColour,
                            COLORREF crHoverColour /* = -1 */)
{
    m_crLinkColour    = crLinkColour;
    m_crVisitedColour = crVisitedColour;

    if (crHoverColour == -1)
        m_crHoverColour = ::GetSysColor(COLOR_HIGHLIGHT);
    else
        m_crHoverColour = crHoverColour;

    if (::IsWindow(m_hWnd))
        Invalidate();
}

COLORREF CHyperLink::GetLinkColour() const
{
    return m_crLinkColour;
}

COLORREF CHyperLink::GetVisitedColour() const
{
    return m_crVisitedColour;
}

COLORREF CHyperLink::GetHoverColour() const
{
    return m_crHoverColour;
}

void CHyperLink::SetVisited(bool bVisited /* = true */)
{
    m_bVisited = bVisited;

    if (::IsWindow(GetSafeHwnd()))
        Invalidate();
}

bool CHyperLink::GetVisited() const
{
    return m_bVisited;
}

void CHyperLink::SetLinkCursor(HCURSOR hCursor)
{
    m_hLinkCursor = hCursor;
    if (m_hLinkCursor == NULL)
        SetDefaultCursor();
}

HCURSOR CHyperLink::GetLinkCursor() const
{
    return m_hLinkCursor;
}

void CHyperLink::SetUnderline(bool bUnderline /* = true */)
{
    m_bUnderline = bUnderline;

    if (::IsWindow(GetSafeHwnd())) {
        LOGFONT lf;
        GetFont()->GetLogFont(&lf);
        lf.lfUnderline = (BYTE) m_bUnderline;

        m_Font.DeleteObject();
        m_Font.CreateFontIndirect(&lf);
        SetFont(&m_Font);

        Invalidate();
    }
}

bool CHyperLink::GetUnderline() const
{
    return m_bUnderline;
}

void CHyperLink::SetAutoSize(bool bAutoSize /* = true */)
{
    m_bAdjustToFit = bAutoSize;

    if (::IsWindow(GetSafeHwnd()))
        PositionWindow();
}

bool CHyperLink::GetAutoSize() const
{
    return m_bAdjustToFit;
}


// Move and resize the window so that the window is the same size
// as the hyperlink text. This stops the hyperlink cursor being active
// when it is not directly over the text. If the text is left justified
// then the window is merely shrunk, but if it is centred or right
// justified then the window will have to be moved as well.
//
void CHyperLink::PositionWindow()
{
    if (!::IsWindow(GetSafeHwnd()) || !m_bAdjustToFit)
      return;

    // Get the current window position
    CRect rect;
    GetWindowRect(rect);

    CWnd* pParent = GetParent();
    if (pParent)
        pParent->ScreenToClient(rect);

    // Get the size of the window text
    CString strWndText;
    GetWindowText(strWndText);

    CDC* pDC = GetDC();
    CFont* pOldFont = pDC->SelectObject(&m_Font);
    CSize Extent = pDC->GetTextExtent(strWndText);
    pDC->SelectObject(pOldFont);
    ReleaseDC(pDC);

    // Get the text justification via the window style
    DWORD dwStyle = GetStyle();

    // Recalc the window size and position based on the text justification
    if (dwStyle & SS_CENTERIMAGE)
        rect.DeflateRect(0, (rect.Height() - Extent.cy)/2);
    else
        rect.bottom = rect.top + Extent.cy;

    if (dwStyle & SS_CENTER)
        rect.DeflateRect((rect.Width() - Extent.cx)/2, 0);
    else if (dwStyle & SS_RIGHT)
        rect.left  = rect.right - Extent.cx;
    else // SS_LEFT = 0, so we can't test for it explicitly
        rect.right = rect.left + Extent.cx;

    // Move the window
    SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
}

/////////////////////////////////////////////////////////////////////////////
// CHyperLink implementation

// The following appeared in Paul DiLascia's Jan 1998 MSJ articles.
// It loads a "hand" cursor from the winhlp32.exe module
void CHyperLink::SetDefaultCursor()
{
    if (m_hLinkCursor == NULL) {                // No cursor handle - load our own
        // Get the windows directory
        CString strWndDir;
        GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
        strWndDir.ReleaseBuffer();

        strWndDir += _T("\\winhlp32.exe");
        // This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
        HMODULE hModule = LoadLibrary(strWndDir);
        if (hModule) {
            HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
            if (hHandCursor)
                 m_hLinkCursor = CopyCursor(hHandCursor);
        }
        FreeLibrary(hModule);
    }
}

LONG CHyperLink::GetRegKey(HKEY key, LPCTSTR szSubkey, LPTSTR szRetData, int retStrLen)
{
    HKEY hKey;
    LONG lRetVal = RegOpenKeyEx(key, szSubkey, 0, KEY_QUERY_VALUE, &hKey);

    if (szRetData == ERROR_SUCCESS) {
        long nDataSize = MAX_PATH;
        TCHAR szData[MAX_PATH];
        RegQueryValue(hKey, NULL, szData, &nDataSize);
        CopyString(szRetData, szData, retStrLen);
        RegCloseKey(hKey);
    }

    return lRetVal;
}

void CHyperLink::ReportError(int nError)
{
   CString str;
   switch (nError) {
        case 0:                       str = "The operating system is out\nof memory or resources."; break;
        case SE_ERR_PNF:              str = "The specified path was not found."; break;
        case SE_ERR_FNF:              str = "The specified file was not found."; break;
        case ERROR_BAD_FORMAT:        str = "The .EXE file is invalid\n(non-Win32 .EXE or error in .EXE image)."; break;
        case SE_ERR_ACCESSDENIED:     str = "The operating system denied\naccess to the specified file."; break;
        case SE_ERR_ASSOCINCOMPLETE:  str = "The filename association is\nincomplete or invalid."; break;
        case SE_ERR_DDEBUSY:          str = "The DDE transaction could not\nbe completed because other DDE transactions\nwere being processed."; break;
        case SE_ERR_DDEFAIL:          str = "The DDE transaction failed."; break;
        case SE_ERR_DDETIMEOUT:       str = "The DDE transaction could not\nbe completed because the request timed out."; break;
        case SE_ERR_DLLNOTFOUND:      str = "The specified dynamic-link library was not found."; break;
        case SE_ERR_NOASSOC:          str = "There is no application associated\nwith the given filename extension."; break;
        case SE_ERR_OOM:              str = "There was not enough memory to complete the operation."; break;
        case SE_ERR_SHARE:            str = "A sharing violation occurred. ";
        default:                      str.Format("Unknown Error (%d) occurred.", nError); break;
   }
   str = "Unable to open hyperlink:\n\n" + str;
   AfxMessageBox(str, MB_ICONEXCLAMATION | MB_OK);
}

HINSTANCE CHyperLink::GotoURL(LPCTSTR szUrl, int nShowCmd)
{
   TCHAR key[MAX_PATH + MAX_PATH];

    // First try ShellExecute()
    HINSTANCE result = ShellExecute(NULL, _T("open"), szUrl, NULL,NULL, nShowCmd);

    // If it failed, get the .htm regkey and lookup the program
    if ((UINT)result <= HINSTANCE_ERROR)
    {
        if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key, MAX_PATH + MAX_PATH) == ERROR_SUCCESS)
        {
            lstrcat(key, _T("\\shell\\open\\command"));

            if (GetRegKey(HKEY_CLASSES_ROOT,key,key, MAX_PATH + MAX_PATH) == ERROR_SUCCESS)
            {
                 TCHAR *pos;
                 pos = _tcsstr(key, _T("\"%1\""));
                 if (pos == NULL) {                     // No quotes found
                      pos = strstr(key, _T("%1"));       // Check for %1, without quotes
                      if (pos == NULL)                   // No parameter at all...
                            pos = key+StringLength(key)-1;
                      else
                            *pos = '\0';                   // Remove the parameter
                 } else {
                      *pos = '\0';                       // Remove the parameter
                 }

                 lstrcat(pos, _T(" "));
                 lstrcat(pos, szUrl);
                 result = (HINSTANCE) WinExec(key,nShowCmd);
            }
        }
    }
    return result;
}

