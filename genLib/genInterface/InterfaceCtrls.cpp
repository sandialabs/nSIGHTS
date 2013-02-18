///////////////////////////////////////////////////////////////////////////////////
//
// InterfaceCtrls.cpp
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
#include <GenLibRes.h>

#include <genApp/ProjectUtil.h>

#include <genInterface/InterfaceMView.h>
#include <genInterface/InterfaceCtrls.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

DialogNode::DialogNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    InterfaceNode(pParent, nIdent, pDescription),
    m_pDlg(NULL)
{
}

DialogNode::~DialogNode()
{
    if (m_pDlg != NULL) {
        delete m_pDlg;
    }
}

CDialog* DialogNode::GetDialog()
{
    return m_pDlg;
}

bool DialogNode::CreateNode(CWnd* pParentWnd)
{
    CCreateContext context;
    context.m_pNewViewClass = RUNTIME_CLASS(CDialogScrollView);
    context.m_pCurrentDoc = Interface::GetInterface()->GetDocument();
    context.m_pNewDocTemplate = NULL;
    context.m_pLastView = NULL;
    context.m_pCurrentFrame = (CFrameWnd*)Interface::GetInterface()->GetFrame();
    CDialogScrollView::Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0,0,0,0), pParentWnd, AFX_IDW_PANE_FIRST+m_nIdent, &context);

    m_pDlg = static_cast<CGenericDialog*>(m_pDescription->m_rtDialog->CreateObject());
    ASSERT_KINDOF(CGenericDialog, m_pDlg);
    if (!static_cast<CGenericDialog*>(m_pDlg)->CreateDlg(this, this)) {
        return false;
    }
    m_pDlg->SetDlgCtrlID(AFX_IDW_PANE_FIRST+m_nIdent);
    m_bCreated = true;
    return true;
}

CWnd* DialogNode::ShowNode(bool bShow, CRect* pRect)
{
    ASSERT(IsCreated() && m_pDlg != NULL);
    ShowWindow(bShow ? SW_SHOW : SW_HIDE);
    m_pDlg->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
    if (bShow && pRect != NULL) {
        SetWindowPos(NULL, pRect->left, pRect->top, pRect->Width(), pRect->Height(), SWP_NOZORDER);
    }
    if (bShow)
        {
            InterfaceNode::GetInterface()->SaveContext(this);

            CString str;

            InterfaceNode*  currNode = this;

            while (currNode != NULL)
                {
                    const char* nodeName = currNode->GetName();
                    if (strcmp(nodeName, InterfaceNode::GetInterface()->GetName()) == 0)
                        break;

                    if (currNode != this)
                        str.Insert(0, " - ");

                    str.Insert(0, currNode->GetName());

                    currNode = currNode->GetParentNode();
                }


            /*

              parent = GetParentNode();
              if (parent != NULL)
              {
              InterfaceNode*  grandParent = parent->GetParentNode();
              if ((grandParent != NULL) && (grandParent->GetParentNode() != InterfaceNode::GetInterface()))
              {
              str.Format("%s - %s - %s", grandParent->GetName(), parent->GetName(), GetName());
              }
              else
              {
              str.Format("%s - %s", parent->GetName(), GetName());
              }
              }
              else
              str.Format("%s", GetName());
            */
            int nImage = GetImage();
            InterfaceNode::GetInterface()->SetInfo(str, nImage);
        }

    return this;
}

bool DialogNode::Enable(int nIdent, bool bEnable)
{
    return false;
}

void DialogNode::Show(int nIdent)
{
}

void DialogNode::CheckRestrictions()
{
    GetParentNode()->CheckRestrictions();
}

void DialogNode::RestoreControl(InterfaceContext& context)
{
    if (m_pDlg == context.m_pDialogLastFocus) {
        ASSERT(::IsWindow(context.m_hWndLastFocus));
        ::SetFocus(context.m_hWndLastFocus);
        ::SendMessage(context.m_hWndLastFocus, EM_SETSEL, 0, -1);
        ::SendMessage(context.m_hWndLastFocus, EM_SCROLLCARET, 0, 0);
        static_cast<FolderNode*>(GetParentNode())->PushContext();
    }
}

