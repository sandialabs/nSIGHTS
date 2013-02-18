///////////////////////////////////////////////////////////////////////////////////
//
// C_ExposedObj.cpp
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
//              base class for exposing selected properties in a modeless dialogs.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#ifndef ADCONSOLEAPP
#include <genPlotClass/C_PlotDef.h>
#include <genApp/U_Menu.h>
#endif

#include <genClass/C_FuncObj.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <genApp/AppConfigFile.h>

#include <genApp/C_ExposedObj.h>

//  statics

#pragma warning(disable : 4073)
#pragma init_seg(lib)  // make sure this is constructed before alloc objects in app

T_SC_BasicPtrArray<ExposedObjC> ExposedObjC::exposedObjects(64);

int ExposedObjC::exposedReadVersion = 0;

ExposedObjC::ExposedObjC(FuncObjC&   FOref,
                         const char* mainT) :

#ifndef ADCONSOLEAPP
    DialogShell("ExpObj"),
    exposedObjectDialogForm(this),
#endif
    exposedFO(FOref),
    mainTitle(mainT),
    m_bProcessing(false)
{
#ifndef ADCONSOLEAPP
    animating = false;
    CRect rect(0, 0, 225, 450);
    CreateShell(NULL, true, mainTitle, rect);
    CenterWindow();
#endif

    exposedObjects.AddTo(this);
}

ExposedObjC::~ExposedObjC()
{
#ifndef ADCONSOLEAPP
    DestroyWindow();
#endif

    exposedObjects.DeleteFrom(this);

}

#ifndef ADCONSOLEAPP

void ExposedObjC::Execute()
{
    exposedFO.DoStatusChk();
    if (exposedFO.GetStatus() == os_OK)
        {
            CWaitCursor w;
            exposedFO.Execute();
            exposedFO.CalcDownStreamObjects();
            PlotDefC::ReplotAfterCalc();
        }

    if (exposedFO.GetStatus() != os_OK)
        PropErrorMsg(exposedFO.GetStatusMsg());

    Update();
}

void ExposedObjC::Update()
{
    if (!IsExposed())
        return;

    char tempStr[80];
    GetTitle(tempStr);
    SetMainWindowTitle(tempStr);
}

void ExposedObjC::GetTitle(char strTitle[80])
{
    CopyString(strTitle, exposedFO.GetID(), 80);
    ConcatString(strTitle, "::", 80);
    ConcatString(strTitle, mainTitle, 80);
}

void ExposedObjC::CloseIfCreated(ExposedObjC* inPtr)
{
    if (inPtr != 0)
        inPtr->UnExpose();
}

void ExposedObjC::UpdateAllExposed()
{
    for (int i = 0; i < exposedObjects.Size(); i++)
        {
            ExposedObjC& currEx = *(exposedObjects[i]);
            currEx.SetExposed();
            currEx.Update();
        }
}

bool  ExposedObjC::IsCreatedAndExposed(ExposedObjC* inPtr)
{
    if (inPtr == 0)
        return false;
    return inPtr->IsExposed();
}
void  ExposedObjC::UpdateIfCreated(ExposedObjC* inPtr)
{
    if (inPtr != 0)
        inPtr->Update();
}

#endif

bool ExposedObjC::DoExposedObjWrite(ExposedObjC* inPtr)
{
#ifndef ADCONSOLEAPP
    bool doWrite = (inPtr != 0) && (inPtr->IsExposed());
#else
    bool doWrite = false;
#endif

    WriteBool(doWrite);
    return doWrite;
}

bool ExposedObjC::DoExposedObjRead()
{
    return ReadBool();
}

void  ExposedObjC::ReadObjectHeader()
{
    exposedReadVersion = ReadInt();
    ReadInt();  // dummy read
    CheckForInputFailure("Reading ExposedObjC Properties");
}


void ExposedObjC:: WriteObjectHeader(int objWriteVer)
{
    WriteInt(objWriteVer);
    WriteInt(0);  // dummy write for backwards compatibility
    WriteLine();
}

#ifndef ADCONSOLEAPP

void ExposedObjC::SetAnimateText()
{
    animateButton.SetSensitive(true);
    if (animating)
        animateButton.SetButtonText("Stop");
    else
        animateButton.SetButtonText("Animate");
}

void ExposedObjC::StdAnimateCallback(int stIndx)
{
    if (!animating) {
        // can't start another animation
        if (m_bProcessing) {
            return;
        }
        animationIndex = stIndx;
        animating = true;
        SetAnimateText();
        SetAllSensitive();

        m_bProcessing = true;
        try {
            do {
                NextAnimationStep();
                // let's read button clicks now
                PumpMessages();
            } while (animating);
        } catch(...) {
            m_bProcessing = false;
            throw;
        }
        m_bProcessing = false;

    } else {
        // this can happen during PumpMessages and it interrupts the loop above
        animating = false;
    }
}

bool ExposedObjC::IsExposed()
{
    return IsVisible();
}

void ExposedObjC::SetExposed()
{
    Manage();

}

void ExposedObjC::UnExpose()
{
    Unmanage();
}


BEGIN_MESSAGE_MAP(ExposedObjC, DialogShell)
//{{AFX_MSG_MAP(ExposedObjC)
ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void ExposedObjC::OnSize(UINT nType, int cx, int cy)
{
    DialogShell::OnSize(nType, cx, cy);

    if (cx > 0 && cy > 0) {
        static int xDrift = (::GetSystemMetrics(SM_CXHSCROLL)-2)/2;
        static int yDrift = (::GetSystemMetrics(SM_CYHSCROLL)-2)/2;
        FormContainer::Resize(xDrift, 0, cx - xDrift, cy - yDrift);
    }
}

#endif

ExposedWrapper::~ExposedWrapper()
{
    delete exposedObj;
}

void ExposedWrapper::WriteExposed()
{
    if (ExposedObjC::DoExposedObjWrite(exposedObj))
        exposedObj->WriteToFile();
    else
        WriteLine();
}

void ExposedWrapper::ReadExposed()
{
    if (ExposedObjC::DoExposedObjRead())
        {
            CreateObject();
            exposedObj->ReadFromFile();
        }

};

#ifndef ADCONSOLEAPP

bool ExposedWrapper::IsExposed()
{
    return ExposedObjC::IsCreatedAndExposed(exposedObj);
}


void ExposedWrapper::CloseIfCreated()
{
    //  ExposedObjC::CloseIfCreated(exposedObj);
    //  have to delete object to allow recreation with different UI --
    //  because of exposed list single & multiple affects

    delete exposedObj;
    exposedObj = 0;
}

void ExposedWrapper::CreateAndUpdate()
{
    CreateObject();
    exposedObj->SetExposed();
    exposedObj->Update();
}

void ExposedWrapper::UpdateIfCreated()
{
    ExposedObjC::UpdateIfCreated(exposedObj);
}


void ExposedWrapper::ToggleExposed(bool close)
{
    if (close)
        CloseIfCreated();
    else
        CreateAndUpdate();
}
#endif

