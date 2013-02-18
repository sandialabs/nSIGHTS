///////////////////////////////////////////////////////////////////////////////////
//
// C_AppMenu.cpp
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
//              support for Object and Page pulldown and popup menus in applications.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include "GenLibRes.h"

#include <string.h>

#include <genApp/C_AllocObj.h>
#include <genApp/C_MenuRoot.h>
#include <genApp/C_ExposedObj.h>
#include <genApp/C_TreeBase.h>

#include <genApp/C_DataPage.h>
#include <genApp/C_PlotPage.h>
#include <genApp/C_ListPage.h>
#include <genApp/C_CompositePage.h>
#include <genApp/C_PlotFolderPage.h>

#include <genApp/DefaultObjects.h>
#include <genApp/AppCopyPaste.h>

#include <genClass/U_Msg.h>
#include <genClass/SC_BoolArray.h>

#include <genApp/C_AppMenu.h>
#include <genApp/C_AllocPage.h>
#include <genApp/WindowSelector.h>

#include <genPlotClassPS/PC_PlatformPS.h>
#include <genListClassPS/C_ListDefPS.h>

#include <genApp/GenAppMessages.h>

#include <genClassPS/PS_FindFiles.h>
#include <genApp/ProjectUtil.h>

#include "genApp/Settings/TemplateDirectorySettingsPage.h"
#include <genUIExt/FileDialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// not in NS -- RK style ...

WindowSelector windowSelector;

static void SelectCallBack(int nItem)
{
    AppMenuC::BringWindowToTop(nItem);
}

static void MinimizePlotsCallBack()
{
    AppMenuC::ShowAppWindows(SW_MINIMIZE);
}

static void TilePlotsCallBack(int nOffset)
{
    if (nOffset == 2) {
        AppMenuC::CascadePlots();
    } else {
        AppMenuC::TilePlots(nOffset == 1);
    }
}

AppMenuC*      AppMenuC::theAppMenu         = 0;

UINT           AppMenuC::mainMenuBarID      = 0;
//      menu bar access for set sensitive / set object pulldowns
UINT           AppMenuC::menuBarObjPos      = 0;
UINT           AppMenuC::objectNewPos       = 0;
UINT           AppMenuC::objectDupPos       = 0;
UINT           AppMenuC::objectCopyPos      = 0;
UINT           AppMenuC::objectCopyPagePos  = 0;
UINT           AppMenuC::objectPastePos     = 0;
UINT           AppMenuC::objectDelPos       = 0;
UINT           AppMenuC::objectApplyPos     = 0;

UINT           AppMenuC::menuBarPagePos     = 0;
UINT           AppMenuC::pageDupPos         = 0;
UINT           AppMenuC::pageDelPos         = 0;
UINT           AppMenuC::pageCopyCurrentPos = 0;
UINT           AppMenuC::pageCopyAllPos     = 0;
UINT           AppMenuC::pagePastePos       = 0;

UINT           AppMenuC::pageBringToTopPos  = 0;
UINT           AppMenuC::windowListStartPos = 1;

const char*    AppMenuC::templateFileExtension = "xxx";
SC_StringArray AppMenuC::templateFileNames(stdFileStrLen);


appFuncObjGlobals::AppFuncArray AppMenuC::systemObj("System");

bool    AppMenuC::objectMenusCreated = false;

void AppMenuC::InitMainMenuBar(UINT nID)
{
    mainMenuBarID = nID;
}

UINT AppMenuC::GetMainMenuBarID()
{
    ASSERT(mainMenuBarID != 0); // must be set
    return mainMenuBarID;
}


void AppMenuC::InitAppData(int init_obj_ID_VALUE)
{
    //      already initialized ??
    if (theAppMenu)
        GenAppInternalError("AppMenuC::InitAppData()");

    PullMenuC::InitializeMapping(init_obj_ID_VALUE);

    systemObj.AddTo(&appDefaultObjects::sysInfoObj);
    systemObj.AddTo(&appDefaultObjects::defaultPenObj);
    systemObj.AddTo(&appDefaultObjects::coldHotMapObj);
    systemObj.AddTo(&appDefaultObjects::whiteBlackMapObj);

    FuncObjC::AddDefaultFO(&appDefaultObjects::defaultPenObj, typeid(DO_PenSet));
    FuncObjC::AddDefaultFO(&appDefaultObjects::coldHotMapObj, typeid(DO_ColorMap));

    appFuncObjGlobals::appFuncData += &systemObj;

    templateFileNames.SetResizable(16);
    theAppMenu = this;
}



void AppMenuC::CreateObjectMenu()
{
    if (!objectMenusCreated)
        {
            theAppMenu->DoObjectMenuCreate();
            objectMenusCreated = true;
        }
}

HMENU AppMenuC::GetCurrentCascade()
{
    ASSERT(MenuPageC::currPageObj);

    const MenuPageC* pageWithPull = MenuPageC::currPageObj->GetActualParent();

    const PullMenuC* currPull = pageWithPull->GetPageObjectMenu();
    if (currPull && pageWithPull->GetPageAllocator()->pageObjectIDs.IsNotEmpty())
        return currPull->m_hMenu;

    return 0;
}

