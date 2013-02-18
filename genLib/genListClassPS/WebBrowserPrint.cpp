///////////////////////////////////////////////////////////////////////////////////
//
// WebBrowserPrint.cpp
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
//      Platform specific implementation of genListClass codes
//  adds Windows specific code for window creation and use of IE HTML renderer.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include "WebBrowserPrint.h"

const size_t BUFSIZE = 255;
static TCHAR g_szBuf[BUFSIZE];
static HHOOK g_hHook = NULL;
static HWND g_hWnd = NULL;
static bool g_bSuccess = false;
static CWebBrowserPrint* g_pWebBrowserPrint = NULL;
static void (CWebBrowserPrint::*g_pDoDlgExchangeFunc)(HWND hwnd) = NULL;

ULONG GetComboList(HWND hwnd, LPTSTR*& ppTextItems)
{
    if (!IsWindow(hwnd))
    {
        g_bSuccess = false;
        return 0;
    }

    //On Win9x, the list of printers is filled completely only when the combo box is dropped down
    //So send F4 keypresses to drop down and retract the combo box
    SendMessage(hwnd, WM_KEYDOWN, 115, 1);
    SendMessage(hwnd, WM_KEYUP, 115, 1);
    SendMessage(hwnd, WM_KEYDOWN, 115, 1);
    SendMessage(hwnd, WM_KEYUP, 115, 1);

    LONG_PTR count =  SendMessage(hwnd, CB_GETCOUNT, 0, 0);
    ppTextItems = new LPTSTR[count];  // allocate storage for count TSTR's

    for (LONG_PTR i = 0; i < count; ++i)
    {
        LONG_PTR textlen = SendMessage(hwnd, CB_GETLBTEXTLEN, i, 0) + 1;
        ppTextItems[i] = new TCHAR[textlen];
        SendMessage(hwnd, CB_GETLBTEXT, i, (WPARAM)ppTextItems[i]);
    }

    return (ULONG) count;
}

void DeleteStrings(ULONG& lCount, LPTSTR*& m_ppTextItems)
{
    if (lCount == 0) return;
    for (ULONG i = 0; i < lCount; ++i)
        delete [] m_ppTextItems[i];
    delete [] m_ppTextItems;
    lCount = 0;
}

CString GetEditText(HWND hwnd)
{
    CString sText;
    size_t textlen = GetWindowTextLength(hwnd) + 1;
    LPTSTR szText = new TCHAR[textlen];
    GetWindowText(hwnd, szText, (int) textlen);
    sText = szText;
    delete [] szText;
    return sText;
}

CString GetRegValue(LPCTSTR szValueName)
{
    CString sValue;
    HKEY hKey;
    ULONG r;
    DWORD dwType;
    DWORD dwData = BUFSIZE;
    g_szBuf[0] = TCHAR('\0');
    r = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\PageSetup"), 0, KEY_READ, &hKey);
    r = RegQueryValueEx(hKey, szValueName, 0, &dwType, (LPBYTE)g_szBuf, &dwData);
    r = RegCloseKey(hKey);
    sValue = g_szBuf;
    return sValue;
}
void RestoreRegValue(LPCTSTR szValueName, LPCTSTR szValue)
{
    HKEY hKey;
    ULONG r;
    DWORD dwData = StringLength(szValue) + 1;
    r = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\PageSetup"), 0, KEY_WRITE, &hKey);
    r = RegSetValueEx(hKey, szValueName, 0, REG_SZ, (LPBYTE)szValue, dwData);
    r = RegCloseKey(hKey);
}

CWebBrowserPrint::CWebBrowserPrint()
: m_pWebBrowser(NULL), m_lPrinterCount(0)
{
}

CWebBrowserPrint::~CWebBrowserPrint()
{
    if (m_pWebBrowser)
        m_pWebBrowser->Release();
    m_pWebBrowser = NULL;
    DeleteStrings(m_lPrinterCount, m_ppPrinterNames);
}

void CWebBrowserPrint::SetWebBrowser(IWebBrowser2* pWebBrowser)
{
    if (m_pWebBrowser)
    {
        m_pWebBrowser->Release();
        m_pWebBrowser = NULL;
    }

    if (pWebBrowser)
    {
        m_pWebBrowser = pWebBrowser;
        m_pWebBrowser->AddRef();
        m_bPrinting = false;
    }
}

