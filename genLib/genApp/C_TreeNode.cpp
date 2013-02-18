///////////////////////////////////////////////////////////////////////////////////
//
// C_TreeNode.cpp
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
//              Provides UI support for a single node (MenuObj or MenuPage) on a tree.
//              Displays icon and object description.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include <genApp/C_TreeBase.h>

#include <genApp/C_TreeNode.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TreeNode::TreeNode()
{
    treeNode = NULL;   // project tree list
    baseTree = 0;
    iconIndex = 0;
    iconMaskIndex = 0;
    typeText = 0;
    idText = 0;
    rcIconID = UINT(-1);
}

TreeNode::~TreeNode()
{
}

void TreeNode::SelectNode()
{
    baseTree->SelectItem(treeNode);
}

void TreeNode::DeleteNode()
{
    if (!baseTree || !::IsWindow(baseTree->m_hWnd))
        return;

    baseTree->ClearNodeData(treeNode);

    // clear to be safe
    baseTree->SetItemData(treeNode, 0);
    baseTree->DeleteItem(treeNode);

    //      clear
    baseTree = 0;
}

void TreeNode::ExpandNode()
{
    baseTree->Expand(treeNode, TVE_EXPAND);
}


void TreeNode::UpdateTreeData()
{
    if (!baseTree || !::IsWindow(baseTree->m_hWnd))
        return;

    TV_INSERTSTRUCT  item;
    char str[255];
    baseTree->SetTreeData(item, *this, str, 255);

    // set item info
    item.itemex.hItem = treeNode;

    // update
    baseTree->SetItem(&item.item);
    baseTree->SetItemState(treeNode, INDEXTOOVERLAYMASK(iconMaskIndex), TVIS_OVERLAYMASK);
}


void TreeNode::GetTextDisplaySettings(bool& sameText,
                                      int&  nLines)
{
    int itemLen = StringLength(typeText);
    sameText = SameString(typeText, idText);
    nLines = 1;
    if ((!sameText) && ((StringLength(idText) + StringLength(typeText)) > maxItemLen)) {
        nLines = 2;
    }
}

bool TreeNode::IsRcIconID()
{
    return rcIconID != UINT(-1);
}

UINT TreeNode::GetRcIconID()
{
    return rcIconID;
}

void TreeNode::SetIcon(UINT nID)
{
    rcIconID = nID;
}

