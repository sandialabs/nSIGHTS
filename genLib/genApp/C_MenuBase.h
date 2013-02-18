///////////////////////////////////////////////////////////////////////////////////
//
// C_MenuBase.h
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
//              Base class for all functional object user-interfaces
//              (DPO_XXXPS, LPO_XXXPS, PPO_XXXPS, UPO_XXXPS). Handles menu creation,
//              UI tasks common to all FuncObjC derived objects (object ID, object buttons),
//              and object calculation initiation. Defines virtual functions for button handling.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_MENUBASE_H
#define C_MENUBASE_H

#include <genClassPS/PS_UIBase.h>
#include <genApp/U_Menu.h>

class MenuObjC;
class FormContainer;

class MenuBaseFormManager : public FormManager {
    friend class MenuBaseC;
public:
    MenuBaseFormManager();

protected:
    virtual CSize SetCalculateFormSize();

protected:
    CSize m_size;
};

class MenuBaseC {
private:
    MenuBaseFormManager     mainForm;
    FormWidget              topRowOuterForm;    // top form - object ID and UR form
    TextEntryUIC            nameField;          // "Object ID"

    FormManager             buttonForm;         // contains buttons on the bottom
    HorizontalSeparator     tsep;

    MenuObjC*               assocObj;
protected:
    FormWidget              URform;
    FormWidget              PPform;

    PushButton              propCancel;
    PushButton              propApply;
    PushButton              propClear;
    PushButton              propDefault;

    FormContainer*          m_pFormContainer;

    bool                    allOK;     // used by all  CheckUIData()

public:
    MenuBaseC();
    virtual             ~MenuBaseC();

    //  these are not virtual due to differences in signature in derived classes
    void                InitMenu();

    void                MenuOpen(bool   defaultButtonOK,
                                 bool   clearButtonOK);

    void                SetMenuData(MenuObjC&  inData);  // updates UI with current object settings
    void                GetMenuData(MenuObjC&  outData); // updates object with current UI settings
    // requires that CheckUIData be called first
    // which occurs in the ApplyCB processing
    //  these are virtual -- all the same signature
    virtual bool        UIdataOK();  // returns true if all UI settings are OK

    virtual void        MenuClose();

    // virtuals for Clear and Apply buttons
    virtual void        MenuClear();                    // does nothing
    virtual void        MenuDefault();                  // sets object ID stat back to default

    void                SetFocus();

protected:
    void SetMainformScrolled(bool bSet, int nX, int nY);

private:
    bool                AssocObjOK();

    void                DoApply();
    static  void        ApplyCB(void* inObj);
    void                DoCancel();
    static  void        CancelCB(void* inObj);
    static  void        ClearCB(void* inObj);
    static  void        DefaultCB(void* inObj);

    void                SetNameField();
    static  void        SetNameFieldCB(void* inObj);

};

#endif // C_MENUBASE_H