void AppMenuC::SetDropdownMenu(CMenu* objDropDown, UINT objectNewPos, HMENU currCascade)
{
    // RemoveMenu unlike ModifyMenu doesn't destroy the handle for a pop-up menu
    objDropDown->RemoveMenu(objectNewPos, MF_BYPOSITION);

    if (currCascade != NULL)
        {
            objDropDown->InsertMenu(objectNewPos,
                                    MF_BYPOSITION | MF_POPUP | MF_ENABLED,
                                    UINT(currCascade), "&New");
        }
    else
        {
            objDropDown->InsertMenu(objectNewPos,
                                    MF_BYPOSITION | MF_GRAYED,
                                    0, "&No objects");
        }


}

void  AppMenuC::ResetObjectMenus()
{
    if (MenuPageC::currPageObj == 0)
        {
            SetMenuBarSensitive();
            return;
        }

    // setup New
    HMENU currCascade = GetCurrentCascade();

    CWnd* pMain = AfxGetMainWnd();
    CMenu* objDropDown = pMain->GetMenu()->GetSubMenu(menuBarObjPos);

    SetDropdownMenu(objDropDown, objectNewPos, currCascade);

    SetMenuBarSensitive();
}

void  AppMenuC::ResetObjectPopupMenu(CMenu* popMenu)
{
    HMENU currCascade = GetCurrentCascade();
    SetDropdownMenu(popMenu, objectNewPos, currCascade);
}

void  AppMenuC::ResetPagePopupMenu(CMenu* popMenu)
{
    HMENU currCascade = GetCurrentCascade();
    popMenu->InsertMenu(0,
                        MF_BYPOSITION|MF_POPUP|MF_ENABLED,
                        UINT(currCascade),
                        "&New Object");
}


CMenu* AppMenuC::GetPageMenu()
{
    CWnd* pMain = AfxGetMainWnd();
    CMenu* menuBar = pMain->GetMenu();
    return menuBar->GetSubMenu(menuBarPagePos);
}

CMenu* AppMenuC::GetObjectMenu()
{
    CWnd* pMain = AfxGetMainWnd();
    CMenu* menuBar = pMain->GetMenu();
    return menuBar->GetSubMenu(menuBarObjPos);
}

CMenu* AppMenuC::GetPageMenu(CMenu& menuBar)
{
    return menuBar.GetSubMenu(menuBarPagePos);
}

CMenu* AppMenuC::GetObjectMenu(CMenu& menuBar)
{
    return menuBar.GetSubMenu(menuBarObjPos);
}


// page and object
void AppMenuC::SetMenuBarSensitive()
{
    CWnd* pMain = AfxGetMainWnd();
    CMenu* menuBar = pMain->GetMenu();

    UINT flags;
    if (MenuRootC::currTreeObj == 0)
        {
            flags = MF_BYPOSITION | MF_DISABLED | MF_GRAYED;
        }
    else
        {
            flags = MF_BYPOSITION | MF_ENABLED;
        }

    menuBar->EnableMenuItem(menuBarPagePos, flags);
    menuBar->EnableMenuItem(menuBarObjPos, flags);

    pMain->DrawMenuBar();
}


void AppMenuC::InitWindowPopupMenu(CMenu* windowMenu, int nStartID)
{
    // remove all
    int nCount = windowMenu->GetMenuItemCount()- windowListStartPos;
    int i;
    for (i = 0; i < nCount; i++)
        {
            VERIFY(windowMenu->RemoveMenu(windowListStartPos, MF_BYPOSITION));
        }


    bool addType = ! PS_MainWindow::AllSameType();
    int nCommandID = nStartID;

    for (i = 0; i < PS_MainWindow::mainWindows.Size(); i++)
        {
            PS_MainWindow& currWnd = *(PS_MainWindow::mainWindows[i]);
            if (!currWnd.IsVisible())
                continue;

            char windowText[80];
            currWnd.GetMainWindowTitle(windowText, addType, 80);

            if (nCommandID == nStartID)
                {
                    VERIFY(windowMenu->AppendMenu(MF_SEPARATOR));
                }
            VERIFY(windowMenu->AppendMenu(MF_STRING|MF_ENABLED, nCommandID++, windowText));
        }
}

void AppMenuC::InitTemplatePopupMenu(CMenu* templateMenu, int nStartID)
{
    // remove all
    int nCount = templateMenu->GetMenuItemCount();
    for (int i = 0; i < nCount; i++)
        {
            VERIFY(templateMenu->RemoveMenu(0, MF_BYPOSITION));
        }

    CString sTemp;
    projectUtilities::GetTemplateDirectory(sTemp);

    char fileString[stdFileStrLen];
    MakeString(fileString, "*.", templateFileExtension, stdFileStrLen);

    char errMsg[psFFErrMsgLen];
    templateFileNames.SetEmpty();
    PS_FindFiles(sTemp, templateFileNames, fileString, errMsg);

    if (templateFileNames.IsEmpty() )
        {
            VERIFY(templateMenu->AppendMenu(MF_STRING|MF_DISABLED, nStartID, "<no files>"));
        }
    else
        {
            int nCommandID = nStartID;
            for (int i = 0; i < templateFileNames.Size(); i++)
                {
                    char dispName[stdFileStrLen];
                    CopyString(dispName, templateFileNames[i], stdFileStrLen);
                    char* extPos = strstr(dispName, templateFileExtension);
                    if (extPos != NULL)
                        {
                            extPos--;
                            *extPos = '\0';
                        }
                    VERIFY(templateMenu->AppendMenu(MF_STRING|MF_ENABLED, nCommandID++, dispName));
                }
        }
}
void AppMenuC::GetSelectedTemplateFileName(int      nsel,
                                           char*    selFName,
                                           int      fNameLen)
{
    if (templateFileNames.Size() < nsel)
        GenAppInternalError("AppMenuC::GetSelectedTemplateFileName");

    CString sTemp;
    projectUtilities::GetTemplateDirectory(sTemp);

    MakeString(selFName, sTemp, "\\", templateFileNames[nsel], fNameLen);
}



