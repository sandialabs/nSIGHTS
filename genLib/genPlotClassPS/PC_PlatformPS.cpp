///////////////////////////////////////////////////////////////////////////////////
//
// PC_PlatformPS.cpp
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
//      Windows specific implementation of PC_Platform.
//  Creates top level  window, manages mouse, menu bar, 
//  status bar and tool bar.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include <genClass/U_Real.h>
#include <genClass/U_Str.h>

#include <genClassPS/PS_UIBase.h>
#include <genPlotClassPS/PC_PlatformPS.h>

#include <genClassPS/PS_Color.h>
#include <genClassPS/PS_FormContainer.h>
#ifdef TGAIMAGE
#include <genClassPS/PS_WriteTGA.h>
#endif // TGAIMAGE
#include <genClassPS/PS_WriteBMP.h>
#include <genUIExt/TitlePopupMenu.h>
#include <genUIExt/FileDialog.h>

#include <genClass/U_Msg.h>

#include <genPlotClassPS/PS_PSOutputSetup.h>
#include <genPlotClassPS/PS_TGAOutputSetup.h>

#include <genApp/ProjectUtil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////
CWndLayout::CWndLayout()
{
}

CWndLayout::~CWndLayout()
{
}

void CWndLayout::InitMenu(FormManager& form)
{
    //form.Create(GetWindow(), WS_DLGFRAME);
    form.Create(GetWindow());
    SetForm(&form);
}

void CWndLayout::MenuOpen()
{
    ResetForm();
}

CWnd* CWndLayout::GetWindow()
{
    return this;
}

BEGIN_MESSAGE_MAP(CWndLayout, CWnd)
    //{{AFX_MSG_MAP(CWndLayout)
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CWndLayout::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (cy > 0 && cx > 0) {
        FormContainer::Resize(0, 0, cx, cy);
    }
}

//////////////////////////////////////////
CWndPlot::CWndPlot(PlotDefC& basePlotDef) :
    basePlotDef(basePlotDef)
{
}

CWndPlot::~CWndPlot()
{
}

BOOL CWndPlot::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CWnd::PreCreateWindow(cs))
        return FALSE;

    cs.dwExStyle |= WS_EX_CLIENTEDGE;
    cs.style &= ~WS_BORDER;

    return TRUE;
}

BEGIN_MESSAGE_MAP(CWndPlot, CWnd)
    //{{AFX_MSG_MAP(CWndPlot)
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CWndPlot::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (cy > 0 && cx > 0) {
        // resize overlay window
        CWnd* pWnd = GetWindow(GW_CHILD);
        if (pWnd != NULL) {
            pWnd->MoveWindow(0, 0, cx, cy);
            // make sure we have just one window
            ASSERT(pWnd->GetWindow(GW_HWNDNEXT) == NULL);
        }
    }
}


CControlBarUpdate::CControlBarUpdate(PC_PlatformPS& platform) :
    m_platform(platform)
{
    isFixedAndVisible = true;
}


bool CControlBarUpdate::IsFixedAndVisible(CControlBar& inWnd)
{
    return inWnd.IsVisible() && (!inWnd.IsFloating());
}

void CControlBarUpdate::CheckStatusChange(CControlBar& inWnd)
{
    if (!m_platform.initComplete)
        return;

    bool nowFixedAndVisible = IsFixedAndVisible(inWnd);
    if ((isFixedAndVisible && (!nowFixedAndVisible)) || ((!isFixedAndVisible) && nowFixedAndVisible))
    {
        isFixedAndVisible = nowFixedAndVisible;

        int updateHeight = GetHeight();

        if (isFixedAndVisible)
            m_platform.basePlotDef.plotVheight -= updateHeight;
        else
            m_platform.basePlotDef.plotVheight += updateHeight;

        m_platform.DoResize();
        m_platform.UpdateControlBarStatus();
    }
}


//////////////////////////////////////////
CPlotStatusBar::CPlotStatusBar(PC_PlatformPS& platform) :
    CControlBarUpdate(platform)
{
}

CPlotStatusBar::~CPlotStatusBar()
{
}

BEGIN_MESSAGE_MAP(CPlotStatusBar, CSizingControlBarGrip)
    //{{AFX_MSG_MAP(CPlotStatusBar)
    ON_WM_WINDOWPOSCHANGING()
    ON_WM_NCLBUTTONUP()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CPlotStatusBar::GetHeight()
{
    return m_platform.CalcStatusBarHeight();
}

CSize CPlotStatusBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
    CSize size = CSizingControlBarGrip::CalcFixedLayout(bStretch, bHorz);
    size.cy = GetHeight();

    return size;
}

void CPlotStatusBar::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
{
    CSizingControlBar::OnWindowPosChanging(lpwndpos);
    CheckStatusChange(*this);
}

void CPlotStatusBar::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
    CSizingControlBarGrip::OnNcLButtonUp(nHitTest, point);
    CheckStatusChange(*this);
}


//////////////////////////////////////////
CPlotToolBar::CPlotToolBar(PC_PlatformPS& platform) :
    CControlBarUpdate(platform)
{
}

CPlotToolBar::~CPlotToolBar()
{
}

int CPlotToolBar::GetHeight()
{
    return m_platform.CalcButtonBarHeight();
}

BEGIN_MESSAGE_MAP(CPlotToolBar, CToolBar)
    //{{AFX_MSG_MAP(CPlotToolBar)
    ON_WM_WINDOWPOSCHANGING()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPlotToolBar::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
{
    CToolBar::OnWindowPosChanging(lpwndpos);
    CheckStatusChange(*this);
}


//////////////////////////////////////////
#define WM_CBMENUSTART          WM_APP+800

CWndTransparent::CWndTransparent(PC_PlatformPS& platform) :
    m_basePlotDef(platform.basePlotDef),
    m_hCurrentCursor(NULL),
    m_bMouseTracking(false),
    m_bLButtonDown(false),
    m_currCBMenu(0)
{
    m_rect.SetRect(0,0,0,0);
}

