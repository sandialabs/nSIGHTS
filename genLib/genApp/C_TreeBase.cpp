///////////////////////////////////////////////////////////////////////////////////
//
// C_TreeBase.cpp
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

#include "GenLibAfx.h"

#include "GenLibRes.h"

#include <stdlib.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClass/C_FuncObj.h>
#include <genApp/C_MenuObj.h>
#include <genApp/C_MenuPage.h>
#include <genApp/C_MenuRoot.h>

#include <genApp/C_TreeBase.h>
#include <genApp/C_AppMenu.h>

#include <genUIExt/TitlePopupMenu.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void TreeNodeArray::SelectAll(TreeBase& baseTree)
{
    for (int i = 0; i < Size(); i++)
        baseTree.SetItemState((*this)[i], TVIS_SELECTED, TVIS_SELECTED);
}

void TreeNodeArray::DeSelectAll(TreeBase& baseTree)
{
    for (int i = 0; i < Size(); i++)
        baseTree.SetItemState((*this)[i], 0, TVIS_SELECTED);
    SetEmpty();
}


bool                        TreeBase::m_bSingleLine = false;
TreeBase::SingleLineTypes   TreeBase::m_nSingleLineType = TreeBase::slID;
bool                        TreeBase::m_bShowNoOutput = false;
bool                        TreeBase::m_bShowConnectedObjects = false;


BEGIN_MESSAGE_MAP(TreeBase, CTreeCtrlEx)
//{{AFX_MSG_MAP(TreeBase)
ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelectItem)
ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnPageExpand)
ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
ON_WM_CREATE()
ON_WM_DESTROY()
ON_WM_LBUTTONUP()
ON_WM_RBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_MESSAGE(WM_KEYDOWN, TreeBase::OnKeyDown)
//ON_MESSAGE(WM_KEYUP, TreeBase::OnKeyUp)
//ON_MESSAGE(TVN_SELCHANGING, OnSelectingItem)
ON_WM_CHAR()
ON_WM_SYSCOLORCHANGE()
ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

TreeBase::TreeBase(MenuRootC& inRoot)
: treeRoot(inRoot)
{
    selectedPages.SetResizable(10);
    selectedObjects.SetResizable(10);
    selectedObjectNodes.SetResizable(10);
    selectedPageNodes.SetResizable(10);
    isDragging = false;
    ignoreSelections = false;
}

TreeBase::~TreeBase()
{
}


void TreeBase::Create(CWnd* pParentWnd, CRect& rect)
{
    DWORD dwStyle = WS_CHILD|WS_VISIBLE
        |TVS_LINESATROOT|TVS_HASBUTTONS|TVS_HASLINES
        |TVS_INFOTIP|TVS_SHOWSELALWAYS;

    if (!m_bSingleLine) {
        dwStyle |= TVS_NONEVENHEIGHT;
    }

    CTreeCtrlEx::Create(dwStyle, rect, pParentWnd, 1);
}

void TreeBase::ResetStyle()
{
    if (m_bSingleLine) {
        CTreeCtrlEx::ModifyStyle(TVS_NONEVENHEIGHT, 0);
    } else {
        CTreeCtrlEx::ModifyStyle(0, TVS_NONEVENHEIGHT);
    }
}

void TreeBase::ClearSelections()
{
    selectedPages.SetEmpty();
    selectedObjects.SetEmpty();
    selectedObjectNodes.DeSelectAll(*this);
    selectedPageNodes.DeSelectAll(*this);
}

void TreeBase::Resize(int x, int y, int cx, int cy)
{
    if (::IsWindow(m_hWnd)) {
        SetWindowPos(NULL, x, y, cx, cy, SWP_NOZORDER|SWP_NOACTIVATE);
    }
}

int TreeBase::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CTreeCtrlEx::OnCreate(lpCreateStruct) == -1)
        return -1;

    Initialize();

    return 0;
}

void TreeBase::OnDestroy()
{
    ClearAllTreeData();
    DeleteAllItems();

    cTreeImageList.DeleteImageList();
    CTreeCtrlEx::OnDestroy();
}

void TreeBase::ClearAllTreeData()
{
    HTREEITEM hRoot = GetRootItem();
    ClearSubTreeData(hRoot);
}

// clean up heap
void TreeBase::ClearSubTreeData(HTREEITEM hItem)
{
    if (hItem != NULL) {
        do {
            if (ItemHasChildren(hItem)) {
                HTREEITEM hChildItem = GetChildItem(hItem);
                ClearSubTreeData(hChildItem);
            }
            ClearNodeData(hItem);
        } while ((hItem = GetNextSiblingItem(hItem)) != NULL);
    }
}