void AppMenuC::BringWindowToTop(int nID)
{
    if (nID == -1) // main window
        {
            CWnd* pWnd = AfxGetMainWnd();
            if (pWnd->IsIconic()) {
                pWnd->ShowWindow(SW_RESTORE);
            }
            pWnd->BringWindowToTop();
            return;
        }

    for (int i = 0; i < PS_MainWindow::mainWindows.Size(); i++)
        {
            PS_MainWindow& currWnd = *(PS_MainWindow::mainWindows[i]);
            if (currWnd.IsVisible() && (nID-- == 0))
                currWnd.MainWindowToTop();
        }

}

void AppMenuC::BringPageToTop()
{
    MenuPageC* page = MenuPageC::currPageObj;
    BringPageToTop(page);
}

void AppMenuC::BringPageToTop(MenuPageC* page)
{
    if (page->assocWindow != NULL)
        page->assocWindow->MainWindowToTop();

}


MenuPageC* AppMenuC::CreateNewMenuPage(const char* pageTypeID)
{
    MenuPageC* newPage  = AllocPageC::CreateMenuPage(pageTypeID);

    // kluge for naming plot windows
    if ((MenuRootC::rootList.Size() > 1) && (newPage->IsPlotPage()) && (MenuRootC::currTreeObj != 0))
        {
            static_cast<PlotPageC*>(newPage)->GetPlot()->preID = MenuRootC::currTreeObj->shortID;
        }

    return newPage;
}



// ****************************************** object operations


static bool GetSelectedObjects(MenuObjArray& selObjs,
                               const AllocPageC* &selPageAllocator)
{
    if (MenuRootC::currTreeObj != 0)
        {
            selObjs = MenuRootC::currTreeObj->GetUI()->selectedObjects;
            selPageAllocator = MenuRootC::currTreeObj->GetUI()->selectedObjectPageAllocator;
        }

    if (selObjs.IsEmpty() && (MenuObjC::currMenuObj != 0) && (MenuPageC::currPageObj != 0))
        {
            selObjs.Alloc(1);
            selObjs += MenuObjC::currMenuObj;
            selPageAllocator = MenuPageC::currPageObj->GetActualParent()->GetPageAllocator();
        }
    return ! selObjs.IsEmpty();
}

static bool GetCopyableObjects(MenuObjArray& selObjs,
                               const AllocPageC* &selPageAllocator)
{
    if (!GetSelectedObjects(selObjs, selPageAllocator))
        return false;

    int nOK = 0;
    for (int i = 0; i < selObjs.Size(); i++)
        if (selObjs[i]->IsCopyable())
            selObjs[nOK++] = selObjs[i];
    selObjs.SetSize(nOK);

    return ! selObjs.IsEmpty();
}

// delete support -- objects and pages
static bool GetAffectedObjects(const MenuObjArray& objToDelete,
                               FuncObjArray& affectedObjects)
{
    // FOs to delete
    FuncObjArray deleteObjects;
    deleteObjects.Alloc(objToDelete.Size());

    for (int i = 0; i < objToDelete.Size(); i++)
        if (objToDelete[i]->objFunction != NULL)
            deleteObjects += objToDelete[i]->objFunction;

    affectedObjects.Alloc(FuncObjC::funcObjList.Size());

    if (deleteObjects.IsEmpty())
        return false;

    // get downstream objects
    for (int i = 0; i < FuncObjC::funcObjList.Size(); i++)
        {
            FuncObjC* currFO = FuncObjC::funcObjList[i];
            if (!deleteObjects.Found(currFO))
                {
                    // is it directly downstream of a delete object
                    for (int j = 0; j < deleteObjects.Size(); j++)
                        if (currFO->UsesAsInput(deleteObjects[j]))
                            {
                                affectedObjects += currFO;
                                break;
                            }
                }
        }

    return affectedObjects.IsNotEmpty();
}


