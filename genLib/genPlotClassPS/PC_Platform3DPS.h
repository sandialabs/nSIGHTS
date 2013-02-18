///////////////////////////////////////////////////////////////////////////////////
//
// PC_Platform3DPS.h
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
//      adds capabilities for 3D plotting only  view control
//  sliders and view animation dialog.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PC_PLATFORM3DPS_H
#define PC_PLATFORM3DPS_H

#include <genPlotClass/C_AnnoObj.h>
#include <genPlotClass/PD_3D.h>

#include <genPlotClassPS/OGL_3DBase.h>
#include <genPlotClassPS/PC_PlatformPS.h>

#include <genClassPS/PS_UIBase.h>
#include <genClassPS/PS_UIClass.h>

#include <genPlotClassPS/PS_ViewAnimationDialog.h>
#include <genPlotClassPS/PS_TransparencyDialog.h>

using namespace genClassPS;


class PC_Platform3DPS : public PC_PlatformPS {
    private:
        DialogShell         vrmlDialog;
        PS_ViewAnimationDialog viewAnimationDialog;
        PS_TransparencyDialog  transparencyDialog;

        // status widgets
        TriSplitFormUIC     sliderForm;

        //  reset controls
        BaseUIC             buttonFrame;
        BaseUIC             attitudeFrame;
        BaseUIC             transFrame;

        //  attitude controls
        EntrySliderUIC      azimuthUI;
        EntrySliderUIC      elevationUI;
        EntrySliderUIC      scaleUI;

        EntrySliderUIC      xtranUI;
        EntrySliderUIC      ytranUI;
        EntrySliderUIC      ztranUI;

        PushButton          resetAttitudeButton;
        PushButton          resetTranslationButton;
        ToggleBoxWidget     optimizeRedrawCheckbox;

    public:
        virtual int         CalcStatusBarHeight();
        virtual int         CalcButtonBarHeight();
    public:
        PD_3D&              base3DplotDef;

    public:

                        PC_Platform3DPS(const char* plotType, PD_3D& assPlt, OGL3DBase& ogl3DObj);
                        ~PC_Platform3DPS();

        void            InitGraphicsWindow();
        void            ResetGraphicsWindow();

        virtual void    RenameGraphicsWindow(const char* newName); // resets window name


        //  att/tran control
        void            UpdateViewSettings();  // get top view and set sliders/status appropriately

        void            AzimuthValueCB();
        void            ElevationValueCB();
        void            ScaleValueCB();
        void            XtranValueCB();
        void            YtranValueCB();
        void            ZtranValueCB();

        void            AzimuthSliderCB(bool bTracking);
        void            ElevationSliderCB(bool bTracking);
        void            ScaleSliderCB(bool bTracking);
        void            XtranSliderCB(bool bTracking);
        void            YtranSliderCB(bool bTracking);
        void            ZtranSliderCB(bool bTracking);

        void            AttitudeResetCB();
        void            TranslationResetCB();

        void            InitViewAnimation();       //  setup/manage

    protected:
        bool            IsOptimizeRedraw();

        virtual UINT    GetMenuResourceID();

        //{{AFX_MSG(PC_Platform3DPS)
        afx_msg void OnAnimation();
        afx_msg void OnTransparency();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

#endif // PC_PLATFORM3DPS_H

