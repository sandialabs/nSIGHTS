///////////////////////////////////////////////////////////////////////////////////
//
// DPO_DerivativePS.h
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

#ifndef DPO_DERIVATIVEPS_H
#define DPO_DERIVATIVEPS_H

#include <genApp/U_MenuDPO.h>
#include <genApp/C_MenuBase.h>

#include "DPO_Derivative.h"

class DPO_DerivativePS : public MenuBaseC {

    friend class DPO_Derivative;

    private:
        FuncObjRefUIC           inputTPObjRefUI;
        MasterSlaveUIC          masterObjRefUI;

        FormWidget              selectForm;
        OptionUIC               derivativeTypeUI;
        OptionUIC               derivativeCalcTypeUI;
        OptionUIC               windowCalcTypeUI;

        FormWidget              valueForm;
        IntEntryUIC             windowPtsSpanUI;
        RealEntryUIC            logEpsUI;
        RealEntryUIC            offsetYUI;

        RealMSSpecUI            windowPctSpanMSUI;
        RealMSSpecUI            windowValSpanMSUI;

//        RadioBoxUIC             timeMultTypeUI;

        BaseUIC                 timeProcessUIC;
        ToggleBoxWidget         useSuperTimeUI;
        FuncObjRefUIC           timeProcessObjRefUI;

        BaseUIC                 optionFrame;
        ToggleBoxWidget         absOutputYUI;

    public:
                        DPO_DerivativePS();

                void    InitMenu();

                void    SetMenuData(DPO_Derivative& inData);
                bool    UIdataOK();
                void    GetMenuData(DPO_Derivative&  outData);

                void    MenuOpen(DPO_Derivative&    inData);

    private:
        void            SetAllSensitive();
        static  void    SetAllSensitiveCB(void* inObj);

        void            ToggleExposedWindowPctSpanValue();
        static  void    ToggleExposedWindowPctSpanValueCB(void* inObj);
        void            ToggleExposedWindowValSpanValue();
        static  void    ToggleExposedWindowValSpanValueCB(void* inObj);

};
#endif //DPO_DERIVATIVEPS_H