void CWebBrowserPrint::ExchangeEditText(HWND hwnd, CString& sText)
{
    if (!IsWindow(hwnd))
    {
        g_bSuccess = false;
        return;
    }

    if (m_bPrinting)
        SetWindowText(hwnd, sText);
    else
    {
        size_t textlen = GetWindowTextLength(hwnd) + 1;
        LPTSTR szText = new TCHAR[textlen];
        GetWindowText(hwnd, szText, (int) textlen);
        sText = szText;
        delete [] szText;
    }
}

void CWebBrowserPrint::ExchangeButtonState(HWND hwnd, bool& bState)
{
    if (!IsWindow(hwnd))
    {
        g_bSuccess = false;
        return;
    }

    if (m_bPrinting)
    {
        bool bCurrentState = SendMessage(hwnd, BM_GETCHECK, 0, 0) ? true : false;
        if (bState != bCurrentState)
            SendMessage(hwnd, BM_CLICK, 0, 0);
    }
    else
        bState = SendMessage(hwnd, BM_GETCHECK, 0, 0) != FALSE ? true : false;
}

void CWebBrowserPrint::ExchangeEditNum(HWND hwnd, ULONG& iValue)
{
    if (!IsWindow(hwnd))
    {
        g_bSuccess = false;
        return;
    }

    if (m_bPrinting)
    {
        wsprintf(g_szBuf, _T("%d"), iValue);
        SetWindowText(hwnd, g_szBuf);
    }
    else
    {
        CString sText = GetEditText(hwnd);
        LPTSTR szStop = NULL;
        iValue = _tcstol(sText, &szStop, 10);
    }
}

void CWebBrowserPrint::ExchangeEditNum(HWND hwnd, float& fValue)
{
    if (!IsWindow(hwnd))
    {
        g_bSuccess = false;
        return;
    }

    if (m_bPrinting)
    {
        ULONG wp = (ULONG)fValue;
        ULONG fp = (ULONG)(1000 * (fValue - wp));
        wsprintf(g_szBuf, _T("%d.%03d"), wp, fp);
        SetWindowText(hwnd, g_szBuf);
    }
    else
    {
        CString sText = GetEditText(hwnd);
        LPTSTR szStop = NULL;
        fValue = (float)_tcstod(sText, &szStop);
    }
}

