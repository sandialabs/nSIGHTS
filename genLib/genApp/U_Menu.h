///////////////////////////////////////////////////////////////////////////////////
//
// U_Menu.h
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

#ifndef U_MENU_H
#define U_MENU_H

#include <genClass/C_FuncObj.h>

#include <genClass/C_Common.h>

#include <genClass/SC_StringArray.h>
#include <genClass/SC_RealFormat.h>

#include <genClassPS/PS_ArgList.h>
#include <genClassPS/PS_UIBase.h>
#include <genClassPS/PS_UIClass.h>

#include <genApp/C_ExposedObj.h>


using namespace genClassPS;

namespace genAppMenuUtilities {

    // ************************************************************** msg vectoring

    extern void PropErrorMsg(const char* errMsg);

    extern void PropNormMsg(const char* normMsg);

    extern void PropClearMsg();

    extern void PropInputFieldError(const char* fieldName,
                                    const char* errMsg);

    // ************************************************************* helper routines

    extern bool     FileOverwriteOK(const char* fName);  // returns true if file does not exist
    // or if user says OK to overrwrite

    // ************************************************************* helper routines

    class PlaneRadioBoxChooserWidget : public RadioBoxChooserWidget  {
    public:
        //  sets up private vars
        // must be called before Init ... routines
        void          PreInitSetUp(bool isHoriz, bool useLongDesc = true);

        void          SetPlane(Plane3D inPlane) {SetChoice(int(inPlane));}

        Plane3D       GetPlane() {return Plane3D(currChoice);}

    };


    class FuncObjRefComboBox : public ComboBoxSelectionWidget {
    private:
        const type_info&    listType;
        FuncObjRefArray     listData;
        bool                            listDataOK;
        FuncObjRef          currFO;

    public:
        FuncObjRefComboBox (const type_info& newType);

        void        ResetListData(FuncObjC* extraObj = 0);
        bool        ListDataOK() {return listDataOK;}

        void        DefaultFuncObjRef(FuncObjRef& inFO);

        void        SetFuncObjRef(const FuncObjRef& inFO);
        void        SetToDefault();

        void        StdMenuInit(FuncObjRef&  inFO,
                                FuncObjC*    extraObj = 0);  // reset, default, and set

        bool        FuncObjRefOK(bool&      prevWasOK,
                                 const char fieldDesc[]);

        FuncObjRef& GetFuncObjRef() {return currFO;}

        bool        CheckDataObj();  // true if next will return OK data obj
        bool        DataObjAvail() {return currFO.objRef != 0;}
        DataObjC*   GetDataObj() {return currFO.GetData(listType);}


    protected:
        void            UpdateFO();
        static  void    UpdateFOCB(void* inObj);
    };


    // ************************************************************* UIC for specific types

#pragma warning( push )
#pragma warning( disable : 4355 ) // 'this' : used in base member initializer list


    class PlaneRadioBoxUIC : public ChooserBaseUIC, public PlaneRadioBoxChooserWidget  {

    public:
        PlaneRadioBoxUIC(): ChooserBaseUIC(this) {}
        void    SetSensitive(bool isSensitive) {ChooserBaseUIC::SetSensitive(isSensitive);}
    };


    class FuncObjRefUIC : public ComboBaseUIC, public FuncObjRefComboBox {

    public:
        FuncObjRefUIC(const type_info& inType);

        // attaches to frame by default
        void    InitFramedUI(char              frameTitle[],
                             Widget            parent,
                             const  ArgListC&  inArgs);      // attachment resources + others

        void    SetSensitive(bool isSensitive) {ComboBaseUIC::SetSensitive(isSensitive);}
    };

#pragma warning( pop )


    // ************************************************************* master slave


    class MasterSlaveUIC  : public BaseUIC  {

    public:
        ToggleBoxWidget     isMaster;
        ToggleBoxWidget     isExposed;
        FuncObjRefUIC       masterObjectList;
        bool                horizontal;
        bool                compressed;
        const bool          exposeOK;

    protected:
        SplitFormUIC        horizSplit;
        FormWidget          toggleForm;

    public:
        MasterSlaveUIC(const type_info& inType,
                       bool       expOK);

        void    InitFramedUI(char             frameTitle[],
                             Widget           parent,
                             const ArgListC&  inArgs,         // attachment resources + others
                             int              expXpos = 50);  // to allow extra toggles in IndexMS

        void    InitFormUI(Widget           parent,
                           const ArgListC&  inArgs,
                           int              expXpos = 50);

        void    SetSensitive(bool isSensitive);

        void    SetMasterSlaveSettings(bool isMas, ExposedObjC* expObj, const FuncObjRef& inFO);  // w expose
        void    SetMasterSlaveSettings(bool isMas, ExposedWrapper& expObj, const FuncObjRef& inFO);  // w expose
        void    SetMasterSlaveSettings(bool isMas, const FuncObjRef& inFO); //w/o expose

        void    GetMasterSlaveSettings(bool& isMas, FuncObjRef& inFO);

        bool    TurnExposedOff();

        void    SetInternalSensitive(); //used in CB

        bool    MasterSlaveOK(bool& prevWasOK,
                              const char fieldDesc[]);

        FormWidget* GetToggleForm();  // used by IndexMS


    private:
        void            InitUI(int expXpos);

        static void SetInternalSensitiveCB(void* inObj);

    };



};   // namespace

using namespace genAppMenuUtilities;

#endif // U_MENU_H

