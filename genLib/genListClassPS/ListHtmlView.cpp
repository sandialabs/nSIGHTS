///////////////////////////////////////////////////////////////////////////////////
//
// ListHtmlView.cpp
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
#include "GenLibRes.h"

#include "ListHtmlView.h"
#include <genUIExt/FileDialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListingHtmlView

IMPLEMENT_DYNCREATE(CListingHtmlView, CHtmlView)

BEGIN_MESSAGE_MAP(CListingHtmlView, CHtmlView)
    //{{AFX_MSG_MAP(CListingHtmlView)
    ON_COMMAND(ID_VIEW_FONTS_LARGEST, OnViewFontsLargest)
    ON_COMMAND(ID_VIEW_FONTS_LARGE, OnViewFontsLarge)
    ON_COMMAND(ID_VIEW_FONTS_MEDIUM, OnViewFontsMedium)
    ON_COMMAND(ID_VIEW_FONTS_SMALL, OnViewFontsSmall)
    ON_COMMAND(ID_VIEW_FONTS_SMALLEST, OnViewFontsSmallest)
    ON_COMMAND(ID_VIEW_STOP, OnViewStop)
    ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
    ON_COMMAND(ID_VIEW_PRINTPREVIEW, OnPrintPreview)
    ON_COMMAND(ID_LIST_PRINT, OnPrint)
    ON_COMMAND(ID_LIST_PRINTSETUP, OnPrintSetup)
    //ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListingHtmlView construction/destruction

CListingHtmlView::CListingHtmlView() :
    m_bDocumentComplete(false)
{
}

CListingHtmlView::~CListingHtmlView()
{
}

BOOL CListingHtmlView::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.lpszClass = AfxRegisterWndClass(0);
    return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CListingHtmlView drawing

void CListingHtmlView::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CListingHtmlView diagnostics

#ifdef _DEBUG
void CListingHtmlView::AssertValid() const
{
    CView::AssertValid();
}

void CListingHtmlView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CListingHtmlView message handlers

void CListingHtmlView::OnViewFontsLargest()
{
    COleVariant vaZoomFactor(4l);

    ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER,
           &vaZoomFactor, NULL);
}

void CListingHtmlView::OnViewFontsLarge()
{
    COleVariant vaZoomFactor(3l);

    ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER,
           &vaZoomFactor, NULL);
}

void CListingHtmlView::OnViewFontsMedium()
{
    COleVariant vaZoomFactor(2l);

    ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER,
           &vaZoomFactor, NULL);
}

void CListingHtmlView::OnViewFontsSmall()
{
    COleVariant vaZoomFactor(1l);

    ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER,
           &vaZoomFactor, NULL);
}

void CListingHtmlView::OnViewFontsSmallest()
{
    COleVariant vaZoomFactor(0l);

    ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER,
           &vaZoomFactor, NULL);
}

// these functions control the font size.  There is no explicit command in the
// CHtmlView class to do this, but we can do it by using the ExecWB() function.

// This demonstrates how we can use the Navigate2() function to load local files
// including local HTML pages, GIFs, AIFF files, etc.
void CListingHtmlView::OnFileOpen()
{
    CString str = "HTML Files|*.htm;*.html|Text Files|*.txt|GIF Files|*.gif|JPEG Files|*.jpg;*.jpeg|AU Files|*.au|AIFF Files|*.aif;*.aiff|XBM Files|*.xbm|All Files|*.*||";

    NTCompatibleFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, str);

    if (fileDlg.DoModal() == IDOK)
        Navigate2(fileDlg.GetPathName(), 0, NULL);
}

void CListingHtmlView::OnViewStop()
{
    Stop();
}

void CListingHtmlView::OnViewRefresh()
{
    Refresh();
}

void CListingHtmlView::OnTitleChange(LPCTSTR lpszText)
{
    // this will change the main frame's title bar
    if (m_pDocument != NULL)
        m_pDocument->SetTitle(lpszText);
}

void CListingHtmlView::OnInitialUpdate()
{
    CHtmlView::OnInitialUpdate();

    //TEST
    // Navigate2("http://www.codeguru.com", 0, NULL);
}

void CListingHtmlView::OnBeforeNavigate2(LPCTSTR /*lpszURL*/, DWORD /*nFlags*/,
    LPCTSTR /*lpszTargetFrameName*/, CByteArray& /*baPostedData*/,
    LPCTSTR /*lpszHeaders*/, BOOL* /*pbCancel*/)
{
}