LRESULT CALLBACK CWebBrowserPrint::CbtProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    switch (nCode)
    {
        case HCBT_CREATEWND:
        {
            HWND hWnd = (HWND)wParam;
            LPCBT_CREATEWND pcbt = (LPCBT_CREATEWND)lParam;
            LPCREATESTRUCT pcs = pcbt->lpcs;
            if ((DWORD)pcs->lpszClass == 0x00008002)
            {
                g_hWnd = hWnd;          // Get hwnd of Page Setup dialog
                pcs->x = -2 * pcs->cx;  // Move dialog off screen
            }
            break;
        }
        case HCBT_ACTIVATE:
        {
            HWND hwnd = (HWND)wParam;
            if (hwnd == g_hWnd)
            {
                g_hWnd = NULL;
                (g_pWebBrowserPrint->*g_pDoDlgExchangeFunc)(hwnd);  // Call thru func ptr
                g_bSuccess = true;
            }
            break;
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

void CWebBrowserPrint::DoPageSetupDlgExchange(HWND hwnd)
{
    bool bPortrait = (m_Orientation == OrientationPortrait);
    bool bLandscape = (m_Orientation == OrientationLandscape);

    ExchangeEditText(GetDlgItem(hwnd, 0x0471), m_sPaperSize);
    ExchangeEditText(GetDlgItem(hwnd, 0x0472), m_sPaperSource);
    ExchangeEditText(GetDlgItem(hwnd, 0x1fd3), m_sHeader);
    ExchangeEditText(GetDlgItem(hwnd, 0x1fd5), m_sFooter);

    ExchangeButtonState(GetDlgItem(hwnd, 0x0420), bPortrait);
    ExchangeButtonState(GetDlgItem(hwnd, 0x0421), bLandscape);
    ExchangeEditNum(GetDlgItem(hwnd, 0x0483), m_fLeftMargin);
    ExchangeEditNum(GetDlgItem(hwnd, 0x0485), m_fRightMargin);
    ExchangeEditNum(GetDlgItem(hwnd, 0x0484), m_fTopMargin);
    ExchangeEditNum(GetDlgItem(hwnd, 0x0486), m_fBottomMargin);

    if (m_bPrinting)
        SendMessage(GetDlgItem(hwnd, 0x0001), BM_CLICK, 0, 0);  // Invoke OK button
    else
        SendMessage(GetDlgItem(hwnd, 0x0002), BM_CLICK, 0, 0);  // Invoke Cancel button

    if (bPortrait)
        m_Orientation = OrientationPortrait;
    else if (bLandscape)
        m_Orientation = OrientationLandscape;
    else
        m_Orientation = OrientationUndefined;
}

void CWebBrowserPrint::DoDisableHeaderFooter(HWND hwnd)
{
    EnableWindow(GetDlgItem(hwnd, 0x1fd3), false);
    EnableWindow(GetDlgItem(hwnd, 0x1fd5), false);
}

bool CWebBrowserPrint::InvokeDlg(OLECMDID cmdid, void (CWebBrowserPrint::*pDoDlgExchangeFunc)(HWND hwnd))
{
    if (!m_pWebBrowser)
        return false;
    if (g_hHook)
        return false;
    g_hHook = SetWindowsHookEx(WH_CBT, CbtProc, NULL, GetCurrentThreadId());
    if (!g_hHook)
        return false;
    g_bSuccess = false;
    g_pWebBrowserPrint = this;
    g_pDoDlgExchangeFunc = pDoDlgExchangeFunc;
    HRESULT hr = m_pWebBrowser->ExecWB(cmdid, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
    UnhookWindowsHookEx(g_hHook);
    g_pWebBrowserPrint = NULL;
    g_hHook = NULL;
    return g_bSuccess;
}

bool CWebBrowserPrint::ReadDlgSettings()
{
    DeleteStrings(m_lPrinterCount, m_ppPrinterNames);
    if (!InvokeDlg(OLECMDID_PAGESETUP, &CWebBrowserPrint::DoPageSetupDlgExchange))
        return false;
    return true;
}

bool CWebBrowserPrint::Print(bool bPreview)
{
    CString sHeader = GetRegValue(_T("header"));
    CString sFooter = GetRegValue(_T("footer"));
    CString sLeftMargin = GetRegValue(_T("margin_left"));
    CString sRightMargin = GetRegValue(_T("margin_right"));
    CString sTopMargin = GetRegValue(_T("margin_top"));
    CString sBottomMargin = GetRegValue(_T("margin_bottom"));

    bool bSuccess = false;
    m_bPrinting = true;
    if (InvokeDlg(OLECMDID_PAGESETUP, &CWebBrowserPrint::DoPageSetupDlgExchange)) {
        if (bPreview)
            m_pWebBrowser->ExecWB(OLECMDID_PRINTPREVIEW, OLECMDEXECOPT_PROMPTUSER, NULL, NULL);
        else
            m_pWebBrowser->ExecWB(OLECMDID_PRINT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);
        bSuccess = true;
    }
    m_bPrinting = false;

    RestoreRegValue(_T("header"), sHeader);
    RestoreRegValue(_T("footer"), sFooter);
    RestoreRegValue(_T("margin_left"), sLeftMargin);
    RestoreRegValue(_T("margin_right"), sRightMargin);
    RestoreRegValue(_T("margin_top"), sTopMargin);
    RestoreRegValue(_T("margin_bottom"), sBottomMargin);

    return bSuccess;
}

CString CWebBrowserPrint::GetPrinterName(ULONG lIndex)
{
    CString sPrinterName;
    if (lIndex < m_lPrinterCount)
        sPrinterName = m_ppPrinterNames[lIndex];
    return sPrinterName;
}

CString CWebBrowserPrint::GetDefaultPrinterName()
{
    CString sPrinterName;

    GetProfileString(_T("windows"), _T("device"), _T(",,,"), g_szBuf, BUFSIZE);
    LPTSTR pch = _tcsstr(g_szBuf, _T(","));
    if (pch) {
        *pch = _T('\0');
        sPrinterName = g_szBuf;
    }

    return sPrinterName;
}

void CWebBrowserPrint::PrintSetup()
{
    InvokeDlg(OLECMDID_PAGESETUP, &CWebBrowserPrint::DoDisableHeaderFooter);
}

