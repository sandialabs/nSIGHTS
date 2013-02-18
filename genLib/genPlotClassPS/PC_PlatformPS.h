///////////////////////////////////////////////////////////////////////////////////
//
// PC_PlatformPS.h
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

#ifndef PC_PLATFORMPS_H
#define PC_PLATFORMPS_H

#include <genClass/C_Common.h>

#include <genPlotClass/PC_Platform.h>

#include <genClassPS/PS_MainWindow.h>
#include <genClassPS/PS_FormContainer.h>
#include <genClassPS/PS_UIBase.h>
#include <genClassPS/PS_UIClass.h>
#include <genClassPS/PS_OGLBase.h>
#include <genClassPS/PS_Color.h>

#include <genPlotClassPS/OGL_Base.h>
#include <genPlotClassPS/PS_ObjectDialog.h>
#include <genPlotClassPS/OGL_Base.h>

#include <genUIExt/SizingControlBarGrip.h>

using namespace genClassPS;

class CWndLayout : public CWnd, public FormContainer
{
public:
                    CWndLayout();
    virtual         ~CWndLayout();

public:
    virtual CWnd*   GetWindow();

    void            InitMenu(FormManager& form);
    void            MenuOpen();

protected:
    //{{AFX_MSG(CWndLayout)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};



class CControlBarUpdate  {
public:
    bool            isFixedAndVisible;
protected:
    PC_PlatformPS&  m_platform;

public:
                CControlBarUpdate(PC_PlatformPS& platform);

    static bool IsFixedAndVisible(CControlBar& inWnd);

protected:

    void        CheckStatusChange(CControlBar& inWnd);
    virtual int GetHeight() = 0;
};


class CPlotStatusBar : public CSizingControlBarGrip, public CControlBarUpdate
{
public:
    CPlotStatusBar(PC_PlatformPS& platform);
    virtual ~CPlotStatusBar();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPlotStatusBar)
    virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
    //}}AFX_VIRTUAL

    // Generated message map functions
protected:
    virtual int GetHeight();


    //{{AFX_MSG(CPlotStatusBar)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
    afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

class CPlotToolBar : public CToolBar, public CControlBarUpdate
{
public:
    CPlotToolBar(PC_PlatformPS& platform);
    virtual ~CPlotToolBar();

    // Generated message map functions
protected:
    virtual int GetHeight();

    //{{AFX_MSG(CPlotToolBar)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};



class CWndPlot : public CWnd
{
public:
    CWndPlot(PlotDefC& basePlotDef);
    virtual ~CWndPlot();

protected:
    PlotDefC&   basePlotDef;

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CWndPlot)
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CWndTop)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

class CWndTransparent : public CWnd
{
    friend class PC_PlatformPS;
public:
    CWndTransparent(PC_PlatformPS& platform);

    void DrawRect(const CRect& rect, COLORREF clrPen);
    void DrawRectClear(const CRect& rect);
    void DrawRectClear();

protected:
    void DrawRect(CDC* pDC, const CRect& rect, COLORREF clrPen);

protected:
    PlotDefC&   m_basePlotDef;

    HCURSOR     m_hCurrentCursor;
    bool        m_bMouseTracking;
    bool        m_bLButtonDown;

    CRect       m_rect;
    COLORREF    m_clrPen;

private:
    enum        {maxCallbacks = 20};
    PC_CallbackMenu* m_currCBMenu;

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CChildView)
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CWndTransparent)
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnPaint();
    afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
    //}}AFX_MSG
    afx_msg void OnPopupCommand(UINT nMenuID);
    DECLARE_MESSAGE_MAP()
};

typedef void (*ResizeCB)(void *);

typedef T_SC_Array<unsigned char> BitmapArray;

// PC_PlatformPS is top level shell for plot
class PC_PlatformPS : public CFrameWnd, public PC_Platform, public PS_MainWindow
{
    friend class CControlBarUpdate;
    friend class CPlotStatusBar;
    friend class CPlotToolBar;
    friend class CWndTransparent;
public:
    PC_PlatformPS(const char* plotType, PlotDefC& assPlt, OGLBase& oglObj,
                int nButtonBarHeight, int nStatusBarHeight, int nStatusBarHeight2);

    //  main virtuals
    virtual void InitGraphicsWindow();                      // creates window
    virtual void ResetGraphicsWindow();                     // resets background pen and window size
    virtual void RenameGraphicsWindow(const char* newName); // resets window name
    virtual void WriteBitmapFile(const char fileName[]);         // writes window as TGA file

    virtual void SetStatusSmall(bool bSmall);

    virtual void    CopyToClipboard(bool replotRequired);