//////////////////
// Update view for first time: add form names
//
void TreeBase::Initialize()
{
    cTreeImageList.Create(16, 16, ILC_COLOR | ILC_MASK, 0, 1);
    cTreeImageList.SetBkColor(::GetSysColor(COLOR_WINDOW)); // probably not necessary
    CBitmap bm;
    VERIFY(bm.LoadBitmap(IDB_PROJECT_TREE_IMAGES));
    cTreeImageList.Add(&bm, RGB(0, 0, 255));
    bm.DeleteObject();
    VERIFY(bm.LoadBitmap(IDB_OVERLAY_IMAGES));
    int nIndex = cTreeImageList.Add(&bm, RGB(0, 0, 255));
    SetImageList(&cTreeImageList, TVSIL_NORMAL);

    // 10 overlay images
    for (int i = 0; i < 10; i++)
        cTreeImageList.SetOverlayImage(nIndex + i,   i + 1);

    aIconList.SetEmpty();
}


// for node data
AppTreeItem&     AppTreeItem::operator= (const AppTreeItem& a)
{
    if (&a != this)
        {
            treeItemType = a.treeItemType;
            assocPage    = a.assocPage;
            assocObj     = a.assocObj;
        }
    return *this;
}


bool AppTreeItem::IsDeleteable() const
{
    if (IsPage())
        return assocPage->IsDeleteable();

    return assocObj->IsDeleteable();
}


void TreeBase::ClearNodeData(HTREEITEM treeNode)
{
    DWORD_PTR itemData = GetItemData(treeNode);
    if (itemData)
        {
            AppTreeItem* currItem = reinterpret_cast<AppTreeItem*>(itemData);
            delete currItem;
            SetItemData(treeNode, 0);
        }
}

bool TreeBase::GetTreeItemData(HTREEITEM     treeNode,
                               AppTreeItem&  currItem)
{
    DWORD_PTR itemData = GetItemData(treeNode);
    if (itemData == 0)
        return false;

    currItem = *(reinterpret_cast<AppTreeItem*>(itemData));
    return true;
}


void TreeBase::SetTreeData( TV_INSERTSTRUCT&    item,
                            TreeNode&           itemData,
                            char*               str,
                            int                 strLen)
{
    if (itemData.IsRcIconID())
        {
            UINT nID = itemData.GetRcIconID();
            int nIndex = FindInIconList(nID);
            if (nIndex < 0)
                {
                    HICON hIcon = HICON(::LoadImage(AfxGetInstanceHandle(),
                                                    MAKEINTRESOURCE(nID),
                                                    IMAGE_ICON, 16, 16,
                                                    LR_SHARED));
                    ASSERT(hIcon != NULL);
                    nIndex = cTreeImageList.Add(hIcon);
                    ASSERT(nIndex != -1);
                    SetImageList(&cTreeImageList, TVSIL_NORMAL);
                }
            itemData.iconIndex = nIndex;
        }

    int nLines;
    if (!m_bSingleLine)
        {
            bool sameText;
            itemData.GetTextDisplaySettings(sameText, nLines);
            if (nLines < 2 && !sameText)
                {
                    MakeString(str, itemData.idText, "::", itemData.typeText, strLen);
                }
            else
                {
                    CopyString(str, itemData.idText, strLen);
                }
        }
    else
        {
            nLines = 1;
            CopyString(str, itemData.idText, strLen);
            if (!SameString(itemData.idText, itemData.typeText))
                {
                    switch (m_nSingleLineType) {
                    case slID:
                        break;
                    case slText:
                        CopyString(str, itemData.typeText, strLen);
                        break;
                    case slBoth:
                        MakeString(str, itemData.idText, "::", itemData.typeText, strLen);
                        break;
                    default:
                        GenAppInternalError("TreeBase::SetTreeData");
                        break;
                    }
                }

        }

    memset (&item, 0, sizeof(item));
    item.itemex.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_INTEGRAL;
    item.itemex.iIntegral = nLines;
    item.itemex.pszText = str;
    item.itemex.cchTextMax = strLen;
    item.itemex.iImage = itemData.iconIndex;
    item.itemex.iSelectedImage = itemData.iconIndex;
}