BEGIN_MESSAGE_MAP(CWndTransparent, CWnd)
    //{{AFX_MSG_MAP(CWndTransparent)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_WM_SETCURSOR()
    ON_WM_NCHITTEST()
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
    ON_COMMAND_RANGE(WM_CBMENUSTART, WM_CBMENUSTART + maxCallbacks, OnPopupCommand)
END_MESSAGE_MAP()

BOOL CWndTransparent::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CWnd::PreCreateWindow(cs))
        return FALSE;

    cs.dwExStyle |= WS_EX_TRANSPARENT;
    return TRUE;
}

void CWndTransparent::OnRButtonDown(UINT nFlags, CPoint point)
{
    m_currCBMenu = m_basePlotDef.GetCallbackMenu();
    if (m_currCBMenu == NULL)
        return;

    TitlePopupMenu popMenu;
    popMenu.CreatePopupMenu();

    CallbackItemArray& cbMenu = m_currCBMenu->menuDesc;

    for (int i = 0; i < cbMenu.Size(); i++)
    {
        CallbackMenuItem& currCB = cbMenu[i];

        switch (currCB.menuItemType) {

            case CallbackMenuItem::mitSeparator  : {
                    VERIFY(popMenu.AppendMenu(MF_SEPARATOR));
                    break;
            }

            case CallbackMenuItem::mitEntry : {
                    int menuID = WM_CBMENUSTART + i;
                    UINT uFlags = MF_STRING;
                    if (currCB.isActive)
                        uFlags |= MF_ENABLED;
                    else
                        uFlags |= MF_GRAYED;
                    VERIFY(popMenu.AppendMenu(uFlags, menuID, currCB.descStr));
                    break;
            }
        }
    }

    popMenu.AddMenuTitle(const_cast<char*>(m_currCBMenu->menuTitle));

    ClientToScreen(&point);
    popMenu.TrackPopupMenu(TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CWndTransparent::OnPopupCommand(UINT nMenuID)
{
    if (m_currCBMenu == NULL)
        return;

    UINT cbOffset = WM_CBMENUSTART;
    UINT cbIndex = nMenuID - cbOffset;

    m_currCBMenu->DoCallback(int(cbIndex));
}

void CWndTransparent::OnLButtonDown(UINT nFlags, CPoint point)
{
    bool shifted = (::GetKeyState(VK_SHIFT) < 0);
    if (m_basePlotDef.plotDefIsActive) {
        m_basePlotDef.PointerSelect(point.x, point.y, shifted);
    } else {
        if (shifted) {
            m_basePlotDef.ProcessSelectionKeys(PlotDefC::pdc_AddConsecutiveObject);
        } else if (::GetKeyState(VK_CONTROL) < 0) {
            m_basePlotDef.ProcessSelectionKeys(PlotDefC::pdc_AddDiscreteObject);
        } else {
            m_basePlotDef.ProcessSelectionKeys(PlotDefC::pdc_SelectObject);
        }
    }
    m_bLButtonDown = true;
}

void CWndTransparent::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_basePlotDef.plotDefIsActive) {
        bool shifted = (::GetKeyState(VK_SHIFT) < 0);
        m_basePlotDef.PointerRelease(point.x, point.y, shifted);
    }
    m_bLButtonDown = false;
}

LRESULT CWndTransparent::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    m_bMouseTracking = false;
    m_basePlotDef.PointerLeave();
    return TRUE;
}

void CWndTransparent::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_bMouseTracking) {
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = this->m_hWnd;
        if (::_TrackMouseEvent(&tme)) {
            m_bMouseTracking = true;
            m_basePlotDef.PointerEnter(point.x, point.y);
        }
    } else {
        if (m_bLButtonDown) {
            bool shifted = (::GetKeyState(VK_SHIFT) < 0);
            if (m_basePlotDef.plotDefIsActive)
                m_basePlotDef.PointerDrag(point.x, point.y, shifted);
            else
            {
                m_basePlotDef.PointerMove(point.x, point.y);
                m_basePlotDef.ProcessSelectionKeys(PlotDefC::pdc_Drag);
            }

        } else {
            m_basePlotDef.PointerMove(point.x, point.y);
        }
    }

    CWnd::OnMouseMove(nFlags, point);
}


BOOL CWndTransparent::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (nHitTest == HTCLIENT && m_hCurrentCursor != NULL) {
        ::SetCursor(m_hCurrentCursor);
        return TRUE;
    }
    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CWndTransparent::OnPaint()
{
    CWnd::OnPaint();
}

void CWndTransparent::DrawRect(const CRect& rect, COLORREF clrPen)
{
    CClientDC dc(this);
    DrawRect(&dc, rect, clrPen);
    m_rect = rect;
    m_clrPen = clrPen;
}

void CWndTransparent::DrawRectClear(const CRect& rect)
{
    if (m_rect.IsRectNull()) {
        return;
    }
    CClientDC dc(this);
    DrawRect(&dc, m_rect, m_clrPen);

    m_rect.SetRect(0,0,0,0);
}

void CWndTransparent::DrawRectClear()
{
    if (m_rect.IsRectNull()) {
        return;
    }
    CClientDC dc(this);
    DrawRect(&dc, m_rect, m_clrPen);

    m_rect.SetRect(0,0,0,0);
}

void CWndTransparent::DrawRect(CDC* pDC, const CRect& rect, COLORREF clrPen)
{
    CPen pen(PS_SOLID, 1, clrPen);
    CPen *penOld = pDC->SelectObject(&pen);
    int nDrawModeOld = pDC->SetROP2(R2_XORPEN);

    pDC->MoveTo(rect.left, rect.top);
    pDC->LineTo(rect.left, rect.bottom);
    pDC->LineTo(rect.right, rect.bottom);
    pDC->LineTo(rect.right, rect.top);
    pDC->LineTo(rect.left, rect.top);

    pDC->SelectObject(penOld);
    pDC->SetROP2(nDrawModeOld);
}


