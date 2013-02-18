///////////////////////////////////////////////////////////////////////////////////
//
// PPD_BasePS.h
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
//      components common to all plots (window size)
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PPD_BASEPS_H
#define PPD_BASEPS_H

#include <genApp/U_MenuPPO.h>
#include <genApp/C_MenuBase.h>
#include "PPD_Base.h"


class PPD_BasePS : public MenuBaseC {
    private:
        PenOptionUIC        backgroundPen;

        BaseUIC             dimFrame;
        ButtonBoxUIC        sizeSetButtons;
        SplitFormUIC        sizeForm;
        IntEntryUIC         plotUsizeUI;
        IntEntryUIC         plotVsizeUI;

        IntEntryUIC         rightMarginUI;
        IntEntryUIC         leftMarginUI;
        IntEntryUIC         topMarginUI;
        IntEntryUIC         bottomMarginUI;

    protected:
        SplitFormUIC        marginFrame;             // right side of contents of lastRow form
        FormWidget          lastRowForm;             // bottom form of menu, to attach next layer to
        FuncObjRefUIC       penSource;

    public:
                        PPD_BasePS();

        void            InitMenu();

        void            MenuOpen(PPD_Base&  inData);

        void            SetMenuData(PPD_Base&   inData);
        bool            UIdataOK();
        void            GetMenuData(PPD_Base&   outData);

        void            MenuDefault();

    protected:
        virtual void    ResetPen();

    private:
        static void     ResetPenCB(void* inObj);

        static void     SetUVSizeCB(void* inObj);
        void            SetUVSize();

};

#endif // !PPD_BASEPS_H