BEGIN_MESSAGE_MAP(DialogNode, CDialogScrollView)
//{{AFX_MSG_MAP(DialogNode)
ON_WM_HELPINFO()
//}}AFX_MSG_MAP
// Standard printing commands
END_MESSAGE_MAP()

BOOL DialogNode::OnHelpInfo(HELPINFO* pHelpInfo)
{
    CDialog* pDlg = GetDialog();
    if (pDlg != NULL) {
        DWORD nHelpId = pDlg->GetDlgCtrlID() + 0x20000;
        AfxGetApp()->WinHelp(nHelpId);
        return TRUE;
    }
    return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
DlgNode::DlgNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    InterfaceNode(pParent, nIdent, pDescription),
    m_pDlg(NULL)
{
}

DlgNode::~DlgNode()
{
    if (m_pDlg != NULL) {
        delete m_pDlg;
    }
}

bool DlgNode::CreateNode(CWnd* pParentWnd)
{
    m_pDlg = static_cast<CGenericDialog*>(m_pDescription->m_rtDialog->CreateObject());
    ASSERT_KINDOF(CGenericDialog,m_pDlg);
    if (!static_cast<CGenericDialog*>(m_pDlg)->CreateDlg(pParentWnd, this)) {
        return false;
    }
    m_pDlg->SetDlgCtrlID(AFX_IDW_PANE_FIRST+m_nIdent);
    m_bCreated = true;
    return true;
}

CWnd* DlgNode::ShowNode(bool bShow, CRect* pRect)
{
    ASSERT(IsCreated() && m_pDlg != NULL);
    m_pDlg->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
    if (bShow && pRect != NULL) {
        m_pDlg->SetWindowPos(NULL, pRect->left, pRect->top, pRect->Width(), pRect->Height(), SWP_NOZORDER);
    }
    return m_pDlg;
}

bool DlgNode::Enable(int nIdent, bool bEnable)
{
    return false;
}

void DlgNode::Show(int nIdent)
{
}

CDialog* DlgNode::GetDialog()
{
    return m_pDlg;
}

//////////////////////////////////////////////////////////////////////////////////
OutlookPane::OutlookPane(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    FolderNode( pParent, nIdent, pDescription),
    COutBarCtrl()
{
}

bool OutlookPane::CreateNode(CWnd* pParentWnd)
{
    pParentWnd;
    for (int i = 0; i < m_aNode.GetSize(); i++) {
        m_aNode[i]->CreateNode(this);
    }
    m_bCreated = true;
    return true;
}

BEGIN_MESSAGE_MAP(OutlookPane, COutBarCtrl)
//{{AFX_MSG_MAP(OutlookPane)
ON_COMMAND(ID_UIX_LARGEICON, OnUixLargeicon)
ON_COMMAND(ID_UIX_SMALLICON, OnUixSmallicon)
//}}AFX_MSG_MAP
ON_MESSAGE(WM_OUTBAR_NOTIFY, OnOutbarNotify)
END_MESSAGE_MAP()

void OutlookPane::OnUixLargeicon()
{
    COutBarCtrl::OnUixLargeicon();
    projectUtilities::SetOutbarIconSize(false);
}

void OutlookPane::OnUixSmallicon()
{
    COutBarCtrl::OnUixSmallicon();
    projectUtilities::SetOutbarIconSize(true);
}

LRESULT OutlookPane::OnOutbarNotify(WPARAM wParam, LPARAM lParam)
{
    switch (wParam) {
    case NM_OB_ITEMCLICK:
        {
            int nIndex = int(lParam);
            int nFolder = GetSelFolder();
            ASSERT(nFolder == 0);
            TRACE2("Clicked on folder %d - item <%s>", nFolder, GetItemText(nIndex));
            static_cast<OutlookBar*>(m_aNode[0])->Show(nIndex);
            return 0;
        }
    case NM_FOLDERCHANGE:
        {
            int nFolder = int(lParam);
            ASSERT(nFolder == 0);
            return 0;
        }
    case NM_OB_ONLABELENDEDIT:
        return 1;
    case NM_OB_ONGROUPENDEDIT:
        return 1;
    case NM_OB_DRAGITEM:
        return 1;
    }
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////
OutlookBar::OutlookBar(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    FolderNode(pParent, nIdent, pDescription)
{
}

bool OutlookBar::CreateNode(CWnd* pParentWnd)
{
    COutBarCtrl::CBarFolder* pFolder = static_cast<OutlookPane*>(pParentWnd)->AddFolder( GetName() );

    for (int i = 0; i < m_aNode.GetSize(); i++ ) {
        InterfaceNode* pNode = m_aNode[i];
        pFolder->InsertItem(pNode->GetName(), pNode->GetImage());
    }
    m_bCreated = true;
    return true;
}

void OutlookBar::Show(int nIdent)
{
    InterfaceNode::GetInterface()->ShowNode(SelectNode(nIdent));
}

//////////////////////////////////////////////////////////////////////////////////
TabbedFolderNode::TabbedFolderNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    FolderNode(pParent, nIdent, pDescription),
    m_nTabLevel(0)
{
}

int TabbedFolderNode::GetTabLevel()
{
    InterfaceNode* pParentNode = GetParentNode();
    int nTabLevel = 0;
    do {
        if (dynamic_cast<TabbedFolderNode*>(pParentNode) != NULL) {
            nTabLevel++;
        }
        pParentNode = pParentNode->GetParentNode();
    } while (pParentNode != NULL);
    return nTabLevel;
}

bool TabbedFolderNode::CreateNode(CWnd* pParentWnd)
{
    DWORD dwStyle = TCS_TABS|WS_CHILD|WS_VISIBLE|TCS_OWNERDRAWFIXED;
    return CreateNodeEx(pParentWnd, dwStyle);
}

bool TabbedFolderNode::CreateNodeEx(CWnd* pParentWnd, DWORD dwStyle)
{
    if (!::IsWindow(m_hWnd)) { // It can be part of dialog resource
        m_nTabLevel = GetTabLevel();
        CTabCtrlEx::Create( dwStyle, CRect(0,0,0,0),
                            pParentWnd,
                            AFX_IDW_PANE_FIRST+m_nTabLevel*100+m_nIdent);
        if (InterfaceNode::GetInterface() != NULL) {
            CTabCtrlEx::SetImageList(InterfaceNode::GetInterface()->GetImageListSmall());
        }
    }

    SetupFolder();

    m_bCreated = true;
    return true;
}

void TabbedFolderNode::SetupFolder()
{
    for (int i = 0; i < m_aNode.GetSize(); i++) {
        AddTab(LPTSTR(m_aNode[i]->GetName()), m_aNode[i]->GetImage());
    }
    SetCurSel(0);
    m_aNode[0]->CreateNode(this);

    CRect rect;
    GetInsideRect(rect);
    if (m_nNodeSel == -1) {
        m_nNodeSel = 0;
    }
    m_aNode[0]->ShowNode(true, &rect);
}


CWnd* TabbedFolderNode::ShowNode(bool bShow, CRect* pRect)
{
    ShowWindow(bShow ? SW_SHOW : SW_HIDE);
    if (bShow && pRect != NULL) {
        SetWindowPos(NULL, pRect->left, pRect->top, pRect->Width(), pRect->Height(), SWP_NOZORDER);
    }
    CRect rect;
    GetInsideRect(rect);
    m_aNode[m_nNodeSel]->ShowNode(bShow, &rect);
    return this;
}

BEGIN_MESSAGE_MAP(TabbedFolderNode, CTabCtrlEx)
//{{AFX_MSG_MAP(TabbedFolderNode)
ON_WM_SIZE()
ON_WM_HELPINFO()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void TabbedFolderNode::OnSize(UINT nType, int cx, int cy)
{
    CTabCtrlEx::OnSize(nType, cx, cy);

    if (nType != SIZE_MINIMIZED ) {
        int nSel = GetCurSel();
        if (nSel >= 0) {
            CWnd* pWnd = NULL;
            int nIdent = m_aNode[nSel]->GetIdent();
            if (dynamic_cast<TabbedFolderNode*>(m_aNode[nSel]) != NULL) {
                pWnd = GetDlgItem(AFX_IDW_PANE_FIRST + (m_nTabLevel+1)*100 + nIdent);
            } else {
                pWnd = GetDlgItem(AFX_IDW_PANE_FIRST+nIdent);
            }
            if (pWnd != NULL) {
                CRect rect;
                GetInsideRect(rect);
                pWnd->MoveWindow(&rect);
            }
        }
    }
}

BOOL TabbedFolderNode::OnHelpInfo(HELPINFO* pHelpInfo)
{
    InterfaceNode* pNode = GetSelNode();
    if (pNode != NULL) {
        DialogNode* pDialogNode = dynamic_cast<DialogNode*>(pNode);
        if (pDialogNode != NULL) {
            DWORD nHelpId = pDialogNode->GetDlgCtrlID() + 0x20000;
            AfxGetApp()->WinHelp(nHelpId);
            return TRUE;
        }
    }
    return FALSE;
}

void TabbedFolderNode::GetInsideRect(CRect& rect)
{
    GetClientRect(rect);
    AdjustRect(FALSE, &rect);
}

bool TabbedFolderNode::Enable(int nIdent, bool bEnable)
{
    return EnableTab(nIdent, bEnable);
}

void TabbedFolderNode::Show(int nIdent)
{
    ASSERT(IsCreated());
    if (m_nNodeSel != -1
        && m_aNode[m_nNodeSel]->IsCreated()) // a bit paranoia for my own bugs
        {
            m_aNode[m_nNodeSel]->ShowNode(false);
        }

    if (!m_aNode[nIdent]->IsCreated()) {
        m_aNode[nIdent]->CreateNode(this);
    }
    m_nNodeSel = nIdent;
    if (GetCurSel() != nIdent) {
        SelectTab(nIdent);
    }
    CRect rect;
    GetInsideRect(rect);
    m_aNode[nIdent]->ShowNode(true, &rect);
    //ShowNode(true);
}

void TabbedFolderNode::OnSelection(int nNewTab)
{
    SetFocus();
    if (GetFocus() == this) { // in the case of exceptional change of focus
        Show(nNewTab);
    }
}

void TabbedFolderNode::SelectFirstEnabledTab()
{
    int nCurrentIndex = GetCurSel();
    if (nCurrentIndex != -1) {
        if (!IsTabEnabled(nCurrentIndex)) {
            for (int i = 0; i < GetItemCount(); i++) {
                if (IsTabEnabled(i)) {
                    SelectTab(i);
                    break;
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
TabbedFolderNode1::TabbedFolderNode1(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
TabbedFolderNode(pParent, nIdent, pDescription)
{
    NodeDescriptionInterator iter(pDescription->GetNext());
    iter.First();
    for (int i = 0; !iter.IsLast(); i++,iter.Next()) {
        NodeDescription* pItem = iter.Get();
        if (pItem->m_rtDialog != NULL) {
            DialogNode* pDialog = new DialogNode(this, i, pItem);
            AddNode(pDialog);
        } else {
            TabbedFolderNode* pTab = new TabbedFolderNode(this, i, pItem);
            AddNode(pTab);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
TabbedFolderNodeRes::TabbedFolderNodeRes(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    TabbedFolderNode(pParent, nIdent, pDescription)
{
    NodeDescriptionInterator iter(pDescription);
    iter.First();
    for (int i = 0; !iter.IsLast(); i++,iter.Next()) {
        NodeDescription* pItem = iter.Get();
        if (pItem->m_rtDialog != NULL) {
            DlgNode* pDialog = new DlgNode(this, i, pItem);
            AddNode(pDialog);
        } else {
            TabbedFolderNode* pTab = new TabbedFolderNode(this, i, pItem);
            AddNode(pTab);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
TabbedFolderRoot::TabbedFolderRoot(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    TabbedFolderNode(pParent, nIdent, pDescription)
{
}

bool TabbedFolderRoot::CreateNode(CWnd* pParentWnd)
{
    DWORD dwStyle = TCS_TABS|WS_CHILD|WS_VISIBLE;
    return CreateNodeEx(pParentWnd, dwStyle);
}