void  AppMenuC::DeleteObject()
{
    MenuObjArray selObjs;
    const AllocPageC* selPageAllocator;

    if (!GetSelectedObjects(selObjs, selPageAllocator))
        return;

    // slight kluge -- only delete objects on current page
    // may fix if someone complains

    MenuPageC& currPage = *MenuPageC::currPageObj;
    int nOK = 0;
    for (int i = 0; i < selObjs.Size(); i++)
        {
            MenuObjC* currObj = selObjs[i];
            if (currObj->IsDeleteable() && currPage.pageObjects.Found(currObj))
                {
                    // delete current
                    if (currObj == MenuObjC::currMenuObj)
                        {
                            currObj->CloseOpenMenu();
                            MenuObjC::currMenuObj = 0;
                        }

                    selObjs[nOK++] = currObj;
                }
        }
    selObjs.SetSize(nOK);

    if (selObjs.IsEmpty())
        return;

    CWaitCursor wait;

    // make copy
    appCopyPaste::CopyObjects(selObjs, selPageAllocator);

    // clear selections
    MenuRootC::currTreeObj->GetUI()->ClearSelections();
    MenuRootC::currTreeObj->GetUI()->IgnoreSelections();

    // get list of objects affected by deletions
    FuncObjArray affectedObjects;
    bool downstreamFound = GetAffectedObjects(selObjs, affectedObjects);

    // do deletions
    for (int i = 0; i < selObjs.Size(); i++)
        {
            MenuObjC* delObj = selObjs[i];

            // remove from page
            MenuPageC::currPageObj->DeleteObject(delObj);

            // destroy
            delete delObj;
        }

    //      recalc & update
    if (downstreamFound)
        {
            FuncObjC::CalcSelectedObjects(foc_PortOnly, affectedObjects);

            // replot
            PlotDefC::ReplotAfterCalc();
            ExposedObjC::UpdateAllExposed();
        }
    else if (MenuPageC::currPageObj->IsPlotPage())
        {
            // redraw if current is plot
            static_cast<PlotPageC*>(MenuPageC::currPageObj)->GetPlottingParent()->GetPlot()->RedrawPlot();
        }
    MenuPageC::currPageObj->UpdatePage();

    MenuRootC::currTreeObj->GetUI()->ProcessSelections();
    MenuRootC::UpdateAllUI();

    MenuRootC::currTreeObj->GetUI()->UpdateSelection();

}


void AppMenuC::AddNewObject(MenuObjC* newObj)
{
    MenuPageC::currPageObj->AddObject(newObj);

    MenuRootC::currTreeObj->AddObjectLeafToRoot(MenuPageC::currPageObj, newObj);

    newObj->UpdateTreeData();

    //      not neccessary -- called in select node
    //  newObj->PropOpenMenu();

    // update selection on tree
    newObj->objSelected = true;
    newObj->SelectNode();
}


void AppMenuC::NewObject(const char* objID)
{
    AddNewObject(AllocObjC::CreateMenuObject(objID, MenuPageC::currPageObj->GetActualParent()));
}


void AppMenuC::DuplicateObject()
{
    MenuObjArray selObjs;
    const AllocPageC* selPageAllocator;
    if (GetCopyableObjects(selObjs, selPageAllocator))
        {
            appCopyPaste::CopyObjects(selObjs, selPageAllocator);
            Sleep(100);
            PasteObject();
        }
}

void AppMenuC::CopyObject()
{
    MenuObjArray selObjs;
    const AllocPageC* selPageAllocator;
    if (GetCopyableObjects(selObjs, selPageAllocator))
        appCopyPaste::CopyObjects(selObjs, selPageAllocator);
}

void AppMenuC::CopyPageObjects()
{
    if (MenuPageC::currPageObj)
        {
            MenuPageC& currPage = *MenuPageC::currPageObj;
            MenuObjArray copyObjs(16);
            for (int i = 0; i < currPage.pageObjects.Size(); i++)
                {
                    MenuObjC* copyObj = currPage.pageObjects[i];
                    if (copyObj->IsCopyable())
                        copyObjs += copyObj;
                }
            appCopyPaste::CopyObjects(copyObjs, currPage.GetPageAllocator());
        }
}


void AppMenuC::PasteObject()
{
    CWaitCursor wait;
    appCopyPaste::PasteObjects();
}


void AppMenuC::ApplyObject()
{
    if (MenuObjC::currMenuObj)
        MenuObjC::currMenuObj->UpdateUIandExecute();
}

// ****************************************** page operations

static bool GetSelectedPages(MenuPageArray& selPages)
{
    if (MenuRootC::currTreeObj != 0)
        {
            selPages = MenuRootC::currTreeObj->GetUI()->selectedPages;
            // take ultimate parents only
            for (int i = 0; i < selPages.Size(); i++)
                {
                    MenuPageC* currPage = selPages[i];
                    if (currPage != 0)
                        for (int j = 0; j < selPages.Size(); j++)
                            {
                                MenuPageC* chkPage = selPages[j];
                                if (currPage->IsParentOf(chkPage))
                                    selPages[j] = 0;
                            }
                }
            selPages.Clean();
        }

    if (selPages.IsEmpty() && (MenuPageC::currPageObj != 0))
        {
            selPages.Alloc(1);
            selPages += MenuPageC::currPageObj;
        }
    return ! selPages.IsEmpty();
}

static bool GetCopyablePages(MenuPageArray& selPages)
{
    if (!GetSelectedPages(selPages))
        return false;

    int nOK = 0;
    for (int i = 0; i < selPages.Size(); i++)
        if (selPages[i]->IsCopyable())
            selPages[nOK++] = selPages[i];
    selPages.SetSize(nOK);

    return ! selPages.IsEmpty();
}

static void GetDeleteMenuObjects(const MenuPageC&       delPage,
                                 MenuObjArray&   menuObjsToDel)
{
    menuObjsToDel += delPage.pageObjects;
    for (int i = 0; i < delPage.childPages.Size(); i++)
        GetDeleteMenuObjects(delPage.childPages.GetRef(i), menuObjsToDel);
}


