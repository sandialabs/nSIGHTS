///////////////////////////////////////////////////////////////////////////////////
//
// PS_ViewAnimationDialog.h
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
//      implements 3D View Animation dialog.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PS_VIEWANIMATIONDIALOG_H
#define PS_VIEWANIMATIONDIALOG_H

#include <genClassPS/PS_DialogShell.h>

#include <genPlotClass/PC_View.h>

class PD_3D;
class PC_Platform3DPS;

class PS_ViewAnimationDialog : public DialogShell {
    public:

        //  classes have to be public because of HP aCC error with nested classes
        class ViewAnimationElementUIC : public BaseUIC {
            private:
                double*             startVal;
                double*             endVal;

            public:
                RealEntryWidget     startValUI;
                RealEntryWidget     endValUI;

            public:
                                    ViewAnimationElementUIC() {};
                                    ~ViewAnimationElementUIC() {};

                void                InitLabeledUI(      char           inLabel[],
                                                        Widget         parent,
                                                  const ArgListC&      inArgs);

                void                SetFormats(const SC_RealFormat& rf);

                void                SetValues(double&   sVal,
                                              double&   eVal);
                void                UpdateFromValues();
                bool                UpdateToValues();
        };

        class ViewAnimationUIC {
            private:
                enum                     {nviewElements = 12};
                ViewAnimationElementUIC  viewAnimationData[nviewElements];  //  XYZ min/max, XYZ tran, az/el/sc

            public:
                                ViewAnimationUIC() {};
                                ~ViewAnimationUIC() {};

                void            Initialize(Widget   parent,
                                           PC_View& stView,
                                           PC_View& endView);

                void            SetFormats(PD_3D&   plotDef);

                void            UpdateFromViews();
                bool            UpdateToViews();
        };

    private:
        //  log scale animation if start and end are +ve
        bool                logScaleInc;

        //  data
        PC_View             viewAnimationStartView;
        PC_View             viewAnimationEndView;
        PC_View             viewAnimationDeltaView;

        //  view animation dialog
        DialogShellFormManager  viewAnimationDialogForm;
        FormWidget          elementForm;
        FormWidget          bottomRowForm;
        FormWidget          nextRowForm;
        FormWidget          toggleForm;
        ViewAnimationUIC    viewAnimationData;

        ToggleBoxWidget     logScaleIncUI;
        IntEntryUIC         numberOfIncUI;

        PushButton          stepButton;
        PushButton          animateButton;
        PushButton          gotoStartButton;
        PushButton          gotoEndButton;
        PushButton          setStartButton;
        PushButton          setEndButton;
        PushButton          endToStartButton;
        PushButton          closeButton;

        //  animation work proc stuff
        bool                animating;
        int                 animationIndex;
        int                 nanimStep;
        bool                m_bProcessing;

        PD_3D&              base3DplotDef;
        PC_Platform3DPS&    base3DPlatform;

    public:
                            PS_ViewAnimationDialog(PD_3D& assPlot, PC_Platform3DPS&  assPlatform);
                            ~PS_ViewAnimationDialog();

        void                OpenViewAnimationDialog(CWnd* pParentWnd);


    private:
        //              callbacks
        static void     StepButtonCB (void* clientData);
        static void     AnimateButtonCB (void* clientData);
        static void     GotoStartButtonCB (void* clientData);
        static void     GotoEndButtonCB (void* clientData);
        static void     SetStartButtonCB (void* clientData);
        static void     SetEndButtonCB (void* clientData);
        static void     EndToStartButtonCB (void* clientData);
        static void     CloseButtonCB (void* clientData);

        //              callback actions
        void            DoStep();
        void            DoAnimate();
        void            DoGotoStart();
        void            DoGotoEnd();
        void            DoSetStart();
        void            DoSetEnd();
        void            DoEndToStart();
        void            DoClose();

        //              misc support
        bool            SetDeltaView();
        void            NextAnimStep();

        //              animation support
        void            SetAnimateText();
        bool            IsAnimating() {return animating;}

        //  view output
        void            DumpViewToCout(const PC_View& outView);

    protected:
        //{{AFX_MSG(PS_ViewAnimationDialog)
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnClose();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

#endif // !PS_VIEWANIMATIONDIALOG_H