//////////////////////////////////////////
PC_PlatformPS::PC_PlatformPS(const char* plotType, PlotDefC& assPlt, OGLBase& oglObj,
                            int nButtonBarHeight, int nStatusBarHeight, int nStatusBarHeight2) :
    oglObject(oglObj),
    PC_Platform(assPlt),
    PS_MainWindow(*this, plotType),
    objectDialog(assPlt, *this),
    m_wndPlotOverlay(*this),
    resizeCB(0),
    m_bOwnerCreated(false),
    buttonBarHeight(nButtonBarHeight),
    statusBarHeight(nStatusBarHeight),
    statusBarHeight2(nStatusBarHeight2),
    m_bFullScreen(false),
    m_wndStatusBar(*this),
    m_wndToolBar(*this),
    m_wndPlot(assPlt)
{
    crossHairCursor = ::LoadCursor(NULL, IDC_CROSS);
    busyCursor = ::LoadCursor(NULL, IDC_WAIT);
    HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(AFX_IDC_MAGNIFY), RT_GROUP_CURSOR);
    zoomCursor = ::LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_MAGNIFY));
    grabCursor = ::LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_TRACK4WAY));

    oglObj.SetOGLBase(&baseOGL);
}

BEGIN_MESSAGE_MAP(PC_PlatformPS, CFrameWnd)
    //{{AFX_MSG_MAP(PC_PlatformPS)
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_CLOSE()
    ON_WM_ACTIVATE()
    ON_COMMAND(ID_TGA_OUTPUT, OnTGAOutput)
    ON_COMMAND(ID_PS_OUTPUT, OnPSOutput)
    ON_COMMAND(ID_PS_OUTPUT_AUTO, OnPSOutputAuto)
    ON_COMMAND(ID_COPYTOCLIPBOARD, OnCopyToClipboard)
    ON_COMMAND(ID_CONTROL_PLOT_OBJECT, OnControlPlotObject)
    ON_COMMAND(ID_PLOT_TOOLBAR, OnPlotToolbar)
    ON_COMMAND(ID_PLOT_CONTROLBAR, OnPlotControlbar)
    ON_UPDATE_COMMAND_UI(ID_PLOT_TOOLBAR, OnUpdatePlotToolbar)
    ON_UPDATE_COMMAND_UI(ID_PLOT_CONTROLBAR, OnUpdatePlotControlbar)
    ON_COMMAND(ID_PLOT_MOUSE_ZOOM, OnPlotMouseZoom)
    ON_COMMAND(ID_PLOT_MOUSE_SELECT, OnPlotMouseSelect)
    ON_COMMAND(ID_PLOT_REFRESH, OnPlotRefresh)
    ON_COMMAND(ID_PLOT_PRINT, OnPlotPrint)
    ON_COMMAND(ID_PLOT_PRINT_TB, OnPlotPrintToolbar)
    //}}AFX_MSG_MAP
    ON_COMMAND(ID_PLOT_INIT_VIEW, OnPlotInitView)
    ON_COMMAND(ID_PLOT_PREVIOUS_VIEW, OnPlotPreviousView)
    ON_COMMAND(ID_PLOT_RESET_VIEW, OnPlotResetView)
    ON_COMMAND(ID_PLOT_SETAXES, OnPlotSetAxes)
    ON_COMMAND(ID_PLOT_RESETAXES, OnPlotResetAxes)
    ON_COMMAND(ID_PLOT_RESET_DUMP, OnPlotResetDump)
    ON_UPDATE_COMMAND_UI(ID_PLOT_RESET_DUMP, OnUpdatePlotResetDump)
    ON_UPDATE_COMMAND_UI(ID_PLOT_RESETAXES, OnUpdatePlotResetAxes)
    ON_COMMAND(ID_PLOT_DUMP, OnPlotDump)
    ON_COMMAND(ID_PLOT_DUMP_RESETINCCOUNT, OnPlotDumpResetIncCount)
    ON_COMMAND(ID_PLOT_PROPAGATE_VIEW, OnPlotPropagateView)
    ON_COMMAND(ID_PLOT_PROPAGATE_SIZE, OnPlotPropagateSize)
    ON_COMMAND(ID_PLOT_FULL_SCREEN, OnPlotFullScreen)
    ON_UPDATE_COMMAND_UI(ID_PLOT_FULL_SCREEN, OnUpdatePlotFullScreen)
END_MESSAGE_MAP()

int PC_PlatformPS::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    EnableCloseInSystemMenu(false);

    return 0;
}

void PC_PlatformPS::OnClose()
{
    MessageBox("Please close application from the main application window");
}

BOOL PC_PlatformPS::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CFrameWnd::PreCreateWindow(cs)) {
        return FALSE;
    }

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

void PC_PlatformPS::PostNcDestroy()
{
    //DON'T delete this;

    MainWindowDeactivated();
}

/* Interesting, but we don't need it - this window is always hidden for taskbar
bool PC_PlatformPS::ShowTaskBarButton(bool bShow)
{
    if (!m_bOwnerCreated) {
        return false;
    }

    ShowWindow(SW_HIDE);
    if (bShow) {
        // This flag suppresses the behavior of the owner window and
        // the window itself becomes visible (from the taskbar point of view).
        ModifyStyleEx(0, WS_EX_APPWINDOW);
    } else {
        // Removing WS_EX_APPWINDOW flag forces the taskbar to find the owner window
        // relying to the frame window -
        // which is invisible and the button gets disappeared
        ModifyStyleEx(WS_EX_APPWINDOW, 0);
    }
    ShowWindow(SW_SHOW); // Refresh of the taskbar

    return true;
}
*/

void PC_PlatformPS::OnPaint()
{
    CFrameWnd::OnPaint();

    basePlotDef.RedrawPlot();

    //!!! TEST Emulation of slow graphics card
    //Sleep(2000);
}

