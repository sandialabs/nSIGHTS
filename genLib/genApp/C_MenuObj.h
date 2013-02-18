///////////////////////////////////////////////////////////////////////////////////
//
// C_MenuObj.h
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
//              Base class for all functional objects with associated UI menus
//              (DPO_XXX, LPO_XXX, PPO_XXX, UPO_XXX). Derived from TreeNode and CfgFileObj.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_MENUOBJ_H
#define C_MENUOBJ_H

#include <genClass/C_FuncObj.h>

#include <genApp/C_CfgFileObj.h>

#ifndef ADCONSOLEAPP
#include <genApp/C_TreeNode.h>
#endif
class MenuPageC;
class AllocObjC;

#ifndef ADCONSOLEAPP
class MenuBaseC;
class ExposedObjC;

class MenuObjC : public CfgFileObj, public TreeNode {
    friend class TreeBase;
#else

    class MenuObjC : public CfgFileObj {
#endif

    private:
        // because MS typeid doesn't work like Unix ...
        friend class    AllocObjC;

#ifndef ADCONSOLEAPP
        friend class    MenuBaseC;
#endif

        const char*           allocName;

        static const char*    defaultAllocName;

    public:

        // qa status types
        enum    QAStatusType {qas_OK,   // object validated
                              qas_TBV,  // object to be validated in next release
                              qas_NO};  // object is non Q

    protected:
        //  ID and type name used on objects for menu objects which
        //  objectID is expected to be tied to a string of FuncObjC::objectIDLen

        char*               objectID;
        const char*         objectType;         //  used in object
        const char*         objectLongDesc;     //  used in property menu title

        // QA status affects object display
        QAStatusType        qaStatus;               // default is qas_OK

        bool                objectCanChangePages;   // for drag support

    public:
        //  status on pane
        bool                objDeleteable;  // must be set for objects that can't be deleted
        // default is true
        bool                objCopyable;    // must be set false for objects that can't be copied
        // default is true
        bool                objSelected;    // is selected object on attached page

        //  most MenuObj wrap a FuncObj
        FuncObjC*           objFunction;    //  initially set to 0,

        //  usually set in derived object constructor
        //  statics
        static MenuObjC*    currMenuObj;    //  current object

    protected:
#ifndef ADCONSOLEAPP
        MenuBaseC*          assocMenu;        // the UI object for this object
#endif

    private:
        T_SC_Array<char*>   registeredFiles;

    public:

        //  constructors and destructors
        MenuObjC(FuncObjC* fo,          // used by objects containing funcObj
                 const char* longDesc);
        MenuObjC(char* bIDref,          // used by other objects
                 const char* bTypeRef,
                 const char* longDesc,
                 const char* bIDval);

        virtual             ~MenuObjC();

        //  member access
        const char*         GetID()                 const;
        const char*         GetType()               const;
        const char*         GetAllocName()      const {return allocName;}
        const char*         GetLongDesc()       const {return objectLongDesc;};

        void                SetID(const char* newID); // sets ID
        void                SetLongDesc(const char* newDesc) {objectLongDesc = newDesc;}
        void                SetAllocName(const char* newAllocName) {allocName = newAllocName;}

#ifndef ADCONSOLEAPP
        void                SetPageID(); // sets current page & all other non-del objects on page to current

        //  support for property page methods and actions -- all objects must have
        //  open, close, apply, cancel and update

        // base class must be called
        virtual void        PropOpenMenu() = 0;       // called to initialize menu
        virtual void        PropCloseMenu();          // called to close menu - usually the base is OK


        virtual bool        ContainsExposedObject() {return false;}
        void                PropApplyAssoc(FuncObjC* assocFunction);

        //  status check/display after recalcs
        virtual void        UpdateTreeData();  // updates icon and text


        bool                IsSelected()    const {return objSelected;}
        bool                IsDeleteable()  const {return objDeleteable;}
        bool                IsCopyable()    const {return objCopyable;}
#endif

        // registering files for UpdatePaths
        void                RegisterFileName(char* inFName);

        //  QA status control
        void                SetTBV(bool isTBV);

        //  file IO stuff
        //  major and minor version are read by ReadFromFile into
        //  object specific version numbers currentObjRead...
        //  also writes MenuObjC class data
        void                WriteObjectHeader(int majVer, int minVer) const;
        //  reads version numbers and FO pointer data
        void                ReadObjectHeader();
        //  skips typeid, calls above
        void                ReadDefaultObjectHeader();


#ifndef ADCONSOLEAPP
        void                SetFocus();

        virtual int         GetHelpID() const;

        void                UpdateUIandExecute();  // F12 key processing
        // if menu set up, check uiData and apply

        void                ExecuteObj();   // PropApply -- different name for clarity of use

        static void         UpdateCurrentObj();  // calls PropApply for current menu obj
        static void         CloseOpenMenu();     // calls PropCloseMenu for current menu obj

        //  called by friendTreeBase && MenuPage
        bool                CanDragToDifferentPage() {return objectCanChangePages;}
        virtual void        ChangePage(MenuPageC& currParent,
                                       MenuPageC& newParent);

    protected:

        //  called only by MenuBaseC button handlers
        virtual void        PropUpdate()    = 0;  // called to update menu temporaries with current data
        // base class does nothing
        virtual void        PropApply()     = 0;  // called when MenuBaseC apply button is pressed
        virtual void        PropCancel();         // called when MenuBaseC Cancel button is pressed
        // base class just calls PropUpdate()

        bool                UpdateOK() {return currMenuObj == this;}
        bool                MenuUpdateOK() {return UpdateOK() && objFunction->StatusOK();}

#endif
    protected:
        // sets allocID and copy delete flags for auto created objects
        void               AutoObjectSetup(const char* inID);


        //  misc private stuff
    private:
        void                LocalCopy(const MenuObjC& a);
        void                InitCommon();


    };

    typedef T_SC_BasicPtrArray<MenuObjC> MenuObjArray;

#endif // C_MENUOBJ_H

