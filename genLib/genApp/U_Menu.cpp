///////////////////////////////////////////////////////////////////////////////////
//
// U_Menu.cpp
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
//              Basic UI support classes used by MenuBaseC derived functional objects.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"


#include "U_Menu.h"
#include "C_MenuObj.h"
#include "C_MenuPage.h"
#include "C_MenuRoot.h"
#include "C_AppMenu.h"

#include <genClass/C_DataObj.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_File.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace genAppMenuUtilities {

    // ************************************************************* msg line stuff

    void PropErrorMsg(const char* errMsg)
    {
        GenAppErrorMsg(MenuObjC::currMenuObj->GetType(), errMsg);
    }

    void PropNormMsg(const char* normMsg)
    {
        GenAppInfoMsg(MenuObjC::currMenuObj->GetType(), normMsg);
    }
    void PropClearMsg()
    {
        GenAppClearMsgs();
    }

    void PropInputFieldError(const char* fieldName,
                             const char* errMsg)
    {
        char tempStr[255];
        CopyString(tempStr, fieldName, 255);
        ConcatString(tempStr, " " , 255);
        ConcatString(tempStr, errMsg, 255);
        PropErrorMsg(tempStr);
    }

    bool FileOverwriteOK(const char* fName)
    {
        if (FileExists(fName))
            {
                char msgStr[200];
                MakeString(msgStr, "File ", fName, " already exists. \n Overwrite it ?", 200);
                if (AfxMessageBox(msgStr, MB_YESNO) == IDNO)
                    return false;
            }

        return true;

    }
    // ************************************************************* plane selector

    void PlaneRadioBoxChooserWidget::PreInitSetUp(bool isHoriz, bool useLongDesc)
    {
        static const char* planeLabels[]  = {"XY (Horizontal)", "XZ (Vertical X)", "YZ (Vertical Y)"};
        static const char* shortPlaneLabels[]  = {"  XY  ", "  XZ  ", "  YZ  "};

        if (useLongDesc)
            RadioBoxChooserWidget::PreInitSetUp(planeLabels, 3, isHoriz);
        else
            RadioBoxChooserWidget::PreInitSetUp(shortPlaneLabels, 3, isHoriz);

    }
    // ************************************************************* main func obj C

    void FuncObjRefComboBox::UpdateFOCB(void* clientData)
    {
        static_cast<FuncObjRefComboBox*>(clientData)->UpdateFO();
    }


    /*
      was a problem here because callback added after create
      callback in menu would get called first before setup complete
    */


    FuncObjRefComboBox::FuncObjRefComboBox (const type_info& newType) :
        listType(newType)
    {
        listWidth = 30;

        // make sure its added before any others
        AddOtherObjectCallback(UpdateFOCB, this);
    }

    void FuncObjRefComboBox::ResetListData(FuncObjC*        extraObj)
    {
        SC_StringArray listLabels;
        listDataOK = AppMenuC::GetFuncObjList(listType, listLabels, listData, "\t", extraObj);
        SetListText(listLabels);
        SetSensitive(listDataOK);
    }

    void FuncObjRefComboBox::DefaultFuncObjRef(FuncObjRef& inFO)
    {
        if (inFO.objRef == 0)
            inFO = listData[0];
    }

    void FuncObjRefComboBox::SetFuncObjRef(const FuncObjRef& inFO)
    {
        for (int i = 0; i < listData.Size(); i++)
            if (listData[i] == inFO)
                {
                    currFO = inFO;
                    SetSelection(i);
                    return;
                }

        //  clear
        currFO = FuncObjRef();
        SetSelection(0);
    }

    void FuncObjRefComboBox::SetToDefault()
    {
        FuncObjRef tmpFO;
        tmpFO.SetDefault(listType);
        SetFuncObjRef(tmpFO);
    }

    void FuncObjRefComboBox::StdMenuInit(FuncObjRef&  inFO,
                                         FuncObjC*    extraObj)
    {
        ResetListData(extraObj);
        DefaultFuncObjRef(inFO);
        SetFuncObjRef(inFO);
    }


    bool FuncObjRefComboBox::FuncObjRefOK(bool&      prevWasOK,
                                          const char fieldDesc[])
    {
        if (!prevWasOK)
            return false;

        if (listData.IsEmpty())
            {
                PropInputFieldError(fieldDesc, "no data available");
                prevWasOK = false;
                return false;
            }

        if (currFO.objRef == 0)
            {
                PropInputFieldError(fieldDesc, "not set (? ?)");
                prevWasOK = false;
                return false;
            }
        return true;
    }

    bool  FuncObjRefComboBox::CheckDataObj()
    {
        if ( currFO.objRef == 0)
            return false;
        return currFO.GetData(listType)->DataOK();
    }

    void FuncObjRefComboBox::UpdateFO()
    {
        currFO = listData[currSelection];
    }


    FuncObjRefUIC::FuncObjRefUIC(const type_info& inType) :
        FuncObjRefComboBox(inType), ComboBaseUIC(this)
    {
    }

    void  FuncObjRefUIC::InitFramedUI(char              frameTitle[],
                                      Widget            parent,
                                      const  ArgListC&  inArgs)
    {
        listWidth = 0;
        ComboBaseUIC::InitFramedUI(frameTitle, parent, inArgs);
    }


    // ************************************************************* master slave

    MasterSlaveUIC::MasterSlaveUIC(const type_info& inType,
                                   bool       expOK) : masterObjectList(inType), exposeOK(expOK)
    {
        isMaster.currValue = true;
        isExposed.currValue = false;
        horizontal = true;
        compressed = false;
    }

    void MasterSlaveUIC::SetInternalSensitiveCB(void* inObj)
    {
        static_cast<MasterSlaveUIC*>(inObj)->SetInternalSensitive();
    }



    void MasterSlaveUIC::InitFramedUI(char             frameTitle[],
                                      Widget           parent,
                                      const ArgListC&  inArgs,
                                      int              expXpos)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);
        InitUI(expXpos);
    }

    void MasterSlaveUIC::InitFormUI(Widget           parent,
                                    const ArgListC&  inArgs,
                                    int              expXpos)
    {
        BaseUIC::InitFormUI(parent, inArgs);
        InitUI(expXpos);
    }

    void MasterSlaveUIC::InitUI(int expXpos)
    {
        ArgListC args;
        if (horizontal)
            {
                args.StdFormAttach();
                if (compressed)
                    horizSplit.PreInitSetUp(0, 30);
                else
                    horizSplit.PreInitSetUp(0, 40);

                horizSplit.InitFormUI(GetForm(), args);

                args.StdFormAttach();
                if (compressed)
                    {
                        masterObjectList.InitWidget(horizSplit.GetRight(), args);
                    }
                else
                    {
                        args.LeftFormAttach(5);
                        masterObjectList.AttachObjectToLabel();
                        masterObjectList.InitLabeledUI("Slave to:", horizSplit.GetRight(), args);
                    }
            }
        else
            {
                args.StdFormAttach();
                toggleForm.InitWidget(GetForm(), args);

                args.StdTopObjectAttach(toggleForm.GetWidget());
                masterObjectList.InitWidget(GetForm(), args);
            }

        args.StdToggleSetup(5);
        FormWidget* toggleParent = GetToggleForm();
        if (compressed)
            isMaster.StdToggleInit("Mst", toggleParent, args);
        else
            isMaster.StdToggleInit("Master", toggleParent, args);
        isMaster.AddOtherObjectCallback(SetInternalSensitiveCB, this);

        if (exposeOK)
            {
                args.StdToggleSetup(expXpos);
                if (compressed)
                    isExposed.StdToggleInit("Exp", toggleParent, args);
                else
                    isExposed.StdToggleInit("Expose", toggleParent, args);
            }
    }

    FormWidget* MasterSlaveUIC::GetToggleForm()
    {
        if (horizontal)
            return horizSplit.GetLeft();

        return &toggleForm;
    }

    void  MasterSlaveUIC::SetInternalSensitive()
    {
        masterObjectList.SetSensitive(!isMaster.GetValue());
        if (exposeOK)
            {
                isExposed.SetSensitive(isMaster.GetValue() || isExposed.GetValue());
                isMaster.SetSensitive(!isExposed.GetValue());
            }
    }

    void MasterSlaveUIC::SetSensitive(bool isSensitive)
    {
        BaseUIC::SetSensitive(isSensitive);
        if (isSensitive)
            {
                isMaster.SetSensitive(true);
                SetInternalSensitive();
            }
        else
            {
                isMaster.SetSensitive(false);
                if (exposeOK)
                    isExposed.SetSensitive(false);
                masterObjectList.SetSensitive(false);
            }
    }

    void  MasterSlaveUIC:: SetMasterSlaveSettings(bool isMas, ExposedObjC* expObj, const FuncObjRef& inFO)
    {
        isMaster.SetValue(isMas);
        if (expObj != 0)
            {
                isExposed.SetValue(expObj->IsExposed());
                expObj->Update();
            }
        else
            isExposed.SetValue(false);
        masterObjectList.SetFuncObjRef(inFO);
        SetInternalSensitive();
    }

    void MasterSlaveUIC::SetMasterSlaveSettings(bool isMas, ExposedWrapper& expObj, const FuncObjRef& inFO)
    {
        SetMasterSlaveSettings(isMas, expObj.exposedObj, inFO);
    }


    void  MasterSlaveUIC:: SetMasterSlaveSettings(bool isMas, const FuncObjRef& inFO)
    {
        isMaster.SetValue(isMas);
        masterObjectList.SetFuncObjRef(inFO);
        SetInternalSensitive();
    }
    void  MasterSlaveUIC::GetMasterSlaveSettings(bool& isMas, FuncObjRef& inFO)
    {
        isMas = isMaster.GetValue();
        inFO  = masterObjectList.GetFuncObjRef();
    }

    bool MasterSlaveUIC::TurnExposedOff()
    {
        return (!(isMaster.GetValue() && isExposed.GetValue()));
    }


    bool MasterSlaveUIC::MasterSlaveOK( bool&      prevWasOK,
                                        const char fieldDesc[])
    {
        if ( isMaster.GetValue())
            return true;

        if (!prevWasOK)
            return false;

        char tmpField[80];
        CopyString(tmpField, fieldDesc, 80);
        ConcatString(tmpField, "master object", 80);

        if (!masterObjectList.FuncObjRefOK(prevWasOK, tmpField))
            {
                prevWasOK = false;
                return false;
            }

        return true;
    }

};  // end namespace

