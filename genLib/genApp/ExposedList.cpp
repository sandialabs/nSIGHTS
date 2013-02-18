///////////////////////////////////////////////////////////////////////////////////
//
// ExposedList.cpp
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

#include "GenLibAfx.h"
#ifndef ADCONSOLEAPP
#include <genApp/U_Menu.h>
#endif

#include <genClass/C_FuncObj.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <genApp/AppConfigFile.h>
#include <genApp/ExposedList.h>

ExposedList :: ExposedList(FuncObjC&        FOref,
                           const char*      mainT,
                           DSC_IndexMSSpec& msRef,
                           int              offVal,
                           SC_StringArray&  strRef) :
    ExposedObjC(FOref, mainT),
#ifndef ADCONSOLEAPP
    exposedMS(msRef),
#endif
    offsetVal(offVal),
    listData(strRef)
{
#ifndef ADCONSOLEAPP

    ArgListC args;


    if (exposedMS.enteredIndexIsSingle)
        {

            args.Clear();
            args.LeftPosAttach(2);
            args.RightPosAttach(98);
            args.BottomFormAttach();
            animRowForm.InitWidget(exposedObjectDialogForm.GetWidget(), args);

            // buttons
            args.Clear();
            args.LeftFormAttach();
            args.RightFormAttach();
            args.BottomFormAttach(2);

            animateButton.StdButtonInit("Animate", animRowForm.GetWidget(), args);
            animateButton.AddOtherObjectCallback(AnimButtonCB, this);

            args.Clear();
            args.LeftFormAttach();
            args.RightFormAttach();
            args.BottomBaseAttach(animRowForm);
            bottomRowForm.InitWidget(exposedObjectDialogForm.GetWidget(), args);

            args.Clear();
            args.BottomFormAttach(2);
            args.SetPlaceHolder();

            args.LeftPosAttach(2);
            args.RightPosAttach(48);
            toStartButton.StdButtonInit(" |<- ", bottomRowForm.GetWidget(), args);
            toStartButton.AddOtherObjectCallback(ToStartButtonCB, this);

            args.ResetPlace();
            args.LeftPosAttach(52);
            args.RightPosAttach(98);
            toEndButton.StdButtonInit(" ->| ", bottomRowForm.GetWidget(), args);
            toEndButton.AddOtherObjectCallback(ToEndButtonCB, this);

            // next row buttons
            args.Clear();
            args.LeftFormAttach();
            args.RightFormAttach();
            args.BottomBaseAttach(bottomRowForm);
            nextRowForm.InitWidget(exposedObjectDialogForm.GetWidget(), args);

            args.Clear();
            args.BottomFormAttach(2);
            args.SetPlaceHolder();

            args.LeftPosAttach(2);
            args.RightPosAttach(48);
            nextButton.StdButtonInit(" > ", nextRowForm.GetWidget(), args);
            nextButton.AddOtherObjectCallback(NextButtonCB, this);

            args.ResetPlace();
            args.LeftPosAttach(52);
            args.RightPosAttach(98);
            prevButton.StdButtonInit(" < ", nextRowForm.GetWidget(), args);
            prevButton.AddOtherObjectCallback(PrevButtonCB, this);

            args.Clear();
            args.LeftFormAttach();
            args.RightFormAttach();
            args.BottomBaseAttach(nextRowForm);
            toggleRowForm.InitWidget(exposedObjectDialogForm.GetWidget(), args);

            args.StdToggleSetup(5);
            startAnimAtZero.StdToggleInit("animate from start", toggleRowForm.GetWidget(), args);
            startAnimAtZero.SetValue(true);

            args.StdFormAttach(5);
            args.BottomWidgetAttach(toggleRowForm.GetWidget());
        }
    else
        {
            args.Clear();
            args.LeftPosAttach(2);
            args.RightPosAttach(98);
            args.BottomFormAttach();
            bottomRowForm.InitWidget(exposedObjectDialogForm.GetWidget(), args);

            args.Clear();
            args.LeftFormAttach();
            args.RightFormAttach();
            args.BottomFormAttach(2);

            applyButton.StdButtonInit(" Apply ", bottomRowForm.GetWidget(), args);
            applyButton.AddOtherObjectCallback(ApplyCB, this);

            // next row buttons
            args.Clear();
            args.LeftFormAttach();
            args.RightFormAttach();
            args.BottomBaseAttach(bottomRowForm);
            nextRowForm.InitWidget(exposedObjectDialogForm.GetWidget(), args);

            args.Clear();
            args.BottomFormAttach(2);
            args.SetPlaceHolder();

            args.LeftPosAttach(2);
            args.RightPosAttach(48);
            selectNoneButton.StdButtonInit(" None ", nextRowForm.GetWidget(), args);
            selectNoneButton.AddOtherObjectCallback(SelectNoneCB, this);

            args.ResetPlace();
            args.LeftPosAttach(52);
            args.RightPosAttach(98);
            selectAllButton.StdButtonInit(" All ", nextRowForm.GetWidget(), args);
            selectAllButton.AddOtherObjectCallback(SelectAllCB, this);

            args.StdFormAttach(10);
            args.BottomWidgetAttach(nextRowForm.GetWidget());
        }

    // list
    args.BottomOffset(5);
    scrolledList.isSingleSel = exposedMS.enteredIndexIsSingle;
    scrolledList.InitWidget(exposedObjectDialogForm.GetWidget(), args);
    scrolledList.AddOtherObjectCallback(ListChangeCB, this);

    SetForm(&exposedObjectDialogForm);
    ResetForm();

    SetAllSensitive();

    // set title
    Update();
#endif

}