void PC_PlatformPS::OnSize(UINT nType, int cx, int cy)
{
    CFrameWnd::OnSize(nType, cx, cy);

    if (initComplete && (cy > 0) && (cx > 0))
    {
        int nTopBottomHeight =  0;

        if (CControlBarUpdate::IsFixedAndVisible(m_wndToolBar))
            nTopBottomHeight += CalcButtonBarHeight();

        if (CControlBarUpdate::IsFixedAndVisible(m_wndStatusBar))
            nTopBottomHeight += CalcStatusBarHeight();

        basePlotDef.plotUwidth = cx - 2*::GetSystemMetrics(SM_CXEDGE);
        basePlotDef.plotVheight = cy - nTopBottomHeight;

        m_wndPlot.Invalidate();

        UpdateControlBarStatus();

        if (resizeCB) {
            (*resizeCB)(resizeCBData);
        }

        if (basePlotDef.IsSubPlot())
            DoResize();

    }
}

void PC_PlatformPS::OnTGAOutput()
{
    PS_TGAOutputSetup dlg(this);

    dlg.SetValues(basePlotDef.tgaOutput);
    INT_PTR nResult = dlg.DoModal();
    if (nResult == IDOK) {
        dlg.GetValues(basePlotDef.tgaOutput);
    }
}

void PC_PlatformPS::OnPSOutput()
{
    PS_PSOutputSetup dlg(this);

    dlg.SetValues(basePlotDef.psOutput);
    INT_PTR nResult = dlg.DoModal();
    if ((nResult == IDOK) || (nResult == IDYES))
    {
        dlg.GetValues(basePlotDef.psOutput);

        if (nResult == IDOK)
            DoPostscriptOutput();
    }
}


void PC_PlatformPS::DoPostscriptOutput()
{
    AfxGetMainWnd()->UpdateWindow();

    CWaitCursor w;
    oglObject.OutputPostscript();

}

void PC_PlatformPS::OnPSOutputAuto()
{
    DoAutoPostscriptOutput();
}

void PC_PlatformPS::OnCopyToClipboard()
{
    CopyToClipboard(false);
}

void PC_PlatformPS::DoAutoPostscriptOutput()
{
    PC_PSOutData& currPS = basePlotDef.psOutput;

    // kluge with file naming
    char tempStr[stdFileStrLen];
    CopyString(tempStr, currPS.outputFile, stdFileStrLen);

    ConcatInt(currPS.nextPSIncrement, currPS.outputFile, 4, stdFileStrLen);

    if (currPS.outputFormat == PC_PSOutData::of_PS)
        ConcatString(currPS.outputFile, ".ps", stdFileStrLen);
    else
        ConcatString(currPS.outputFile, ".eps", stdFileStrLen);

    CWaitCursor w;
    oglObject.OutputPostscript();

    // restore default
    CopyString(currPS.outputFile, tempStr, stdFileStrLen);

    currPS.nextPSIncrement++;
}

void PC_PlatformPS::OnControlPlotObject()
{
    objectDialog.OpenObjectDialog();
}

void PC_PlatformPS::AddResizeCallback(ResizeCB  resCB,
                                      void*     resData)

{
    if (resizeCB) {
        GenAppInternalError("PC_PlatformPS::AddResizeCallback");
    }

    resizeCB = resCB;
    resizeCBData = resData;
}

void PC_PlatformPS::EnableCloseInSystemMenu(bool bEnable)
{
    CMenu* pMenu = GetSystemMenu(FALSE);
    ASSERT(pMenu != NULL);
    if (bEnable) {
        pMenu->ModifyMenu(SC_CLOSE, MF_BYCOMMAND|MF_ENABLED);
    } else {
        pMenu->ModifyMenu(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
    }
}

void PC_PlatformPS::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE) {
        TRACE("Activate PC_PlatformPS %d\r\n", nState);
        MainWindowActivated();
    }
    CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
}


void PC_PlatformPS::DoManualTGA()
{
    NTCompatibleFileDialog dlg(false, NULL,
                    basePlotDef.tgaOutput.tgaRootFile,
                    OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
                    NULL,
                    this);

    if (basePlotDef.tgaOutput.dumpFormat == PC_DumpTGA::dumpTGA)
    {
        dlg.m_ofn.lpstrFilter = "Targa Bitmap (*.TGA)\0*.TGA\0\0";
        dlg.m_ofn.lpstrDefExt = "TGA";
    }
    else if (basePlotDef.tgaOutput.dumpFormat == PC_DumpTGA::dumpJPEG)
    {
        dlg.m_ofn.lpstrFilter = "Jpeg Bitmap (*.JPG)\0*.JPG\0\0";
        dlg.m_ofn.lpstrDefExt = "JPG";
    }
    else
    {
        dlg.m_ofn.lpstrFilter = "Windows Bitmap (*.BMP)\0*.BMP\0\0";
        dlg.m_ofn.lpstrDefExt = "BMP";
    }


    if (dlg.DoModal() == IDOK)
    {
        CopyString(basePlotDef.tgaOutput.tgaRootFile, dlg.GetPathName(), stdFileStrLen);
        CWaitCursor w;
        basePlotDef.RedrawPlot();
        WriteBitmapFile(basePlotDef.tgaOutput.tgaRootFile);
   }
}

void PC_PlatformPS::SetTGAButtonText()
{
/*
    if (basePlotDef.tgaOutput.dumpMethod == PC_DumpTGA::dtga_FullAuto) {
        tgaReset.SetButtonText("Is:Auto");
    } else {
        tgaReset.SetButtonText("Is:Semi");
    }
*/
}


bool PC_PlatformPS::IsMinimized()
{
    return CFrameWnd::IsIconic();
}

