///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ExtendedProfileContourPS.h
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
//      File I/O and user interface for all nsPFOGL functional objects
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PPO_GRIDCONTOURPS_H
#define PPO_GRIDCONTOURPS_H

#include <genPPO/PPO_ContourSpecPS.h>
#include <genPPO/genPPOUtil.h>
#include <objPPO/objPPOUtil.h>
#include <nsPPO/nsPPOUtil.h>

#include "PPO_ExtendedProfileContour.h"

class PPO_ExtendedProfileContourPS : public PPO_ContourSpecPS {
    private:
        ExtendedProfileDataUIC  extendedProfileDataUI;
        CoordMapUIC             coordMapUI;

        BaseUIC                 optionFrame;
        ToggleBoxWidget         overrideMasterUI;

        FormWidget              extrusionRow;
        BoolRadioBoxUIC         plotAsTubesUI;          // 3D only
        TubeExtrusionSpecUIC    tubeDataUI;
        bool                        is3D;

    public:
                        PPO_ExtendedProfileContourPS();
                        ~PPO_ExtendedProfileContourPS(){};

                void    InitMenu(const PPO_ExtendedProfileContour& inData);

                void    MenuOpen(PPO_ExtendedProfileContour&    inData);

                void    SetMenuData(PPO_ExtendedProfileContour& inData);
                bool    UIdataOK();
                void    GetMenuData(PPO_ExtendedProfileContour& outData);

    private:
                void    OverrideMasterPen();
        static  void    OverrideMasterPenCB(void* inObj);

        static  void    SetAllSensitiveCB(void* inObj);
                void    SetAllSensitive();

};

#endif // PPO_GRIDCONTOURPS_H

