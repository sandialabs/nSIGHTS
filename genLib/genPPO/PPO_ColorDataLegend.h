///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ColorDataLegend.h
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

#ifndef PPO_COLORDATALEGEND_H
#define PPO_COLORDATALEGEND_H

#include "genPPO/PPO_AnnoBase.h"
#include <genPFOGL/PFO_ColorDataLegendGL.h>

class PlotDefC;
class PPO_ColorDataLegendPS;

class PPO_ColorDataLegend : public PPO_AnnoBase, protected PFO_ColorDataLegendGL {
    public:
        friend class PPO_ColorDataLegendPS;

    public:
                        PPO_ColorDataLegend(PlotDefC* assPlt);
                        PPO_ColorDataLegend(PlotDefC* assPlt, const PPO_ColorDataLegend& a);
                        ~PPO_ColorDataLegend(){};

        PPO_ColorDataLegend& operator= (const PPO_ColorDataLegend& a);

        void            PropOpenMenu();
        void            PropUpdate();
        void            PropApply();

        void            WriteToFile();
        bool            ReadFromFile();

    private:
        PPO_ColorDataLegendPS&  MenuRef();

        //  AnnoObjC virtual for UI update
        void            UpdateULUI();

};
#endif // !PPO_COLORDATALEGEND_H