void PC_PlatformPS::OnPlotInitView()
{
    basePlotDef.ViewTop();
}
void PC_PlatformPS::OnPlotPreviousView()
{
    basePlotDef.ViewPrev();
}
void PC_PlatformPS::OnPlotResetView()
{
    ResetView();
}
void PC_PlatformPS::OnPlotSetAxes()
{
    basePlotDef.SetAxesFromView();
}
void PC_PlatformPS::OnPlotResetAxes()
{
    basePlotDef.RestoreSetAxes();
}
void PC_PlatformPS::OnPlotResetDump()
{
    if (basePlotDef.tgaOutput.dumpMethod == PC_DumpTGA::dtga_FullAuto)
        basePlotDef.tgaOutput.dumpMethod = PC_DumpTGA::dtga_SemiAuto;
    else
        basePlotDef.tgaOutput.dumpMethod = PC_DumpTGA::dtga_FullAuto;
    SetTGAButtonText();
}

void PC_PlatformPS::OnPlotDump()
{
    if (basePlotDef.tgaOutput.dumpMethod == PC_DumpTGA::dtga_Manual)
    {
        DoManualTGA();
    }
    else
    {
        int ndump = 1;
        if (basePlotDef.tgaOutput.dumpMethod == PC_DumpTGA::dtga_SemiAuto)
            ndump = basePlotDef.tgaOutput.dumpFrameCount;

        CWaitCursor w;
        char  tgafName[stdFileStrLen];
        for (int i = 0; i < ndump; i++)
        {
            basePlotDef.tgaOutput.GetNextFileName(tgafName, stdFileStrLen);
            WriteBitmapFile(tgafName);
        }
    }
}
void PC_PlatformPS::OnPlotDumpResetIncCount()
{
    basePlotDef.tgaOutput.nextIncrement = 0;
}
void PC_PlatformPS::OnPlotPropagateView()
{
    basePlotDef.ViewPropagate();
}
void PC_PlatformPS::OnPlotPropagateSize()
{
    basePlotDef.SizePropagate();
}
void PC_PlatformPS::OnPlotFullScreen()
{
    OnViewFullScreen();
}

void PC_PlatformPS::OnUpdatePlotResetDump(CCmdUI* pCmdUI)
{
    if (basePlotDef.tgaOutput.dumpMethod == PC_DumpTGA::dtga_FullAuto)
    {
        pCmdUI->SetText("Is:Auto");
        pCmdUI->SetCheck(true);
    }
    else
    {
        pCmdUI->SetText("Is:Semi");
        pCmdUI->SetCheck(false);
    }
}
void PC_PlatformPS::OnUpdatePlotResetAxes(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(basePlotDef.axesHaveBeenSet);
}
void PC_PlatformPS::OnUpdatePlotFullScreen(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bFullScreen);
}

void PC_PlatformPS::ResetView()
{
    ResetGraphicsWindow();

    basePlotDef.CalcOutput(foc_Apply);

    basePlotDef.CalcDownStreamObjects();

    PlotDefC::ReplotAfterCalc();
}

void PC_PlatformPS::CalcBorderSize(int& nWidth, int& nHeight)
{
    nWidth = 2*(::GetSystemMetrics(SM_CXFRAME)+2*::GetSystemMetrics(SM_CXEDGE));

    nHeight = ::GetSystemMetrics(SM_CYMENU);
    nHeight += ::GetSystemMetrics(SM_CYCAPTION);
    nHeight += 2*(::GetSystemMetrics(SM_CYFRAME)+::GetSystemMetrics(SM_CYEDGE));

    int nbottom = CalcStatusBarHeight();
    int ntop = CalcButtonBarHeight();

    if (initComplete)
    {
        if (!CControlBarUpdate::IsFixedAndVisible(m_wndToolBar))
            ntop = 0;

        if (!CControlBarUpdate::IsFixedAndVisible(m_wndStatusBar))
            nbottom = 0;
    }
    nHeight += nbottom + ntop;

}

