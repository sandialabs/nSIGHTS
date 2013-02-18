///////////////////////////////////////////////////////////////////////////////////
//
// C_AppMenu.h
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

#ifndef C_APPMENU_H
#define C_APPMENU_H

#include <genClass/C_FuncObj.h>
#include <genClassPS/PS_MainWindow.h>
#include <genApp/G_AppObj.h>

// forward refs
class SC_StringArray;
class MenuObjC;
class MenuPageC;


//      there should be one and only one of these ....
class AppMenuC {

private:
    // the one and only .....
    static  AppMenuC*                theAppMenu;


    //      makes sure CreateObjectMenus is only called once ...
    static bool                      objectMenusCreated;

    static appFuncObjGlobals::AppFuncArray systemObj;

protected:
    //      menu bar access for set sensitive / set object pulldowns
    static  UINT                    menuBarObjPos;
    static  UINT                    objectNewPos;
    static  UINT                    objectDupPos;
    static  UINT                    objectCopyPos;
    static  UINT                    objectCopyPagePos;
    static  UINT                    objectPastePos;
    static  UINT                    objectDelPos;
    static  UINT                    objectApplyPos;

    static  UINT                    menuBarPagePos;
    static  UINT                    pageDupPos;
    static  UINT                    pageDelPos;
    static  UINT                    pageCopyCurrentPos;
    static  UINT                    pageCopyAllPos;
    static  UINT                    pagePastePos;
    static  UINT                    pageBringToTopPos;
    static  UINT                    windowListStartPos;
    static  UINT                    mainMenuBarID;

    static  const char*             templateFileExtension;
    static SC_StringArray           templateFileNames;


public:

    static  void                    InitMainMenuBar(UINT mainMenuBarID);
    static  UINT                    GetMainMenuBarID();

    //      base class version should be called before globals are used in derived version
    virtual void                    InitAppData(int init_obj_ID_VALUE);

    //      page operations -
    static  MenuPageC*              CreateNewMenuPage(const char* pageTypeID);


    //      object menu setup
    static  void                    CreateObjectMenu();
    static  void                    ResetObjectMenus();  // sets obj menu after page change

    // page and object changes
    static  void                    SetMenuBarSensitive();
    static  CMenu*                  GetPageMenu();
    static  CMenu*                  GetObjectMenu();
    static  CMenu*                  GetPageMenu(CMenu& mainMenu);
    static  CMenu*                  GetObjectMenu(CMenu& mainMenu);

    static  void                    InitWindowPopupMenu(CMenu* windowMenu, int nStartID);
    static  void                    InitTemplatePopupMenu(CMenu* windowMenu, int nStartID);
    static  void                    BringWindowToTop(int nID);

    //      basic menu bar support
    static  void    NewObject(const char* objID);
    static  void    DeleteObject();
    static  void    CopyObject();
    static  void    CopyPageObjects();
    static  void    PasteObject();
    static  void    ApplyObject();
    static  void    DuplicateObject();
    static  void    NewPlotFolder();

    static  void    NewDataPage();
    static  void    NewListPage();
    static  void    NewXYPage();
    static  void    NewXYZPage();
    static  void    NewCompositePage();
    static  void    DuplicatePage();
    static  void    DeletePage();
    static  void    CopyCurrentPage();
    static  void    CopyAllPages();
    static  void    PastePage();
    static  void    ClearAllPages();
    static  void    CollapseAllPages();

    static  void    BringPageToTop();

    //      FuncObj selection support
    //      moved from FuncObjRefComboBox in U_Menu for more generic usage
    static  bool    GetFuncObjList(const type_info&   listType,
                                   SC_StringArray&    listObjDesc,
                                   FuncObjRefArray&   listObjects,
                                   const char*        sepStr, // between each column
                                   FuncObjC*          extraObj = 0);

    // WindowSelector support
    static  void    InitWindowSelector();
    static  void    SwitchWindowSelector();
    static  bool    IsWindowSelectorEnabled();
    static  void    RefreshWindowSelector(PS_MainWindow* pcurrWnd, //0 is main frame
                                          bool bSaveCurrent);


    static  void    IndicatePropMenuOpen();
    static  void    SetPlotFont(bool bSmall);

    static  bool    IsAnyAppWindowOpen();
    static  void    ShowAppWindows(int nCmdShow);

    static  void    TilePlots(bool bVertically = false);
    static  void    CascadePlots();

    // for template directory
    static void     SetTemplateFileExtension(const char* fExt) {templateFileExtension = fExt;}

    static void     GetSelectedTemplateFileName(int     nsel,
                                                char*   selFName,
                                                int     fNameLen);

    // Support for context calls from the tree
    static  void    ResetObjectPopupMenu(CMenu* popMenu);
    static  void    ResetPagePopupMenu(CMenu* popMenu);

    // for paste support
    static AppMenuC&        GetApp() {return *theAppMenu;}
    virtual void            PrePasteSetup();           // for sampvar updating on read
    virtual void            PostPasteFixup();          // default does nothing

protected:

    virtual void    DoObjectMenuCreate() = 0;


    static void     NewPage(const char* pageTypeID,  bool  doApply);

    static HMENU   GetCurrentCascade();
    static void    SetDropdownMenu(CMenu* objDropDown, UINT objectNewPos, HMENU currCascade);

    static void    AddNewPage(MenuPageC* newPage);

private:
    static void    AddNewObject(MenuObjC* newObj);

    static void    MakeRefStr(char                  outStr[],  // assumed at 80 char min
                              int                   maxLen,
                              const char*           rootID,
                              const MenuPageC*      currPage,
                              const char*           systemCategoryID, // if page == 0
                              const char*           FOID,
                              const char*           sepStr,
                              DataObjC*             dataObj);

    static void    BringPageToTop(MenuPageC*        page);
    static void    BringPlotToTop(MenuPageC*        plotPage);
    static void    BringListToTop(MenuPageC*        plotPage);

    static int     GetPageFOCount(const type_info&  listType,
                                  const MenuPageC&  currPage,
                                  FuncObjC*         extraObj,
                                  int&              pageCount);

    static void    AddPageFO(const type_info&       listType,
                             const MenuPageC&       currPage,
                             FuncObjC*              extraObj,
                             const char*            rootID,
                             const char*            sepStr,
                             SC_StringArray&        listObjDesc,
                             FuncObjRefArray&       listObjects);

};

#endif // C_APPMENU_H

