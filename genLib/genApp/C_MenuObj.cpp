///////////////////////////////////////////////////////////////////////////////////
//
// C_MenuObj.cpp
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
//              Base class for all functional objects with associated UI menus
//              (DPO_XXX, LPO_XXX, PPO_XXX, UPO_XXX). Derived from TreeNode and CfgFileObj.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#ifndef ADCONSOLEAPP
#include <stdlib.h>
#include <genPlotClass/C_PlotDef.h>
#include <genListClass/C_ListDef.h>
#endif

#include <genClass/C_DataObj.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#ifndef ADCONSOLEAPP
#include <genClassPS/PS_Color.h>
#include <genApp/C_ObjHelp.h>
#include <genApp/C_TreeBase.h>
#endif

#include <genApp/AppConfigFile.h>
#include <genApp/G_FileNames.h>

#ifndef ADCONSOLEAPP
#include <genApp/U_Menu.h>
#include <genApp/ProjectUtil.h>
#include <genApp/C_MenuBase.h>
#endif

#include "C_AllocObj.h"
#include "C_MenuPage.h"
#include "C_MenuRoot.h"

#include "C_MenuObj.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//  statics
MenuObjC*   MenuObjC::currMenuObj   = 0; //  current object
const char*     MenuObjC::defaultAllocName = "MenuObjC";

//  ******************************************************** constructor/destructor

MenuObjC :: MenuObjC(FuncObjC* fo,
                     const char* longDesc) :
    objFunction(fo),
    objectLongDesc(longDesc),
#ifndef ADCONSOLEAPP
    assocMenu(0),
#endif
    allocName(defaultAllocName)
{
    objectID                = 0;
    objectType              = 0;
    InitCommon();
}

MenuObjC :: MenuObjC(char* bIDref, const char* bTypeRef, const char* longDesc, const char* bIDval) :
    objectID(bIDref),
    objectType(bTypeRef),
    objectLongDesc(longDesc),
    objFunction(0),
#ifndef ADCONSOLEAPP
    assocMenu(0),
#endif
    allocName(defaultAllocName)
{
    CopyString(objectID, bIDval, FuncObjC::objectIDLen);
    InitCommon();
}

void MenuObjC::InitCommon()
{
    objectCanChangePages = true;

    objSelected   = false;
    objDeleteable = true;
    objCopyable   = true;
    qaStatus      = qas_OK;
#ifndef ADCONSOLEAPP
    iconIndex = 4;
    projectUtilities::SetModifiedFlag(true);
#endif
}


MenuObjC :: ~MenuObjC ()
{
    objFunction = 0;

#ifndef ADCONSOLEAPP
    if (currMenuObj == this)
        PropCloseMenu();

    projectUtilities::SetModifiedFlag(true);
    //      delete tree representation
    DeleteNode();
#endif

    // deregister files
    for (int i = 0; i < registeredFiles.Size(); i++)
        appFileNameData.DeleteFrom(registeredFiles[i]);

}

#ifndef ADCONSOLEAPP

void MenuObjC :: SetID(const char* newID)
{
    if (objFunction)
        objFunction->SetID(newID);
    else
        CopyString(objectID, newID, FuncObjC::objectIDLen);
}

void MenuObjC::SetPageID()
{
    if (MenuPageC::currPageObj)
        {
            // update ID of other pages
            T_SC_BasicPtrArray<MenuObjC>& pageObj = MenuPageC::currPageObj->pageObjects;

            for (int i = 0; i < pageObj.Size(); i++)
                {
                    if (pageObj[i] != this)
                        {
                            MenuObjC& currObj = *(pageObj[i]);
                            if (!currObj.IsDeleteable())
                                currObj.SetID(GetID());
                        }
                }

            CopyString(MenuPageC::currPageObj->pageID, GetID(), FuncObjC::objectIDLen);
            MenuPageC::currPageObj->UpdatePage();
        }
}



const char*  MenuObjC::GetID() const
{
    if (objFunction)
        return objFunction->GetID();

    return objectID;
}

const char* MenuObjC::GetType() const
{
    if (objFunction)
        return objFunction->GetType();
    return objectType;
}


int     MenuObjC::GetHelpID() const
{
    return ObjHelpC::GetHelpIndex(allocName);
}


//  ******************************************************** default Prop Menu methods

void MenuObjC::PropOpenMenu()
{
    CloseOpenMenu();
    ASSERT(assocMenu == 0);

    currMenuObj = this;
    objSelected = true;
    UpdateTreeData();

    AppMenuC::SetMenuBarSensitive();
    AppMenuC::IndicatePropMenuOpen();
}

void MenuObjC::PropCloseMenu()
{
    if (assocMenu != 0)
        {
            assocMenu->MenuClose();
            delete assocMenu;
            assocMenu = 0;
        }

    currMenuObj = 0;
    GenAppClearMsgs();
    objSelected = false;
    UpdateTreeData();
}


void MenuObjC::PropCancel()
{
    PropUpdate();
}