void TreeBase::AddPageItem(MenuPageC* pageToAdd,
                           HTREEITEM  insertAfter)
{
    TV_INSERTSTRUCT item;
    char str[255];
    SetTreeData(item, *pageToAdd, str, 255);

    item.hParent = NULL;
    if (pageToAdd->parentPage != NULL)
        item.hParent = pageToAdd->parentPage->treeNode;

    if (insertAfter == NULL)
        item.hInsertAfter = TVI_LAST;
    else
        item.hInsertAfter = insertAfter;

    item.itemex.mask = item.itemex.mask | TVIF_PARAM;
    if (pageToAdd->IsExpanded())
        {
            item.itemex.mask = item.itemex.mask | TVIF_STATE;
            item.itemex.state = TVIS_EXPANDED;
            item.itemex.stateMask = 255;
        }

    // item data
    AppTreeItem* newItem = new AppTreeItem(pageToAdd);
    item.itemex.lParam = reinterpret_cast<long>(newItem);

    pageToAdd->baseTree = this;
    pageToAdd->treeNode = InsertItem(&item);
}

int     TreeBase::FindInIconList(UINT nID)
{
    for (int i = 0; i < aIconList.Size(); i++ ) {
        if (aIconList[i] == nID) {
            return i;
        }
    }
    return -1;
}

void TreeBase::AddObjectItem(MenuPageC* pageToAddTo,
                             MenuObjC*  objectToAdd,
                             HTREEITEM  insertAfter)
{
    // Add new Icon to the image list and set up iconIndex
    if (objectToAdd->IsRcIconID())
        {
            UINT nID = objectToAdd->GetRcIconID();
            int nIndex = FindInIconList(nID);
            if (nIndex < 0) {
                HICON hIcon = HICON(::LoadImage(AfxGetInstanceHandle(),
                                                MAKEINTRESOURCE(nID),
                                                IMAGE_ICON, 16, 16,
                                                LR_SHARED));
                ASSERT(hIcon != NULL);
                nIndex = cTreeImageList.Add(hIcon);
                ASSERT(nIndex != -1);
                SetImageList(&cTreeImageList, TVSIL_NORMAL);
            }
            objectToAdd->iconIndex = nIndex;
        }


    TV_INSERTSTRUCT item;
    char str[255];
    SetTreeData(item, *objectToAdd, str, 255);

    ASSERT(pageToAddTo->treeNode != NULL);

    item.hParent = pageToAddTo->treeNode;

    if (insertAfter == NULL)
        item.hInsertAfter = TVI_LAST;
    else
        item.hInsertAfter = insertAfter;

    item.itemex.mask = item.itemex.mask | TVIF_PARAM;

    AppTreeItem* newItem = new AppTreeItem(pageToAddTo, objectToAdd);
    item.itemex.lParam = reinterpret_cast<long>(newItem);

    objectToAdd->baseTree = this;
    objectToAdd->treeNode = InsertItem(&item);

}

/////////////////////////////////////////////////////////////////////////////
// TreeBase message handlers

void TreeBase::OnSelectItem(NMHDR* pNMHDR, LRESULT* pResult)
{
    if (ignoreSelections)
        return;

    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
    HTREEITEM hTreeItem = pNMTreeView->itemNew.hItem;

    SelectTreeItem(hTreeItem);
}

void TreeBase::UpdateSelection()
{
    HTREEITEM currSel = GetSelectedItem();
    if (currSel != NULL)
        SelectTreeItem(currSel);
}

bool TreeBase::PageIsSelected()
{
    HTREEITEM currSel = GetSelectedItem();
    if (currSel == NULL)
        return false;

    AppTreeItem currItem;
    if (!GetTreeItemData(currSel, currItem))
        return false;

    return currItem.IsPage();
}

bool TreeBase::ObjectIsSelected()
{
    HTREEITEM currSel = GetSelectedItem();
    if (currSel == NULL)
        return false;

    AppTreeItem currItem;
    if (!GetTreeItemData(currSel, currItem))
        return false;

    return currItem.IsObject();
}