void AppMenuC::DeletePage()
{
    MenuPageArray selPages;
    if (!GetSelectedPages(selPages))
        return;

    int nOK = 0;
    for (int i = 0; i < selPages.Size(); i++)
        {
            MenuPageC* currPage = selPages[i];
            if (currPage->IsDeleteable())
                selPages[nOK++] = currPage;
        }
    selPages.SetSize(nOK);

    if (selPages.IsEmpty())
        return;

    CWaitCursor wait;

    // make copy
    appCopyPaste::CopyPages(selPages);

    // clear selections
    MenuRootC& currTree = *MenuRootC::currTreeObj;
    currTree.GetUI()->ClearSelections();
    currTree.GetUI()->IgnoreSelections();

    MenuObjArray delMenuObjects;
    delMenuObjects.SetResizable(32);
    for (int i = 0; i < selPages.Size(); i++)
        {
            MenuPageC* delPage = selPages[i];
            GetDeleteMenuObjects(*(selPages[i]), delMenuObjects);
        }

    if (delMenuObjects.Found(MenuObjC::currMenuObj))
        MenuObjC::CloseOpenMenu();

    // get downstream FO
    FuncObjArray delFuncObjects;
    delFuncObjects.SetResizable(32);

    bool downstreamFound = GetAffectedObjects(delMenuObjects, delFuncObjects);

    // do deletions
    MenuPageC* finalPage = 0;

    for (int i = 0; i < selPages.Size(); i++)
        {
            MenuPageC* delPage = selPages[i];
            if (i == selPages.UpperBound())
                {
                    finalPage = delPage->parentPage;
                    if (finalPage == NULL)
                        {
                            int lastIndex = currTree.pageLeafs.FoundAt(delPage);
                            if (lastIndex == currTree.pageLeafs.UpperBound())
                                finalPage = currTree.pageLeafs[lastIndex - 1];
                            else if (lastIndex >= 0)
                                finalPage = currTree.pageLeafs[lastIndex + 1];
                            else
                                finalPage = currTree.pageLeafs[0];
                        }
                }

            if (delPage == MenuPageC::currPageObj)
                {
                    // close menu if open
                    MenuObjC::CloseOpenMenu();
                    MenuPageC::currPageObj = 0;
                }

            // remove from root
            if (delPage->parentPage == NULL)
                MenuRootC::currTreeObj->DelPageLeafFromRoot(delPage);

            // destroy
            delete delPage;
        }

    //      recalc & update
    if (downstreamFound)
        {
            // update to make sure all downstream objects still exist
            // fixes bug in list page delete

            int nOK = 0;
            for (int i = 0; i < delFuncObjects.Size(); i++)
                {
                    if (FuncObjC::funcObjList.Found(delFuncObjects[i]))
                        {
                            delFuncObjects[nOK++] = delFuncObjects[i];
                        }
                }
            delFuncObjects.SetSize(nOK);
            if (delFuncObjects.IsNotEmpty())
                {
                    FuncObjC::CalcSelectedObjects(foc_PortOnly, delFuncObjects);
                    // replot
                    PlotDefC::ReplotAfterCalc();
                    ExposedObjC::UpdateAllExposed();
                }
        }

    currTree.GetUI()->ProcessSelections();

    if (finalPage)
        {
            if ((!downstreamFound) && (finalPage->IsPlotPage()))
                static_cast<PlotPageC*>(finalPage)->GetPlottingParent()->GetPlot()->RedrawPlot();

            finalPage->SelectPage();
            finalPage->ExpandNode();
            // if open menu was closed when page deleted
            if ((!MenuObjC::currMenuObj) && (!finalPage->pageObjects.IsEmpty()))
                finalPage->pageObjects[0]->PropOpenMenu();
        }

    MenuRootC::UpdateAllUI();
    MenuRootC::currTreeObj->GetUI()->UpdateSelection();

    // bring app window to top
    BringWindowToTop(-1);
}

void AppMenuC::ClearAllPages()
{
    CWaitCursor wait;

    CopyAllPages();

    // clear selections
    MenuRootC::currTreeObj->GetUI()->ClearSelections();

    MenuRootC::currTreeObj->SetDefault();

    //  recalc all objects
    FuncObjC::CalcAllObjects(foc_PortOnly);
    PlotDefC::ReplotAfterCalc();

    // bring app window to top
    BringWindowToTop(-1);
}

void AppMenuC::CollapseAllPages()
{
    MenuRootC::currTreeObj->GetUI()->CollapseAllPages();
}


void AppMenuC::AddNewPage(MenuPageC* newPage)
{
    // add to root if OK && parent not set already
    if ((newPage->parentPage == NULL) && (!newPage->AcceptAsParent(NULL)))
        {
            // else current page is parent
            newPage->parentPage = MenuPageC::currPageObj;
            if ((MenuPageC::currPageObj == 0) || (!newPage->AcceptAsParent(MenuPageC::currPageObj)))
                GenAppInternalError("AppMenuC::AddNewPage");
            MenuPageC::currPageObj->childPages += newPage;
        }

    MenuRootC::currTreeObj->AddPageLeafToRoot(newPage);

    for (int i = 0; i < newPage->pageObjects.Size(); i++)
        MenuRootC::currTreeObj->AddObjectLeafToRoot(newPage, newPage->pageObjects[i]);

    // de-select old page
    if (MenuPageC::currPageObj != 0)
        MenuPageC::currPageObj->UnselectPage();

    // open page
    newPage->ExpandNode();
    newPage->SelectPage();
    // first menu
    if (!newPage->pageObjects.IsEmpty())
        newPage->pageObjects[0]->PropOpenMenu();
}

