///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CreateBEETResponseFunctionPS.h
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

#ifndef DPO_CREATEBEETRESPONSEFUNCTIONPS_H
#define DPO_CREATEBEETRESPONSEFUNCTIONPS_H

#include <genApp/U_MenuDPO.h>
#include <genApp/C_MenuBase.h>

#include "DPO_CreateBEETResponseFunction.h"

class DPO_CreateBEETResponseFunctionPS : public MenuBaseC {

    friend class DPO_CreateBEETResponseFunction;

    private:
        FuncObjRefUIC           pressureInputObjRefUI;

        BaseUIC                 baroFrame;
        ToggleBoxWidget         calcBEUI;
        FuncObjRefUIC           barometricInputObjRefUI;

        BaseUIC                 etFrame;
        ToggleBoxWidget         calcETUI;
        FuncObjRefUIC           earthTideInputObjRefUI;

        RealMSSpecUI            maxTimeDelayMSUI;

        FormWidget              timeBaseRow;
        OptionUIC               timeBaseUI;
        RealMSSpecUI            specTimeSpacingMSUI;

        BaseUIC                 options;
        OptionUIC               convertpressurePUI;

        BaseUIC                 statusFrame;
        RealStatusUIC           timeSpacingStatus;
        IntStatusUIC            memoryStatus;


    public:
                        DPO_CreateBEETResponseFunctionPS();

                void    InitMenu();

                void    SetMenuData(DPO_CreateBEETResponseFunction& inData);
                bool    UIdataOK();
                void    GetMenuData(DPO_CreateBEETResponseFunction&  outData);

                void    MenuOpen(DPO_CreateBEETResponseFunction&    inData);

    private:
        void            SetAllSensitive();
        static  void    SetAllSensitiveCB(void* inObj);

        void            ToggleExposedMaxTimeDelayValue();
        static  void    ToggleExposedMaxTimeDelayValueCB(void* inObj);
        void            ToggleExposedTimeSpacingValue();
        static  void    ToggleExposedTimeSpacingValueCB(void* inObj);

};
#endif //DPO_CREATEBEETRESPONSEFUNCTIONPS_H

