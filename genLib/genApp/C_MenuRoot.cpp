///////////////////////////////////////////////////////////////////////////////////
//
// C_MenuRoot.cpp
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
// DESCRIPTION:  Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#ifndef ADCONSOLEAPP
#include "GenLibRes.h"
#endif

#include <genClass/U_Str.h>

#include <genClass/C_FuncObj.h>

#include <genApp/AppConfigFile.h>

#include <genApp/C_MenuObj.h>
#include <genApp/C_MenuPage.h>
#include <genApp/C_DataPage.h>

#include <genApp/C_AllocObj.h>

#ifndef ADCONSOLEAPP
#include <genApp/C_AppMenu.h>
#include <genApp/C_TreeBase.h>
#include <genApp/ProjectUtil.h>
#endif

#include <genApp/C_MenuRoot.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//  declare static members
#pragma warning(disable : 4073)
#pragma init_seg(lib)  // make sure this is constructed before any objects in app

T_SC_BasicPtrArray<MenuRootC> MenuRootC::rootList(8);
MenuRootC* MenuRootC::currTreeObj   = 0; //  current object

// ***************************************************************** construct/destruct


MenuRootC::MenuRootC(const char* rID, const char* sID, const char* ibDesc) :
    rootID(rID), shortID(sID), infoBarDesc(ibDesc), pageLeafs(32)
{
    rootList.AddTo(this);
#ifndef ADCONSOLEAPP
    treeUI = 0;
#endif
}

MenuRootC::~MenuRootC()
{
    rootList.DeleteFrom(this);
#ifndef ADCONSOLEAPP
    delete treeUI;
#endif
}

#ifndef ADCONSOLEAPP

void MenuRootC::SetupUI(CWnd* pParentWnd)
{
    if (treeUI != 0)
        return;

    MenuObjC::currMenuObj = 0;
    MenuPageC::currPageObj = 0;

    treeUI = new TreeBase(*this);
    CRect rect;
    pParentWnd->GetClientRect(rect);
    treeUI->Create(pParentWnd, rect);

    for (int i = 0; i < pageLeafs.Size(); i++)
        {
            MenuPageC& currPage = pageLeafs.GetRef(i);
            currPage.baseTree = treeUI;
            currPage.AddToTree(NULL);
        }
}

MenuPageC* MenuRootC::GetPageWithSelectedObject(MenuPageC& currPage)
{
    if (currPage.IsExpanded())
        {
            for (int i = 0; i < currPage.pageObjects.Size(); i++)
                if (currPage.pageObjects[i]->IsSelected())
                    return &currPage;

            for (int i = 0; i < currPage.childPages.Size(); i++)
                {
                    MenuPageC* selPage = GetPageWithSelectedObject(currPage.childPages.GetRef(i));
                    if (selPage != 0)
                        return selPage;
                }
        }

    return 0;
}

void MenuRootC::DoSelectCurrentPage(MenuPageC& currPage)
{
    if (currPage.IsSelected())
        {
            currPage.SelectPage();
            return;
        }

    for (int i = 0; i < currPage.childPages.Size(); i++)
        DoSelectCurrentPage(currPage.childPages.GetRef(i));
}


void MenuRootC::SelectRoot()
{
    MenuRootC::currTreeObj = this;

    MenuPageC* selPage = 0;
    for (int i = 0; i < pageLeafs.Size(); i++)
        {
            pageLeafs.GetRef(i).UpdatePage();
            selPage = GetPageWithSelectedObject(pageLeafs.GetRef(i));
            if (selPage != 0)
                break;
        }

    if (selPage == 0)
        selPage = pageLeafs[0];

    if (selPage)
        selPage->SelectPage();
}

void MenuRootC::SelectCurrentPage()
{
    for (int i = 0; i < pageLeafs.Size(); i++)
        DoSelectCurrentPage(pageLeafs.GetRef(i));
}

void MenuRootC::UnSelectRoot()
{
    MenuObjC* selObj = MenuObjC::currMenuObj;
    // anything open ?
    if (selObj != 0)
        {
            // yes so close
            selObj->PropCloseMenu();
            // and reset selected status which normally gets set
            // to false by PropCloseMenu();
            selObj->objSelected = true;
        }

    // unselect UI
    if (treeUI != 0)
        treeUI->Select(0, 0);

    // no tree, page, or object selected
    MenuRootC::currTreeObj = NULL;
    MenuPageC::currPageObj = NULL;
    MenuObjC::currMenuObj  = NULL;
}