void TreeBase::SelectTreeItem(HTREEITEM hTreeItem)
{
    AppTreeItem currItem;
    if (!GetTreeItemData(hTreeItem, currItem))
        return;

    bool addItem = (::GetKeyState(VK_CONTROL) < 0) || (::GetKeyState(VK_SHIFT) < 0);
    if (!addItem)
        {
            ClearSelections();
        }

    // page change ?
    if ((MenuPageC::currPageObj != currItem.assocPage) &&
        (MenuPageC::currPageObj != 0))
        {
            // unselect existing
            MenuPageC::currPageObj->UnselectPage();
        }

    if (MenuPageC::currPageObj == 0)
        currItem.assocPage->SelectPage();

    MenuObjC* selObj = 0;

    bool addObject = false;
    bool addPage = false;

    switch (currItem.treeItemType) {
    case AppTreeItem::titPage :
        {
            // find selected object on page
            // default is first
            if (MenuPageC::currPageObj->pageObjects.IsNotEmpty())
                {
                    selObj = MenuPageC::currPageObj->pageObjects[0];

                    for (int i = 0; i < MenuPageC::currPageObj->pageObjects.Size(); i++)
                        {
                            MenuObjC* currObj = MenuPageC::currPageObj->pageObjects[i];
                            if (currObj->IsSelected())
                                {
                                    selObj = currObj;
                                    break;
                                }
                        }
                }

            addPage = true;
            for (int i = 0; i < selectedPages.Size(); i++)
                if (selectedPages[i] == MenuPageC::currPageObj)
                    addPage = false;

            break;
        }
    case AppTreeItem::titObject : {
        selObj = currItem.assocObj;
        const AllocPageC* currPageAllocator = MenuPageC::currPageObj->GetActualParent()->GetPageAllocator();
        if (selObj != 0)
            {
                if (currPageAllocator != selectedObjectPageAllocator)
                    {
                        selectedObjects.SetEmpty();
                        selectedObjectNodes.DeSelectAll(*this);
                    }
                addObject = selObj->objCopyable;
                for (int i = 0; i < selectedObjects.Size(); i++)
                    if (selectedObjects[i] == selObj)
                        addObject = false;
            }
        selectedObjectPageAllocator = currPageAllocator;
        break;
    }
    }

    if (selObj != 0)
        {
            selObj->PropOpenMenu();
            selObj->UpdateTreeData();
            if (m_bShowConnectedObjects)
                MenuRootC::UpdateCurrentTree();
        }

    // reselect if necessary
    selectedPageNodes.SelectAll(*this);
    selectedObjectNodes.SelectAll(*this);

    // add current selections
    if (addPage)
        {
            selectedPages += MenuPageC::currPageObj;
            selectedPageNodes += hTreeItem;
        }
    if (addObject)
        {
            selectedObjects += selObj;
            selectedObjectNodes += hTreeItem;
        }

}

void TreeBase::PageExpandChange(MenuPageC& selPage)
{
    selPage.UpdatePage();
}

void TreeBase::OnToggleNode(HTREEITEM hItem)
{
    AppTreeItem currItem;
    if (!GetTreeItemData(hItem, currItem))
        return;

    if (currItem.IsPage())
        {
            MenuPageC& selPage = currItem.GetPage();
            selPage.pageExpanded = ! selPage.pageExpanded;
            PageExpandChange(selPage);
        }
}

void TreeBase::OnPageExpand(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    AppTreeItem currItem;
    if (!GetTreeItemData(pNMTreeView->itemNew.hItem, currItem))
        return;

    if (currItem.IsPage())
        {
            MenuPageC& selPage = currItem.GetPage();
            selPage.pageExpanded = (pNMTreeView->action != TVE_COLLAPSE);
            PageExpandChange(selPage);
        }

    *pResult = 0;
}

CString TreeBase::GetOutText(HTREEITEM hItem)
{
    AppTreeItem currItem;
    if (!GetTreeItemData(hItem, currItem))
        return "???";

    CString sOutText;

    TreeNode* currNode;
    if (currItem.IsPage())
        {
            currNode = currItem.assocPage;
        }
    else
        {
            currNode = currItem.assocObj;
        }

    bool sameText;
    int nLines;
    currNode->GetTextDisplaySettings(sameText, nLines);
    if (sameText || nLines < 2)
        {
            sOutText.Empty();
        }
    else
        {
            sOutText = currNode->typeText;
        }
    return sOutText;
}

