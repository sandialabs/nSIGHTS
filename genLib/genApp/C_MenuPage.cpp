///////////////////////////////////////////////////////////////////////////////////
//
// C_MenuPage.cpp
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
//              collects MenuObjC objects on a page in the object tree UI.
//              Handles child pages for drag/drop and object re-arrangement.
//              Derived from TreeNode and CfgFileObj.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#ifndef ADCONSOLEAPP
#include <stdlib.h>
#endif

#include <iostream>

#include <genClass/C_FuncObj.h>

#include <genApp/C_AllocObj.h>
#include <genApp/C_AllocPage.h>

#include <genApp/AppConfigFile.h>

#include <genApp/C_MenuObj.h>
#include <genApp/C_MenuRoot.h>

#ifndef ADCONSOLEAPP
#include <genApp/C_TreeBase.h>
#include <genApp/C_AppMenu.h>
#endif

#include <genApp/C_MenuPage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//  declare static members

MenuPageC* MenuPageC::currPageObj   = 0; //  current object

// ***************************************************************** construct/destruct

MenuPageC::MenuPageC(const char* pID)
    : pageObjects(16), childPages(4)
{
    //  initialize members
    CopyString(pageID, pID, FuncObjC::objectIDLen);
#ifndef ADCONSOLEAPP
    typeText = pID;
#endif
    pageAllocator = 0;
    InitCommon();
}

MenuPageC::MenuPageC(const MenuPageC& a)
    : pageObjects(16), childPages(4)
{
    //  initialize members
    CopyString(pageID, a.pageID, FuncObjC::objectIDLen);

#ifndef ADCONSOLEAPP
    typeText        = a.typeText;
#endif
    pageAllocator   = a.pageAllocator;
    InitCommon();
}

void MenuPageC::InitCommon()
{
#ifndef ADCONSOLEAPP
    assocWindow         = 0;
    idText              = pageID;
#endif

    pageSelected        = false;
    pageExpanded        = false;
    pageDeleteable      = true;
    pageCopyable        = true;

    parentPage          = 0;
}

MenuPageC::~MenuPageC()
{
#ifndef ADCONSOLEAPP
    assocWindow     = 0;
#endif

    //  delete all child pages
    // usually goes faster from last to 1st
    int nchildren = childPages.UpperBound();
    // clear children - prevents problems in tree draw
    childPages.SetEmpty();

    for (int i = nchildren; i >= 0; i--)
        {
            MenuPageC* childPage = childPages[i];
            childPages[i] = 0;
            childPage->parentPage = 0;  // don't need to update this
            delete childPage;
        }

    //  delete all attached objects
    // usually goes faster from last to 1st
    int nobj = pageObjects.UpperBound();

    // clear page objects - prevents problems in tree draw
    pageObjects.SetEmpty();
    for (int i = nobj; i >= 0; i--)
        {
            delete pageObjects[i];
        }

    if (currPageObj == this)
        currPageObj = 0;

#ifndef ADCONSOLEAPP
    //      delete tree representation
    DeleteNode();
#endif

    // remove from parent
    if (parentPage != NULL)
        parentPage->childPages.DeleteFrom(this);

}

#ifndef ADCONSOLEAPP

void MenuPageC::UnselectPage()
{
    if (!pageSelected)
        return;

    // close any open menus
    for (int i = 0; i < pageObjects.Size(); i++)
        {
            MenuObjC& currObj = pageObjects.GetRef(i);
            if (currObj.IsSelected())
                {
                    currObj.PropCloseMenu();
                    // reset selection status for next page select
                    // if this is part of closing a page (normally set false on PropClose)
                    if (!pageExpanded)
                        currObj.objSelected = true;
                }
        }

    pageSelected = false;
    currPageObj = 0;

    AppMenuC::ResetObjectMenus();
    UpdatePage();
}


void MenuPageC::SelectPage()
{
    //  change global to current page
    currPageObj = this;
    pageSelected = true;

    // open selected menu if any
    for (int i = 0; i < pageObjects.Size(); i++)
        {
            MenuObjC& currObj = pageObjects.GetRef(i);
            if (currObj.IsSelected())
                {
                    currObj.PropOpenMenu();
                }
        }

    AppMenuC::ResetObjectMenus();

    UpdatePage();
}

