///////////////////////////////////////////////////////////////////////////////////
//
// DPO_STEPWISEPS.h
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

#ifndef DPO_STEPWISEPS_H
#define DPO_STEPWISEPS_H

#include <genApp/U_Menu.h>
#include <genApp/C_MenuBase.h>

#include "DPO_SAObjBasePS.h"
#include "DPO_STEPWISE.h"

class DPO_STEPWISEPS : public DPO_SAObjBasePS {
    private:
        BaseUIC             optionFrame;
        FormWidget          optRow1Form;
        ToggleBoxWidget     optBACKWARDUI;
        ToggleBoxWidget     optPRESSUI;
        ToggleBoxWidget     optRANKUI;
        ToggleBoxWidget     optSTAND01UI;
        FormWidget          optRow2Form;
        ToggleBoxWidget     optSTEPWISEUI;
        ToggleBoxWidget     optFORCEUI;
        ToggleBoxWidget     optWEIGHTUI;
        ToggleBoxWidget     optDROPUI;

        FormWidget          parameterForm;
        RealEntryUIC        optSiginUI;
        RealEntryUIC        optSigoutUI;
        OptionUIC           decFormatUI;
        OptionUIC           sciFormatUI;

        BaseUIC             outputFrame;
        FormWidget          outRow1Form;
        ToggleBoxWidget     outMEANUI;
        ToggleBoxWidget     outCORRUI;
        ToggleBoxWidget     outSSCPUI;
        ToggleBoxWidget     outINVERSEUI;
        FormWidget          outRow2Form;
        ToggleBoxWidget     outSTEPSUI;
        ToggleBoxWidget     outRESIDUALSUI;
        ToggleBoxWidget     outOTHERUI;

        FormWidget          selectForm;
        BaseUIC             forceFrame;
        BaseUIC             dropFrame;
        BaseUIC             weightFrame;

        ScrolledListWidget  forceIndexesUI;
        ScrolledListWidget  weightIndexUI;
        ScrolledListWidget  dropIndexesUI;

        bool                listDataOK;

    public:
                        DPO_STEPWISEPS();

                void    InitMenu();
                void    MenuOpen(DPO_STEPWISE&  inData);

                void    SetMenuData(DPO_STEPWISE&   inData);
                bool    UIdataOK();
                void    GetMenuData(DPO_STEPWISE&  outData);

    private:
        void            SetAllSensitive();
        static  void    SetAllSensitiveCB(void* inObj);

        void            SetLists();
        static  void    SetListsCB(void* inObj);
};


#endif // !DPO_STEPWISEPS_H