void TreeBase::OnRButtonDown(UINT nFlags, CPoint point)
{
    TVHITTESTINFO hitTestInfo;
    hitTestInfo.pt = point;
    HitTest(&hitTestInfo);
    bool bChangeExpansion = false;
    if (hitTestInfo.flags == TVHT_ONITEMRIGHT)
        {
            if (IsInExtendedRect(hitTestInfo.hItem, point))
                {
                    CRect rc;
                    GetItemRect(hitTestInfo.hItem, &rc, TRUE);
                    bChangeExpansion = true;
                }
        }
    else if (hitTestInfo.flags & TVHT_ONITEM)
        {
            bChangeExpansion = true;
        }
    if (bChangeExpansion)
        {
            CTreeCtrlEx::SelectItem(hitTestInfo.hItem);
        }

    // To deal with right clicks below the tree
    if (hitTestInfo.hItem == NULL)
        return;

    AppTreeItem currItem;
    if (!GetTreeItemData(hitTestInfo.hItem, currItem))
        return;

    TitlePopupMenu titlePopupMenu;

    CMenu mainMenu;
    mainMenu.LoadMenu(AppMenuC::GetMainMenuBarID());
    switch (currItem.treeItemType) {
    case AppTreeItem::titPage:
        {
            CMenu* popMenu = AppMenuC::GetPageMenu(mainMenu);
            AppMenuC::ResetPagePopupMenu(popMenu);
            titlePopupMenu.Attach(popMenu->m_hMenu);
            titlePopupMenu.AddMenuTitle("Page");
            break;
        }
    case AppTreeItem::titObject:
        {
            CMenu* popMenu = AppMenuC::GetObjectMenu(mainMenu);
            AppMenuC::ResetObjectPopupMenu(popMenu);
            titlePopupMenu.Attach(popMenu->m_hMenu);
            titlePopupMenu.AddMenuTitle("Object");
            break;
        }
    default:
        ASSERT(0);
        break;
    }
    CPoint ptScreen = point;
    ClientToScreen(&ptScreen);
    titlePopupMenu.TrackPopupMenu(TPM_LEFTBUTTON|TPM_RIGHTBUTTON, ptScreen.x, ptScreen.y, AfxGetMainWnd());
    titlePopupMenu.Detach();
    mainMenu.Detach();
}


void TreeBase::OnSysColorChange()
{
    CWnd::OnSysColorChange();

    // Reset the background color of our image list when notified
    cTreeImageList.SetBkColor(::GetSysColor(COLOR_WINDOW));
}


// *************************************   drag/drop support

void TreeBase::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    targetNode = NULL;
    currDragTreeItem = pNMTreeView->itemNew.hItem;

    if (!GetTreeItemData(currDragTreeItem, currDragAppItem))
        return;

    if (!currDragAppItem.IsDeleteable())
        {
            GenAppErrorMsg("Tree", "cannot drag fixed objects/pages");
            return;
        }

    m_pDragImage = CreateDragImage(currDragTreeItem);  // get the image list for dragging
    // CreateDragImage() returns NULL if no image list
    // associated with the tree view control
    if (!m_pDragImage)
        return;

    isDragging = true;
    m_pDragImage->BeginDrag(0, CPoint(-15,-15));
    POINT pt = pNMTreeView->ptDrag;
    ClientToScreen( &pt );
    m_pDragImage->DragEnter(NULL, pt);
    SetCapture();

    m_nTimerID = (int) SetTimer(1, 75, NULL);
}

void TreeBase::MoveTargetNode(HTREEITEM hItem)
{
    CImageList::DragShowNolock(FALSE);

    CFont* pOldFont = NULL;
    CFont* pFont = GetFont();
    LOGFONT logFont;
    pFont->GetLogFont(&logFont);
    CDC* pDC = GetDC();
    pOldFont = pDC->SelectObject(pFont);

    // It'd be easy without custom drawing:
    //SelectDropTarget(hItem);
    //targetNode = hItem;

    if (targetNode != hItem) {

        if (targetNode != NULL) {
            CString sOutText = GetOutText(targetNode);
            DrawItem(*pDC, targetNode, COLOR_MENUTEXT, sOutText, 0);
        }
        if (hItem != NULL) {
            CRect rcItem;
            CTreeCtrlEx::GetItemRect2(*pDC, hItem, rcItem);
            DrawDisabledItem(*pDC, rcItem, COLOR_BACKGROUND);
        }
    } else {
        if (targetNode != NULL) {
            CString sOutText = GetOutText(targetNode);
            DrawItem(*pDC, targetNode, COLOR_MENUTEXT, sOutText, 0);
            CRect rcItem;
            CTreeCtrlEx::GetItemRect2(*pDC, hItem, rcItem);
            DrawDisabledItem(*pDC, rcItem, COLOR_BACKGROUND);
        }
    }

    pDC->SelectObject(pOldFont);
    targetNode = hItem;

    CImageList::DragShowNolock(TRUE);
}

void TreeBase::OnMouseMove(UINT nFlags, CPoint point)
{
    if (isDragging) {
        UINT flags;
        POINT pt = point;
        ClientToScreen(&pt);
        HTREEITEM hItem = HitTest(point, &flags);
        CImageList::DragMove(pt);

        if (hItem != NULL)
            MoveTargetNode(hItem);
    }
}

