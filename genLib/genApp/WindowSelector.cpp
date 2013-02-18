///////////////////////////////////////////////////////////////////////////////////
//
// WindowSelector.cpp
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
// DESCRIPTION:  Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//              Implements WindowSelector UI control
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "GenLibRes.h"

#include "WindowSelector.h"

#include <genClass/U_Str.h>

#include <AfxPriv.h> // AfxLoadString, ...

#include <genMFC/DllLoader.h>
#include <genApp/ProjectUtil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int ID_SELECTION_LIST = 1;

class Win2kUser32Dll : public DllLoad
{
public:
    void Load();

    void SetLayeredWindowAttributes(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

private:
    static char szDllName[];
    static char szFunctionName[];
};

char Win2kUser32Dll::szDllName[] = "User32.Dll";
char Win2kUser32Dll::szFunctionName[] = "SetLayeredWindowAttributes";

Win2kUser32Dll w2KDll;

void Win2kUser32Dll::Load()
{
    LoadDll(szDllName);
}

#ifndef WS_EX_LAYERED

#define WS_EX_LAYERED    0x00080000
#define LWA_COLORKEY     0x00000001
#define LWA_ALPHA        0x00000002

extern "C" {
    WINUSERAPI BOOL WINAPI SetLayeredWindowAttributes(HWND hwnd,
                                                      COLORREF crKey,
                                                      BYTE bAlpha,
                                                      DWORD dwFlags);
}

#endif // WS_EX_LAYERED

typedef WINUSERAPI BOOL (WINAPI* SetLayeredWindowAttributesProc)(HWND hwnd,
                                                                 COLORREF crKey,
                                                                 BYTE bAlpha,
                                                                 DWORD dwFlags);

void Win2kUser32Dll::SetLayeredWindowAttributes(HWND hWnd,
                                                COLORREF crKey,
                                                BYTE bAlpha,
                                                DWORD dwFlags)
{
    SetLayeredWindowAttributesProc pfn = SetLayeredWindowAttributesProc(DllProcLoad(szFunctionName));
    (*pfn)(hWnd, crKey, bAlpha, dwFlags);
}

/////////////////////////////////////////////////////////////////////////////
// SelectorList

BEGIN_MESSAGE_MAP(SelectorList, CListCtrl)
//{{AFX_MSG_MAP(SelectorList)
ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void SelectorList::OnSize(UINT nType, int cx, int cy)
{
    if (cx > 0 && cy > 0 && ::IsWindow(m_hWnd)) {
        LVCOLUMN column;
        column.mask = LVCF_WIDTH;
        column.cx = cx;
        SetColumn(0, &column);
    }
    CListCtrl::OnSize(nType, cx, cy);
}

/////////////////////////////////////////////////////////////////////////////
// WindowSelector

WindowSelector::WindowSelector() :
    m_bInit(false),
    m_bEnable(false),
    m_bOwnerCreated(false),
    m_bNotify(true),
    m_bProcessingCallback(false),
    m_bButtonPinned(false),
    m_cbSelect(NULL),
    m_minPlotsCallback(NULL),
    m_tilePlotsCallback(NULL)
{
}

WindowSelector::~WindowSelector()
{
}

bool WindowSelector::IsEnabled()
{
    return m_bInit && m_bEnable;
}

bool WindowSelector::Switch()
{
    m_bEnable = !m_bEnable;
    ShowWindow(m_bEnable ? SW_SHOW : SW_HIDE);
    return m_bEnable;
}

bool WindowSelector::IsPinned()
{
    return m_bButtonPinned;
}

BEGIN_MESSAGE_MAP(WindowSelector, CWnd)
//{{AFX_MSG_MAP(WindowSelector)
ON_WM_SIZE()
ON_WM_CLOSE()
ON_NOTIFY(LVN_ITEMCHANGED, ID_SELECTION_LIST, OnItemchanged)
ON_WM_NCLBUTTONDOWN()
ON_WM_NCACTIVATE()
ON_WM_NCHITTEST()
ON_WM_NCPAINT()
//}}AFX_MSG_MAP
ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)

