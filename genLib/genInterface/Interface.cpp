///////////////////////////////////////////////////////////////////////////////////
//
// Interface.cpp
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

#include <genInterface/Interface.h>
#include <genInterface/OutlookFrame.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

Interface* InterfaceNode::applicationInterface = NULL;

////////////////////////////////////////////////////////////////////////////////////////
InterfaceNode::InterfaceNode(InterfaceNode* pParent, int nIdent,
                             NodeDescription* pDescription) :
    m_nIdent(nIdent),
    m_pDescription(pDescription),
    m_bCreated(false),
    m_pParent(pParent)
{
}

InterfaceNode::~InterfaceNode()
{
    DestroyNode();
}

InterfaceNode* InterfaceNode::GetParentNode()
{
    return m_pParent;
}

void InterfaceNode::DestroyNode()
{
    for (int i = 0; i < m_aNode.GetSize(); i++) {
        delete m_aNode[i];
    }
    m_aNode.RemoveAll();
}

InterfaceNode* InterfaceNode::GetNode(int nItem)
{
    return m_aNode[nItem];
}

void InterfaceNode::AddNode(InterfaceNode* pNode)
{
    m_aNode.Add( pNode );
}

const char* InterfaceNode::GetName()
{
    return m_pDescription->m_szName;
}

int InterfaceNode::GetImage()
{
    return m_pDescription->m_ImageID;
}

bool InterfaceNode::CreateNode(CWnd*)
{
    m_bCreated = true;
    return true;
}

CWnd* InterfaceNode::ShowNode(bool bShow, CRect* pRect)
{
    return NULL;
}

bool InterfaceNode::Enable(int nIdent, bool bEnable)
{
    return false;
}

void InterfaceNode::Show(int nIdent)
{
    return;
}

bool InterfaceNode::IsCreated()
{
    return m_bCreated;
}

Interface* InterfaceNode::GetInterface()
{
    return applicationInterface;
}

void InterfaceNode::CheckRestrictions()
{
    GetParentNode()->CheckRestrictions();
}

void InterfaceNode::RestoreControl(InterfaceContext& context)
{
}

bool InterfaceNode::DescriptionOK()
{
    return m_pDescription != NULL && IsCreated();
}


ListFactory* InterfaceNode::GetListFactory()
{
    if (!DescriptionOK()) {
        return NULL;
    }
    return m_pDescription->m_pListFactory;
}



////////////////////////////////////////////////////////////////////////////////////////
FolderNode::FolderNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    InterfaceNode(pParent, nIdent, pDescription),
    m_nNodeSel(-1)
{
}

FolderNode::~FolderNode()
{
}

InterfaceNode* FolderNode::SelectNode(int nNode)
{
    m_nNodeSel = nNode;
    return GetNode(m_nNodeSel);
}

InterfaceNode* FolderNode::GetSelNode()
{
    if (m_nNodeSel < 0) {
        return NULL;
    }
    return GetNode(m_nNodeSel);
}

void FolderNode::DestroyNode()
{
    InterfaceNode::DestroyNode();
    m_nNodeSel = -1;
}

void FolderNode::PushContext()
{
    PushContext(InterfaceNode::GetInterface()->m_interfaceContext);
}

void FolderNode::PushContext(InterfaceContext& context)
{
    context.Push(m_nNodeSel);
    FolderNode* pParent = dynamic_cast<FolderNode*>(GetParentNode());
    if (pParent != NULL) {
        pParent->PushContext(context);
    }
}

void FolderNode::PopContext(InterfaceContext& context, bool bControl)
{
    int nNodeSel = context.Pop();
    if (nNodeSel == -1 || nNodeSel >= m_aNode.GetSize()) {
        nNodeSel = 0;
    }
    Show(nNodeSel);
    FolderNode* pFolder = dynamic_cast<FolderNode*>(m_aNode[nNodeSel]);
    if (pFolder != NULL) {
        pFolder->PopContext(context, bControl);
    } else if (bControl) {
        m_aNode[nNodeSel]->RestoreControl(context);
    }
}