BOOL TreeBase::PreTranslateMessage(MSG* pMsg)
{
    if (isDragging) {
        if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) {
            isDragging = false;
            CImageList::DragLeave(NULL);
            CImageList::EndDrag();
            ReleaseCapture();
            KillTimer(m_nTimerID);

            MoveTargetNode(NULL);

            delete m_pDragImage;
            m_pDragImage = NULL;

            return TRUE;            // DO NOT process further
        }
    }

    return CTreeCtrlEx::PreTranslateMessage(pMsg);
}

void TreeBase::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent != m_nTimerID) {
        CTreeCtrlEx::OnTimer(nIDEvent);
        return;
    }

    // It doesn't matter that we didn't initialize m_timerticks
    m_timerticks++;

    POINT pt;
    GetCursorPos(&pt);
    RECT rect;
    GetClientRect(&rect);
    ClientToScreen(&rect);

    CImageList::DragMove(pt);

    HTREEITEM hItem = GetFirstVisibleItem();

    if (pt.y < rect.top + 10)  {
        // We need to scroll up
        // Scroll slowly if cursor near the treeview control
        int slowscroll = 6 - (rect.top + 10 - pt.y) / 20;
        if (0 == (m_timerticks % (slowscroll > 0? slowscroll : 1))) {
            CImageList::DragShowNolock(FALSE);
            SendMessage(WM_VSCROLL, SB_LINEUP);

            MoveTargetNode(hItem);

            CImageList::DragShowNolock(TRUE);
        }
    } else if (pt.y > rect.bottom - 10)  {
        // We need to scroll down
        // Scroll slowly if cursor near the treeview control
        int slowscroll = 6 - (pt.y - rect.bottom + 10 ) / 20;
        if (0 == (m_timerticks % (slowscroll > 0? slowscroll : 1))) {
            CImageList::DragShowNolock(FALSE);

            SendMessage(WM_VSCROLL, SB_LINEDOWN);
            int nCount = GetVisibleCount();
            for (int i=0; i<nCount-1; i++ )
                hItem = GetNextVisibleItem(hItem);

            if (hItem != NULL)
                MoveTargetNode(hItem);
            else
                // redraw the double line as last item because it can be scrolled
                MoveTargetNode(targetNode);

            CImageList::DragShowNolock(TRUE);
        }
    }
}