ON_COMMAND(ID_MINIMIZE_ALL_PLOTS, OnMinimizeAllPlots)
ON_COMMAND(ID_TILE_ALL_PLOTS_HORZ, OnTileAllPlotsHorz)
ON_COMMAND(ID_TILE_ALL_PLOTS_VERT, OnTileAllPlotsVert)
ON_COMMAND(ID_CASCADE_ALL_PLOTS, OnCascadeAllPlots)
ON_COMMAND(ID_TOGGLE_TRANSPARENCY, OnToggleTransparency)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// WindowSelector message handlers

void WindowSelector::OnSize(UINT nType, int cx, int cy)
{
    if (cx > 0 && cy > 0 && ::IsWindow(m_list.m_hWnd)) {
        m_list.MoveWindow(0, 16, cx, cy);
    }
}

void WindowSelector::OnClose()
{
    Switch();
}

void WindowSelector::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    ASSERT(!m_bProcessingCallback);
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    if (pNMListView->uNewState & LVIS_SELECTED) {
        if (!(pNMListView->uOldState & LVIS_SELECTED)) {
            if (m_bNotify && m_cbSelect != NULL) {
                m_bProcessingCallback = true;
                try {
                    // SelectItem(int nItem) can be called from callback, we want to avoid its processing
                    (*m_cbSelect)(pNMListView->iItem-1);

                } catch(...) {
                    m_bProcessingCallback = false;
                    throw;
                }
                m_bProcessingCallback = false;
            }
        }
    }

    *pResult = 0;
}

bool WindowSelector::IsProcessingSelectionCallback()
{
    return m_bProcessingCallback;
}

BOOL WindowSelector::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CWnd::PreCreateWindow(cs)) {
        return FALSE;
    }
    // don't want to be in task bar
    if (!m_bOwnerCreated) {
        CRect rect;
        LPCTSTR lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,
                                                ::LoadCursor(NULL, IDC_ARROW),
                                                HBRUSH(COLOR_BTNFACE+1),
                                                NULL);
        m_bOwnerCreated = m_wndOwnerWindow.CreateEx(0,
                                                    lpszClass,
                                                    "Hidden Owner",
                                                    WS_POPUP,
                                                    rect,
                                                    NULL,
                                                    0) != FALSE;
        if (m_bOwnerCreated) {
            m_wndOwnerWindow.ShowWindow(SW_HIDE);
        }
    }

    // set the hidden window as a parent of frame window
    if (m_bOwnerCreated) {
        cs.hwndParent = m_wndOwnerWindow.GetSafeHwnd();
    }

    return TRUE;
}

void WindowSelector::Init( SelectCallback cbSelect,
                           PlotsCallback minPlotsCallback,
                           TilePlotsCallback tilePlotsCallback)
{
    m_cbSelect = cbSelect;
    m_minPlotsCallback = minPlotsCallback;
    m_tilePlotsCallback = tilePlotsCallback;
    VERIFY(m_bmpPinned.LoadBitmap(IDB_PP_PINNED));
    VERIFY(m_bmpUnpinned.LoadBitmap(IDB_PP_UNPINNED));

    LPCTSTR lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,
                                            ::LoadCursor(NULL, IDC_ARROW),
                                            HBRUSH(COLOR_BTNFACE+1),
                                            NULL);
    int xScreen = ::GetSystemMetrics(SM_CXFULLSCREEN);
    int yScreen = ::GetSystemMetrics(SM_CYFULLSCREEN);
    CRect rect(xScreen-120, 0, xScreen, 220);

    VERIFY(CWnd::CreateEx(WS_EX_PALETTEWINDOW|WS_EX_TOOLWINDOW,
                          lpszClass,
                          "Window List",
                          WS_POPUPWINDOW|WS_CAPTION|WS_THICKFRAME,
                          rect,
                          NULL,
                          0));

    if (!m_wndToolBar.Create(this)
        //if (!m_wndToolBar.CreateEx(this)
        || !m_wndToolBar.LoadToolBar(IDR_WINDOW_SELECTOR_TOOLBAR))
        {
            TRACE0("Failed to create toolbar\n");
        }
    m_wndToolBar.SetHeight(10);

    m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
                             CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC );
    if (projectUtilities::IsWindows2000()) {
        w2KDll.Load();
        if (!w2KDll.IsLoaded()) {
            m_wndToolBar.GetToolBarCtrl().HideButton(ID_TOGGLE_TRANSPARENCY, true);
        } else {
            if (projectUtilities::GetTransparent()) {
                ToggleTransparency(projectUtilities::GetAlpha());
            }
        }
    } else {
        m_wndToolBar.GetToolBarCtrl().HideButton(ID_TOGGLE_TRANSPARENCY, true);
    }

    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

    VERIFY(m_list.Create(WS_CHILD|WS_VISIBLE|WS_BORDER
                         |LVS_REPORT|LVS_NOCOLUMNHEADER|LVS_SHOWSELALWAYS|LVS_SINGLESEL,
                         rect,
                         this,
                         ID_SELECTION_LIST));
    VERIFY(m_list.InsertColumn(0, "", LVCFMT_LEFT, rect.Width()-4) != -1);
    Redraw();
    m_bInit = true;
}

