///////////////////////////////////////////////////////////////////////////////////
//
// PPO_SingleConfLimitsPS.h
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
//      File I/O and user interface for all osPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PPO_SINGLECONFLIMITSPS_H
#define PPO_SINGLECONFLIMITSPS_H

#include <genPPO/PPO_ColorBasePS.h>

#include "PPO_SingleConfLimits.h"

class PPO_SingleConfLimitsPS : public PPO_ColorBasePS {
    private:

        FormWidget                  topRow;
        //FuncObjRefUIC               covarArrayObjRefUI;  with ColorBase now inputDataObjRefUI

        FormWidget                  indexRow;
        BoolRadioBoxUIC             allParamUI;
        ComboBoxSelectionUIC        xDataIndxUI;

        BaseUIC                     lineFrame;
        OptionUIC                   cLimitsUI;
        ToggleBoxUIC                plotConfLimitsUI;
        LineThickOptionUIC          covarLineThkUI;

        BaseUIC                     pointFrame;
        ToggleBoxUIC                plotBestEstimateUI;
        SymbolTypeOptionWidget      pointTypeUI;
        SymbolSizeOptionWidget      pointSizeUI;

        FormWidget                  widthRow;
        RadioBoxUIC                 widthSpecUI;
        RealEntryUIC                widthSpecValueUI;
        SymbolSizeOptionUIC         widthSpecPixelUI;
        RealEntryUIC                widthSpecPctUI;

        FormWidget                  colorForm;
        BoolRadioBoxUIC             plotWithColorMapUI;
        PenOptionUIC                fixedColorPenUI;
        BoolRadioBoxUIC             colorMapValueUI;
        //FuncObjRefUIC             colorMapDataObjRefUI;

        PSC_LegendOverrideUIC       legendOverrideUI;

        bool                        covarDataOK;

    public:
                        PPO_SingleConfLimitsPS();
                        ~PPO_SingleConfLimitsPS(){};

                void    InitMenu(const PPO_SingleConfLimits& inData);

                void    MenuOpen(PPO_SingleConfLimits& inData);

                void    SetMenuData(PPO_SingleConfLimits&  inData);
                bool    UIdataOK();
                void    GetMenuData(PPO_SingleConfLimits&          outData);

    private:
        static  void    SetAllSensitiveCB(void* inObj);
                void    SetAllSensitive();
        static  void    SetCovarIndexCB(void* inObj);
                void    SetCovarIndex();
};


#endif // !PPO_SINGLECONFLIMITSPS_H