void TreeBase::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (!isDragging)
        return;

    // general cleanup
    isDragging = false;
    CImageList::DragLeave(this);
    CImageList::EndDrag();
    ReleaseCapture();
    KillTimer(m_nTimerID);

    delete m_pDragImage;
    m_pDragImage = NULL;

    // get copy of object selections, the clear all selections
    MenuObjArray draggedObjects = selectedObjects;
    ClearSelections();

    if (targetNode == NULL)
        return;

    // get data for node
    AppTreeItem dragTarget;
    if (!GetTreeItemData(targetNode, dragTarget))
        return;

    MenuPageC& targetPage = dragTarget.GetPage();

    MenuPageC* oldParent = 0;
    MenuPageC* newParent = 0;

    if (currDragAppItem.IsPage())
        {
            oldParent = currDragAppItem.GetPage().parentPage;
            newParent = targetPage.parentPage;

            // get tree item for assoc page
            HTREEITEM targetNode = targetPage.treeNode;

            // can't be the same as the source tree
            if (targetNode == currDragTreeItem)
                return;

            // drag page cannot be parent of target
            MenuPageC* currParent = newParent;
            while (currParent != NULL)
                {
                    if (currParent == currDragAppItem.assocPage)
                        {
                            GenAppErrorMsg("Tree", "cannot drop parent on its children");
                            return;
                        }
                    currParent = currParent->parentPage;
                }

            MenuPageC& draggedPage = currDragAppItem.GetPage();

            // default insert after target page at same level
            HTREEITEM  insertAfter = targetNode;

            // are we dragging a page onto another page as a child?
            if (dragTarget.IsObject() && targetPage.AcceptAsChild(draggedPage))
                {
                    if (!draggedPage.AcceptAsParent(&targetPage))
                        {
                            GenAppErrorMsg("Tree", "the target cannot be a parent of the dragged page");
                            return;
                        }

                    // tell the page we are changing
                    draggedPage.ParentChanging(&targetPage);

                    //  get the next insertion spot
                    insertAfter = GetInsertAfterObject(dragTarget);

                    // parent is target
                    newParent = &targetPage;
                }
            else
                {
                    if ((newParent != NULL) && (!newParent->AcceptAsChild(draggedPage)))
                        {
                            GenAppErrorMsg("Tree", "the dragged page cannot be a child of the target");
                            return;
                        }

                    if (!draggedPage.AcceptAsParent(newParent))
                        {
                            GenAppErrorMsg("Tree", "the target cannot be a parent of the dragged page");
                            return;
                        }

                    // adding at same level as target page
                    draggedPage.ParentChanging(targetPage.parentPage);
                }

            // delete the page node & all its children from the tree
            draggedPage.RemoveFromTree();

            // add the new page after the target
            draggedPage.baseTree = this;
            draggedPage.AddToTree(insertAfter);

            if (newParent != NULL)
                newParent->ObjectPageChangeComplete(oldParent);

        }
    else
        {
            HTREEITEM  insertAfter = GetInsertAfterObject(dragTarget);

            oldParent = currDragAppItem.assocPage;
            newParent = dragTarget.assocPage;

            // moving around on same page ?
            MenuObjC& draggedObj = currDragAppItem.GetObject();
            if (dragTarget.assocPage ==  currDragAppItem.assocPage)
                {
                    // dragging on to self ?
                    if (dragTarget.IsObject() && (dragTarget.assocObj == currDragAppItem.assocObj))
                        return;
                }
            else
                {
                    bool sameActualPage = dragTarget.GetPage().GetActualParent() == currDragAppItem.GetPage().GetActualParent();
                    // process multiple selections
                    // only if current is in list
                    if (draggedObjects.Found(currDragAppItem.assocObj))
                        {
                            for (int i = 0; i < draggedObjects.Size(); i++)
                                {
                                    MenuObjC& currObj = draggedObjects.GetRef(i);
                                    if (!(sameActualPage || currObj.CanDragToDifferentPage()))
                                        {
                                            GenAppErrorMsg("Tree", "one or more selected objects cannot be dragged onto another page");
                                            return;
                                        }
                                    if (!dragTarget.GetPage().AcceptAsObject(currObj))
                                        {
                                            GenAppErrorMsg("Tree", "one or more selected objects cannot be dragged that page");
                                            return;
                                        }
                                }

                            // all OK so add them
                            MenuPageC* lastPage = 0;
                            for (int i = 0; i < draggedObjects.Size(); i++)
                                {
                                    MenuObjC& currObj = draggedObjects.GetRef(i);
                                    // klugy approach to getting page
                                    AppTreeItem selectedDrag;
                                    if (!GetTreeItemData(currObj.treeNode, selectedDrag))
                                        return;
                                    MenuPageC& objPage = selectedDrag.GetPage();
                                    currObj.ChangePage(objPage, dragTarget.GetPage());

                                    currObj.DeleteNode();
                                    AddObjectItem(dragTarget.assocPage, &currObj, insertAfter);
                                    insertAfter = currObj.treeNode;

                                    lastPage = &objPage;
                                }

                            dragTarget.GetPage().ObjectPageChangeComplete(lastPage);

                            ResetTree();
                            return;
                        }

                    // single object processing
                    // dragging onto another page
                    if (!(sameActualPage || draggedObj.CanDragToDifferentPage()))
                        {
                            GenAppErrorMsg("Tree", "one or more selected objects cannot be dragged onto another page");
                            return;
                        }

                    if (!dragTarget.GetPage().AcceptAsObject(draggedObj))
                        {
                            GenAppErrorMsg("Tree", "cannot drag that object onto that page");
                            return;
                        }

                    draggedObj.ChangePage(currDragAppItem.GetPage(), dragTarget.GetPage());
                    dragTarget.GetPage().ObjectPageChangeComplete(&currDragAppItem.GetPage());
                }

            draggedObj.DeleteNode();
            AddObjectItem(dragTarget.assocPage, &draggedObj, insertAfter);
        }

    // reset the object page order based on the tree
    ResetTree();

    if ((oldParent != NULL) || (newParent != NULL))
        {
            if (oldParent == newParent)
                {
                    oldParent->ResetChildren();
                }
            else
                {
                    if (oldParent != NULL)
                        oldParent->ResetChildren();
                    if (newParent != NULL)
                        newParent->ResetChildren();
                }
        }

}

