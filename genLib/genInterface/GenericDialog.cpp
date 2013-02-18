///////////////////////////////////////////////////////////////////////////////////
//
// GenericDialog.cpp
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

#include <genInterface/GenericDialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if _MFC_VER < 0x0700
void AFXAPI DestructElements(ControlInfo** pElements, int nCount)
{
    for(int i = nCount; i > 0; i--, pElements++)
        delete(*pElements);
}
#endif

ResizableWindow::ResizableWindow() :
    m_nMinWidth(0),
    m_nMinHeight(0),
    m_nOld_cx(0),
    m_nOld_cy(0),
    m_bInSize(false)
{
}

ResizableWindow::~ResizableWindow()
{
}

ControlInfo& ControlInfo::Add(int nCtrlID, ResizeInfo resizeInfo)
{
    ControlInfo* pControlInfo = new ControlInfo;
    pControlInfo->m_nCtrlID = nCtrlID;
    pControlInfo->m_resizeInfo = resizeInfo;
    m_controlInfo.Add(pControlInfo);
    return *this;
}

void ResizableWindow::InitMinimal(int nMinX, int nMinY)
{
    m_nMinWidth  = nMinX;
    m_nMinHeight = nMinY;
}

void ResizableWindow::Reset()
{
    m_nMinWidth = m_nMinHeight = m_nOld_cx = m_nOld_cy = 0;
}

ControlInfo& ResizableWindow::SetControlInfo(UINT nCtrlId, ResizeInfo resizeInfo)
{
    // Add resizing behaviour for the control
    return m_controlInfo.Add(nCtrlId, resizeInfo);
}

void ResizableWindow::Sizing(CWnd* pWnd, int cx, int cy)
{
    if (m_bInSize) {
        return;
    }
    m_bInSize = true;

    // IMPORTANT: if necessary, resize dialog itself
    if (cx > m_nMinWidth || cy > m_nMinHeight) {
        CRect rc;
        pWnd->GetWindowRect(&rc);
        pWnd->GetParent()->ScreenToClient(rc);
        rc.right = rc.left + cx < m_nMinWidth ? m_nMinWidth : cx,
            rc.bottom = cy < m_nMinHeight ? m_nMinHeight : cy;
        pWnd->MoveWindow(rc);
    }

    int dx = cx - m_nOld_cx;
    int dy = cy - m_nOld_cy;

    if (m_nOld_cx != 0) {
        // Move and Size the controls using the information
        // we got in SetControlInfo()
        CRect rectWnd;
        for (int i = 0; i < m_controlInfo.m_controlInfo.GetSize(); i++) {
            ControlInfo& controlInfo = (*m_controlInfo.m_controlInfo[i]);
            CWnd* pWndCtrl = pWnd->GetDlgItem(controlInfo.m_nCtrlID);
            if (pWndCtrl == NULL) {
                TRACE("Control ID - %d NOT FOUND!!\n",controlInfo.m_nCtrlID);
                continue;
            }

            if (controlInfo.m_resizeInfo == resizeNone) {
                continue; // do nothing if anchored to top and orleft
            }

            ResizeInfo resizeInfo = controlInfo.m_resizeInfo;
            pWndCtrl->GetWindowRect(&rectWnd);
            pWnd->ScreenToClient(&rectWnd);

            pWnd->InvalidateRect(rectWnd); // to avoid mouse trails

            // to change horizontal position
            if (cx >= m_nMinWidth) {
                if (resizeInfo & resizeHorizontal) {
                    rectWnd.right += dx;
                } else if (resizeInfo & anchoreRight) {
                    rectWnd.OffsetRect(dx,0);
                }
            }

            // to change vertical position
            if (cy >= m_nMinHeight) {
                if (resizeInfo & resizeVertical) {
                    rectWnd.bottom += dy;
                } else if (resizeInfo & anchoreBottom) {
                    rectWnd.OffsetRect(0,dy);
                }
            }

            // replace control if something has changed
            if (cx >= m_nMinWidth || cy >= m_nMinHeight) {
                pWndCtrl->MoveWindow(&rectWnd);
                if (controlInfo.m_controlInfo.GetSize() > 0) {
                    // resize children
                }
            }
        }
    }
    // Save old dialog size
    if (cx >= m_nMinWidth) {
        m_nOld_cx = cx;
    }
    if (cy >= m_nMinHeight) {
        m_nOld_cy = cy;
    }
    m_bInSize = false;
}

/////////////////////////////////////////////////////////////////////////////
// CGenericDialog dialog

IMPLEMENT_DYNAMIC(CGenericDialog, CDialog)

CGenericDialog::CGenericDialog(UINT nIDTemplate, CWnd* pParentWnd) :
CDialog(nIDTemplate, pParentWnd)
{
}

InterfaceNode* CGenericDialog::GetInterfaceNode()
{
    return m_pNode;
}


BEGIN_MESSAGE_MAP(CGenericDialog, CDialog)
//{{AFX_MSG_MAP(CGenericDialog)
ON_WM_SIZE()
ON_WM_CREATE()
ON_WM_DESTROY()
ON_WM_SHOWWINDOW()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////
// CGenericDialog message handlers

BOOL CGenericDialog::OnInitDialog()
{
    CDialog::OnInitDialog();
    return FALSE;
}

BOOL CGenericDialog::CreateDlg(CWnd* pParent, InterfaceNode* pNode)
{
    m_pNode = pNode;
    return Create(m_lpszTemplateName, pParent);
}

//
// Set the dialog controls new position and size
void CGenericDialog::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    if (nType == SIZE_MINIMIZED) {
        return;
    }

    ResizableWindow::Sizing(this, cx, cy);
}

void CGenericDialog::OnDestroy()
{
    // Important: Reset the internal values in case of reuse of the dialog
    // without deleting.
    ResizableWindow::Reset();
    CDialog::OnDestroy();
}

int CGenericDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Remember the original size so later we can calculate
    // how to place the controls on dialog Resize
    ResizableWindow::InitMinimal(lpCreateStruct->cx, lpCreateStruct->cy);
    return 0;
}

// Oh mein Gott...
// Through the maze of MFC/Win32 message processing...
// see CDialog::PreTranslateMessage(MSG* pMsg)
BOOL CGenericDialog::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN) {
        if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {
            ::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);
            return TRUE;                    // DO NOT process further
        }
        if (pMsg->wParam == VK_TAB && GetKeyState(VK_CONTROL) < 0) {
            return FALSE;
        }
        if (pMsg->wParam == VK_F11) {
            // don't eat window selector
            return FALSE;
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}

void CGenericDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);
    UpdateData(!bShow);

    //  CtrlUpdate virtual
    if (bShow) {
        ResetAllControls();
    }
}

void CGenericDialog::InitData(bool bFirstInit)
{
    /* OnShow replaced this call
       if (!bFirstInit) {
       UpdateData(FALSE);
       }
    */

    CheckRestrictions();
}

void CGenericDialog::CheckRestrictions()
{
}

void CGenericDialog::DoCtrlUpdate()
{
    CheckRestrictions();
}

