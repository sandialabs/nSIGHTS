///////////////////////////////////////////////////////////////////////////////////
//
// WebBrowserPrint.h
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

// WebBrowserPrint.h: interface for the CWebBrowserPrint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEBBROWSERPRINT_H__DA19EEF7_7132_11D3_BB8B_00C04FA3471C__INCLUDED_)
#define AFX_WEBBROWSERPRINT_H__DA19EEF7_7132_11D3_BB8B_00C04FA3471C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWebBrowserPrint
{
public:
    enum Orientation {
        OrientationUndefined,
        OrientationPortrait,
        OrientationLandscape
    };
    enum PrintRange {
        PrintRangeUndefined,
        PrintRangeAll,
        PrintRangePages,
        PrintRangeSelection
    };
    enum PrintFrames {
        PrintFramesUndefined,
        PrintFramesScreen,
        PrintFramesSelected,
        PrintFramesIndividually
    };

    CWebBrowserPrint();
    virtual ~CWebBrowserPrint();

    void SetWebBrowser(IWebBrowser2* pWebBrowser);
    bool Print(bool bPreview = false);
    bool ReadDlgSettings();
    CString GetPrinterName(ULONG lIndex);
    ULONG GetPrinterCount() { return m_lPrinterCount; }
    CString GetDefaultPrinterName();

    void PrintSetup(); // disabled header/footer

    // Page Setup dialog settings
    CString m_sPaperSize;
    CString m_sPaperSource;
    CString m_sHeader;
    CString m_sFooter;
    Orientation m_Orientation;
    float m_fLeftMargin;
    float m_fTopMargin;
    float m_fRightMargin;
    float m_fBottomMargin;

private:
    void ExchangeEditText(HWND hwnd, CString& sText);
    void ExchangeButtonState(HWND hwnd, bool& bState);
    void ExchangeEditNum(HWND hwnd, float& fValue);
    void ExchangeEditNum(HWND hwnd, ULONG& lValue);
    void DoPageSetupDlgExchange(HWND hWnd);
    void DoDisableHeaderFooter(HWND hWnd);

    bool InvokeDlg(OLECMDID cmdid, void (CWebBrowserPrint::*pDoDlgExchangeFunc)(HWND hwnd));
    static LRESULT CALLBACK CbtProc(int nCode, WPARAM wParam, LPARAM lParam);

    IWebBrowser2* m_pWebBrowser;
    ULONG m_lPrinterCount;
    LPTSTR* m_ppPrinterNames;
    bool m_bPrinting;
};

#endif // !defined(AFX_WEBBROWSERPRINT_H__DA19EEF7_7132_11D3_BB8B_00C04FA3471C__INCLUDED_)