IWebBrowser2* CListingHtmlView::GetBrowserApp()
{
    return m_pBrowserApp;
}

LPDISPATCH CListingHtmlView::GetDocumentX()
{
    LPDISPATCH result;
    m_wndBrowser.InvokeHelper(0xcb, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
    return result;
}

void CListingHtmlView::OnDocumentComplete(LPCTSTR lpszURL)
{
    CHtmlView::OnDocumentComplete(lpszURL);
    m_bDocumentComplete = true;
}

void CListingHtmlView::OnPrintPreview()
{
    //ExecWB(OLECMDID_PRINTPREVIEW, OLECMDEXECOPT_PROMPTUSER, NULL, NULL);

    Print(m_sHeader, m_sFooter, true);
}

void CListingHtmlView::OnPrint()
{
    //ExecWB(OLECMDID_PRINT, OLECMDEXECOPT_DODEFAULT, NULL, NULL);

    Print(m_sHeader, m_sFooter, false);
}

void CListingHtmlView::OnPrintSetup()
{
    m_wbprint.PrintSetup();
}

// This prints header and footer just to the printer,
// it does not work for preview mode
// TEST:    Print(CString("My header"), CString("My new footer"));
void CListingHtmlView::Print_AtlernateMethod(CString& sHeader, CString& sFooter)
{
    HRESULT hr = S_OK;

    // Variables needed to send IStream header to print operation.
    HGLOBAL hG = 0;
    IStream *pStream= NULL;
    IUnknown *pUnk = NULL;
    ULONG lWrote = 0;
    LPSTR sMem = NULL;

        // Initialize header and footer parameters to send to ExecWB().
    SAFEARRAYBOUND psabBounds[1];
    psabBounds[0].lLbound = 0;
    psabBounds[0].cElements = 2;
    SAFEARRAY* psaHeadFoot = SafeArrayCreate(VT_VARIANT, 1, psabBounds);
    if (psaHeadFoot == NULL) {
        return;
    }

    VARIANT vHeadStr, vFootStr;
    long rgIndices;
    VariantInit(&vHeadStr);
    VariantInit(&vFootStr);
    try {

        // Argument 1: Header
        vHeadStr.vt = VT_BSTR;
        vHeadStr.bstrVal = sHeader.AllocSysString();
        if (vHeadStr.bstrVal == NULL) {
            throw 2;
        }

        // Argument 2: Footer
        vFootStr.vt = VT_BSTR;
        vFootStr.bstrVal = sFooter.AllocSysString();
        if (vFootStr.bstrVal == NULL) {
            throw 3;
        }

        rgIndices = 0;
        SafeArrayPutElement(psaHeadFoot, &rgIndices, static_cast<void *>(&vHeadStr));
        rgIndices = 1;
        SafeArrayPutElement(psaHeadFoot, &rgIndices, static_cast<void *>(&vFootStr));

        // SAFEARRAY must be passed ByRef or else MSHTML transforms it into NULL.
        VARIANT vArg;
        VariantInit(&vArg);
        vArg.vt = VT_ARRAY | VT_BYREF;
        vArg.parray = psaHeadFoot;

        ExecWB(OLECMDID_PRINT, OLECMDEXECOPT_DONTPROMPTUSER, &vArg, NULL);

        // The following line unfortunatelly does not work
        //ExecWB(OLECMDID_PRINTPREVIEW, OLECMDEXECOPT_DONTPROMPTUSER, &vArg, NULL);

        return; // WebBrowser control will clean up the SAFEARRAY after printing.

    } catch (int) {
        VariantClear(&vHeadStr);
        VariantClear(&vFootStr);
        if (psaHeadFoot) {
            SafeArrayDestroy(psaHeadFoot);
        }
        if (sMem) {
            CoTaskMemFree(sMem);
        }
        if (hG != NULL) {
            GlobalFree(hG);
        }
        if (pStream != NULL) {
            pStream->Release();
            pStream = NULL;
        }
    }
}


void CListingHtmlView::InitPrint()
{
    m_wbprint.SetWebBrowser(m_pBrowserApp);
}

void CListingHtmlView::Print(CString& sHeader, CString& sFooter, bool bPreview)
{
    if (!m_wbprint.ReadDlgSettings()) {
        ASSERT(0);
        return;
    }

    m_wbprint.m_sHeader = sHeader;
    m_wbprint.m_sFooter = sFooter;

    m_wbprint.Print(bPreview);
}

