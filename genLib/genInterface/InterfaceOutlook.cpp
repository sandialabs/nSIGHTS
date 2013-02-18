///////////////////////////////////////////////////////////////////////////////////
//
// InterfaceOutlook.cpp
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
// DESCRIPTION: Windows specific code for implementing Outlook style
//              application with dialogs and object trees.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genInterface/OutlookFrame.h>
#include <genInterface/InterfaceCtrls.h>
#include <genInterface/InterfaceOutlook.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

InterfaceOutlook::InterfaceOutlook(InterfaceNode* pParent, int nIdent,
                                   NodeDescription* pDescription,
                                   InterfaceContext& interfaceContext) :
    Interface(pParent, nIdent, pDescription, interfaceContext),
    m_pActiveNode(NULL),
    m_pView(NULL)
{
    FolderNode* pMain = new OutlookPane(this, nIdent, pDescription);
    AddNode(pMain);
    //  NodeDescription* pDesc = pDescription;
    //  pMain->AddNode(new OutlookBar2(pMain, 0, &pDesc[0])); move to derived
}

bool InterfaceOutlook::CreateNode(CWnd* pParentWnd, CDocument* pDocument)
{
    m_pFrameWnd = pParentWnd;

    /*
      SetInfo("paCalc", 1);
      CMainFrame* pFrame = static_cast<CMainFrame*>(pParentWnd);

      CCreateContext context;
      context.m_pNewViewClass = RUNTIME_CLASS(CPACalcView);
      context.m_pCurrentDoc = pDocument;
      context.m_pNewDocTemplate = NULL;
      context.m_pLastView = NULL;
      context.m_pCurrentFrame = (CFrameWnd*)m_pFrameWnd;
      m_pView     = pFrame->CreateView(&context);
    */

    COutlookFrame* pFrame = static_cast<COutlookFrame*>(pParentWnd);

    OutlookPane* pOutlookPane = GetOutlookPane();
    DWORD dwFlags = COutBarCtrl::fAnimation|COutBarCtrl::fSelHighlight;
    pOutlookPane->Create(WS_CHILD|WS_VISIBLE, CRect(0,0,0,0),
                         &pFrame->m_wndControlBar,
                         2,
                         dwFlags);
    pOutlookPane->SetOwner(&pFrame->m_wndControlBar);
    pOutlookPane->SetNotifyWnd(pOutlookPane);
    pOutlookPane->SetImageList(&pFrame->m_imaLarge, COutBarCtrl::fLargeIcon);
    pOutlookPane->SetImageList(&pFrame->m_imaSmall, COutBarCtrl::fSmallIcon);
    pOutlookPane->SetAnimationTickCount(10);
    pOutlookPane->SetAnimSelHighlight(200);

    pOutlookPane->CreateNode(pParentWnd);

    pOutlookPane->iSelFolder = 0;
    pOutlookPane->SetSelFolder(0);

    /* 1/9/2001 CMainFrame::ResetInterface solves it now
    //
    // resize control bar to adjust pOutlookBar rect
    // Is there any better way to do that ?
    CRect rect;
    pFrame->m_wndControlBar.GetClientRect(rect);
    pFrame->m_wndControlBar.SendMessage(WM_SIZE, 0, MAKELPARAM(rect.Width(), rect.Height()));
    */
    return Interface::CreateNode(pParentWnd, pDocument);
}

void InterfaceOutlook::DestroyNode()
{
    InterfaceNode::DestroyNode();
    if (m_pView != NULL && ::IsWindow(m_pView->m_hWnd)) {
        m_pView->DestroyWindow();
    }
}

CWnd* InterfaceOutlook::GetParentalWnd()
{
    return GetView();
}


void InterfaceOutlook::ShowActive()
{
    if (m_pActiveNode != NULL)
        {
            m_pActiveNode->ShowNode(true);
        }
}

void InterfaceOutlook::ShowNode(InterfaceNode* pNode)
{
    if (m_pActiveNode != NULL) {
        m_pActiveNode->ShowNode(false);
    }

    CWnd* pWnd = GetParentalWnd();

    if (!pNode->IsCreated()) {
        pNode->CreateNode(pWnd);
    }

    //  CRect rect;
    //  pWnd->GetClientRect(rect);
    //  static_cast<CPACalcView*>(pWnd)->m_pClient = pNode->ShowNode(true, &rect);

    m_pActiveNode = pNode;
}

void InterfaceOutlook::Show(int nIdent)
{
    //  FolderNode* pFolderNode = static_cast<FolderNode*>(GetOutlookPane()->GetNode(0));
    //  ShowNode(pFolderNode->SelectNode(nIdent));
}

COutBarCtrl* InterfaceOutlook::GetOutlookCtrl()
{
    return GetOutlookPane();
}

OutlookPane* InterfaceOutlook::GetOutlookPane()
{
    return static_cast<OutlookPane*>(m_aNode[0]);
}

void InterfaceOutlook::CheckRestrictions()
{
    GetOutlookPane()->m_aNode[0]->CheckRestrictions();
}
