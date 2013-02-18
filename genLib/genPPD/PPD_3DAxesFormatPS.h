///////////////////////////////////////////////////////////////////////////////////
//
// PPD_3DAxesFormatPS.h
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
//      increment/grid line formatting for 3D plots
//
//      derived from PPD_Base.  Adds components for all 3D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PPD_3DAXESFORMATPS_H
#define PPD_3DAXESFORMATPS_H

#include <genPlotClass/C_PlotDef.h>
#include <genPlotClass/PC_3DAxesFormat.h>

#include <genApp/C_MenuBase.h>
#include <genApp/U_MenuPPO.h>

class AxesLinePosWidget : public OptionChooserWidget {
    public:
        void                        PreInitSetUp();
        void                        SetAxesLinePos(PC_AxesFormat::AxesLinePos inLine) {SetChoice(int(inLine));}
        PC_AxesFormat::AxesLinePos  GetAxesLinePos() {return PC_AxesFormat::AxesLinePos(currChoice);}
};
class AxesTicPosWidget : public OptionChooserWidget {
    public:
        void                        PreInitSetUp();
        void                        SetAxesTicPos(PC_AxesFormat::AxesTicPos inTic) {SetChoice(int(inTic));}
        PC_AxesFormat::AxesTicPos  GetAxesTicPos() {return PC_AxesFormat::AxesTicPos(currChoice);}
};

class AxesFormatUIC : public BaseUIC {
    public:
        AxesIncOptionWidget         uMajor;
        AxesIncOptionWidget         uMinor;
        AxesLinePosWidget           uLines;
        AxesTicPosWidget            uTics;

        AxesIncOptionWidget         vMajor;
        AxesIncOptionWidget         vMinor;
        AxesLinePosWidget           vLines;
        AxesTicPosWidget            vTics;

    public:
        void    InitLabeledUI(      char      labText[],
                                    Widget    parent,
                              const ArgListC& inArgs);      // attachment resources + others

        void  InitAxesFormatWidgets();  // parent is form

        void  SetAxesFormat(const PC_AxesFormat& uFormat,
                            const PC_AxesFormat& vFormat);
        void  GetAxesFormat(PC_AxesFormat& uFormat,
                            PC_AxesFormat& vFormat);

        void  SetSensitive(bool inSens);

    private:
        static  void    SetAxesSensitiveCB(void* inObj);
};



class PPD_3DAxesFormatPS : public MenuBaseC {
    private:
        PenOptionUIC            axesLinePenUI;
        PenOptionUIC            majorPenUI;
        PenOptionUIC            minorPenUI;

        BaseUIC                 formatFrame;
        FormWidget              topRowForm;
        LabelWidget             uMajTitleLabel;
        LabelWidget             uMinTitleLabel;
        LabelWidget             uLineTitleLabel;
        LabelWidget             uTicTitleLabel;
        LabelWidget             vMajTitleLabel;
        LabelWidget             vMinTitleLabel;
        LabelWidget             vLineTitleLabel;
        LabelWidget             vTicTitleLabel;

        AxesFormatUIC           axesFormatData[PC_3DAxesFormat::naxes3DPlanes];
        ToggleBoxWidget         autoFormatUI;

        BaseUIC                 miscForm;
        BaseUIC                 axesFrame;
        IntEntryUIC             axesOffset;
        LineThickOptionUIC      axesThk;

        BaseUIC                 majorTicFrame;
        IntEntryUIC             majorTicLen;
        LineThickOptionUIC      majorTicThk;

        BaseUIC                 minorTicFrame;
        LineThickOptionUIC      minorTicThk;
        IntEntryUIC             minorTicLen;

        BaseUIC                 gridFrame;
        LineThickOptionUIC      majorGridThk;
        LineTypeOptionUIC       majorGridType;
        LineThickOptionUIC      minorGridThk;
        LineTypeOptionUIC       minorGridType;

    public:
                            PPD_3DAxesFormatPS() {};

        void                InitMenu();
        void                MenuOpen(PlotDefC&  inPD);
        void                SetMenuData(const PC_3DAxesFormat&  inData);
        void                SetMenuData(const PC_3DAxesFormat&  inData,
                                                MenuObjC& inObj);
        bool                UIdataOK();
        void                GetMenuData(PC_3DAxesFormat& outData,
                                        MenuObjC&       outObj);

        void                MenuDefault();
        void                MenuClear();

    private:
        void            SetAllSensitive();
        static  void    SetAllSensitiveCB(void* inObj);

};

#endif // !PPD_3DAXESFORMATPS_H

