///////////////////////////////////////////////////////////////////////////////////
//
// PS_ObjectDialog.cpp
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
// DESCRIPTION:
//
//      implements Object Control dialog. 
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include <genPlotClass/C_PlotObj.h>
#include <genPlotClass/C_AnnoObj.h>
#include <genPlotClass/C_PlotDef.h>

#include <genPlotClassPS/PC_PlatformPS.h>
#include <genPlotClassPS/PS_ObjectDialog.h>


void PS_ObjectDialog::PS_ObjectControl::SetControlFromObject()
{
    char expandedName[80];
    MakeString(expandedName, currObj->GetID(), "::", currObj->GetType(), 80);
    objStatus.SetToggleText(expandedName);
    objStatus.SetValue(currObj->doPlot);
}



void PS_ObjectDialog::PS_ObjectControl::SetObjectFromControl()
{
    currObj->doPlot = objStatus.GetValue();
}

void  PS_ObjectDialog::PS_ObjectControl::SetControl(bool controlVal)
{
    objStatus.SetValue(controlVal);
}

PS_ObjectDialog :: PS_ObjectDialog(PlotDefC& assPlot, PC_PlatformPS&  assPlatform)  :
    basePlotDef(assPlot),
    basePlatform(assPlatform),
    controlDialogForm(this),
    controlScrollForm(NULL),
    DialogShell("::Control")
{
}

PS_ObjectDialog :: ~PS_ObjectDialog ()
{
    delete controlScrollForm;
}

void PS_ObjectDialog::FocusCB (void* clientData)
{
    static_cast<PS_ObjectDialog*>(clientData)-> DoFocusIn();
}

void  PS_ObjectDialog::CloseButtonCB (void* clientData)
{
    static_cast<PS_ObjectDialog*>(clientData)->DoClose();
}

void  PS_ObjectDialog::ApplyButtonCB (void* clientData)
{
    static_cast<PS_ObjectDialog*>(clientData)-> DoApply();
}

void  PS_ObjectDialog::SetAllButtonCB (void* clientData)
{
    static_cast<PS_ObjectDialog*>(clientData)-> DoSetClearAll(true);
}

void  PS_ObjectDialog::ClearAllButtonCB (void* clientData)
{
    static_cast<PS_ObjectDialog*>(clientData)-> DoSetClearAll(false);
}

void PS_ObjectDialog::DoClose()
{
    Unmanage();
}

void PS_ObjectDialog::DoApply()
{
    CWaitCursor w;
    for (int i = 0; i < controlData.Size(); i++)
        controlData.GetRef(i).SetObjectFromControl();
    basePlotDef.RedrawPlot();
}

void  PS_ObjectDialog::DoSetClearAll(bool setValue)
{
    for (int i = 0; i < controlData.Size(); i++)
        controlData.GetRef(i).SetControl(setValue);
}

void  PS_ObjectDialog::DoFocusIn()
{
    // plot objects match ?
    int nobj = basePlotDef.plotObjList.Size() + basePlotDef.annoObjList.Size();

    if (nobj  == controlData.Size())
    {
        //controlDialog.DisplayBusyCursor();
        CWaitCursor w;
        // OK - refresh
        nobj = 0;
        int i;
        for (i = 0; i < basePlotDef.annoObjList.Size(); i++)
        {
            controlData.GetRef(nobj).currObj = basePlotDef.annoObjList[i];
            controlData.GetRef(nobj++).SetControlFromObject();
        }

        for (i = 0; i < basePlotDef.plotObjList.Size(); i++)
        {
            controlData.GetRef(nobj).currObj = basePlotDef.plotObjList[i];
            controlData.GetRef(nobj++).SetControlFromObject();
        }
        //controlDialog.DisplayDefaultCursor();
        return;
    }

    // no so init
    ResetObjectControl();
}