void MenuObjC::UpdateTreeData()
{
    iconMaskIndex = 0;

    if ((currMenuObj != 0) && objSelected && (this != currMenuObj))
        objSelected = false;

    if (objSelected)
        {
            iconMaskIndex = imtSelected;
        }

    if (objFunction && (!objSelected))
        {
            switch (objFunction->GetStatus())
                {
                case os_Bad     :  {iconMaskIndex = imtBad; break; }
                case os_NotSet  :  {iconMaskIndex = imtNotSet; break;}
                case os_NoData  :  {iconMaskIndex = imtNoData; break;}
                }

            if ((iconMaskIndex == 0) && TreeBase::m_bShowConnectedObjects && currMenuObj && (currMenuObj->objFunction != 0))
                {
                    if (objFunction->UsesAsInput(currMenuObj->objFunction))
                        iconMaskIndex = imtUsesCurrent;
                    else if (currMenuObj->objFunction->UsesAsInput(objFunction))
                        iconMaskIndex = imtUsedByCurrent;
                }

            if ((iconMaskIndex == 0) && TreeBase::m_bShowNoOutput)
                {
                    if (!objFunction->IsInUse())
                        {
                            iconMaskIndex = imtNotInUse;
                        }
                }
        }

    if ((iconMaskIndex == 0) && (ContainsExposedObject()))
        {
            iconMaskIndex = imtExposed;
        }


    if (iconMaskIndex == 0)
        {
            switch (qaStatus)
                {
                case qas_OK  : {break; }
                case qas_TBV : {iconMaskIndex = imtQA_None; break; }
                case qas_NO  : {iconMaskIndex = imtQA_TBV; break; }
                }
        }

    typeText = GetType();
    idText = GetID();
    TreeNode::UpdateTreeData();
}


//  ******************************************************** object stuff

void MenuObjC::UpdateCurrentObj()
{
    if (currMenuObj != 0)
        currMenuObj->PropUpdate();
}


void MenuObjC::CloseOpenMenu()
{
    if (currMenuObj != 0)
        currMenuObj->PropCloseMenu();
}

//  recalculation stuff
void MenuObjC::UpdateUIandExecute()
{
    bool doApply = true;
    if (assocMenu)
        doApply = assocMenu->UIdataOK();
    if (doApply)
        PropApply();
}

void MenuObjC::ExecuteObj()
{
    PropApply();
}

void  MenuObjC::PropApply()
{
    if (objFunction)
        PropApplyAssoc(objFunction);
    else
        UpdateTreeData();

    PropUpdate();
    projectUtilities::SetModifiedFlag(true);
}

void MenuObjC::PropApplyAssoc(FuncObjC* assocFunction)
{
    GenAppClearMsgs();

    {
        CWaitCursor w;
        assocFunction->Execute();
        assocFunction->CalcDownStreamObjects();
        PlotDefC::ReplotAfterCalc();
    }

    if (assocFunction->StatusNotOK())
        {
            PropErrorMsg(assocFunction->GetStatusMsg());
        }

    MenuRootC::UpdateCurrentTree();
}


#endif

void MenuObjC::AutoObjectSetup(const char* inID)
{
    allocName = inID;
    objDeleteable = false;
    objCopyable = false;
}

void MenuObjC::SetTBV(bool isTBV)
{
    if (isTBV)
        qaStatus = qas_TBV;
    else
        qaStatus = qas_OK;
}


// ******************************************  File name changing

void MenuObjC::RegisterFileName(char* inFName)
{
    if (registeredFiles.IsEmpty())
        registeredFiles.SetResizable(4);
    registeredFiles += inFName;
    appFileNameData.AddTo(inFName, objFunction);
}
// ******************************************  File I/O


void  MenuObjC::ReadObjectHeader()
{
    currentObjReadMajorVersion = ReadInt();
    currentObjReadMinorVersion = ReadInt();
    if (objFunction)
        {
            ReadAndMatchFuncObj(objFunction);
            ReadText(objFunction->objID, FuncObjC::objectIDLen);
            objFunction->objIDisDefault = ReadBool();
        }
    else
        {
            ReadText(objectID, FuncObjC::objectIDLen);
        }

    objDeleteable       = ReadBool();
    objCopyable             = ReadBool();

    objSelected = ReadBool();

    CheckForInputFailure("Reading MenuObjC Properties");
}

void  MenuObjC::ReadDefaultObjectHeader()
{
    char tempStr[40];
    ReadText(tempStr, 40);
    ReadObjectHeader();
}

void MenuObjC:: WriteObjectHeader(int majVer, int minVer) const
{
    WriteText(allocName);

    WriteInt(majVer);
    WriteInt(minVer);
    WriteLine();

    //  address for object matching
    if ( objFunction)
        {
            WriteFuncObj(objFunction);
            WriteText(objFunction->GetID());
            WriteBool(objFunction->objIDisDefault);
            WriteLine();
        }
    else
        {
            WriteText(objectID);
        }

    WriteBool(objDeleteable);
    WriteBool(objCopyable);
    WriteBool(objSelected);
    WriteLine();

    // reset real format
    SetRealConvFormat(SC_RealFormat());
}

#ifndef ADCONSOLEAPP

void MenuObjC::SetFocus()
{
    if (assocMenu != NULL) {
        assocMenu->SetFocus();
    }
}

void MenuObjC::ChangePage(MenuPageC& currParent,
                          MenuPageC& newParent)
{

}

#endif // ADCONSOLEAPP