void AppMenuC::NewPage(const char*  pageTypeID,
                       bool         doApply)
{
    MenuPageC* newPage  = AppMenuC::CreateNewMenuPage(pageTypeID);

    AddNewPage(newPage);

    if (doApply)
        newPage->pageObjects[0]->ExecuteObj();
}

void AppMenuC::NewPlotFolder()
{
    AddNewPage(CreateNewMenuPage(PlotFolderPageAlloc::PageTypeName()));
}


void AppMenuC::NewDataPage()
{
    // new data pages added as child to current if OK
    MenuPageC* dataPage  = AppMenuC::CreateNewMenuPage(DataPageAlloc::PageTypeName());

    if ((MenuPageC::currPageObj != 0) && (MenuRootC::currTreeObj != 0))
        {
            MenuPageC* currParent = MenuPageC::currPageObj;
            if (MenuRootC::currTreeObj->PageIsSelected())
                currParent = MenuPageC::currPageObj->parentPage;
            if ((currParent != NULL) &&
                (dataPage->AcceptAsParent(currParent)) &&
                (currParent->AcceptAsChild(*dataPage)))
                {
                    dataPage->parentPage = currParent;
                    currParent->childPages += dataPage;
                }
        }

    AddNewPage(dataPage);
}

void AppMenuC::NewListPage()
{
    NewPage(ListPageAlloc::PageTypeName(), false);
}

void AppMenuC::NewXYPage()
{
    NewPage(Plot2DXYPageAlloc::PageTypeName(), true);
}

void AppMenuC::NewXYZPage()
{
    NewPage(Plot3DXYZPageAlloc::PageTypeName(), true);
}

void AppMenuC::NewCompositePage()
{
    NewPage(CompositePlotPageAlloc::PageTypeName(), true);
}

void AppMenuC::DuplicatePage()
{
    MenuPageArray selPages;
    if (GetCopyablePages(selPages))
        {
            appCopyPaste::CopyPages(selPages);
            Sleep(100);
            PastePage();
        }
}


void AppMenuC::CopyCurrentPage()
{
    MenuPageArray selPages;
    if (GetCopyablePages(selPages))
        appCopyPaste::CopyPages(selPages);
}

void AppMenuC::CopyAllPages()
{
    appCopyPaste::CopyPages(MenuRootC::currTreeObj->pageLeafs);
}


void AppMenuC::PastePage()
{
    CWaitCursor wait;
    appCopyPaste::PastePages();
}

void AppMenuC::PrePasteSetup()
{
    // default does nothing
}

void AppMenuC::PostPasteFixup()
{
    // default does nothing
}

int AppMenuC::GetPageFOCount(const type_info&  listType,
                             const MenuPageC&  currPage,
                             FuncObjC*         extraObj,
                             int&              pageCount)
{
    int count = 0;
    bool foundOnPage = false;
    for (int i = 0; i < currPage.pageObjects.Size(); i++)
        {
            MenuObjC& obj = currPage.pageObjects.GetRef(i);
            if ((obj.objFunction != 0) && (&obj != MenuObjC::currMenuObj) && (obj.objFunction != extraObj))
                {
                    int nob = obj.objFunction->HasOutPort(listType);
                    if (nob > 0)
                        {
                            foundOnPage = true;
                            count += nob;
                        }
                }
        }
    if (foundOnPage)
        pageCount++;

    for (int i = 0; i < currPage.childPages.Size(); i++)
        count += GetPageFOCount(listType, currPage.childPages.GetRef(i), extraObj, pageCount);

    return count;
}

void AppMenuC::AddPageFO(const type_info&       listType,
                         const MenuPageC&       currPage,
                         FuncObjC*              extraObj,
                         const char*            rootID,
                         const char*            sepStr,
                         SC_StringArray&        listObjDesc,
                         FuncObjRefArray&       listObjects)
{
    char tempStr[255];
    if (currPage.pageObjects.IsNotEmpty())
        {
            for (int i = 0; i < currPage.pageObjects.Size(); i++)
                {
                    MenuObjC& obj = currPage.pageObjects.GetRef(i);
                    if ((obj.objFunction != 0) && (&obj != MenuObjC::currMenuObj) && (obj.objFunction != extraObj))
                        {
                            for (int j = 0; j < obj.objFunction->HasOutPort(listType); j++)
                                {
                                    DataObjC* tmpDO = 0;
                                    if (listType != typeid(FuncObjC))
                                        tmpDO = obj.objFunction->GetOutPortData(listType, j);
                                    MakeRefStr(tempStr, 255, rootID, &currPage, NULL, obj.GetID(), sepStr, tmpDO);
                                    listObjDesc += tempStr;
                                    listObjects += FuncObjRef(*obj.objFunction, j);
                                }
                        }
                }
        }

    for (int i = 0; i < currPage.childPages.Size(); i++)
        AddPageFO(listType, currPage.childPages.GetRef(i), extraObj, rootID, sepStr,
                  listObjDesc, listObjects);
}


