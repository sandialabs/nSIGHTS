///////////////////////////////////////////////////////////////////////////////////
//
// MainFrm.cpp
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
// DESCRIPTION: SingleFrame derived class
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Resource.h"

#include "App/nPostRoot.h"
#include "App/nPostAppMenu.h"

#include "MainFrm.h"
#include "ProjectDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CMainFrame, CSingleFrame)

BEGIN_MESSAGE_MAP(CMainFrame, CSingleFrame)
//{{AFX_MSG_MAP(CMainFrame)
ON_WM_CREATE()
ON_WM_SYSCOMMAND()
//}}AFX_MSG_MAP
ON_COMMAND(ID_FILE_UPDATEPATHS, CProjectFrame::OnFilePathUpdate)

ON_COMMAND(ID_OBJECT_APPLY, CProjectFrame::OnObjectApply)
ON_COMMAND(ID_OBJECT_COPY, CProjectFrame::OnObjectCopy)
ON_COMMAND(ID_OBJECT_COPYPAGE, CProjectFrame::OnObjectCopyPage)
ON_COMMAND(ID_OBJECT_DELETE, CProjectFrame::OnObjectDelete)
ON_COMMAND(ID_OBJECT_DUPLICATE, CProjectFrame::OnObjectDuplicate)
ON_COMMAND(ID_OBJECT_PASTE, CProjectFrame::OnObjectPaste)
ON_COMMAND(ID_OBJECT_LISTCONNECTIONS, CProjectFrame::OnObjectListConnections)
ON_COMMAND(ID_OBJECT_CREATEPLOTFOLDER, CProjectFrame::OnCreatePlotFolder)

ON_UPDATE_COMMAND_UI(ID_OBJECT_APPLY, CProjectFrame::OnUpdateObjectApply)
ON_UPDATE_COMMAND_UI(ID_OBJECT_COPY, CProjectFrame::OnUpdateObjectCopy)
ON_UPDATE_COMMAND_UI(ID_OBJECT_COPYPAGE, CProjectFrame::OnUpdateObjectCopyPage)
ON_UPDATE_COMMAND_UI(ID_OBJECT_DELETE, CProjectFrame::OnUpdateObjectDelete)
ON_UPDATE_COMMAND_UI(ID_OBJECT_DUPLICATE, CProjectFrame::OnUpdateObjectDuplicate)
ON_UPDATE_COMMAND_UI(ID_OBJECT_PASTE, CProjectFrame::OnUpdateObjectPaste)
ON_UPDATE_COMMAND_UI(ID_OBJECT_LISTCONNECTIONS, CProjectFrame::OnUpdateObjectListConnections)
ON_UPDATE_COMMAND_UI(ID_OBJECT_CREATEPLOTFOLDER, CProjectFrame::OnUpdateCreatePlotFolder)


ON_COMMAND(ID_PAGE_DUPLICATE, CProjectFrame::OnPageDuplicate)
ON_COMMAND(ID_PAGE_DELETE, CProjectFrame::OnPageDelete)
ON_COMMAND(ID_PAGE_COPYCURRENT, CProjectFrame::OnPageCopyCurrent)
ON_COMMAND(ID_PAGE_COPYALL, CProjectFrame::OnPageCopyAll)
ON_COMMAND(ID_PAGE_PASTE, CProjectFrame::OnPagePaste)
ON_COMMAND(ID_PAGE_LISTALLCONNECTIONS, CProjectFrame::OnPageListAllConnections)
ON_COMMAND(ID_PAGE_COLLAPSETREE, CProjectFrame::OnPageCollapseAll)
ON_UPDATE_COMMAND_UI(ID_PAGE_DUPLICATE, CProjectFrame::OnUpdatePageDuplicate)
ON_UPDATE_COMMAND_UI(ID_PAGE_DELETE, CProjectFrame::OnUpdatePageDelete)
ON_UPDATE_COMMAND_UI(ID_PAGE_COPYCURRENT, CProjectFrame::OnUpdatePageCopyCurrent)
ON_UPDATE_COMMAND_UI(ID_PAGE_COPYALL, CProjectFrame::OnUpdatePageCopyAll)
ON_UPDATE_COMMAND_UI(ID_PAGE_PASTE, CProjectFrame::OnUpdatePagePaste)
ON_UPDATE_COMMAND_UI(ID_PAGE_LISTALLCONNECTIONS, CProjectFrame::OnUpdatePageListAllConnections)
ON_UPDATE_COMMAND_UI(ID_PAGE_COLLAPSETREE, CProjectFrame::OnUpdatePageCollapseAll)

ON_COMMAND(ID_PAGE_NEWDATA, CProjectFrame::OnPageNewData)
ON_COMMAND(ID_PAGE_NEWLIST, CProjectFrame::OnPageNewList)
ON_COMMAND(ID_PAGE_NEWOUTPUT, OnPageNewOutput)
ON_COMMAND(ID_PAGE_NEW_3DXYZ, CProjectFrame::OnPageNew3dXYZ)
ON_COMMAND(ID_PAGE_NEW_2DXY, CProjectFrame::OnPageNew2dXY)
ON_COMMAND(ID_PAGE_NEW_COMPOSITE, CProjectFrame::OnPageNewComposite)
ON_COMMAND(ID_PAGE_BRING_PLOT_TO_TOP, CProjectFrame::OnPageBringPlotToTop)
ON_COMMAND(ID_WINDOW_LIST, CProjectFrame::OnWindowList)
ON_UPDATE_COMMAND_UI(ID_WINDOW_LIST, CProjectFrame::OnUpdateWindowList)
ON_COMMAND(ID_VIEW_SETTINGS, CProjectFrame::OnViewSettings)
ON_COMMAND(ID_MINIMIZE_ALL_PLOTS1, CProjectFrame::OnMinimizeAllPlots)
ON_UPDATE_COMMAND_UI(ID_MINIMIZE_ALL_PLOTS1, CProjectFrame::OnUpdateMinimizeAllPlots)

ON_COMMAND(ID_HELP_FINDER, CProjectFrame::OnHelpFinder)
ON_COMMAND(ID_HELP, CProjectFrame::OnHelp)
ON_COMMAND(ID_CONTEXT_HELP, CProjectFrame::OnContextHelp)
ON_COMMAND(ID_DEFAULT_HELP, CProjectFrame::OnHelpFinder)

END_MESSAGE_MAP()


CMainFrame::CMainFrame()
{
    m_nPulldownMenuFrom = ID_PULLMENU_START;
    m_nPulldownMenuTo = ID_PULLMENU_END;
    m_nWindowMenuStart = ID_WINDOW_MENU_START;
    m_nWindowMenuEnd = ID_WINDOW_MENU_END;
    m_nWindowMenuPos = 5;
    m_nTemplateMenuStart = ID_TEMPLATE_MENU_START;   //;ID_AUTOSETUP
    m_nTemplateMenuEnd = ID_TEMPLATE_MENU_END;
    m_nTemplateMenuPos = 3;
    AppMenuC::SetTemplateFileExtension("nPost");
    frameRoot = &projectRoot;
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::LoadTemplateFile(const char* tfName)
{
    CProjectDoc* docPtr =CProjectDoc::GetDoc();
    if (docPtr != NULL)
        {
            if (!docPtr->SaveModified())
                return;
            docPtr->OnOpenTemplateDocument(tfName);
        }
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    mainToolbarID = IDR_MAINFRAME;
    return CSingleFrame::OnCreate(lpCreateStruct);
}

void CMainFrame::OnPageNewOutput()
{
    nPostAppMenuC::NewOutputPage();
}