// creates window
void PC_PlatformPS::InitGraphicsWindow()
{
    if (initComplete)
        return;

    UINT nMenuID = GetMenuResourceID();
    VERIFY(LoadFrame(nMenuID));

    int xBorder, yBorder;
    CalcBorderSize(xBorder, yBorder);
    VERIFY(SetWindowPos(&CWnd::wndTop, 0, 0,
                            basePlotDef.plotUwidth + xBorder,
                            basePlotDef.plotVheight + yBorder,
                            SWP_NOMOVE));

    SetWindowText("Loading...");
    ShowWindow(SW_SHOW);

    // style settings as per OpenGL doc
    if (!m_wndPlot.Create(NULL, _T("Plot window"),
            WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
            CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
    {
        TRACE0("Failed to create plot window\n");
    }

    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
            | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)
        || !m_wndToolBar.LoadToolBar(IDR_PLOT_TOOLBAR))
    {
        TRACE0("Failed to create toolbar\n");
    }

    DrawMouseToolbarButtons();

    if (!m_wndStatusBar.Create(_T("Control Bar"), this, 10)) {
        TRACE0("Failed to create control bar\n");
    }

    m_wndStatusBar.SetBarStyle(m_wndStatusBar.GetBarStyle()
        | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

    m_wndStatusBar.SetSCBStyle(m_wndStatusBar.GetSCBStyle()
        | SCBS_SHOWEDGES | SCBS_SIZECHILD);

    EnableDocking(CBRS_ALIGN_ANY);

    m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
    m_wndStatusBar.EnableDocking(CBRS_ALIGN_BOTTOM);

    DockControlBar(&m_wndToolBar, AFX_IDW_DOCKBAR_TOP);
    DockControlBar(&m_wndStatusBar, AFX_IDW_DOCKBAR_BOTTOM);

    CString sProfile(_T("PlatformBarState"));
    if (VerifyBarState(sProfile))
    {
        m_wndStatusBar.LoadState(sProfile);
        LoadBarState(sProfile);
    }

    CRect rect;
    VERIFY(m_wndPlotOverlay.Create(NULL, _T("WndPlotOverlay"),
        WS_VISIBLE|WS_CHILD, rect, &m_wndPlot, 0));

    VERIFY(layoutStatusBar.Create(NULL, _T("LayoutStatusBar"),
        WS_VISIBLE|WS_CHILD, rect, &m_wndStatusBar, 3));

    // setup bottom status bar
    statusForm.SetSmall(projectUtilities::IsPlotSmallFont());
    layoutStatusBar.InitMenu(statusForm);

    baseOGL.InitOpenGL(&m_wndPlot);
}

void PC_PlatformPS::SetStatusSmall(bool bSmall)
{
    statusForm.SetSmall(bSmall);
    RecalcLayout();
    layoutStatusBar.ResetForm();
}

// resets background pen and window size
void PC_PlatformPS::ResetGraphicsWindow()
{
    //  set background pen
    if (basePlotDef.defaultPenSet)
    {
        plotBackgroundColor = ConvertSC_ColorSpec(basePlotDef.defaultPenSet->GetColor(basePlotDef.plotBackgroundPen));
    }
    else
    {
        plotBackgroundColor = GetStdColor(sc_White);
    }

    // we are changing to window size so that the 'm_wndPlot' will
    // be set the size specified in basePlotDef
    DoResize();
}

void PC_PlatformPS::DoResize()
{
    CRect rect;
    GetWindowRect(rect);
    int xBorder, yBorder;
    CalcBorderSize(xBorder, yBorder);
    rect.right = rect.left + basePlotDef.plotUwidth + xBorder;

    // subplots don't get any display area
    if (basePlotDef.IsMasterPlot())
        rect.bottom = rect.top + basePlotDef.plotVheight + yBorder;
    else
        rect.bottom = rect.top + yBorder;

    SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
}

// resets window name
void PC_PlatformPS::RenameGraphicsWindow(const char* newName)
{
    if (objectDialog.IsVisible())
    {
        char controlName[80];
        MakeString(controlName, newName, "::Control", 80);
        objectDialog.SetWindowText(controlName);
    }

    SetMainWindowTitle(newName);
}


void PC_PlatformPS::UpdateControlBarStatus()
{
}


void PC_PlatformPS::GetCurrentBitmap(BitmapArray& bitmapData,
                                     BITMAPINFOHEADER& header)
{
    if (!baseOGL.IsCurrent())
        basePlotDef.RedrawPlot();

    CWnd* pWnd2 = &m_wndPlot;
    CRect rect;
    pWnd2->GetClientRect(rect);
    ASSERT(rect.top == 0 && rect.left == 0);
    int nX = rect.right;
    int nY = rect.bottom;
    nX -= nX % 4; // lines have to 32 bytes aligned
    int nBytes = 3 * nX * nY;
    bitmapData.Alloc(nBytes);

    // Copy from OpenGL
    ::glReadBuffer(GL_FRONT);  // make sure we get the front buffer
    ::glReadPixels(0, 0, nX, nY, GL_RGB, GL_UNSIGNED_BYTE, bitmapData.tListData);

    header.biSize           = sizeof(BITMAPINFOHEADER);
    header.biWidth          = nX;
    header.biHeight         = nY;
    header.biSizeImage      = nBytes;
    header.biPlanes         = 1;
    header.biBitCount       = 3 * 8; // RGB
    header.biCompression    = BI_RGB;
    header.biXPelsPerMeter  = 0;
    header.biYPelsPerMeter  = 0;
    header.biClrUsed        = 0;
    header.biClrImportant   = 0;
}

static void SwapRGB(void* pBitmapData, int nTriads)
{
    #pragma pack(push)
    #pragma pack(1)
    struct RGBTriad {
      BYTE bRed;
      BYTE bGreen;
      BYTE bBlue;

    };
    #pragma pack(pop)

    RGBTriad* p = (RGBTriad*)pBitmapData;
    for (int i = 0; i < nTriads; i++) {
        BYTE bSave = p->bRed;
        p->bRed = p->bBlue;
        p->bBlue = bSave;
        p++;
    }
}


void PC_PlatformPS::WriteBitmapFile(const char fileName[])
{
    BitmapArray bitmapData;
    BITMAPINFOHEADER header;
    GetCurrentBitmap(bitmapData, header);

#ifdef TGAIMAGE
    if (basePlotDef.tgaOutput.dumpFormat == PC_DumpTGA::dumpTGA)
    {
        PS_WriteTGA(fileName, header, bitmapData.tListData);
    }
    else
#endif TGAIMAGE
    {
        SwapRGB(bitmapData.tListData, header.biSizeImage/3);
        PS_WriteBmp(fileName, header, bitmapData.tListData);
    }
}




void PC_PlatformPS::CopyToClipboard(bool replotRequired)
{
    CWaitCursor w;
    if (replotRequired)
    {

        // We have to redraw the plot, because this function has been called from
        // pop-up menu which obstructs the plot
        basePlotDef.RedrawPlot();
    }

    BitmapArray bitmapData;
    BITMAPINFOHEADER header;
    GetCurrentBitmap(bitmapData, header);
    SwapRGB(bitmapData.tListData, header.biSizeImage/3);

    HANDLE handle = (HANDLE)::GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER) + header.biSizeImage);
    if (handle != NULL) {
        char *pData = (char *) ::GlobalLock((HGLOBAL)handle);
        memcpy(pData, &header, sizeof(BITMAPINFOHEADER));
        memcpy(pData+sizeof(BITMAPINFOHEADER), bitmapData.tListData, header.biSizeImage);
        ::GlobalUnlock((HGLOBAL)handle);

        // Push DIB in clipboard
        m_wndPlot.OpenClipboard();
        EmptyClipboard();
        SetClipboardData(CF_DIB, handle);
        CloseClipboard();
    }
}


