///////////////////////////////////////////////////////////////////////////////////
//
// C_MenuPage.h
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

#ifndef C_MENUPAGE_H
#define C_MENUPAGE_H

#include <genClass/C_Common.h>
#include <genClass/C_FuncObj.h>
#include <genClass/U_Str.h>
#include <genClass/T_SC_BasicPtrArray.h>

#include <genApp/C_CfgFileObj.h>

#ifndef ADCONSOLEAPP
#include <genApp/C_TreeNode.h>
#include <genClassPS/PS_MainWindow.h>
#endif

class MenuObjC;                     // forward decl only need pointer type
class MenuPageC;
class AllocPageC;
#ifndef ADCONSOLEAPP
class AppMenuC;
class PullMenuC;
#endif

namespace appCopyPaste {
    extern void PastePages();
};

typedef T_SC_BasicPtrArray<MenuPageC> MenuPageArray;

#ifndef ADCONSOLEAPP
class MenuPageC: public CfgFileObj, public TreeNode {
    friend class TreeBase;
    friend class AppMenuC;
    friend class PlotFolderPageC;
    friend void appCopyPaste::PastePages();
#else
    class MenuPageC: public CfgFileObj {
#endif

        friend class MenuRootC;
        friend class AllocPageC;

    private:
        AllocPageC*         pageAllocator;

    public:
        char                pageID[FuncObjC::objectIDLen];

#ifndef ADCONSOLEAPP
        PS_MainWindow*      assocWindow;  // assoc top level window
#endif


        //      objects on page
        T_SC_BasicPtrArray<MenuObjC> pageObjects;   // ptr to start of list

        //      page properties
        bool                pageDeleteable;         // page can be deleted
        bool                pageCopyable;           // page can be copied

        bool                pageSelected;           // page is current page
        bool                pageExpanded;           // page is open on tree

        // for nested pages
        MenuPageC*          parentPage;             // default null is tree root
        MenuPageArray       childPages;             // only used by pages that can have children

        //      global access
        static MenuPageC*   currPageObj;            // current object

    public:
        //  functions
        MenuPageC(const char* pID);
        MenuPageC(const MenuPageC& a);
        virtual             ~MenuPageC();

#ifndef ADCONSOLEAPP
        CString             GetTitle()    const;
        void                SelectPage();
        void                UnselectPage();
        virtual void        UpdatePage();        // default changes page name to reflect first object ID ID

        //  general status stuff
        bool                IsSelected()    const {return pageSelected;}
        bool                IsExpanded()        const {return pageExpanded;}
        bool                IsDeleteable()      const {return pageDeleteable;}
        bool                IsCopyable()        const {return pageCopyable;}

        const AllocPageC*   GetPageAllocator() const {return pageAllocator;}
#endif

        // management of MenuObj on page
        void                AddObject(MenuObjC* obj);    //  adds newly constructed button
        void                DeleteObject(MenuObjC* obj); //  removes button from page list

        //  config file support
        virtual bool        OKforCommandLine() const;    // used for cmd line cfg file objects

        void                ReadDefaultObjectHeader();
        virtual void        WriteToFile();
        virtual bool        ReadFromFile();

#ifndef ADCONSOLEAPP
        //  status check/display after recalcs
        virtual void        UpdateTreeData();  // updates icon and text


        // connection listing support
        // base class version OK for data, plot, and list pages
        virtual void        GetPageFO(FuncObjArray& pageFO) const;

        // composite plot support
        virtual bool        IsPlotPage() const;  // used by composite

        // other typing support
        bool                IsSameType(MenuPageC* pageToCheck) const;
        bool                IsSameType(const char* pageTypeNameCheck) const;

        // deferred menu stuff
        const PullMenuC*    GetPageObjectMenu()  const;
        bool                ObjectOKForPage(const char*  objID) const;

        virtual void        ObjectPageChangeComplete(MenuPageC* oldParent) const;
        virtual bool        AcceptAsParent(MenuPageC* newParent) const;
        // returns true if newParent == NULL (root) default


        bool                IsParentOf(const MenuPageC* potentialChild) const;

#endif // ADCONSOLEAPP
        virtual const MenuPageC*  GetActualParent() const;
        // default returns this -- for plot folders which must defer to
        // parent type


    protected:
        void                ReadChildren();


    private:
        void                InitCommon();

#ifndef ADCONSOLEAPP
        // used by friend MenuRootC
        void                UpdatePageUI();
        void                ClearPageUI();

        //      used by friend TreeBase
        void                RemoveFromTree();  // removes self & children from tree
        void                AddToTree(HTREEITEM insertAfter);  // adds self & children to tree
        virtual bool        AcceptAsChild(MenuPageC& newChild) const;
        virtual void        ParentChanging(MenuPageC* newParent); // null if root


        bool                AcceptAsObject(const MenuObjC& newObject) const;

        virtual void        ResetChildren(); // for composite page plot order change

#endif // ADCONSOLEAPP


    };


#endif // C_MENUPAGE_H