bool   AppMenuC::GetFuncObjList(const type_info&  listType,
                                SC_StringArray&   listObjDesc,
                                FuncObjRefArray&  listObjects,
                                const char*       sepStr, // between each column
                                FuncObjC*         extraObj)
{
    using namespace appFuncObjGlobals;

    // ******** count all FO with output
    // start with system objects
    int count = 0;
    int nPage = 0;
    for (int i = 0; i < appFuncData.Size(); i++)
        {
            int startCount = count;
            AppFuncArray& currData = *(appFuncData[i]);
            for (int j = 0; j < currData.Size(); j++)
                count += currData[j]->HasOutPort(listType);
            if (startCount < count)
                nPage++;
        }

    // and add all roots
    for (int i = 0; i < MenuRootC::rootList.Size(); i++)
        {
            MenuRootC& root = MenuRootC::rootList.GetRef(i);
            for (int j = 0; j < root.pageLeafs.Size(); j++)
                count += GetPageFOCount(listType, root.pageLeafs.GetRef(j), extraObj, nPage);
        }

    //      reset input
    int listCount = count < 1 ? 1 : count;
    listObjDesc.DeAlloc();
    listObjDesc.SetStringLen(255);
    listObjDesc.Alloc(listCount);
    listObjects.Alloc(listCount);

    //  set initial for count = 0
    CopyString(listObjDesc[0], "<none available>", 45);

    char tempStr[80];
    FuncObjRef tempRef;
    listObjects[0] = tempRef;

    bool oneRoot = MenuRootC::GetNTree() == 1;

    // add system objects
    for (int i = 0; i < appFuncData.Size(); i++)
        {
            AppFuncArray& currData = *(appFuncData[i]);
            for (int j = 0; j < currData.Size(); j++)
                {
                    FuncObjC* currObj = currData[j];
                    int nObj = currObj->HasOutPort(listType);
                    for (int k = 0; k < nObj; k++)
                        {
                            DataObjC* tmpDO = 0;
                            if (listType != typeid(FuncObjC))
                                tmpDO = currObj->GetOutPortData(listType, k);
                            MakeRefStr(tempStr, 80, NULL, NULL, currData.categoryID, currObj->GetID(), sepStr, tmpDO);

                            listObjDesc += tempStr;

                            tempRef.objRef  = currObj;
                            tempRef.objIndx = k;

                            listObjects += tempRef;
                        }
                }
        }

    // set initial values for IDs
    const char* rootID = NULL;

    //  go through pages & objects
    for (int i = 0; i < MenuRootC::rootList.Size(); i++)
        {
            MenuRootC& root = MenuRootC::rootList.GetRef(i);
            if (!oneRoot)
                rootID = root.shortID;
            for (int j = 0; j < root.pageLeafs.Size(); j++)
                AddPageFO(listType, root.pageLeafs.GetRef(j), extraObj, rootID, sepStr,
                          listObjDesc, listObjects);
        }

    listObjects.SetSize(listCount);
    listObjDesc.SetSize(listCount);

    return (count > 0);
}


void AppMenuC::MakeRefStr(char              outStr[],  // assumed at 80 char min
                          int               maxLen,
                          const char*       rootID,
                          const MenuPageC*  currPage,
                          const char*       systemCategoryID, // if page == 0
                          const char*       FOID,
                          const char*       sepStr,
                          DataObjC*         dataObj)
{
    // clear
    SetToNull(outStr);

    // object desc
    const char* desc = "Object";

    if (dataObj)
        desc = dataObj->GetID();

    MakeString(outStr, desc, sepStr, FOID, sepStr, maxLen);
    if (currPage == 0)
        {
            ConcatString(outStr, "System:", maxLen);
            ConcatString(outStr, systemCategoryID, maxLen);
            return;
        }

    // build page ID backwards
    char pageStr[255];
    SetToNull(pageStr);
    while (currPage != 0)
        {
            InsertString(pageStr, currPage->pageID, 0, 255);
            InsertString(pageStr, "\\", 0, 255);
            currPage = currPage->parentPage;
        }

    // insert root
    if (rootID != NULL)
        {
            InsertString(pageStr, ":", 0, 255);
            InsertString(pageStr, rootID, 0, 255);
        }

    ConcatString(outStr, pageStr, maxLen);
}



void AppMenuC::InitWindowSelector()
{
    windowSelector.Init(SelectCallBack, MinimizePlotsCallBack, TilePlotsCallBack);
}

void AppMenuC::SwitchWindowSelector()
{
    if (windowSelector.Switch()) {
        RefreshWindowSelector(NULL, false);
    }
}

bool AppMenuC::IsWindowSelectorEnabled()
{
    return windowSelector.IsEnabled();
}

// when both pointers NULL -> mainframe
void AppMenuC::RefreshWindowSelector(PS_MainWindow*  pcurrWnd,
                                     bool            bSaveCurrent)
{
    if (!windowSelector.IsEnabled() || windowSelector.IsProcessingSelectionCallback())
        {
            return;
        }
    int nOldSelectID;
    if (bSaveCurrent)
        {
            nOldSelectID = windowSelector.GetSelectedID();
        }
    // remove all
    windowSelector.RemoveAll();

    // fill it in
    int nCommandID = 0;
    int nSelectID = 0;
    windowSelector.AddItem(nCommandID++, "Main Menu");

    bool addType = !PS_MainWindow::AllSameType();

    int nWindows = 0;
    for (int i = 0; i < PS_MainWindow::mainWindows.Size(); i++)
        {
            PS_MainWindow& currWnd = *(PS_MainWindow::mainWindows[i]);
            if (!currWnd.IsVisible())
                continue;

            char windowText[80];
            currWnd.GetMainWindowTitle(windowText, addType, 80);

            if (pcurrWnd == &currWnd)
                {
                    nSelectID = nCommandID;
                }

            windowSelector.AddItem(nCommandID++, windowText);
            nWindows++;
        }

    // select current
    if (bSaveCurrent)
        {
            windowSelector.SelectItem(nOldSelectID);
        }
    else
        {
            windowSelector.SelectItem(nSelectID);
        }
    windowSelector.EnableTileCascade(nWindows >= 2);
    windowSelector.Redraw();
}


