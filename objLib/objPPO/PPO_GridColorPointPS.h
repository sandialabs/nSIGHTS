///////////////////////////////////////////////////////////////////////////////////
//
// PPO_GridColorPointPS.h
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
//      File I/O and user interface for all objPFOGL objects
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PPO_GRIDCOLORPOINTPS_H
#define PPO_GRIDCOLORPOINTPS_H

#include <genApp/U_MenuPPO.h>

#include <objPPO/PPO_GridColorBasePS.h>

#include "PPO_GridColorPoint.h"

class PPO_GridColorPointPS : public PPO_GridColorBasePS {
    public:
        BaseUIC                 symFrame;
        SymbolTypeOptionWidget  pointSymbolUI;
        SymbolSizeOptionWidget  pointSymSizeUI;
        PSC_EdgeSpecUIC         edgeSpecUI;

    public:
                        PPO_GridColorPointPS();
                        ~PPO_GridColorPointPS(){};

                void    InitMenu(const PPO_GridColorPoint&  inData);

                void    MenuOpen(PPO_GridColorPoint&    inData);

                void    SetMenuData(PPO_GridColorPoint& inData);
                bool    UIdataOK();
                void    GetMenuData(PPO_GridColorPoint&         outData);

    private:
                void    SetAllSensitive();
        static  void    SetAllSensitiveCB(void* inObj);
};

#endif // !PPO_GRIDCOLORPOINTPS_H

