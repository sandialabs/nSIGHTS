///////////////////////////////////////////////////////////////////////////////////
//
// PPD_3DAxesLabelPS.h
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
//      axes and increment labeling for 3D plot
//
//      derived from PPD_Base.  Adds components for all 3D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PPD_3DAXESLABELPS_H
#define PPD_3DAXESLABELPS_H

#include <genPlotClass/PC_3DAxesLabel.h>

#include <genApp/C_MenuBase.h>
#include <genApp/U_MenuPPO.h>

class PPD_3DAxesLabelPS : public MenuBaseC {
    private:
        BaseUIC             topFrame;
        ToggleBoxWidget     plotAxesLabelUI;
        ToggleBoxWidget     plotIncrementLabelUI;
        ToggleBoxWidget     autoPositionLabelsUI;

        BaseUIC             labelFrame;
        TextEntryUIC        xlabelField;
        TextEntryUIC        ylabelField;
        TextEntryUIC        zlabelField;

        FontUIC                 labelFont;
        FontUIC                 incrementFont;

        BaseUIC                  miscForm;
        FormWidget               xyForm;
        LabelWidget              xyTitleLabel;
        BoolOptionChooserWidget  xyLabIsVertUI;
        BoolOptionChooserWidget  xLabYPosIsMaxUI;
        BoolOptionChooserWidget  yLabXPosIsMaxUI;

        FormWidget               zForm;
        LabelWidget              zTitleLabel;
        OptionChooserWidget      zLabOrientationUI;
        BoolOptionChooserWidget  zLabYPosIsMaxUI;
        BoolOptionChooserWidget  zLabXPosIsMaxUI;

    public:
                        PPD_3DAxesLabelPS() {};

        void            InitMenu();
        void            SetMenuData(const PC_3DAxesLabel&   inData);
        void            SetMenuData(const PC_3DAxesLabel&   inData,
                                          MenuObjC&         inObj);
        bool            UIdataOK();
        void            GetMenuData(PC_3DAxesLabel& outData,
                                    MenuObjC&       outObj);
        void            MenuDefault();

    private:
        void            SetAllSensitive();
        static  void    SetAllSensitiveCB(void* inObj);

};

#endif // !PPD_3DAXESLABELPS_H