//  object control
void PS_ObjectDialog::OpenObjectDialog()
{
    if (!IsCreated())
    {
        //controlDialog.CreateShell(basePlatform.plotShell.GetWidget(), true, "Plot Object Control");
        CRect rect(0, 0, 180, 400);

        char plotName[80];
        basePlatform.GetMainWindowTitle(plotName, false, 80);
        ConcatString(plotName, "::Control", 80);

        CreateShell(NULL, true, plotName, rect);

        ArgListC args;

        //args.AddArg(XmNautoUnmanage,          True);
        //args.AddWidthHeight(175, 400);
        //controlDialogForm.CreateWidget(controlDialog, args);
        //TODO:
        //controlDialogForm.AddCallback(XmNfocusCallback, FocusCB, this);

        // add buttons to bottom
        // bottom row buttons
        args.Clear();
        args.LeftFormAttach();
        args.RightFormAttach();
        args.BottomFormAttach();
        bottomRowForm.InitWidget(controlDialogForm.GetWidget(), args);

        args.Clear();
        args.AddArg(Arg::XmNwidth, 76);
        args.BottomFormAttach(2);
        args.SetPlaceHolder();

        // close
        args.LeftPosAttach(2);
        //TODO:
        //controlCloseButton.FixedSizeButtonArgs(args);
        controlCloseButton.StdButtonInit(" Close ", bottomRowForm.GetWidget(), args);
        controlCloseButton.AddOtherObjectCallback(CloseButtonCB, this);

        // apply
        args.ResetPlace();
        args.LeftPosAttach(50);
        controlApplyButton.StdButtonInit("Apply", bottomRowForm.GetWidget(), args);
        controlApplyButton.AddOtherObjectCallback(ApplyButtonCB, this);

        // next row buttons
        args.Clear();
        args.LeftFormAttach();
        args.RightFormAttach();
        args.BottomBaseAttach(bottomRowForm);
        nextRowForm.InitWidget(controlDialogForm.GetWidget(), args);

        args.Clear();
        args.AddArg(Arg::XmNwidth, 76);
        args.BottomFormAttach(2);
        args.SetPlaceHolder();

        // step button
        args.LeftPosAttach(2);
        controlClearButton.StdButtonInit(" Clear ", nextRowForm.GetWidget(), args);
        controlClearButton.AddOtherObjectCallback(ClearAllButtonCB, this);

        //  to start
        args.ResetPlace();
        args.LeftPosAttach(50);
        controlAllButton.StdButtonInit(" Set ", nextRowForm.GetWidget(), args);
        controlAllButton.AddOtherObjectCallback(SetAllButtonCB, this);

        // scroll window
        args.Clear();
        args.StdFormAttach();
        args.BottomBaseAttach(nextRowForm);
        //controlScrollArea.needHorizontal = false;
        controlScrollArea.InitWidget(controlDialogForm.GetWidget(), args);
        SetForm(&controlDialogForm);
    }

    Manage();
}

void PS_ObjectDialog::ResetObjectControl()
{
    CWaitCursor w;

    controlScrollArea.Destroy();
    if (controlData.Size() > 0)
        ClearObjectControl();

    // reset form
    ArgListC args;
    args.StdFormAttach();

    if (controlScrollForm != NULL) {
        controlScrollForm->Remove();
        delete controlScrollForm;
    }
    controlScrollForm = new FormWidget;
    controlScrollForm->InitWidget(controlScrollArea, args);

    // count objects
    int nObj = basePlotDef.plotObjList.Size() + basePlotDef.annoObjList.Size();
    // no objects
    if (noLabel.IsCreated()) {
        noLabel.DestroyWindow();
    }
    if (nObj == 0) {
        noLabel.StdLabelInit("No objects to control", controlScrollForm->GetWidget(), args);
        //controlScrollForm->Manage();
        return;
    }

    controlData.AllocAndCreate(nObj);
    nObj = 0;
    int i;
    for (i = 0; i < basePlotDef.annoObjList.Size(); i++)
    {

        PlotObjBaseC& plotObj = basePlotDef.annoObjList.GetRef(i);
        controlData.GetRef(nObj).objStatus.StdToggleInit(plotObj.GetID(), controlScrollForm->GetWidget(), args);
        controlData.GetRef(nObj).currObj = &plotObj;
        controlData.GetRef(nObj).SetControlFromObject();
        args.Clear();
        args.StdTopObjectAttach(controlData.GetRef(nObj++).objStatus.GetWidget());
    }

    for (i = 0; i < basePlotDef.plotObjList.Size(); i++)
    {

        PlotObjBaseC& plotObj = basePlotDef.plotObjList.GetRef(i);
        controlData.GetRef(nObj).objStatus.StdToggleInit(plotObj.GetID(), controlScrollForm->GetWidget(), args);
        controlData.GetRef(nObj).currObj = &plotObj;
        controlData.GetRef(nObj).SetControlFromObject();
        args.Clear();
        args.StdTopObjectAttach(controlData.GetRef(nObj++).objStatus.GetWidget());
    }

    //controlScrollForm->Manage();
    controlData.SetSize(nObj);
}

void PS_ObjectDialog::ClearObjectControl()
{
    if (controlData.IsEmpty())
        return;

    controlScrollArea.Destroy();
}

BEGIN_MESSAGE_MAP(PS_ObjectDialog, DialogShell)
    //{{AFX_MSG_MAP(PS_ObjectDialog)
    ON_WM_SIZE()
    ON_WM_ACTIVATE()
    ON_WM_CLOSE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



void PS_ObjectDialog::OnSize(UINT nType, int cx, int cy)
{
    DialogShell::OnSize(nType, cx, cy);

    if (cx > 0 && cy > 0) {
        static int xDrift = (::GetSystemMetrics(SM_CXHSCROLL)-2)/2;
        static int yDrift = (::GetSystemMetrics(SM_CYHSCROLL)-2)/2;
        FormContainer::Resize(xDrift, 0, cx - xDrift, cy - yDrift);
    }
}

void PS_ObjectDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE) {

//      TRACE("Activate PS_ObjectDialog %d\r\n", nState);

        DoFocusIn();
        ResetForm();
        controlScrollArea.Invalidate();
        controlScrollArea.UpdateWindow();
    }
    DialogShell::OnActivate(nState, pWndOther, bMinimized);
}

void PS_ObjectDialog::OnClose()
{
    DoClose();
}