void MenuPageC::UpdatePage()
{
    //  update each button label
    for (int i = 0; i < pageObjects.Size(); i++)
        pageObjects[i]->UpdateTreeData();

    // set page ID to first menu item
    if (!pageObjects.IsEmpty())
        {
            CopyString(pageID, pageObjects[0]->GetID(), FuncObjC::objectIDLen);
        }

    for (int i = 0; i < childPages.Size(); i++)
        childPages[i]->UpdatePage();

    UpdateTreeData();
}

void MenuPageC::UpdateTreeData()
{
    if (!IsRcIconID()) {
        iconIndex = pageExpanded ? 1 : 0;
    }
    iconMaskIndex = 0;

    if (pageSelected && (this != currPageObj))
        pageSelected = false;

    //if (pageExpanded && pageSelected)
    if (pageSelected) {
        iconMaskIndex = imtSelected;
    }

    for (int i = 0; i < pageObjects.Size(); i++)
        {
            MenuObjC& currObj = pageObjects.GetRef(i);
            if (currObj.objFunction)
                {
                    switch (currObj.objFunction->GetStatus())
                        {
                        case os_Bad     :  {
                            iconMaskIndex = imtBad;
                            break;
                        }
                        case os_NotSet  :  {
                            if (iconMaskIndex < imtNotSet)
                                iconMaskIndex = imtNotSet;
                            break;
                        }
                        case os_NoData  :  {
                            if (iconMaskIndex < imtNoData)
                                iconMaskIndex = imtNoData;
                            break;
                        }
                        default: ;
                        }
                }
        }

    TreeNode::UpdateTreeData();
}
#endif


void  MenuPageC::AddObject(MenuObjC* obj)
{
    pageObjects += obj;
}

void  MenuPageC::DeleteObject(MenuObjC* obj)
{
    pageObjects.DeleteFrom(obj);
}

// ******************************************  File I/O

bool MenuPageC::OKforCommandLine() const
{
    return false;
}

void MenuPageC::WriteToFile()
{
    //  write header
    WriteText(pageAllocator->GetPageTypeName());

    enum {pageVersion = 1}; // start at 1
    WriteInt(pageVersion);
    WriteLine();

    //  write properties
    WriteText(pageID);
    WriteBool(pageSelected);
    WriteBool(pageExpanded);
    WriteBool(pageDeleteable);
    WriteLine();

    //  get button count and write
    WriteInt(pageObjects.Size());
    WriteLine();
    for (int i = 0; i < pageObjects.Size(); i++)
        pageObjects[i]->WriteToFile();

    // write children
    WriteInt(childPages.Size());
    WriteLine();
    for (int i = 0; i < childPages.Size(); i++)
        childPages[i]->WriteToFile();

}

bool MenuPageC::ReadFromFile()
{
    int pageVersion = 0;
    if (usePageObjectVersioning)
        pageVersion = ReadInt();

    //read properties
    ReadText(pageID, FuncObjC::objectIDLen);

    pageSelected   = ReadBool();
    pageExpanded   = ReadBool();
    pageDeleteable = ReadBool();
    CheckForInputFailure("Reading Page Properties");

    // page properties done add to view
    MenuRootC::currTreeObj->AddPageLeafToRoot(this);

    //  read button count
    int nObjToRead = ReadInt();
    CheckForInputFailure("Reading page object count");

    //  type ID
    char tempID[40];

    //  read default buttons created by CreateNewMenuPage
    int nObjCreated = 0;
    for (int i = 0; i < pageObjects.Size(); i++)
        {
            MenuObjC&  currObj = pageObjects.GetRef(i);
            if (currObj.ReadFromFile())
                nObjCreated++;
            MenuRootC::currTreeObj->AddObjectLeafToRoot(this, &currObj);
        }

    //  read user entered buttons
    for (int i = nObjCreated; i < nObjToRead; i++)
        {
            ReadText(tempID, 40);
            MenuObjC* currObj = AllocObjC::CreateMenuObject(tempID, GetActualParent());

            // std::cout << "Reading obj" << tempID<< std::endl;

            currObj->ReadFromFile();
            AddObject(currObj);

            MenuRootC::currTreeObj->AddObjectLeafToRoot(this, currObj);
        }

    if (pageVersion > 0)
        ReadChildren();

    return true;
}

