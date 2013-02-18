///////////////////////////////////////////////////////////////////////////////////
//
// InterfacemView.cpp
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

#include <genApp/C_MenuPage.h>
#include <genApp/C_AppMenu.h>

#include <genMFC/ProjectTreeView.h>
#include <genMFC/ProjectView.h>

#include <genInterface/InterfaceCtrls.h>

#include "InterfaceMView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int MENUTREE_VIEW_WIDTH = 150;

MViewNode::MViewNode(InterfaceNode* pParent, int nIdent,
                     NodeDescription* pDescription,
                     MenuRootC& projectRoot) :
    InterfaceNode(pParent, nIdent, pDescription),
    projectRoot(projectRoot)
{
}

MViewNode::~MViewNode()
{
}

FormContainer* MViewNode::GetFormContainer()
{
    CProjectView* pProjectView = static_cast<CProjectView*>(m_wndSplitter.GetPane(0,1));
    return static_cast<FormContainer*>(pProjectView);
}

bool MViewNode::CreateNode(CWnd* pParentWnd)
{
    VERIFY(m_wndSplitter.CreateStatic(pParentWnd,
                                      1, 2,
                                      WS_CHILD|WS_VISIBLE,
                                      AFX_IDW_PANE_FIRST+m_nIdent));

    CCreateContextProject context;
    context.m_pNewViewClass = RUNTIME_CLASS(CProjectTreeView);
    context.m_pCurrentDoc = Interface::GetInterface()->GetDocument();
    context.m_pNewDocTemplate = NULL;
    context.m_pLastView = NULL;
    context.m_pCurrentFrame = (CFrameWnd*)Interface::GetInterface()->GetFrame();
    context.m_pProjectRoot = &projectRoot;

    CSplitterWnd& sw = m_wndSplitter;


    context.m_pNewViewClass = RUNTIME_CLASS(CProjectView);
    if (!sw.CreateView(0, 1,
                       RUNTIME_CLASS(CProjectView),
                       CSize(100, 200),
                       &context) )
        {
            TRACE("***failed to create splitter view RIGHT\n");
            sw.DestroyWindow();
            return FALSE;
        }

    if (!sw.CreateView(0, 0,
                       RUNTIME_CLASS(CProjectTreeView),
                       CSize(MENUTREE_VIEW_WIDTH, 200), &context))
        {
            TRACE("***failed to create splitter view LEFT\n");
            sw.DestroyWindow();
            return FALSE;
        }
    // Double-check for ON_NOTIFY in message map above
    VERIFY(AFX_IDW_PANE_FIRST == sw.IdFromRowCol(0,0));

    // SetupUI
    CWnd* pTreeWnd = m_wndSplitter.GetPane(0, 0);
    projectRoot.SetupUI(pTreeWnd);

    AppMenuC::CreateObjectMenu();

    m_bCreated = true;
    return true;
}


// for app specific updating -- defaults do nothing
void MViewNode::BeforeShow(bool bShow)
{
}
void MViewNode::AfterShow(bool bShow)
{
}

CWnd* MViewNode::ShowNode(bool bShow, CRect* pRect)
{
    ASSERT(IsCreated() && ::IsWindow(m_wndSplitter.m_hWnd));
    m_wndSplitter.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
    if (bShow && pRect != NULL) {
        m_wndSplitter.SetWindowPos(NULL, pRect->left, pRect->top, pRect->Width(), pRect->Height(), SWP_NOZORDER);
    }

    BeforeShow(bShow);
    if (bShow)
        {
            // this resets the current root and sets the menu page and obj
            //  according to their internal selected status

            //  make sure it is setup
            CWnd* pWnd = m_wndSplitter.GetPane(0, 0);
            projectRoot.SetupUI(pWnd);

            projectRoot.SelectRoot();

            static_cast<CProjectTreeView*>(pWnd)->projectRoot = &projectRoot;
        }
    else
        {
            // this is a bit of a kluge
            // we need to close the menu (to avoid asserts when it is opened
            // next) but still retain MenuObjC::objSelected as true;
            projectRoot.UnSelectRoot();
        }
    AfterShow(bShow);

    if (bShow) {
        GetInterface()->SaveContext(this);
    }

    if (bShow)
        {
            CString str;
            if ((GetParentNode() != NULL) && (dynamic_cast<OutlookBar*>(GetParentNode()) == NULL))
                {
                    str.Format("%s - %s", GetParentNode()->GetName(), GetName());
                } else
                {
                    str.Format("%s", GetName());
                }
            int nImage = GetImage();
            InterfaceNode::GetInterface()->SetInfo(str, nImage);
            AppMenuC::IndicatePropMenuOpen();
        }

    AppMenuC::SetMenuBarSensitive();

    return &m_wndSplitter;
}

bool MViewNode::Enable(int nIdent, bool bEnable)
{
    return false;
}

void MViewNode::Show(int nIdent)
{
}

void MViewNode::CheckRestrictions()
{
    GetParentNode()->CheckRestrictions();
}