ExposedList :: ~ExposedList()
{
}

void ExposedList::WriteToFile()
{
    WriteLine();   // dummy required for cases when exposed is last data written in obj
}

#ifndef ADCONSOLEAPP

void  ExposedList::SetAllSensitive()
{
    bool dataAvail = (listData.Size() > 0);
    bool buttonOK = dataAvail && (!animating);
    scrolledList.SetSensitive(buttonOK);
    if (exposedMS.enteredIndexIsSingle)
        {
            toStartButton.SetSensitive(buttonOK);
            toEndButton.SetSensitive(buttonOK);
            nextButton.SetSensitive(buttonOK);
            prevButton.SetSensitive(buttonOK);
            animateButton.SetSensitive(dataAvail);
        }
    else
        {
            selectAllButton.SetSensitive(buttonOK);
            selectNoneButton.SetSensitive(buttonOK);
            applyButton.SetSensitive(buttonOK && (!scrolledList.currMultipleSelection.IsEmpty()));
        }
}

void ExposedList::DoNewIndex(int newSel)
{
    int newIndx = newSel + offsetVal;
    if (newIndx != exposedMS.enteredIndexValue)
        {
            scrolledList.SetSingleSelection(newSel);
            exposedMS.enteredIndexValue = newIndx;
            Execute();
        }
}

void ExposedList::DoListChangeCB()
{
    if (exposedMS.enteredIndexIsSingle)
        DoNewIndex(scrolledList.GetSingleSelection());
    else
        SetAllSensitive();
}

void ExposedList::ListChangeCB(void* inObj)
{
    (static_cast<ExposedList*>(inObj))->DoListChangeCB();
}



void ExposedList::DoSelectNone()
{
    if (!exposedMS.enteredIndexIsSingle)
        {
            scrolledList.ClearAll();
            SetAllSensitive();
        }
}

void ExposedList::SelectNoneCB(void* inObj)
{
    (static_cast<ExposedList*>(inObj))->DoSelectNone();
}

void ExposedList::DoSelectAll()
{
    if (!exposedMS.enteredIndexIsSingle)
        {
            scrolledList.SelectAll();
            SetAllSensitive();
        }
}

void ExposedList::SelectAllCB(void* inObj)
{
    (static_cast<ExposedList*>(inObj))->DoSelectAll();
}

void ExposedList::DoApply()
{
    if (!exposedMS.enteredIndexIsSingle)
        {
            scrolledList.GetMultipleSelection(exposedMS.enteredIndexArray);
            for ( int i = 0; i < exposedMS.enteredIndexArray.Size(); i++)
                exposedMS.enteredIndexArray[i] += offsetVal;

            Execute();
        }
}

