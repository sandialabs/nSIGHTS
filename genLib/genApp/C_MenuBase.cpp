///////////////////////////////////////////////////////////////////////////////////
//
// C_MenuBase.cpp
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

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include <genClassPS/PS_FormContainer.h>
#include <genApp/C_MenuObj.h>

#include <genApp/C_MenuBase.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

MenuBaseC::MenuBaseC() :
    m_pFormContainer(NULL),
    assocObj(0)
{
}

MenuBaseC::~MenuBaseC()
{
    FormContainer* pForm = m_pFormContainer;
    if (pForm != NULL) {
        pForm->SetForm(NULL);
    }
}

bool MenuBaseC::AssocObjOK()
{
    return (assocObj != 0) && (assocObj->assocMenu == this);
}

void MenuBaseC::DoApply()
{
    if (AssocObjOK() && UIdataOK())
        assocObj->PropApply();
}

void MenuBaseC::ApplyCB (void* inObj)
{
    static_cast<MenuBaseC*>(inObj)->DoApply();
}

void MenuBaseC::DoCancel()
{
    if (AssocObjOK())
        assocObj->PropCancel();
    PropClearMsg();
}

void MenuBaseC::CancelCB (void* inObj)
{
    static_cast<MenuBaseC*>(inObj)->DoCancel();
}

void MenuBaseC::ClearCB (void* inObj)
{
    static_cast<MenuBaseC*>(inObj)->MenuClear();
}

void MenuBaseC::DefaultCB(void* inObj)
{
    static_cast<MenuBaseC*>(inObj)->MenuDefault();
}

void MenuBaseC::SetNameField()
{
    if (AssocObjOK())
        {
            char newName[60];
            nameField.GetFieldString(newName, 40);

            if (StringLength(newName) == 0)
                nameField.SetFieldString(assocObj->GetType());
            else
                {
                    assocObj->SetID(newName);
                    assocObj->UpdateTreeData();
                }
        }
}

void MenuBaseC::SetNameFieldCB(void* inObj)
{
    static_cast<MenuBaseC*>(inObj)->SetNameField();
}


void MenuBaseC::InitMenu()
{
    // hook it up to the frame
    FormContainer* pForm = FormContainer::GetFormContainer();
    pForm->SetForm(&mainForm);

    ArgListC  args;

    // top form
    args.Clear();
    args.StdFormAttach();
    args.BottomOffset(2);
    topRowOuterForm.InitWidget(mainForm.GetWidget(), args);

    // object ID
    args.StdSplitSetup(30);
    nameField.SetFieldLen(FuncObjC::objectIDLen);
    nameField.AttachObjectToFrame();
    nameField.InitFramedUI("Object ID", topRowOuterForm.GetWidget(), args);
    nameField.AddOtherObjectCallback(SetNameFieldCB, this);

    //  upper right form adjacent to name input
    args.StdSplitFinish(70);
    URform.InitWidget(topRowOuterForm.GetWidget(), args);

    // top separator
    args.Clear();
    args.TopBaseAttach(topRowOuterForm);
    args.TopOffset(-2);
    args.RightFormAttach();
    args.LeftFormAttach();
    tsep.InitWidget(mainForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRowOuterForm.GetWidget());
    args.BottomFormAttach();

    PPform.InitWidget(mainForm.GetWidget(), args);


    // buttons at the bottom
    pForm->SetLowerForm(&buttonForm, 34);

    args.Clear();
    args.TopFormAttach(2);
    //args.BottomFormAttach(4);
    args.AddArg(Arg::XmNwidth, 76);
    args.SetPlaceHolder();

    args.RightFormAttach(3);
    propApply.StdButtonInit("Apply", &buttonForm, args);
    propApply.AddOtherObjectCallback(ApplyCB, this);

    args.ResetPlace();
    args.RightOffset(8);
    args.SetPlaceHolder();
    args.RightBaseAttach(propApply);
    propCancel.StdButtonInit("Cancel", &buttonForm, args);
    propCancel.AddOtherObjectCallback(CancelCB, this);

    args.ResetPlace();
    args.RightOffset(8);
    args.RightBaseAttach(propCancel);
    propClear.StdButtonInit("Clear", &buttonForm, args);
    propClear.AddOtherObjectCallback(ClearCB, this);

    args.ResetPlace();
    args.RightOffset(8);
    args.RightBaseAttach(propClear);
    propDefault.StdButtonInit("Default", &buttonForm, args);
    propDefault.AddOtherObjectCallback(DefaultCB, this);

    m_pFormContainer = pForm;
}

void MenuBaseC::MenuOpen(bool   defaultButtonOK,
                         bool   clearButtonOK)
{
    FormContainer* pForm = m_pFormContainer;
    ASSERT(pForm);
    CWnd* currWnd = pForm->GetWindow();

    buttonForm.Create(currWnd, WS_DLGFRAME);
    mainForm.Create(currWnd);
    pForm->ResetForm();
    mainForm.ShowWindow(SW_SHOW);
    mainForm.GetParentWnd()->ShowWindow(SW_SHOW);

    propClear.SetSensitive(clearButtonOK);
    propDefault.SetSensitive(defaultButtonOK);

    if (currWnd)
        {
            currWnd->Invalidate();  // attempt to fix RRs bug with overlaying menus
            currWnd->UpdateWindow();
        }
}

void MenuBaseC::MenuClose()
{
    ::SetFocus(NULL); // necessary to ensure that object id field is updated on changing
    // menus if it has the focus
    MenuObjC::currMenuObj = 0;
    assocObj = 0;
    mainForm.Delete();
}


void MenuBaseC::MenuClear()
{
}
void MenuBaseC::MenuDefault()
{
    if (assocObj && assocObj->objFunction)
        assocObj->objFunction->objIDisDefault = true;
}

void MenuBaseC::SetMenuData(MenuObjC& inData)
{
    assocObj = &inData;
    MenuObjC::currMenuObj = &inData;
    nameField.SetFieldString(inData.GetID());
    mainForm.UpdateWindow();

    int helpID = inData.GetHelpID();
    mainForm.SetWindowContextHelpId(helpID);
}

bool MenuBaseC::UIdataOK()
{
    allOK = true;
    return true;
}

void MenuBaseC::GetMenuData(MenuObjC&  outData)
{
    //      we only want to update on explicit entry -- see SetNameFieldCB above
    //      char tempStr[80];
    //  nameField.GetFieldString(tempStr, 80);
    //      outData.SetID(tempStr);
}



void MenuBaseC::SetMainformScrolled(bool bSet, int nX, int nY)
{
    mainForm.m_bUseScrollSkills = bSet;
    mainForm.m_size = CSize(nX, nY);
}

void MenuBaseC::SetFocus()
{
    mainForm.SetFocus();
}

MenuBaseFormManager::MenuBaseFormManager() :
    m_size(600, 800)
{
}

CSize MenuBaseFormManager::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = LayoutManager::SetWindowPosX(0, m_size.cx);
    szForm.cy = LayoutManager::SetWindowPosY(0, m_size.cy);
    return szForm;
}