ListFactory* FolderNode::GetListFactory()
{
    if (!DescriptionOK())
        return NULL;

    ListFactory* currLst = NULL;
    //  look for selected listing first
    if (m_nNodeSel >= 0) {
        currLst = m_aNode[m_nNodeSel]->GetListFactory();
    }

    // fall back to current
    if (currLst == NULL) {
        currLst = m_pDescription->m_pListFactory;
    }

    return currLst;
}


////////////////////////////////////////////////////////////////////////////////////////
Interface::Interface(InterfaceNode* pParent, int nIdent,
                     NodeDescription* pDescription,
                     InterfaceContext& interfaceContext) :
    FolderNode(pParent, nIdent, pDescription),
    m_pFrameWnd(NULL),
    m_pDocument(NULL),
    m_interfaceContext(interfaceContext),
    m_bRestore(false)
{
    applicationInterface = this;
}

bool Interface::CreateNode(CWnd* pParentWnd, CDocument* pDocument)
{
    m_pFrameWnd = pParentWnd;
    m_pDocument = pDocument;
    m_bCreated = true;
    return true;
}

CImageList* Interface::GetImageListSmall()
{
    COutlookFrame* pFrame = static_cast<COutlookFrame*>(m_pFrameWnd);
    return &pFrame->m_imaSmall;
}

CImageList* Interface::GetImageListLarge()
{
    COutlookFrame* pFrame = static_cast<COutlookFrame*>(m_pFrameWnd);
    return &pFrame->m_imaLarge;
}

void Interface::SetInfo(const CString& str, int nImage)
{
    COutlookFrame* pFrame = static_cast<COutlookFrame*>(m_pFrameWnd);
    m_sInfoText = str;
    pFrame->m_wndInfoBar.SetText(str);
    CImageList* pImageList = GetImageListSmall();
    HICON hIcon = pImageList->ExtractIcon(nImage);
    pFrame->m_wndInfoBar.SetIcon(hIcon);
}

CString     Interface::GetInfoText()
{
    return m_sInfoText;
}

void Interface::RestoreContext(bool bRestoreControl)
{
    m_bRestore = true;
    try {
        PopContext(m_interfaceContext, bRestoreControl);
    } catch(...) {
        m_bRestore = false;
        throw;
    }
    m_bRestore = false;
}

void Interface::SaveContext(InterfaceNode* pNode)
{
    if (m_bRestore) {
        return;
    }
    m_interfaceContext.Clear();
    static_cast<FolderNode*>(pNode->GetParentNode())->PushContext(m_interfaceContext);
}

void Interface::SaveContextFocus(CWnd* pDialog, HWND hWnd)
{
    m_interfaceContext.m_pDialogLastFocus = pDialog;
    m_interfaceContext.m_hWndLastFocus = hWnd;
}

void Interface::ClearContext()
{
    m_interfaceContext.Clear();
}


FormContainer* Interface::GetFormContainer()
{
    /*
      CProjectFrame* pFrame = static_cast<CProjectFrame*>(AfxGetMainWnd());

      VERIFY(pFrame);
      FolderNode* pFolderNode = static_cast<FolderNode*>(pFrame->m_pInterface->GetOutlookPane()->GetNode(0));
      InterfaceNode* pInterfaceNode = pFolderNode->GetSelNode();
      FolderNode* pFolderNode2 = dynamic_cast<FolderNode*>(pInterfaceNode);
      while (pFolderNode2 != NULL) {
      pInterfaceNode = pFolderNode2->GetSelNode();
      pFolderNode2 = dynamic_cast<FolderNode*>(pInterfaceNode);
      }
      if (dynamic_cast<MViewNode*>(pInterfaceNode) == NULL) {
      return NULL;
      }
      MViewNode* pNode = static_cast<MViewNode*>(pInterfaceNode);
      return static_cast<FormContainer*>(pNode->GetFormContainer());
    */
    return NULL;
}

