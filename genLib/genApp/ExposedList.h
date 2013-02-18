///////////////////////////////////////////////////////////////////////////////////
//
// ExposedList.h
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
//              Implements exposed object support for list selection type data.
//              Derived from ExposedObjC
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_EXPOSEDLIST_H
#define C_EXPOSEDLIST_H

#include <genClass/SC_StringArray.h>
#include <genDataClass/DSC_IndexMSSpec.h>

#ifndef ADCONSOLEAPP
#include <genClassPS/PS_UIClass.h>
#endif

#include <genApp/C_ExposedObj.h>

class ExposedList : public ExposedObjC {

    friend class ExposedListWrapper;

#ifndef ADCONSOLEAPP
private:
    ScrolledListWidget          scrolledList;

    FormWidget                  animRowForm;
    FormWidget                  nextRowForm;
    FormWidget                  bottomRowForm;
    FormWidget                  toggleRowForm;

    // anim start
    ToggleBoxWidget             startAnimAtZero;

    //  single selection
    PushButton                  toStartButton;
    PushButton                  toEndButton;
    PushButton                  nextButton;
    PushButton                  prevButton;

    //  mult selection
    PushButton                  selectNoneButton;
    PushButton                  selectAllButton;
    PushButton                  applyButton;

    DSC_IndexMSSpec&            exposedMS;
#endif
    const int                   offsetVal;
    SC_StringArray&             listData;
    SC_StringArray              currListData;

public:

    virtual                     ~ExposedList();

#ifndef ADCONSOLEAPP
    virtual void                Update();
#endif

    //  C_CfgFileOBJ virtual
    virtual void                WriteToFile();


protected:
#ifndef ADCONSOLEAPP
    virtual void                NextAnimationStep();
#endif

private:
    ExposedList(FuncObjC&           FOref,
                const char*         mainT,
                DSC_IndexMSSpec&    msRef,
                int                 offVal,
                SC_StringArray&     strRef);

#ifndef ADCONSOLEAPP

    //  for call back support

    static void                 SelectNoneCB(void* inObj);
    static void                 SelectAllCB(void* inObj);
    static void                 ApplyCB(void* inObj);

    static void                 ListChangeCB(void* inObj);
    static void                 ToStartButtonCB(void* inObj);
    static void                 ToEndButtonCB(void* inObj);
    static void                 NextButtonCB(void* inObj);
    static void                 PrevButtonCB(void* inObj);
    static void                 AnimButtonCB(void* inObj);

    void                        DoSelectNone();
    void                        DoSelectAll();
    void                        DoApply();

    void                        DoListChangeCB();
    void                        DoToStartButtonCB();
    void                        DoToEndButtonCB();
    void                        DoNextButtonCB();
    void                        DoPrevButtonCB();
    void                        DoAnimButtonCB();

    void                        DoNewIndex(int newSel);
    void                        SetAllSensitive();
#endif

};

class ExposedListWrapper : public ExposedWrapper {
private:
    // used by create virtual
    FuncObjC&           FOref;
    const char*         mainTitle;
    DSC_IndexMSSpec&    msRef;
    int                 offVal;
    SC_StringArray&     strRef;

public:

    ExposedListWrapper(FuncObjC&        inFOref,
                       DSC_IndexMSSpec& iSpec,
                       int              inOffVal,
                       SC_StringArray&  inStrRef) :
        FOref(inFOref), mainTitle(iSpec.indexID),
        msRef(iSpec), offVal(inOffVal),
        strRef(inStrRef) {}

    virtual         ~ExposedListWrapper() {};

    virtual void    CreateObject();

};

#endif // C_EXPOSEDLIST_H

