///////////////////////////////////////////////////////////////////////////////////
//
// PC_Platform2DPS.cpp
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
//      adds capabilities for 2D plotting cursor reporting area.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include <genClass/U_Str.h>
#include <genClassPS/PS_UIBase.h>
#include <genPlotClass/C_PlotDef.h>
#include <genPlotClassPS/PC_Platform2DPS.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

PC_Platform2DPS::PC_Platform2DPS(const char* plotType, PD_2D& assPlt, OGL2DBase& ogl2DObj) :
    base2DplotDef(assPlt),
    PC_PlatformPS(plotType, assPlt, ogl2DObj, 26, 77, 66)
{
    lastWidgetAttached = 0;
    m_wndStatusBar.isFixedAndVisible = base2DplotDef.showReportArea;
}

PC_Platform2DPS::~PC_Platform2DPS ()
{
}

int PC_Platform2DPS::CalcStatusBarHeight()
{
    if (statusForm.IsSmall())
        return statusBarHeight2;

    return statusBarHeight;
}

void PC_Platform2DPS::UpdateControlBarStatus()
{
    PC_PlatformPS::UpdateControlBarStatus();
    base2DplotDef.showReportArea = CControlBarUpdate::IsFixedAndVisible(m_wndStatusBar);
}

int PC_Platform2DPS::CalcButtonBarHeight()
{
    return buttonBarHeight;
}

void PC_Platform2DPS::UpdateViewSettings()
{
}

void PC_Platform2DPS::InitGraphicsWindow()
{
    if (initComplete)
        return;

    PC_PlatformPS::InitGraphicsWindow();
    SetFrameIcon(IDI_2DPLOT);
    layoutStatusBar.MenuOpen();

    initComplete = true;
    RecalcLayout();
}

void PC_Platform2DPS::ResetGraphicsWindow()
{
    PC_PlatformPS::ResetGraphicsWindow();
    /*
    if (base2DplotDef.showReportArea)
        statusForm.Manage();
    else
        statusForm.Unmanage();
    */
    ShowControlBar(&m_wndStatusBar, base2DplotDef.showReportArea, TRUE);
}

UINT PC_Platform2DPS::GetMenuResourceID()
{
    return IDR_VIEW_2D;
}

BEGIN_MESSAGE_MAP(PC_Platform2DPS, PC_PlatformPS)
    //{{AFX_MSG_MAP(PC_PlatformPS)
    ON_UPDATE_COMMAND_UI(ID_PLOT_CONTROLBAR, OnUpdatePlotControlbar)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void PC_Platform2DPS::OnUpdatePlotControlbar(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(true);
//  pCmdUI->Enable(base2DplotDef.showReportArea);
    PC_PlatformPS::OnUpdatePlotControlbar(pCmdUI);
}