void PC_PlatformPS::PrintScreenImage(CDC* pDCPrint)
{
    CWaitCursor w;


    BitmapArray bitmapData;
    BITMAPINFOHEADER header;
    GetCurrentBitmap(bitmapData, header);
    SwapRGB(bitmapData.tListData, header.biSizeImage/3);


    // Initialize the bitmap header info.
    BITMAPINFO  bmi;
    memset(&bmi, 0, sizeof(BITMAPINFO));
    memcpy(&bmi.bmiHeader, &header, sizeof(BITMAPINFOHEADER));

    CSize m_szPage;
    // Get page size in pixels
    m_szPage.cx  = pDCPrint->GetDeviceCaps(HORZRES);
    m_szPage.cy = pDCPrint->GetDeviceCaps(VERTRES);

    int nRatioX = pDCPrint->GetDeviceCaps(ASPECTX);
    int nRatioY = pDCPrint->GetDeviceCaps(ASPECTY);

    CClientDC dc(CWnd::GetDesktopWindow());
    int nRatioX_screen = dc.GetDeviceCaps(ASPECTX);
    int nRatioY_screen = dc.GetDeviceCaps(ASPECTY);

    float fPageRatioX = float(m_szPage.cx)/(header.biWidth * float(nRatioX_screen) / nRatioX);
    float fPageRatioY = float(m_szPage.cy)/(header.biHeight * float(nRatioY_screen) / nRatioY);

    CSize szImageOnPage;
    if (fPageRatioX < fPageRatioY) {
        szImageOnPage.cx = m_szPage.cx;
        szImageOnPage.cy = int(m_szPage.cy / fPageRatioY * fPageRatioX);
    } else {
        szImageOnPage.cx = int(m_szPage.cx / fPageRatioX * fPageRatioY);
        szImageOnPage.cy = m_szPage.cy;
    }

    CSize szOffset((m_szPage.cx - szImageOnPage.cx)/2, (m_szPage.cy - szImageOnPage.cy)/2);

    pDCPrint->SetMapMode(MM_TEXT);

    // Stretch the Dib to fit the image size.
    int nRet = ::StretchDIBits(pDCPrint->GetSafeHdc(),
                              szOffset.cx, szOffset.cy,
                              szImageOnPage.cx,
                              szImageOnPage.cy,
                              0, 0,
                              bmi.bmiHeader.biWidth,
                              bmi.bmiHeader.biHeight,
                              bitmapData.tListData,
                              &bmi,
                              DIB_RGB_COLORS, SRCCOPY);

    if (nRet == GDI_ERROR)
        TRACE0("Failed in StretchDIBits()");
}

// We have to redraw the plot sometimes, because this function can be called from
// pop-up menu which obstructs the plot
void PC_PlatformPS::Print(bool bRedraw)
{
    CDC printDC;
    CPrintInfo printInfo;
    if (!AfxGetApp()->GetPrinterDeviceDefaults(&printInfo.m_pPD->m_pd)) {
        AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_PRINT_SETUP);
        if (!AfxGetApp()->GetPrinterDeviceDefaults(&printInfo.m_pPD->m_pd)) {
            AfxMessageBox("Cannot get settings of current/default printer");
            return;
        }
    }
    HDC hDC = printInfo.m_pPD->m_pd.hDC;
    if (hDC == NULL)
        hDC = printInfo.m_pPD->CreatePrinterDC();
    if (hDC != NULL) {
        printDC.Attach(hDC);
    } else  {
        AfxMessageBox("Cannot find printer. Please check installed/default printers.");
        return;
    }

    DOCINFO di;
    ::ZeroMemory(&di, sizeof (DOCINFO));
    di.cbSize = sizeof (DOCINFO);
    CString szAppName;
    szAppName.LoadString(AFX_IDS_APP_TITLE);
    di.lpszDocName = szAppName;

    printDC.StartDoc(&di);

    printDC.StartPage();

        if (bRedraw)
            basePlotDef.RedrawPlot();

        PrintScreenImage(&printDC);

    printDC.EndPage();
    printDC.EndDoc();
    printDC.Detach();
}

void PC_PlatformPS::ClearPlatformOverlay()
{
    m_wndPlotOverlay.DrawRectClear();
}

void PC_PlatformPS::ClearPlatformRectangle(const PC_ObjLimitRect& rectToClear)
{
    CRect rect(rectToClear.ulXpos, rectToClear.ulYpos,
               rectToClear.lrXpos, rectToClear.lrYpos);
    m_wndPlotOverlay.DrawRectClear(rect);
}

void PC_PlatformPS::DrawPlatformRectangle(const PC_ObjLimitRect& rectToDraw, bool useSelectColor)
{
    CRect rect(rectToDraw.ulXpos, rectToDraw.ulYpos,
               rectToDraw.lrXpos, rectToDraw.lrYpos );
    m_wndPlotOverlay.DrawRect(rect, useSelectColor ? RGB(255,0,0) : RGB(255,255,255));
}

void PC_PlatformPS::DisplayBusyCursor()
{
    DisplayCursor(busyCursor);
}

void PC_PlatformPS::DisplayNormalCursor()
{
    DisplayCursor(crossHairCursor);
}

void PC_PlatformPS::DisplayGrabCursor()
{
    DisplayCursor(grabCursor);
}

void PC_PlatformPS::DisplayZoomCursor()
{
    DisplayCursor(zoomCursor);
}

void PC_PlatformPS::DisplayCursor(HCURSOR hCursor)
{
    m_wndPlotOverlay.m_hCurrentCursor = hCursor;
    if (m_wndPlotOverlay.m_hCurrentCursor != NULL) {
        ::SetCursor(m_wndPlotOverlay.m_hCurrentCursor);
    }
}


void PC_PlatformPS::SetFrameIcon(UINT nID)
{
    HICON hIcon = HICON(::LoadImage(AfxGetInstanceHandle(),
                                MAKEINTRESOURCE(nID),
                                IMAGE_ICON, 16, 16,
                                LR_SHARED));
    ASSERT(hIcon != NULL);
    SetIcon(hIcon, FALSE);
}