void WindowSelector::RemoveAll()
{
    m_list.DeleteAllItems();
}

void WindowSelector::AddItem(int nItem, const CString& sMenu)
{
    m_list.InsertItem(nItem, LPCSTR(sMenu));
}

void WindowSelector::Redraw()
{
    CRect rect;
    GetClientRect(rect);
    rect.top += 16;
    m_list.MoveWindow(rect);
}

void WindowSelector::SelectItem(int nItem)
{
    m_bNotify = false;
    try {
        if (nItem == -1) {
            for (int i = 0; i < m_list.GetItemCount(); i++) {
                VERIFY(m_list.SetItemState(i, 0, LVIS_SELECTED));
            }
        } else {
            VERIFY(m_list.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED));
        }
    } catch(...) {
        m_bNotify = true;
        throw;
    }
    m_bNotify = true;
}

int WindowSelector::GetSelectedID()
{
    for (int i = 0; i < m_list.GetItemCount(); i++) {
        if (m_list.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED) {
            return i;
        }
    }
    return -1;
}

void WindowSelector::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
    CPoint pt = point;
    ScreenToClient(&pt);
    pt.y = -pt.y;
    if (m_rcButton.PtInRect(pt)) {
        m_bButtonPinned = !m_bButtonPinned;
        SendMessage(WM_NCPAINT);
        if (m_bButtonPinned) {
            windowSelector.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
        } else {
            windowSelector.SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
        }
        return;
    }
    CWnd::OnNcLButtonDown(nHitTest, point);
}

BOOL WindowSelector::OnNcActivate(BOOL bActive)
{
    BOOL bRc = CWnd::OnNcActivate(bActive);
    SendMessage(WM_NCPAINT);
    return bRc;
}

NCHITRETVAL  WindowSelector::OnNcHitTest(CPoint point)
{
    NCHITRETVAL ht = CWnd::OnNcHitTest(point);
    if (ht == long(HTCAPTION)) {
        ScreenToClient(&point);
        SendMessage(WM_NCPAINT);
    }
    return ht;

}

void WindowSelector::OnNcPaint()
{
    CWnd::OnNcPaint();

    CRect rc;
    GetWindowRect(rc);

    rc.right = rc.Width() - (::GetSystemMetrics(SM_CXSMSIZE) + ::GetSystemMetrics(SM_CXFRAME) + ::GetSystemMetrics(SM_CXBORDER));
    rc.right -= 4;
    rc.left =  rc.right - ::GetSystemMetrics(SM_CXSMSIZE) - 6;

    rc.top = ::GetSystemMetrics(SM_CYFRAME);
    rc.bottom = rc.top + ::GetSystemMetrics(SM_CYSMSIZE);
    CDC* pDC = GetWindowDC();

    CDC dcMem;
    dcMem.CreateCompatibleDC(pDC);
    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
    CBitmap* pOldBitmap = dcMem.SelectObject(&bitmap);

    CRect rect(0, 0, rc.Width(), rc.Height());
    if (m_bButtonPinned) {
        dcMem.DrawFrameControl(rect, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_CHECKED);
        dcMem.DrawState(CPoint(1, 1), CSize(rect.Width()-2, rect.Height()-2),
                        &m_bmpPinned, DST_BITMAP);
    } else {
        dcMem.DrawFrameControl(rect, DFC_BUTTON, DFCS_BUTTONPUSH);
        dcMem.DrawState(CPoint(1, 1), CSize(rect.Width()-2, rect.Height()-2),
                        &m_bmpUnpinned, DST_BITMAP);
    }

    pDC->BitBlt(rc.left, rc.top, rc.Width(), rc.Height(),
                &dcMem, 0, 0, SRCCOPY);

    dcMem.SelectObject(pOldBitmap);
    ReleaseDC(&dcMem);
    ReleaseDC(pDC);

    m_rcButton = rc;
}