    void    AddResizeCallback(ResizeCB  resCB, void* resData);

    void    DoManualTGA();
    void    SetTGAButtonText();

    virtual bool IsMinimized();

    virtual void DisplayBusyCursor();
    virtual void DisplayNormalCursor();
    virtual void DisplayGrabCursor();
    virtual void DisplayZoomCursor();
    virtual void ClearPlatformOverlay();
    virtual void ClearPlatformRectangle(const PC_ObjLimitRect& rectToClear);
    virtual void DrawPlatformRectangle(const PC_ObjLimitRect& rectToDraw, bool useSelectColor);


    void    OnViewFullScreen();

    void    ResetView();

    void    DoResize();

    void    DoPostscriptOutput();

    void    DoAutoPostscriptOutput();

protected:
    virtual UINT    GetMenuResourceID() = 0;
    virtual int     CalcStatusBarHeight() = 0;
    virtual int     CalcButtonBarHeight() = 0;

    virtual void    UpdateControlBarStatus();  // called by control bars after size/status change


    void    CalcBorderSize(int& width, int& height);
    void    DisplayCursor(HCURSOR hCursor);
    void    EnableCloseInSystemMenu(bool bEnable);

    void    SetFrameIcon(UINT nID);

    bool    VerifyBarState(LPCTSTR lpszProfileName);

    void    PrintScreenImage(CDC* pDC);
    void    Print(bool bRedraw);

public:
    CPlotToolBar        m_wndToolBar;
    CPlotStatusBar      m_wndStatusBar;
    CWndPlot            m_wndPlot;
    CWndTransparent     m_wndPlotOverlay;

    CWndLayout          layoutStatusBar;
    FormManager         statusForm;

    PS_OGLBase          baseOGL; // base OGL for window
    OGLBase&            oglObject;      //  curr rendering  object
    COLORREF            plotBackgroundColor;

    // mouse cursors
    HCURSOR             crossHairCursor;
    HCURSOR             busyCursor;
    HCURSOR             grabCursor;
    HCURSOR             zoomCursor;

    int                 buttonBarHeight;
    int                 statusBarHeight;
    int                 statusBarHeight2;

    PS_ObjectDialog     objectDialog;

    // full screen support
    bool                m_bFullScreen;
    WINDOWPLACEMENT     m_wplPrevious;
    CRect               m_rectFullScreenPlace;

protected:
    ResizeCB            resizeCB;
    void*               resizeCBData;

    // hide form task bar
    bool                m_bOwnerCreated;
    CWnd                m_wndOwnerWindow;

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(PC_PlatformPS)
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void PostNcDestroy();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    //}}AFX_VIRTUAL

    // Generated message map functions
protected:
    //{{AFX_MSG(PC_PlatformPS)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnClose();
    afx_msg void OnTGAOutput();
    afx_msg void OnPSOutput();
    afx_msg void OnPSOutputAuto();
    afx_msg void OnCopyToClipboard();
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnControlPlotObject();
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* pMMI);
    afx_msg void OnPlotToolbar();
    afx_msg void OnPlotControlbar();
    afx_msg void OnUpdatePlotToolbar(CCmdUI* pCmdUI);
    afx_msg void OnUpdatePlotControlbar(CCmdUI* pCmdUI);
    afx_msg void OnSetFocus(CWnd *pOldWnd);
    afx_msg void OnPlotMouseZoom();
    afx_msg void OnPlotMouseSelect();
    afx_msg void OnPlotRefresh();
    afx_msg void OnPlotPrint();
    afx_msg void OnPlotPrintToolbar();
    //}}AFX_MSG
    afx_msg void OnPlotInitView();
    afx_msg void OnPlotPreviousView();
    afx_msg void OnPlotResetView();
    afx_msg void OnPlotSetAxes();
    afx_msg void OnPlotResetAxes();
    afx_msg void OnPlotResetDump();
    afx_msg void OnPlotDump();
    afx_msg void OnPlotDumpResetIncCount();
    afx_msg void OnPlotPropagateView();
    afx_msg void OnPlotPropagateSize();
    afx_msg void OnPlotFullScreen();
    afx_msg void OnUpdatePlotResetDump(CCmdUI* pCmdUI);
    afx_msg void OnUpdatePlotFullScreen(CCmdUI* pCmdUI);
    afx_msg void OnUpdatePlotResetAxes(CCmdUI* pCmdUI);
    DECLARE_MESSAGE_MAP()


private:
            void    DrawMouseToolbarButtons();
            void    GetCurrentBitmap(BitmapArray& bitmapData,
                                     BITMAPINFOHEADER& header);

};

#endif // PC_PLATFORMPS_H

