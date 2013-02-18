///////////////////////////////////////////////////////////////////////////////////
//
// InterfaceOutlook.h
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
//              Interface for the InterfaceVar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERFACEOUTLOOK_H__A1005FEB_E082_11D3_BCCC_00104B4C74FD__INCLUDED_)
#define AFX_INTERFACEOUTLOOK_H__A1005FEB_E082_11D3_BCCC_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <genInterface/Interface.h>

////////////////// Interface with outlook control in the left pane /////////
class OutlookPane;

class InterfaceOutlook : public Interface
{
public:
    InterfaceOutlook(InterfaceNode* pParent, int nIdent,
                     NodeDescription* pDescription,
                     InterfaceContext& interfaceContext);

    virtual bool    CreateNode(CWnd* pParentWnd, CDocument* pDocument);
    virtual void    DestroyNode();

    virtual void    ShowNode(InterfaceNode* pNode);
    virtual void    CheckRestrictions();

    virtual void    Show(int nIdent);

    OutlookPane*    GetOutlookPane();
    virtual COutBarCtrl* GetOutlookCtrl();

    CWnd*           GetView();
    void            ResetView();

    void            ShowActive();   // redisplays active node -- used for update after run

protected:
    virtual CWnd*   GetParentalWnd();

protected:
    InterfaceNode*  m_pActiveNode;
    CWnd*           m_pView;
};

inline CWnd* InterfaceOutlook::GetView()
{
    return m_pView;
}
inline void InterfaceOutlook::ResetView()
{
    m_pView = NULL;
}

#endif // AFX_INTERFACEOUTLOOK_H__A1005FEB_E082_11D3_BCCC_00104B4C74FD__INCLUDED_