void MenuPageC::ReadChildren()
{
    childPages.SetEmpty();
    int nchildren = ReadInt();
    for (int i = 0; i < nchildren; i++)
        {
            char  tempID[80];
            ReadText(tempID, 80);

            //  check type
            if (!AllocPageC::MenuPageExists(tempID))
                InputError("Page type conversion error ??");

            //  create new page
#ifndef ADCONSOLEAPP
            MenuPageC* newPage = AppMenuC::CreateNewMenuPage(tempID);
#else
            MenuPageC* newPage  = AllocPageC::CreateMenuPage(tempID);
#endif

            newPage->parentPage = this;
            childPages += newPage;

            // read it
            newPage->ReadFromFile();
        }
}


const MenuPageC* MenuPageC::GetActualParent() const
{
    return this;
}

#ifndef ADCONSOLEAPP

// connection listing support
void MenuPageC::GetPageFO(FuncObjArray& pageFO) const
{
    pageFO.Alloc(pageObjects.Size());
    for (int i = 0; i < pageObjects.Size(); i++)
        {
            FuncObjC* currObj = pageObjects[i]->objFunction;
            if (currObj)
                pageFO += currObj;
        }

}

CString MenuPageC::GetTitle() const
{
    CString sName = typeText;
    return sName;
}


bool MenuPageC::AcceptAsChild(MenuPageC& newChild) const
{
    return GetPageAllocator()->AcceptAsChild(newChild.GetPageAllocator()->GetPageTypeName());
}

bool MenuPageC::AcceptAsParent(MenuPageC* newParent) const
{
    // root is usually OK
    return newParent == NULL;
}

void MenuPageC::ParentChanging(MenuPageC* newParent)
{
    parentPage = newParent;
}

void MenuPageC::RemoveFromTree()
{
    for (int i = 0; i < pageObjects.Size(); i++)
        pageObjects[i]->DeleteNode();

    for (int i = 0; i < childPages.Size(); i++)
        childPages[i]->RemoveFromTree();

    DeleteNode();
}

void MenuPageC::AddToTree(HTREEITEM insertAfter)
{
    baseTree->AddPageItem(this, insertAfter);
    for (int i = 0; i < pageObjects.Size(); i++)
        baseTree->AddObjectItem(this, pageObjects[i]);

    for (int i = 0; i < childPages.Size(); i++)
        {
            childPages[i]->parentPage = this;
            childPages[i]->baseTree = baseTree;
            childPages[i]->AddToTree(NULL);
        }
}

void MenuPageC::UpdatePageUI()
{
    UpdateTreeData();
    for (int i = 0; i < pageObjects.Size(); i++)
        pageObjects[i]->UpdateTreeData();
    for (int i = 0; i < childPages.Size(); i++)
        childPages[i]->UpdatePageUI();

}

void MenuPageC::ClearPageUI()
{
    for (int i = 0; i < pageObjects.Size(); i++)
        pageObjects[i]->ClearUI();
    for (int i = 0; i < childPages.Size(); i++)
        childPages[i]->ClearPageUI();
}


bool MenuPageC::AcceptAsObject(const MenuObjC& newObject) const
{
    return GetActualParent()->GetPageAllocator()->ObjectOKForPage(newObject.GetAllocName());
}

bool MenuPageC::IsPlotPage() const
{
    return false;
}

bool MenuPageC::IsSameType(MenuPageC* pageToCheck) const
{
    return pageToCheck->GetPageAllocator() == pageAllocator;
}

bool MenuPageC::IsSameType(const char* pageTypeNameCheck) const
{
    return pageAllocator->IsSameType(pageTypeNameCheck);
}

void MenuPageC::ResetChildren()
{
    // default does nothing
}


const PullMenuC* MenuPageC::GetPageObjectMenu() const
{
    return &GetActualParent()->pageAllocator->pageObjectMenu;
}

bool MenuPageC::ObjectOKForPage(const char*  objID)     const
{
    return GetActualParent()->GetPageAllocator()->ObjectOKForPage(objID);
}


void MenuPageC::ObjectPageChangeComplete(MenuPageC* oldParent) const
{
    // default does nothing
}

bool MenuPageC::IsParentOf(const MenuPageC* potentialChild) const
{
    // kluge
    if (childPages.Found(potentialChild))
        return true;

    for (int i = 0; i < childPages.Size(); i++)
        if (childPages[i]->IsParentOf(potentialChild))
            return true;

    return false;
}

#endif