void AppMenuC::IndicatePropMenuOpen()
{
    if ((MenuObjC::currMenuObj == NULL) || (MenuPageC::currPageObj == NULL))
        return;


    CString sText = MenuObjC::currMenuObj->GetLongDesc();
    AfxGetMainWnd()->SendMessage(WM_GENAPP_PROPMENU, WPARAM(LPCSTR(sText)));
}

void AppMenuC::SetPlotFont(bool bSmall)
{
    for (int i = 0; i < PlotDefC::plotList.Size(); i++)
        PlotDefC::plotList[i]->platformData->SetStatusSmall(bSmall);
}

bool AppMenuC::IsAnyAppWindowOpen()
{
    for (int i = 0; i < PS_MainWindow::mainWindows.Size(); i++)
        {
            if (PS_MainWindow::mainWindows[i]->IsVisible())
                return true;
        }

    return false;
}


void AppMenuC::ShowAppWindows(int nCmdShow)
{
    for (int i = 0; i < PS_MainWindow::mainWindows.Size(); i++)
        {
            PS_MainWindow& currWnd = *(PS_MainWindow::mainWindows[i]);
            if (currWnd.IsVisible())
                currWnd.ShowMainWindow(nCmdShow);
        }

    AfxGetMainWnd()->ShowWindow(nCmdShow);

    if (nCmdShow == SW_MINIMIZE) {
        windowSelector.SelectItem(-1);
    }
}

static void CalculateTileSizes(int nWindows, int& nColumns, int& nRows, int& nPlus)
{
    nColumns = 1;
    for (int i = 2; ; i++) {
        int nIntermediate = nWindows / i;
        if (i > nIntermediate) {
            break;
        }
        nColumns = i;
    }
    nPlus = nWindows % nColumns;
    nRows = nWindows / nColumns;
}

static int GetNumberPlotWindows()
{
    return PlotDefC::plotList.Size();
}

void AppMenuC::TilePlots(bool bVertically)
{
    int nWindows = GetNumberPlotWindows();

    int nColumns;
    int nRows;
    int nPlus;
    if (bVertically) {
        // tile vertically
        CalculateTileSizes(nWindows, nRows, nColumns, nPlus);
    } else {
        // tile horizontally
        CalculateTileSizes(nWindows, nColumns, nRows, nPlus);
    }

    CRect rect;
    VERIFY(::SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0));

    int nRow = 0;
    int nColumn = 0;

    for (int i = 0; i < MenuRootC::rootList.Size(); i++)
        {
            MenuRootC& root = MenuRootC::rootList.GetRef(i);
            for (int j = 0; j < root.pageLeafs.Size(); j++)
                {
                    MenuPageC& page = root.pageLeafs.GetRef(j);
                    if (page.IsPlotPage())
                        {
                            int nWidth = rect.Width() / nColumns;
                            int nX0 = rect.left + nColumn * nWidth;

                            int nRowsActual = nColumn >= (nColumns - nPlus) ? nRows+1 : nRows;
                            int nHeight = rect.Height() / nRowsActual;
                            int nY0 = rect.top + nRow * nHeight;

                            page.assocWindow->ResizeMainWindow(nX0, nY0, nWidth, nHeight);

                            nRow = (nRow+1) % nRowsActual;
                            if (nRow == 0)
                                {
                                    nColumn++;
                                }
                        }
                }
        }
    if (windowSelector.IsWindowVisible()) {
        windowSelector.BringWindowToTop();
    }
}

void AppMenuC::CascadePlots()
{
    CRect rect;
    VERIFY(::SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0));

    // John suggests the size to be a half of the screen
    //int nWidth = rect.Width() / 2;
    //int nHeight = rect.Height() / 2;

    // Radim means two thirds look better
    int nWidth = rect.Width() / 3 * 2;
    int nHeight = rect.Height() / 3 * 2;

    int nDeltaX = ::GetSystemMetrics(SM_CXSMICON) + ::GetSystemMetrics(SM_CXSIZEFRAME);
    int nDeltaY = ::GetSystemMetrics(SM_CYCAPTION);
    int nWindow = 0;
    for (int i = 0; i < MenuRootC::rootList.Size(); i++)
        {
            MenuRootC& root = MenuRootC::rootList.GetRef(i);
            for (int j = 0; j < root.pageLeafs.Size(); j++)
                {
                    MenuPageC& page = root.pageLeafs.GetRef(j);
                    if (page.IsPlotPage())
                        {
                            int nX0 = rect.left + nWindow * nDeltaX;
                            int nY0 = rect.top + nWindow * nDeltaY;

                            if (nX0 + nWidth > rect.Width() || nY0 + nHeight > rect.Height()) {
                                nX0 = 0;
                                nY0 = 0;
                                nWindow = 0;
                            } else {
                                nWindow++;
                            }

                            page.assocWindow->ResizeMainWindow(nX0, nY0, nWidth, nHeight);
                        }
                }
        }
    if (windowSelector.IsWindowVisible()) {
        windowSelector.BringWindowToTop();
    }

}