void ExposedList::ApplyCB(void* inObj)
{
    (static_cast<ExposedList*>(inObj))->DoApply();
}


void ExposedList::DoToStartButtonCB()
{
    DoNewIndex(0);
}

void ExposedList::ToStartButtonCB(void* inObj)
{
    (static_cast<ExposedList*>(inObj))->DoToStartButtonCB();
}

void ExposedList::DoToEndButtonCB()
{
    DoNewIndex(listData.Size() - 1);
}

void ExposedList::ToEndButtonCB(void* inObj)
{
    (static_cast<ExposedList*>(inObj))->DoToEndButtonCB();
}

void ExposedList::DoNextButtonCB()
{
    int newIndx = scrolledList.GetSingleSelection();
    newIndx++;
    if (newIndx < listData.Size())
        DoNewIndex(newIndx);
}

void ExposedList::NextButtonCB(void* inObj)
{
    (static_cast<ExposedList*>(inObj))->DoNextButtonCB();
}

void ExposedList::DoPrevButtonCB()
{
    int newIndx = scrolledList.GetSingleSelection();
    newIndx --;
    if (newIndx >= 0)
        DoNewIndex(newIndx);
}


void ExposedList::PrevButtonCB(void* inObj)
{
    (static_cast<ExposedList*>(inObj))->DoPrevButtonCB();
}

void ExposedList::DoAnimButtonCB()
{
    int stIndx = 0;
    if (!startAnimAtZero.GetValue())
        stIndx = scrolledList.GetSingleSelection();

    StdAnimateCallback(stIndx);
    SetAnimateText();
    SetAllSensitive();
}

void  ExposedList::NextAnimationStep()
{
    if (!animating)
        return;

    scrolledList.SetSingleSelection(animationIndex);
    exposedMS.enteredIndexValue = animationIndex + offsetVal;
    Execute();

    animationIndex++;
    if (animationIndex == listData.Size())
        {
            animating = false;
            SetAnimateText();
            SetAllSensitive();
        }
}

void ExposedList::AnimButtonCB(void* inObj)
{
    (static_cast<ExposedList*>(inObj))->DoAnimButtonCB();
}

void ExposedList::Update()
{
    if (!IsExposed())
        return;

    ExposedObjC::Update();
    if (listData.IsEmpty())
        {
            SC_StringArray temp;
            temp.Alloc(1);
            temp += "<< no data >>";
            scrolledList.SetListText(temp);
            exposedMS.enteredIndexValue = offsetVal;
            currListData.DeAlloc();
        }
    else
        {
            // has list changed ?
            if (!(currListData == listData))
                {
                    // update if different
                    scrolledList.SetListText(listData);
                    currListData = listData;
                }

            if (exposedMS.enteredIndexIsSingle)
                {
                    int currIndx = exposedMS.enteredIndexValue - offsetVal;
                    if (currIndx < 0)
                        currIndx = 0;
                    if (currIndx >= listData.Size())
                        currIndx = listData.Size() - 1;
                    scrolledList.SetSingleSelection(currIndx);
                    scrolledList.SetSelectionVisible();
                    exposedMS.enteredIndexValue = currIndx + offsetVal;
                }
            else
                {
                    SC_IntArray currIndex(exposedMS.enteredIndexArray);
                    int maxIndex = listData.Size();
                    int nOK = 0;
                    int i;
                    for (i = 0; i < currIndex.Size(); i++)
                        if ((currIndex[i] - offsetVal) < maxIndex)
                            {
                                currIndex[nOK++] =  currIndex[i] - offsetVal;
                            }
                    currIndex.SetSize(nOK);
                    scrolledList.SetMultipleSelection(currIndex);

                    for (i = 0; i < nOK; i++)
                        exposedMS.enteredIndexArray[i] = currIndex[i] + offsetVal;
                    exposedMS.enteredIndexArray.SetSize(nOK);
                }
        }
    SetAllSensitive();

}

#endif

void ExposedListWrapper::CreateObject()
{
    if (exposedObj == 0)
        exposedObj = new ExposedList(FOref, mainTitle, msRef, offVal, strRef);
}