// Handles TTN_NEEDTEXT message to display tooltips for the toolbar.
// This code is based on CFrameWnd::OnToolTipText
BOOL WindowSelector::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
    ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

    // allow top level routing frame to handle the message
    if (GetRoutingFrame() != NULL)
        return FALSE;

    // need to handle both ANSI and UNICODE versions of the message
    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
    TCHAR szFullText[256];
    CString strTipText;
    UINT nID = (UINT) pNMHDR->idFrom;
    if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
        pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
        {
            // idFrom is actually the HWND of the tool
            nID = ((UINT)(WORD)::GetDlgCtrlID((HWND)nID));
        }

    if (nID != 0) // will be zero on a separator
        {
            AfxLoadString(nID, szFullText);
            // this is the command id, not the button index
            AfxExtractSubString(strTipText, szFullText, 1, '\n');
        }
#ifndef _UNICODE
    if (pNMHDR->code == TTN_NEEDTEXTA)

        //              lstrcpyn(pTTTA->szText, strTipText,
        //            (sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
        CopyString(pTTTA->szText, strTipText, 80);
    else
        _mbstowcsz(pTTTW->szText, strTipText,
                   (sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
#else
    if (pNMHDR->code == TTN_NEEDTEXTA)
        _wcstombsz(pTTTA->szText, strTipText,
                   (sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
    else
        CopyString(pTTTA->szText, strTipText, 80);

    //              lstrcpyn(pTTTW->szText, strTipText,
    //            (sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
#endif
    *pResult = 0;

    // bring the tooltip window above other popup windows
    ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
                   SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

    return TRUE;    // message was handled
}

void WindowSelector::OnMinimizeAllPlots()
{
    if (m_minPlotsCallback != NULL) {
        (*m_minPlotsCallback)();
        SelectItem(-1);
    }
}

void WindowSelector::OnTileAllPlotsHorz()
{
    if (m_tilePlotsCallback != NULL) {
        (*m_tilePlotsCallback)(0);
        SelectItem(-1);
    }
}

void WindowSelector::OnTileAllPlotsVert()
{
    if (m_tilePlotsCallback != NULL) {
        (*m_tilePlotsCallback)(1);
        SelectItem(-1);
    }
}

void WindowSelector::OnCascadeAllPlots()
{
    if (m_tilePlotsCallback != NULL) {
        (*m_tilePlotsCallback)(2);
        SelectItem(-1);
    }
}

void WindowSelector::EnableTileCascade(bool bEnable)
{
    UINT nStyle = m_wndToolBar.GetButtonStyle(1);
    if (bEnable) {
        nStyle &= ~TBBS_DISABLED;
    } else {
        nStyle |= TBBS_DISABLED;
    }
    m_wndToolBar.SetButtonStyle(1, nStyle);
    m_wndToolBar.SetButtonStyle(2, nStyle);
    m_wndToolBar.SetButtonStyle(3, nStyle); // cascade
}

static void ToggleTransparency(HWND hWnd, BYTE nAlpha)
{
    ::SetWindowLong(hWnd, GWL_EXSTYLE,
                    ::GetWindowLong(hWnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
    w2KDll.SetLayeredWindowAttributes(hWnd, RGB(0,0,0), nAlpha, LWA_ALPHA);
}

void WindowSelector::OnToggleTransparency()
{
    projectUtilities::SetTransparent(!projectUtilities::GetTransparent());
    BYTE nAlpha = projectUtilities::GetAlpha();
    ToggleTransparency(nAlpha);
}

void WindowSelector::SetAlpha(int nAlpha)
{
    if (projectUtilities::GetTransparent()) {
        ShowWindow(false);
        // switch off
        ToggleTransparency(nAlpha);
        // switch in
        ToggleTransparency(nAlpha);
        ShowWindow(true);
    }
}

void WindowSelector::ToggleTransparency(int nAlpha)
{
    //w2KDll.ToggleTransparency(m_hWnd, nAlpha);
    ::ToggleTransparency(m_hWnd, nAlpha);
}

