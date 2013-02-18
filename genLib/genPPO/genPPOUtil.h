///////////////////////////////////////////////////////////////////////////////////
//
// genPPOUtil.h
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

#ifndef GENPPOUTIL_H
#define GENPPOUTIL_H

#include <genApp/U_Menu.h>
#include <genApp/U_MenuPPO.h>
#include <genApp/U_MenuDPO.h>

#include <genPlotClass/C_PlotObj.h>
#include <genPlotClass/C_AnnoObj.h>

#include <genPlotClass/PSC_ColorLegendSpec.h>
#include <genPlotClass/PSC_AnnoSpec.h>
#include <genPlotClass/PSC_XYLabelSpec.h>
#include <genPlotClass/PSC_XYZLabelSpec.h>
#include <genPlotClass/PSC_ArrowSpec.h>
#include <genPlotClass/PSC_ExtrusionSpec.h>
#include <genPlotClass/PSC_Histogram.h>


namespace genPPOMenuUtilities {

    class XYLabelSpecUIC  {
        public:
            FormWidget          topRowForm;
            FuncObjRefUIC       dataSource;
            PenOptionUIC        labelPenUI;

            FormWidget          secondRowForm;
            HAlignOptionUIC     horizontalPosUI;
            VAlignOptionUIC     verticalPosUI;

            FontUIC             labelFontUI;

            BaseUIC             optionsFrame;
            IntEntryUIC         pixelOffsetUI;
            ToggleBoxWidget     clipToAxesUI;

        public:
                            XYLabelSpecUIC(const type_info& labType);

            void            StdInit(Widget     parent,
                                    ArgListC&  attachments);

            bool            UIdataOK(bool& allOK);

            void            MenuOpen(PlotObjC&  inObj);

            void            SetXYLabelSpec(const PSC_XYLabelSpec& inData);
            void            GetXYLabelSpec(PSC_XYLabelSpec& outData);

    };

    class XYZLabelSpecUIC  {
        public:
            FormWidget          topRowForm;
            FuncObjRefUIC       dataSource;
            PenOptionUIC        labelPenUI;

            FormWidget          secondRowForm;
            RadioBoxUIC         labelAttitudeUI;
            BaseUIC             rotationFrame;
            ToggleBoxWidget     autoLabelRotationUI;
            RealEntryWidget     rotationAngleUI;

            FormWidget          thirdRowForm;
            HAlignOptionUIC     horizontalPosUI;
            VAlignOptionUIC     verticalPosUI;

            FontUIC             labelFontUI;

            BaseUIC             optionsFrame;
            ToggleBoxWidget     clipToAxesUI;


        public:
                            XYZLabelSpecUIC(const type_info& labType);

            void            StdInit(Widget     parent,
                                    ArgListC&  attachments);

            bool            UIdataOK(bool& allOK);

            void            MenuOpen(PlotObjC&  inObj);

            void            SetXYZLabelSpec(const PSC_XYZLabelSpec& inData);
            void            GetXYZLabelSpec(PSC_XYZLabelSpec& outData);

            void            SetSensitive();
        private:
            static  void    SetSensitiveCB(void* inObj);
    };


    class ColorLegendSpecUIC  {
        public:
            FormWidget          topRowForm;
            FuncObjRefUIC       dataSource;
            OptionUIC           orientationUI;
            PenOptionUIC        legendPenUI;

            BaseUIC             locSizeFrame;
            SplitFormUIC        locSizeSplit;
            RealEntryUIC        xField;
            RealEntryUIC        yField;
            RealEntryUIC        lengthField;
            RealEntryUIC        aspectField;

            BaseUIC             formatFrame;
            ToggleBoxWidget     ticToggle;
            ToggleBoxWidget     opaqueToggle;
            ToggleBoxWidget     frameToggle;
            LineThickOptionUIC  frameThk;

            FontUIC             legendFontUI;

            FormWidget          labelIncForm;
            BaseUIC             labelFrame;
            ToggleBoxWidget     labelToggle;
            ToggleBoxWidget     labelLeftToggle;
            ToggleBoxWidget     defaultToggle;
            TextEntryUIC        labField;

            BaseUIC             incFrame;
            ToggleBoxWidget     incLeftToggle;

        public:
                            ColorLegendSpecUIC();

            void            StdInit(Widget     parent,
                                    ArgListC&  attachments);

            bool            UIdataOK(bool& allOK);

            void            MenuOpen(AnnoObjC&  inObj, PSC_ColorLegendSpec& inData);

            void            SetColorLegendSpec(const PSC_ColorLegendSpec& inData, const PSC_AnnoSpec& inAnno);
            void            GetColorLegendSpec(PSC_ColorLegendSpec& outData, PSC_AnnoSpec& outAnno);

            void            SetSensitive();
        private:
            static  void    SetSensitiveCB(void* inObj);
    };

    class ArrowSpecUIC  {
        public:
            BaseUIC                 specFrame;
            ToggleBoxWidget         plotHeadsUI;
            ToggleBoxWidget         fillHeadsUI;
            LineThickOptionUIC      lineThkUI;
            SymbolSizeOptionUIC     headSizeUI;

        public:
                            ArrowSpecUIC();

            void            StdInit(Widget     parent,
                                    ArgListC&  attachments);

            bool            UIdataOK(bool& allOK);

            void            SetArrowSpec(const PSC_ArrowSpec& inData);
            void            GetArrowSpec(PSC_ArrowSpec& outData);

            void            SetSensitive(bool inSens);
        private:
            static  void    SetSensitiveCB(void* inObj);
    };

    class HistogramUIC  {
        public:
            FormWidget              posRow;
            BoolRadioBoxUIC         orientationXUI;
            BoolRadioBoxUIC         positionIsValueUI;
            RealEntryUIC            indexOffsetUI;

            FormWidget              baselineRow;
            BoolRadioBoxUIC         baselineIsValueUI;
            RealEntryUIC            baselineValueUI;
            RadioBoxUIC             widthSpecUI;

            FormWidget              widthRow;
            LineThickOptionUIC      widthLineThkUI;
            RealEntryUIC            widthSpecValueUI;
            SymbolSizeOptionUIC     widthSpecPixelUI;
            RealEntryUIC            widthSpecPctUI;
            PSC_EdgeSpecUIC         edgeSpecUI;

        public:
                            HistogramUIC();

            void            StdInit(Widget     parent,
                                    ArgListC&  attachments);

            bool            UIdataOK(bool& allOK);

            void            MenuOpen(PlotObjC&  inObj,
                                     PSC_Histogram& inData);

            void            SetHistogram(const PSC_Histogram& inData);
            void            GetHistogram(PSC_Histogram& outData);

            void            SetSensitive();
        private:
            static  void    SetSensitiveCB(void* inObj);
    };


    class TubeExtrusionSpecUIC  {
        public:
            BaseUIC                 specFrame;
            ToggleBoxWidget         capEndsUI;
            TubePointsOptionWidget  nextrusionPointsUI;
            SymbolSizeOptionWidget  extrusionPixelSizeUI;

        public:
                            TubeExtrusionSpecUIC();

            void            StdInit(Widget     parent,
                                    ArgListC&  attachments);

            bool            UIdataOK(bool& allOK);

            void            SetTubeExtrusionSpec(const PSC_TubeExtrusionSpec& inData);
            void            GetTubeExtrusionSpec(PSC_TubeExtrusionSpec& outData);

            void            SetSensitive(bool inSens);
    };



}

using namespace genPPOMenuUtilities;


#endif // !GENPPOUTIL_H

