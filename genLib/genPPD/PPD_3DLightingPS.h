///////////////////////////////////////////////////////////////////////////////////
//
// PPD_3DLightingPS.h
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
//      lighting specification for 3D plots
//
//      derived from PPD_Base.  Adds components for all 3D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PPD_3DLIGHTINGPS_H
#define PPD_3DLIGHTINGPS_H

#include <genPlotClass/C_PlotDef.h>
#include <genPlotClass/PC_Lighting.h>

#include <genApp/C_MenuBase.h>
#include <genApp/U_MenuPPO.h>

class PenAndSliderUIC : public BaseUIC {
    public:
        PenOptionChooserWidget  penUI;
        EntrySliderUIC          intensityUI;


    public:
        void            StdInit(     char       label[],
                                     Widget     parent,
                               const ArgListC&  inArgs);

        void            SetPenAndSlider(int     penNum,
                                        double  intensityVal);

        void            GetPenAndSlider(int&     penNum,
                                        double&  intensityVal);

        void            SetSensitive(bool isSensitive);

};


class PPD_3DLightingPS : public MenuBaseC {
    private:
        BaseUIC             topFrame;
        ToggleBoxWidget     useLightingUI;
        ToggleBoxWidget     useDefaultLightUI;
        ToggleBoxWidget     showDefaultLightUI;

        PenAndSliderUIC     ambientLightUI;
        PenAndSliderUIC     defaultDiffuseLightUI;
        PenAndSliderUIC     defaultSpecularLightUI;

        FormWidget          defaultLightPosForm;
        BoolRadioBoxUIC     defaultLightIsFixedUI;

        SplitFormUIC        posSplit;
        RealEntryUIC        fixedElevationUI;
        RealEntryUIC        fixedAzimuthUI;
        RealEntryUIC        relativeElevationUI;
        RealEntryUIC        relativeAzimuthUI;
        RealEntryUIC        defaultLightWUI;

    public:
                            PPD_3DLightingPS() {};

        void                InitMenu();
        void                MenuOpen(PlotDefC&  inPD);
        void                SetMenuData(const PC_Lighting&  inData);
        void                SetMenuData(const PC_Lighting&  inData,
                                              MenuObjC&     inObj);
        bool                UIdataOK();
        void                GetMenuData(PC_Lighting& outData,
                                        MenuObjC&    outObj);

        void                MenuDefault();

    private:
        void            SetAllSensitive();
        static  void    SetAllSensitiveCB(void* inObj);

};

#endif // !PPD_3DLIGHTINGPS_H

