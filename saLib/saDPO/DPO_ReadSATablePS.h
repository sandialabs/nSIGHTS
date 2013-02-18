///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadSATablePS.h
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

#ifndef DPO_READSATABLEPS_H
#define DPO_READSATABLEPS_H

#include <genApp/U_Menu.h>
#include <genApp/C_MenuBase.h>

#include "DPO_ReadSATable.h"

class DPO_ReadSATablePS : public MenuBaseC {
    private:
        FileEntryUIC        saFnameUI;
        RadioBoxUIC         saFileFormatUI;
        BaseUIC             goldsimIndepVarFrame;
        FormWidget          toggleForm;
        ToggleBoxWidget     translateIndepVarNamesUI;
        ToggleBoxWidget     selectOnlyNewNamesUI;
        ToggleBoxWidget     sortByNamesUI;
        FileEntryUIC        varNameFnameUI;
        TextEntryUIC        goldsimDepVarUI;

        BaseUIC             statusFrame;
        IntStatusUIC        nsteps;
        IntStatusUIC        nvariables;
        IntStatusUIC        nvectors;

    public:
                        DPO_ReadSATablePS() {};

                void    InitMenu();
                void    MenuClose();

                void    SetMenuData(DPO_ReadSATable&    inData);
                bool    UIdataOK();
                void    GetMenuData(DPO_ReadSATable&  outData);

                void    MenuClear();
                void    MenuDefault();

    private:
        void            SetAllSensitive();
        static  void    SetAllSensitiveCB(void* inObj);

};


#endif // !DPO_READSATABLEPS_H