void MenuRootC::CloseUI()
{
    if (!treeUI)
        return;

    treeUI->DestroyWindow();
    delete treeUI;
    treeUI = 0;

    // for safeties sake ...
    for (int i = 0; i < pageLeafs.Size(); i++)
        pageLeafs.GetRef(i).ClearPageUI();
}


void MenuRootC::CloseAllUI()
{
    for (int i = 0; i < rootList.Size(); i++)
        rootList[i]->CloseUI();
}

int     MenuRootC::GetNTree()
{
    return rootList.Size();
}

bool MenuRootC::PageIsSelected()
{
    if (treeUI != NULL)
        return treeUI->PageIsSelected();
    return false;
}

bool MenuRootC::ObjectIsSelected()
{
    if (treeUI != NULL)
        return treeUI->ObjectIsSelected();
    return false;
}


void MenuRootC::CreateDefaultPage(const char* defPageID)
{
    MenuPageC* defaultPg = AppMenuC::CreateNewMenuPage(defPageID);

    // default is not deleteable
    defaultPg->pageDeleteable = false;
    defaultPg->pageSelected = true;
    defaultPg->pageExpanded = true;
    MenuPageC::currPageObj = defaultPg;

    // add the page to the root
    AddPageLeafToRoot(defaultPg);

    // add folder object
    if (!defaultPg->pageObjects.IsEmpty())
        AddObjectLeafToRoot(defaultPg, defaultPg->pageObjects[0]);
}


void MenuRootC::SetDefault()
{
    ClearAll();
}

#endif

void  MenuRootC::AddPageLeafToRoot(MenuPageC*  pageToAdd)
{
    if (pageToAdd->parentPage == NULL)
        pageLeafs += pageToAdd;

#ifndef ADCONSOLEAPP
    // add to UI
    pageToAdd->UpdateTreeData();
    if (treeUI != NULL)
        {
            pageToAdd->baseTree = treeUI;
            treeUI->AddPageItem(pageToAdd, NULL);
            pageToAdd->UpdateTreeData();
        }
#endif
}

void  MenuRootC::DelPageLeafFromRoot(MenuPageC*  pageToDel)
{
    pageLeafs.DeleteFrom(pageToDel);
}


void MenuRootC::AddObjectLeafToRoot(MenuPageC*  pageBeingAddedTo,
                                    MenuObjC*   objectToAdd)
{
#ifndef ADCONSOLEAPP

    // set initial text values
    objectToAdd->UpdateTreeData();
    if (treeUI != NULL)
        {
            treeUI->AddObjectItem(pageBeingAddedTo, objectToAdd);
            // do it again now the UI is there
            objectToAdd->UpdateTreeData();
        }
#endif
}


// ******************************************  File I/O

void MenuRootC::WriteToFile(bool cmdLineOnly)
{
    //  write header
    WriteText("TreeRoot");

    //  write properties
    WriteText(rootID);

    if (cmdLineOnly)
        {
            int nOK = 0;
            for (int i = 0; i < pageLeafs.Size(); i++)
                if (pageLeafs[i]->OKforCommandLine())
                    nOK++;

            // get page count and write
            WriteInt(nOK);
            WriteLine();
            for (int i = 0; i < pageLeafs.Size(); i++)
                if (pageLeafs[i]->OKforCommandLine())
                    pageLeafs[i]->WriteToFile();
        }
    else
        {
            // get page count and write
            WriteInt(pageLeafs.Size());
            WriteLine();

            for (int i = 0; i < pageLeafs.Size(); i++)
                pageLeafs[i]->WriteToFile();
        }
}


bool MenuRootC::ReadFromFile()
{
#ifndef ADCONSOLEAPP
    ResetTreeUI();
#endif

    // dummy read on  header
    char tempStr[80];
    ReadText(tempStr, 80);

    // and on ID
    ReadText(tempStr, 80);

    //  read button count
    int nPageToRead = ReadInt();
    CheckForInputFailure("Reading page count");

    // read pages
    currTreeObj = this;

    for (int i = 0; i < nPageToRead; i++)
        {
            char  tempID[80];
            ReadText(tempID, 80);

            //  check for page OK
            if (!AllocPageC::MenuPageExists(tempID))
                InputError("Page type conversion error ??");

            //  create new page

#ifndef ADCONSOLEAPP
            MenuPageC* newPage = AppMenuC::CreateNewMenuPage(tempID);
#else
            MenuPageC* newPage  = AllocPageC::CreateMenuPage(tempID);
#endif

            newPage->parentPage = 0;  // is root

            // read it
            newPage->ReadFromFile();
        }

    return true;
}

void MenuRootC::Destroy()
{
#ifndef ADCONSOLEAPP
    CloseUI();
#endif
    ClearAll();
}

