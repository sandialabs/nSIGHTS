///////////////////////////////////////////////////////////////////////////////////
//
// PS_ObjectDialog.h
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

#ifndef PS_OBJECTDIALOG_H
#define PS_OBJECTDIALOG_H

#include <genClass/C_Common.h>
#include <genClass/T_SC_AllocPtrArray.h>

#include <genPlotClass/C_PlotObjBase.h>

#include <genClassPS/PS_DialogShell.h>

using namespace genClassPS;

class PlotDefC;
class PC_PlatformPS;

class PS_ObjectDialog : public DialogShell {
    public:
        //  class has to be public because of HP aCC error with nested classes
        //  combo widget & data
        class PS_ObjectControl {
            public:
                ToggleBoxWidget     objStatus;
                PlotObjBaseC*       currObj;

            public:
                PS_ObjectControl() : currObj(0) {}
                            ~PS_ObjectControl() {}

                void        SetControlFromObject();
                void        SetObjectFromControl();
                void        SetControl(bool controlVal);
        };

    private:
        //  control menu
        DialogShellFormManager controlDialogForm;
        ScrolledForm        controlScrollArea;
        FormWidget*         controlScrollForm;

        FormWidget          bottomRowForm;
        FormWidget          nextRowForm;

        PushButton          controlClearButton;
        PushButton          controlAllButton;
        PushButton          controlCloseButton;
        PushButton          controlApplyButton;

        LabelWidget         noLabel;

        T_SC_AllocPtrArray<PS_ObjectControl> controlData;

        PlotDefC&           basePlotDef;
        PC_PlatformPS&      basePlatform;

    public:
                            PS_ObjectDialog(PlotDefC& assPlot, PC_PlatformPS&  assPlatform);
        virtual             ~PS_ObjectDialog();

        void                OpenObjectDialog();     // called from PC_Platform menu bar

    private:
        //  object control
        void                ResetObjectControl();
        void                ClearObjectControl();

        //  control callbacks
        static void         CloseButtonCB (void* clientData);
        static void         ApplyButtonCB (void* clientData);
        static void         SetAllButtonCB (void* clientData);
        static void         ClearAllButtonCB (void* clientData);
        static void         FocusCB (void* clientData);

        void                DoSetClearAll(bool setValue);
        void                DoClose();
        void                DoApply();
        void                DoFocusIn();

    protected:
        //{{AFX_MSG(PS_ViewAnimationDialog)
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
        afx_msg void OnClose();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

#endif // !PS_OBJECTDIALOG_H

