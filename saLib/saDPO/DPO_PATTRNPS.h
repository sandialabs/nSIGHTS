///////////////////////////////////////////////////////////////////////////////////
//
// DPO_PATTRNPS.h
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DPO_PATTRNPS_H
#define DPO_PATTRNPS_H

#include <genApp/U_Menu.h>
#include <genApp/C_MenuBase.h>
#include <genApp/U_MenuPPO.h>

#include "DPO_SAObjBasePS.h"
#include "DPO_PATTRN.h"

class DPO_PATTRNPS : public DPO_SAObjBasePS {
    private:

        TextEntryUIC        obsStrUI;

        BaseUIC             testFrame;
        FormWidget          testForm[4];
        ToggleBoxWidget     testsUI[DFO_PATTRN::ptLastTest];

        FormWidget          parForm1;
        IntEntryUIC         nreplicatesUI;
        IntEntryUIC         nxintUI;
        IntEntryUIC         nyintUI;
        IntEntryUIC         randSeedUI;

        FormWidget          parForm2;
        IntEntryUIC         nrandomPatternsUI;
        RealEntryUIC        alphaCutoffUI;
        OptionUIC           mtest1UI;
        BaseUIC             optionForm;
        ToggleBoxWidget     logarithmsUI;

        BaseUIC             outputFrame;
        ToggleBoxWidget     verboseUI;
        ToggleBoxWidget     verboseMCUI;
        ToggleBoxWidget     verboseCVUI;
        ToggleBoxWidget     coefficientsUI;
        ToggleBoxWidget     rankCoefficientsUI;
        TextEntryUIC        mcRepStrUI;

        RealFormatUIC       coeffFormatUI;

        SC_BoolArray        testSel;

    public:
                        DPO_PATTRNPS() {};

                void    InitMenu();
                void    MenuOpen(DPO_PATTRN&    inData);

                void    SetMenuData(DPO_PATTRN& inData);
                bool    UIdataOK();
                void    GetMenuData(DPO_PATTRN&  outData);

    private:
        void            SetAllSensitive();
        static  void    SetAllSensitiveCB(void* inObj);

        void            SetSel();

        bool            NreplicatesSel();
        bool            NxintSel();
        bool            NyintSel();
        bool            MonteCarloSel();
        bool            LogarithmsSel();
        bool            AlphaCutoffSel();

};


#endif // !DPO_PATTRNPS_H

