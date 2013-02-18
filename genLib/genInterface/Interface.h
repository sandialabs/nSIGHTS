///////////////////////////////////////////////////////////////////////////////////
//
// Interface.h
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

// Interface.h: interface for the Interface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERFACE_H__A1005FE9_E082_11D3_BCCC_00104B4C74FD__INCLUDED_)
#define AFX_INTERFACE_H__A1005FE9_E082_11D3_BCCC_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <genClassPS/PS_FormContainer.h>

#include <genInterface/InterfaceDesc.h>
#include <genInterface/InterfaceContext.h>
#include <genUIExt/OutBarCtrl.h>
#include <genUIExt/TabCtrlEx.h>


//////////////////// Generic Interface node /////////////////////////
class Interface;

class InterfaceNode
{
public:
    InterfaceNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);
    virtual ~InterfaceNode();

    virtual bool    CreateNode(CWnd* pParentWnd);
    virtual void    DestroyNode();
    virtual CWnd*   ShowNode(bool bShow = true, CRect* pRect = NULL);
    virtual bool    Enable(int nIdent, bool bEnable = true); // Enable/Disable
    virtual void    Show(int nIdent);

    virtual const char*     GetName();
    virtual int             GetImage();
    int                     GetIdent() { return m_nIdent; }

    InterfaceNode*      GetRoot();
    void                AddNode(InterfaceNode* pNode);
    InterfaceNode*      GetNode(int nIdent);

    InterfaceNode*      GetParentNode();
    bool                IsCreated();
    virtual void        CheckRestrictions();

    static Interface*   GetInterface();

    virtual     void        RestoreControl(InterfaceContext& context);

    // listing/error procs
    virtual ListFactory*    GetListFactory();

protected:
    bool                    DescriptionOK();

public:
    CArray<InterfaceNode*,InterfaceNode*> m_aNode;

protected:
    InterfaceNode*      m_pParent;
    NodeDescription*    m_pDescription;
    int                 m_nIdent;
    bool                m_bCreated;

protected:
    static Interface*   applicationInterface;
};

//////////////////// Generic Interface folder definition //////////
class FolderNode : public InterfaceNode
{
public:
    FolderNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription);
    virtual ~FolderNode();

    virtual void            DestroyNode();
    virtual InterfaceNode*  SelectNode(int nNode);
    InterfaceNode*          GetSelNode();

    ListFactory*            GetListFactory();

    void                    PushContext();
    void                    PushContext(InterfaceContext& context);
    void                    PopContext(InterfaceContext& context, bool bControl);

protected:
    int         m_nNodeSel;
};

///////////////// Base Interface Definition ////////////////////////

class Interface : public FolderNode
{
    friend class FolderNode;
public:
    Interface(InterfaceNode* pParent, int nIdent,
              NodeDescription* pDescription,
              InterfaceContext& interfaceContext);

    virtual void ShowNode(InterfaceNode* pNode) {};
    virtual COutBarCtrl* GetOutlookCtrl() { return NULL; }

public:
    bool            CreateNode(CWnd* pParentWnd, CDocument* pDocument);

    CImageList*     GetImageListSmall();
    CImageList*     GetImageListLarge();
    void            SetInfo(const CString& str, int nImage);

    CString         GetInfoText();

    void            RestoreContext(bool bRestoreControl);
    void            SaveContext(InterfaceNode* pNode);
    void            SaveContextFocus(CWnd* pDialog, HWND hWnd);
    void            ClearContext();

    CWnd*           GetFrame();
    CDocument*      GetDocument();

    static FormContainer* GetFormContainer();

protected:
    CWnd*               m_pFrameWnd;
    CDocument*          m_pDocument;
    CString             m_sInfoText;
    InterfaceContext&   m_interfaceContext;
    bool                m_bRestore;
};

inline CWnd* Interface::GetFrame()
{
    return m_pFrameWnd;
}

inline CDocument* Interface::GetDocument()
{
    return m_pDocument;
}

#endif // AFX_INTERFACE_H__A1005FE9_E082_11D3_BCCC_00104B4C74FD__INCLUDED_

