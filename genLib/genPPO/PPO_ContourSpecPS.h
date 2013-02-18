///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ContourSpecPS.h
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
//      File I/O and user interface for all genPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PPO_CONTOURSPECPS_H
#define PPO_CONTOURSPECPS_H

#include <genPlotClass/PSC_ContourSpec.h>
#include <genPPO/PPO_BasePS.h>


class PPO_ContourSpecPS : public PPO_BasePS {
    private:
        MasterSlaveUIC              specMS;
        TriSplitFormUIC             contourOptions;
        ToggleBoxWidget             outputCoordsUI;

        BaseUIC                     incFrame;
        BaseUIC                     formatFrame;

        BoolRadioBoxUIC             calcIncValUI;
        BoolRadioBoxUIC             doLogIncUI;
        RealEntryUIC                startValUI;
        RealEntryUIC                endValUI;
        RealEntryUIC                incValUI;
        IntEntryUIC                 nIncsUI;


    protected:
        FormWidget                  topRowForm;  // for split data
        FuncObjRefUIC               dataObjRefUI;
        PSC_LegendOverrideUIC       legendGCUI;  // last UI element for lower attachments
        PenOptionChooserWidget      contourPenUI;
        bool                        overrideMaster; // for override in subclasses
        LineThickOptionWidget       contourLineThkUI;
        LineTypeOptionWidget        contourLineTypeUI;
        ToggleBoxWidget             mitreLinesUI;

    public:
                        PPO_ContourSpecPS(const type_info& dataObjType);
                        ~PPO_ContourSpecPS(){};


                void    InitMenu(const PPO_Base&   inData,
                                       bool        splitTopRow);

                            // doesnt init PPO_Base or top row
                void    InitMenu(const PPO_Base&   inData,
                                        ArgListC&  attachments);


                void    MenuOpen(PSC_ContourSpec&  inContourSpec);

                void    SetMenuData(PPO_Base&  inData,
                                    PSC_ContourSpec&  inContourSpec);
                bool    UIdataOK();
                void    GetMenuData(PPO_Base& outData,
                                    PSC_ContourSpec&  outContourSpec);

    protected:
                void    SetAllSensitive();

    private:
        static  void    SetAllSensitiveCB(void* inObj);
};

#endif // !PPO_CONTOURSPECPS_H