void PC_PlatformPS::OnViewFullScreen()
{
    if (m_bFullScreen) {

        LockWindowUpdate();
        LONG style = ::GetWindowLong(m_hWnd,GWL_STYLE);
        style |= WS_CAPTION;
        ::SetWindowLong(m_hWnd, GWL_STYLE, style);
        SetWindowPlacement(&m_wplPrevious);
        UnlockWindowUpdate();

        m_bFullScreen = false;

    } else {

        m_bFullScreen = true;

        GetWindowPlacement(&m_wplPrevious);

        LockWindowUpdate();
        LONG style = ::GetWindowLong(m_hWnd, GWL_STYLE);
        style &= ~WS_CAPTION;
        ::SetWindowLong(m_hWnd, GWL_STYLE, style);

        int cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
        int cyScreen = ::GetSystemMetrics(SM_CYSCREEN);
        int cxBorder = ::GetSystemMetrics(SM_CXBORDER);
        int cyBorder = ::GetSystemMetrics(SM_CYBORDER);
        int cyMenu = ::GetSystemMetrics(SM_CYMENU);
        m_rectFullScreenPlace = CRect(-cxBorder, -cyBorder,
                                      cxScreen + 2 * cxBorder, cyScreen + 2 * cyBorder);

        WINDOWPLACEMENT wpNew = m_wplPrevious;
        wpNew.showCmd =  SW_SHOWNORMAL;
        wpNew.rcNormalPosition = m_rectFullScreenPlace;

        SetWindowPlacement(&wpNew);
        UnlockWindowUpdate();
    }
}

void PC_PlatformPS::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
    if (m_bFullScreen) {
        lpMMI->ptMaxSize.y =
        lpMMI->ptMaxTrackSize.y = m_rectFullScreenPlace.Height();
        lpMMI->ptMaxSize.x =
        lpMMI->ptMaxTrackSize.x = m_rectFullScreenPlace.Width();
    } else {
        CFrameWnd::OnGetMinMaxInfo(lpMMI);
    }
}

BOOL PC_PlatformPS::PreTranslateMessage(MSG* pMsg)
{
    // When there is an escape key toggle the full screen mode
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE && m_bFullScreen)
    {
        OnViewFullScreen();
    }

    // (Ctrl/Shift)Space key processing
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_SPACE)
    {
        if (::GetKeyState(VK_SHIFT) < 0) {
            basePlotDef.ProcessSelectionKeys(PlotDefC::pdc_AddConsecutiveObject);
        } else if (::GetKeyState(VK_CONTROL) < 0) {
            basePlotDef.ProcessSelectionKeys(PlotDefC::pdc_AddDiscreteObject);
        } else {
            basePlotDef.ProcessSelectionKeys(PlotDefC::pdc_SelectObject);
        }
    }

    return CFrameWnd::PreTranslateMessage(pMsg);
}

void PC_PlatformPS::DrawMouseToolbarButtons()
{
    CToolBarCtrl& currToolBar = m_wndToolBar.GetToolBarCtrl();
    currToolBar.PressButton(ID_PLOT_MOUSE_ZOOM, basePlotDef.plotDefIsActive);
    currToolBar.PressButton(ID_PLOT_MOUSE_SELECT, !basePlotDef.plotDefIsActive);
}

void PC_PlatformPS::OnPlotMouseZoom()
{
    basePlotDef.plotDefIsActive = true;
    DrawMouseToolbarButtons();
}

void PC_PlatformPS::OnPlotMouseSelect()
{
    basePlotDef.plotDefIsActive = false;
    DrawMouseToolbarButtons();
}

void PC_PlatformPS::OnPlotPrint()
{
    Print(true);
}

void PC_PlatformPS::OnPlotPrintToolbar()
{
    Print(false);
}

void PC_PlatformPS::OnPlotRefresh()
{
    basePlotDef.RedrawPlot();
}

bool PC_PlatformPS::VerifyBarState(LPCTSTR lpszProfileName)
{
    CDockState state;
    state.LoadState(lpszProfileName);

    for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
    {
        CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
        ASSERT(pInfo != NULL);
        INT_PTR nDockedCount = pInfo->m_arrBarID.GetSize();
        if (nDockedCount > 0)
        {
            // dockbar
            for (int j = 0; j < nDockedCount; j++)
            {
                UINT nID = (UINT) pInfo->m_arrBarID[j];
                if (nID == 0)
                    continue; // row separator
                if (nID > 0xFFFF)
                    nID &= 0xFFFF; // placeholder - get the ID
                if (GetControlBar(nID) == NULL)
                    return false;
            }
        }

        if (!pInfo->m_bFloating) // floating dockbars can be created later
            if (GetControlBar(pInfo->m_nBarID) == NULL)
                return false; // invalid bar ID
    }

    return true;
}

void PC_PlatformPS::OnPlotToolbar()
{
    ShowControlBar(&m_wndToolBar, !m_wndToolBar.IsVisible(), TRUE);
    DoResize();
}

void PC_PlatformPS::OnUpdatePlotToolbar(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_wndToolBar.IsVisible());
}

void PC_PlatformPS::OnPlotControlbar()
{
    ShowControlBar(&m_wndStatusBar, !m_wndStatusBar.IsVisible(), TRUE);
    DoResize();
}

void PC_PlatformPS::OnUpdatePlotControlbar(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_wndStatusBar.IsVisible());
}

void PC_PlatformPS::OnSetFocus(CWnd* pOldWnd)
{
    // forward focus to the plot window
    m_wndPlot.SetFocus();
}

BOOL PC_PlatformPS::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // let the plot have first crack at the command
    if (m_wndPlot.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
        return TRUE;

    // otherwise, do default handling
    return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


BOOL PC_PlatformPS::OnEraseBkgnd(CDC* pDC)
{
    if (initComplete) {
        return FALSE;
    }
    return CFrameWnd::OnEraseBkgnd(pDC);
}

