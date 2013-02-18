///////////////////////////////////////////////////////////////////////////////////
//
// UPO_WriteCurveArrayPS.h
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
//      File I/O and user-interface for objUFO.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef UPO_WriteCurveArrayPS_H
#define UPO_WriteCurveArrayPS_H

#include <genApp/C_MenuBase.h>

#include "UPO_WriteCurveArray.h"


class UPO_WriteCurveArrayPS : public MenuBaseC {
    private:

        class CurveDataUIC {
        public:
            FuncObjRefUIC               curveObjRefUI;
            ToggleBoxWidget             doCurveUI;
            TextEntryWidget             curveIDUI;
            FormWidget                  outsideForm;
            HorizontalSeparator         hSep;

                        CurveDataUIC();

            void        InitCurveData();  // parent is outsideForm
            void        SetCurveData(const UFO_WriteCurveArray::CurveData& curveData);
            bool        CurveDataOK(bool& prevOK);
            void        GetCurveData(UFO_WriteCurveArray::CurveData& curveData);
            void        SetAllSensitive();
        private:

            static  void    SetAllSensitiveCB(void* inObj);
            void            ObjChange();
            static  void    ObjChangeCB(void* inObj);

        };

        class CurveScrolledArea : public ScrolledForm {
            protected:
                CSize       SetCalculateFormSize();
        };

    private:
            CurveDataUIC            curveDataUI[UFO_WriteCurveArray::maxCurveData];
            CurveScrolledArea       scrollArea;
            FileEntryUIC            outField;

            FormWidget              titleForm;
            LabelWidget             title1;
            LabelWidget             title2;
            HorizontalSeparator     hSep;

    public:
                        UPO_WriteCurveArrayPS();
                        ~UPO_WriteCurveArrayPS(){};

                void    InitMenu();

                void    MenuOpen(UPO_WriteCurveArray&   inData);
                void    MenuClose();

                void    SetMenuData(UPO_WriteCurveArray&    inData);
                bool    UIdataOK();
                void    GetMenuData(UPO_WriteCurveArray&   outData);

                void    MenuClear();


    private:
                void    SetAllSensitive();
};

#endif // !UPO_WriteCurveArrayPS_H