void MenuRootC::DestroyAll()
{
    for (int i = 0; i < rootList.Size(); i++)
        rootList[i]->Destroy();
#ifndef ADCONSOLEAPP
    currTreeObj = 0;
#endif
}

void MenuRootC::ClearAll()
{
#ifndef ADCONSOLEAPP
    bool treeOK = (treeUI != NULL);
    if (treeOK)
        treeUI->ClearAllTreeData();
#endif
    //  delete all attached pages
    if (pageLeafs.IsEmpty())
        return;

#ifndef ADCONSOLEAPP
    if (treeOK)
        treeUI->IgnoreSelections();
#endif

    // usually goes faster from last to 1st
    for (int i = pageLeafs.UpperBound(); i >= 0; i--)
        {
            MenuPageC* delPage = pageLeafs[i];
            pageLeafs[i] = 0;       // to stop pageLeafs from changing in MenuPageC dtor
            delete delPage;
        }
    pageLeafs.SetEmpty();

#ifndef ADCONSOLEAPP
    if (treeOK)
        treeUI->ProcessSelections();
#endif
}


#ifndef ADCONSOLEAPP

void MenuRootC::ClearOldDefaults()
{
    if (pageLeafs.IsEmpty() || (!pageLeafs[0]->IsSameType(DataPageAlloc::PageTypeName())))
        return;

    // in old version default data page (first page) always had
    // default objects that weren't deleteable
    // these are changed to deleteable to allow cleanup
    MenuObjArray& firstPage = pageLeafs[0]->pageObjects;

    // skip first object which is DatapgDesc and is still not deleteable
    for (int i = 1; i < firstPage.Size(); i++)
        {
            firstPage.GetRef(i).objDeleteable = true;
        }
}


void MenuRootC::SetDefaultDataPage()
{
    ClearAll();
    ResetTreeUI();
    CreateDefaultPage(DataPageAlloc::PageTypeName());
    // select first page
    pageLeafs[0]->SelectPage();
}



void MenuRootC::ResetTreeUI()
{
    SetTreeUI();
    if (treeUI != 0) {
        treeUI->ResetStyle();
    }
}

void MenuRootC::SetTreeUI()
{
    int nSingleLine, nSingleLineType, nShowUnused, nShowConnections;
    projectUtilities::GetTreeViewSettings(nSingleLine, nSingleLineType, nShowUnused, nShowConnections);
    TreeBase::m_bSingleLine = nSingleLine != 0;
    TreeBase::m_nSingleLineType = TreeBase::SingleLineTypes(nSingleLineType);
    TreeBase::m_bShowNoOutput = nShowUnused != 0;
    TreeBase::m_bShowConnectedObjects = nShowConnections != 0;
}

void MenuRootC::UpdateAllUI()
{
    SetTreeUI();
    for (int i = 0; i < rootList.Size(); i++)
        rootList[i]->UpdateUI();
}

void MenuRootC::UpdateUI()
{
    if (treeUI == 0) {
        return;
    }
    treeUI->ResetStyle();
    for (int i = 0; i < pageLeafs.Size(); i++)
        pageLeafs[i]->UpdatePageUI();

    treeUI->Invalidate();
    treeUI->UpdateWindow();
}

void MenuRootC::UpdateCurrentTree()
{
    if (MenuRootC::currTreeObj)
        {
            MenuPageArray& currPages = MenuRootC::currTreeObj->pageLeafs;
            for (int i = 0; i < currPages.Size(); i++)
                if (currPages[i]->pageExpanded)
                    currPages[i]->UpdatePage();
        }
}


MenuPageC*  MenuRootC::FindPage(const char* idToMatch,
                                const MenuPageC* onPage) const
{
    const MenuPageArray* pageList;
    if (onPage == 0)
        {
            pageList = &pageLeafs;
        }
    else
        {
            pageList = &onPage->childPages;
        }
    for (int i = 0; i < pageList->Size(); i++)
        {
            MenuPageC* currPage = (*pageList)[i];
            if (currPage->pageObjects.IsNotEmpty() &&
                (SameString(currPage->pageObjects[0]->GetID(), idToMatch)))
                return currPage;
        }

    return 0;
}


bool MenuRootC::PageExists(const char* pageID,
                           const MenuPageC* onPage) const
{
    return FindPage(pageID, onPage) != 0;
}


void MenuRootC::DeletePage(const char* pageID,
                           MenuPageC* onPage)
{
    MenuPageC* pageToDel = FindPage(pageID, onPage);
    if (pageToDel)
        {
            if (onPage == NULL)
                DelPageLeafFromRoot(pageToDel);
            delete pageToDel;
        }
}

#endif

