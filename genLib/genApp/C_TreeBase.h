///////////////////////////////////////////////////////////////////////////////////
//
// C_TreeBase.h
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
// DESCRIPTION: Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//              Provides UI support for object tree operations. Includes drag/drop
//              for nested pages and for object re-arrangement.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_TREEBASE_H
#define C_TREEBASE_H

#include <genClass/T_SC_Array.h>

#include <genApp/C_MenuPage.h>
#include <genApp/C_MenuObj.h>
#include <genUIExt/TreeCtrlEx.h>

class TreeNode;
class TreeBase;
class AllocPageC;

// this is used to keep track of shift/ctrl selected pages and objects
class TreeNodeArray : public T_SC_Array<HTREEITEM> {

public:
    TreeNodeArray():T_SC_Array<HTREEITEM>() {}
    TreeNodeArray(const TreeNodeArray& a):T_SC_Array<HTREEITEM>(a) {}

    void    SelectAll(TreeBase& baseTree);
    void    DeSelectAll(TreeBase& baseTree);
};


class AppTreeItem {
public:
    enum                    TreeItemType {titPage, titObject, titNone};
    TreeItemType    treeItemType;
    MenuPageC*              assocPage;
    MenuObjC*               assocObj;
public:
    AppTreeItem()
    {       treeItemType = titNone;
        assocPage = 0;
        assocObj = 0;}
    AppTreeItem(MenuPageC*          aPage)
    {       treeItemType = titPage;
        assocPage = aPage;
        assocObj = 0;}
    AppTreeItem(MenuPageC*  aPage,
                MenuObjC*             aObj)
    {       treeItemType = titObject;
        assocPage = aPage;
        assocObj = aObj;}
    AppTreeItem(const AppTreeItem& a)
    {       treeItemType = a.treeItemType;
        assocPage = a.assocPage;
        assocObj = a.assocObj;}

    AppTreeItem&           operator= (const AppTreeItem& a);

    bool                   IsDeleteable()  const;
    bool                   IsPage()        const   {return treeItemType == titPage;}
    bool                   IsObject()      const   {return treeItemType == titObject;}
    MenuPageC&             GetPage()       const   {return *assocPage;}
    MenuObjC&              GetObject()     const   {return *assocObj;}
};

class MenuRootC;

class TreeBase : public CTreeCtrlEx
{
    friend class TreeNode;

public:

    enum SingleLineTypes {    slID = 0,
                              slText = 1,
                              slBoth = 2
    };

    MenuPageArray             selectedPages;
    MenuObjArray              selectedObjects;
    const AllocPageC*         selectedObjectPageAllocator;

    static  bool              m_bSingleLine;
    static  SingleLineTypes   m_nSingleLineType;
    static  bool              m_bShowNoOutput;
    static  bool              m_bShowConnectedObjects;

protected:
    CImageList                cTreeImageList;
    T_SC_Array<UINT>          aIconList;

private:
    TreeNodeArray             selectedObjectNodes;
    TreeNodeArray             selectedPageNodes;

    // dragging support
    bool                      isDragging;
    AppTreeItem               currDragAppItem;
    HTREEITEM                 currDragTreeItem;
    HTREEITEM                 targetNode;
    CImageList*               m_pDragImage;

    MenuRootC&                treeRoot;  // root associated with tree

    // scroll during drag'n'drop
    UINT                      m_nTimerID;
    UINT                      m_timerticks;

    bool                      ignoreSelections;  // for updates during deletions

public:
    TreeBase(MenuRootC& inRoot);
    virtual                   ~TreeBase();
    void                      Create(CWnd* pParentWnd, CRect& rect = CRect(0,0,0,0));
    void                      Resize(int x, int y, int cx, int cy);

    void                      AddPageItem(MenuPageC* pageToAdd,
                                          HTREEITEM  insertAfter = NULL);

    void                      AddObjectItem(MenuPageC* pageToAddTo,
                                            MenuObjC*  objectToAdd,
                                            HTREEITEM  insertAfter = NULL);

    void                      ResetStyle();
    void                      ClearSelections();

    void                      CollapseAllPages();

    void                      ClearAllTreeData();

    void                      IgnoreSelections() {ignoreSelections = true;}
    void                      ProcessSelections() {ignoreSelections = false;}
    void                      UpdateSelection();

    bool                      PageIsSelected();   // returns true if selected tree item is page
    bool                      ObjectIsSelected(); // returns true if selected tree item is object

protected:
    void                      Initialize();

    // CTreeCtrlEx2 virtuals
    virtual CString GetOutText(HTREEITEM hItem);
    virtual void    OnToggleNode(HTREEITEM hItem);



    void                    ClearSubTreeData(HTREEITEM hItem);
    //      clear object data
    void                    ClearNodeData(HTREEITEM treeNode); // clearsitem allocated as node data

    void                    SetTreeData(TV_INSERTSTRUCT&    item,
                                        TreeNode&           itemData,
                                        char*               descStr,
                                        int                 strLen);

    bool                    GetTreeItemData(HTREEITEM     treeNode,
                                            AppTreeItem&  currItem);



private:
    void                    PageExpandChange(MenuPageC& selPage);
    void                    SelectTreeItem(HTREEITEM hTreeItem);

    int                             FindInIconList(UINT nID);
    void                    ClearPage(MenuPageC& pageToClear);

    void                    ResetTree();
    void                    ResetChildren(HTREEITEM parentItem);
    HTREEITEM               GetInsertAfterObject(const AppTreeItem& dragTarget);

    void                    MoveTargetNode(HTREEITEM hItem);


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(TreeBase)
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

    // Generated message map functions
protected:
    //{{AFX_MSG(TreeBase)
    afx_msg void    OnSelectItem(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void    OnPageExpand(NMHDR* pNMHDR, LRESULT* pResult);

    afx_msg void    OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void    OnDestroy();
    afx_msg void    OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void    OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void    OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void    OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void    OnSysColorChange();
    afx_msg void    OnTimer(UINT_PTR nIDEvent);

    //to handle key events
    afx_msg LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
    /*
    afx_msg LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam);
    bool ctrlKeyDown;
    afx_msg LRESULT OnSelectingItem(WPARAM wParam, LPARAM lParam);
    */


    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

#endif // C_TREEBASE_H