HTREEITEM TreeBase::GetInsertAfterObject(const AppTreeItem& dragTarget)
{
    HTREEITEM  insertAfter = NULL;
    if (dragTarget.IsObject())
        {
            if (dragTarget.IsDeleteable())
                insertAfter = dragTarget.GetObject().treeNode;
            else
                {
                    MenuPageC& targetPage = dragTarget.GetPage();
                    // reset to last non deleteable on page
                    for (int i = 0; i < targetPage.pageObjects.Size(); i++)
                        {
                            if (targetPage.pageObjects[i]->IsDeleteable())
                                break;
                            insertAfter = targetPage.pageObjects[i]->treeNode;
                        }
                }
        }
    return insertAfter;
}
void TreeBase::ResetTree()
{
    // first clear all page/objects
    // start at the root
    for (int i = 0; i < treeRoot.pageLeafs.Size(); i++)
        ClearPage(*(treeRoot.pageLeafs[i]));
    treeRoot.pageLeafs.SetEmpty();

    HTREEITEM rootItem  = GetRootItem();
    while (rootItem  != NULL)
        {
            AppTreeItem nextRoot;
            if (!GetTreeItemData(rootItem, nextRoot))
                GenAppInternalError("Tree::Base not GenApp item?");

            // only pages off root
            if (!nextRoot.IsPage())
                GenAppInternalError("Tree::Base root not a page item?");

            // add page
            treeRoot.pageLeafs += nextRoot.assocPage;

            // all children of root
            ResetChildren(rootItem);

            rootItem = GetNextItem(rootItem, TVGN_NEXT);
        }
}

void TreeBase::ResetChildren(HTREEITEM parentItem)
{
    // all children of parent
    HTREEITEM childItem  = GetChildItem(parentItem);
    while (childItem != NULL)
        {
            AppTreeItem nextChild;
            if (!GetTreeItemData(childItem, nextChild))
                GenAppInternalError("TreeBase::ResetChildren - not GenApp item?");

            if (nextChild.IsPage())
                {
                    // add page
                    MenuPageC* parent = nextChild.GetPage().parentPage;
                    parent->childPages += nextChild.assocPage;

                    // and its objects
                    ResetChildren(childItem);
                }
            else
                {
                    if (!nextChild.IsObject())
                        GenAppInternalError("Tree::Base expecting object");

                    // add object to tree
                    nextChild.GetPage().pageObjects += nextChild.assocObj;
                }
            childItem = GetNextItem(childItem, TVGN_NEXT);
        }
}


void TreeBase::ClearPage(MenuPageC& pageToClear)
{
    pageToClear.pageObjects.SetEmpty();
    for (int i = 0; i < pageToClear.childPages.Size(); i++)
        ClearPage(*(pageToClear.childPages[i]));
    pageToClear.childPages.SetEmpty();
}

void TreeBase::CollapseAllPages()
{
    HTREEITEM rootItem  = GetRootItem();
    while (rootItem  != NULL)
        {
            AppTreeItem nextRoot;
            if (!GetTreeItemData(rootItem, nextRoot))
                GenAppInternalError("Tree::Base not GenApp item?");

            // only pages off root
            if (!nextRoot.IsPage())
                GenAppInternalError("Tree::Base root not a page item?");

            Expand(rootItem, TVE_COLLAPSE);

            nextRoot.GetPage().pageExpanded = false;

            rootItem = GetNextItem(rootItem, TVGN_NEXT);
        }
}


LRESULT TreeBase::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
    //int loword = LOWORD(wParam);
    //int hiword = HIWORD(wParam);
    if(wParam == VK_DELETE)
    {
        AppTreeItem currItemD;
        if (!GetTreeItemData(GetSelectedItem(), currItemD))
            return(0L);
        if(currItemD.IsDeleteable())
        {
            if (currItemD.IsObject())
            {
                //currItem.GetObject()  //MenuObjC&
                AppMenuC::DeleteObject();
            }
            else if(currItemD.IsPage())
            {
                //currItem.GetPage()  //MenuPageC&
                AppMenuC::DeletePage();
            }
        }
    }
        //case 0x03: //'ctrl-c'

    return(0L);
}

void TreeBase::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch(nChar)
    {
        case VK_TAB:
        {
            AppTreeItem currItem;
            if (!GetTreeItemData(GetSelectedItem(), currItem))
                return;
            if (currItem.IsObject())
            {
                currItem.GetObject().SetFocus();
            }
            return;
        }
        default:
            CTreeCtrlEx::OnChar(nChar, nRepCnt, nFlags);
    }
}
/*
LRESULT TreeBase::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
    if(wParam == VK_CONTROL)
    {
        ctrlKeyDown = false;
    }
    return(0L);
}

LRESULT TreeBase::OnSelectingItem(WPARAM wParam, LPARAM lparam)
{
    if(ctrlKeyDown)
    {
        return(true);
    }
    return(false);
}
*/

