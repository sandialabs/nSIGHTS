///////////////////////////////////////////////////////////////////////////////////
//
// C_MenuRoot.h
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

#ifndef C_MENUROOT_H
#define C_MENUROOT_H

#include <genClass/T_SC_BasicPtrArray.h>

#include <genApp/C_CfgFileObj.h>
#include <genApp/C_MenuPage.h>

class MenuObjC;

#ifndef ADCONSOLEAPP
class TreeBase;
#endif

class MenuRootC: public CfgFileObj {
public:
    //  list maintenance
    static T_SC_BasicPtrArray<MenuRootC>        rootList;       // start of list
    MenuPageArray           pageLeafs;      // ptr to start of list page on tree

    // klugy approach but consistent with MenuObj and MenuPage
    static  MenuRootC*      currTreeObj;

    const char*             rootID;          // for debugging
    const char*             shortID;         // for plots/window selector on multiple root apps
    const char*             infoBarDesc;     // for info bar description on multiple root apps

#ifndef ADCONSOLEAPP
private:
    TreeBase*               treeUI;         // hidden UI object
#endif
public:
    //  functions
    MenuRootC(const char* rID, const char* shID = 0, const char* ibDesc = 0);
    ~MenuRootC();   // in theory all MenuRoots are statically allocated
    // and this only gets called at exit so we really
    // don't care about cleanup too much. This will change
    // if we have dyanamically allocated MenuRootC

    void                    Destroy();
    static void             DestroyAll();
    void                    ClearAll();  // deletes all objects and pages -- for file new
#ifndef ADCONSOLEAPP

    void                    SetupUI(CWnd* pParentWnd);  // intializes MFC tree -- until this is done Add/Del does not affect UI
    // allows for file loading
    TreeBase*               GetUI() {return treeUI; }   // hidden UI object
    void                    CloseUI();                  // closes tree
    void                    UpdateUI();

    static void             CloseAllUI();               // all trees
    static  void            UpdateAllUI();
    static void             UpdateCurrentTree();        // updates current tree icons

    void                    SelectRoot();               // resets current menu page & object
    void                    UnSelectRoot();
    void                    SelectCurrentPage();        // selects current menu page & object

    bool                    PageIsSelected();   // returns true if selected tree item is page
    bool                    ObjectIsSelected(); // returns true if selected tree item is object

    static int              GetNTree();

    //      management

    virtual void            SetDefault(); // base class does clear all

#endif
    void                    AddPageLeafToRoot(MenuPageC*    pageToAdd);
    void                    DelPageLeafFromRoot(MenuPageC*  pageToDel);

    void                    AddObjectLeafToRoot(MenuPageC*  pageBeingAddedTo,
                                                MenuObjC*   objectToAdd);


    //  FileObj stuff
    void                    ReadDefaultObjectHeader();
    void                    WriteToFile(bool cmdLineOnly = false);
    bool                    ReadFromFile();

    // misc support
    MenuPageC*              FindPage(const char*      pageToFind,
                                     const MenuPageC* onPage = 0) const;
    bool                    PageExists(const char* pageID,
                                       const MenuPageC* onPage = 0) const;
    void                    DeletePage(const char* pageID,
                                       MenuPageC* onPage = 0);

protected:
#ifndef ADCONSOLEAPP
    void                    CreateDefaultPage(const char* defPageID);

    void                    ClearOldDefaults(); // clears defaults from previous versions
    void                    SetDefaultDataPage(); // common implementation of SetDefault

    static void             SetTreeUI();
    void                    ResetTreeUI();

private:

    void                    DoSelectCurrentPage(MenuPageC& currPage);
    MenuPageC*              GetPageWithSelectedObject(MenuPageC& currPage);
#endif
private:
    void                    InitCommon();

    //      no copying these ...
    MenuRootC(const MenuRootC& a) {};
    MenuRootC&  operator= (const MenuRootC& a) {};


};

#endif // C_MENUROOT_H

