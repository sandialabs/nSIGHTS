///////////////////////////////////////////////////////////////////////////////////
//
// NPreInterfaceOutlook.cpp
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract
// DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
// retains certain rights in this software.
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


#include "stdafx.h"

#include "NPreInterfaceOutlook.h"
#include "NPreInterfaceApp.h"

#include "MainFrm.h"
#include "nPreDoc.h"
#include "nPreView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

NPreInterfaceOutlook::NPreInterfaceOutlook(InterfaceNode* pParent, int nIdent,
    NodeDescription* pDescription,
    InterfaceContext& interfaceContext) :
  InterfaceOutlook(pParent, nIdent, pDescription, interfaceContext)
{
  FolderNode* pMain = static_cast<FolderNode*>(GetNode(0));
  pMain->AddNode(new OutlookBar2(pMain, 0, &pDescription[0]));
}

bool NPreInterfaceOutlook::CreateNode(CWnd* pParentWnd, CDocument* pDocument)
{
  m_pFrameWnd = pParentWnd;
  SetInfo("nPre", 1);
  CMainFrame* pFrame = static_cast<CMainFrame*>(pParentWnd);

  CCreateContext context;
  context.m_pNewViewClass = RUNTIME_CLASS(CNPreView);
  context.m_pCurrentDoc = pDocument;
  context.m_pNewDocTemplate = NULL;
  context.m_pLastView = NULL;
  context.m_pCurrentFrame = (CFrameWnd*)m_pFrameWnd;
  m_pView = pFrame->CreateView(&context);

  return InterfaceOutlook::CreateNode(pParentWnd, pDocument);
}

void NPreInterfaceOutlook::ShowNode(InterfaceNode* pNode)
{
  InterfaceOutlook::ShowNode(pNode);

  CWnd* pWnd = GetParentalWnd();
  CRect rect;
  pWnd->GetClientRect(rect);
  static_cast<CNPreView*>(pWnd)->m_pClient = pNode->ShowNode(true, &rect);
}

