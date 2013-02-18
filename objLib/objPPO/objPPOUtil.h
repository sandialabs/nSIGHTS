///////////////////////////////////////////////////////////////////////////////////
//
// objPPOUtil.h
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

#ifndef OBJPPOUTIL_H
#define OBJPPOUTIL_H

#include <genApp/U_Menu.h>
#include <genApp/U_MenuPPO.h>
#include <genApp/U_MenuDPO.h>
#include <genPlotClass/C_PlotObj.h>

#include <objPlotClass/PSC_Horsetail.h>
#include <objPlotClass/PSC_3DGridData.h>
#include <objPlotClass/PSC_3DCubeData.h>
#include <objPlotClass/PSC_3DCoordMap.h>


namespace objPPOMenuUtilities {

    class HorsetailUIC  {
        public:
            FormWidget              colorForm;
            BoolRadioBoxUIC         plotWithColorMapUI;
            PenOptionUIC            fixedColorPenUI;
            FuncObjRefUIC           colorMapDataObjRefUI;

            BaseUIC                 formatFrame;
            LineThickOptionWidget   lineThickUI;
            LineTypeOptionWidget    lineTypeUI;

            PSC_LegendOverrideUIC   legendDataUI;

        public:
                            HorsetailUIC();

            void            StdInit(Widget     parent,
                                    ArgListC&  attachments);

            bool            UIdataOK(bool& allOK);

            void            MenuOpen(PlotObjC&  inObj,
                                     PSC_Horsetail& inData);
            void            SetHorsetail(const PSC_Horsetail& inData);
            void            GetHorsetail(PSC_Horsetail& outData);

            void            SetSensitive();
        private:
            static  void    SetSensitiveCB(void* inObj);
    };


    // doesnt include coord map
    class GridDataUIC  {
        public:
            OptionUIC               zvalueSourceUI;
            FuncObjRefUIC           otherGridZObjRefUI;
            RealMSSpecUI            fixedZValueMSUI;

        public:
                            GridDataUIC();

            void            StdInit(Widget          parent,
                                    FuncObjRefUIC&  gridDataUI,
                                    FormWidget&     topRowForm);

            bool            UIdataOK(bool& allOK);

            void            MenuOpen(PlotObjC&  inObj,
                                     PSC_3DGridData& inData);

            void            Set3DGridData(const PSC_3DGridData& inData);
            void            Get3DGridData(PSC_3DGridData& outData);

            void            SetSensitive();
        private:
            static  void    SetSensitiveCB(void* inObj);
    };

    // doesnt include coord map
    class CubeDataUIC  {
        public:
            FuncObjRefUIC       cubeIndexesObjRefUI;
            SC_StringArray      cubeLabels;
        private:
            FuncObjRefUIC*      p_cubeDataUI;
            bool                useIndexes;

        public:
                            CubeDataUIC();

            void            StdInit(Widget          parent,
                                    FuncObjRefUIC&  cubeDataUI,
                                    FormWidget&     topRowForm,
                                    bool            useIndx = true);

            bool            UIdataOK(bool& allOK);

            void            MenuOpen(PlotObjC&       inObj,
                                     PSC_3DCubeData& inData);

            void            Set3DCubeData(const PSC_3DCubeData& inData);
            void            Get3DCubeData(PSC_3DCubeData& outData);

            void            SetSensitive();
            void            ResetLabels();
        private:
            static  void    SetSensitiveCB(void* inObj);
            static  void    ResetLabelsCB(void* inObj);

    };

    class CoordMapUIC  {
        public:
            BaseUIC                 coordMapFrame;
            OptionUIC               xMapUI;
            OptionUIC               yMapUI;
            OptionUIC               zMapUI;

        public:
                            CoordMapUIC() {};

            void            StdInit(Widget          parent,
                                    ArgListC&  attachments);

            void            SetLabels(SC_StringArray& cubeLab);
            void            Set3DCoordMap(const PSC_3DCoordMap& inData);
            void            Get3DCoordMap(PSC_3DCoordMap& outData);

    };


};

using namespace objPPOMenuUtilities;


#endif // !OBJPPOUTIL_H

