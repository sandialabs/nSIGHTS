///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ShadeRegion.h
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
//      Basic plotting objects used by all applications, primarily concerned with plot annotation:
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_SHADEREGION_H
#define PFO_SHADEREGION_H

#include <genClass/C_Graphic.h>
#include <genClass/DO_ColorMap.h>
#include <genClass/SC_RealFormat.h>

#include <genDataClass/DSC_RealMSSpecBase.h>

#include <genPlotClass/C_PlotObj.h>
#include <genPlotClass/PSC_Font.h>
#include <genPlotClass/PSC_StippleSpec.h>
#include <genPlotClass/PSC_LineSpec.h>
#include <genPlotClass/PSC_GridLineLabelFormat.h>


class PFO_ShadeRegion : public PlotObjC  {
    public:
        enum                    {maxLabelLen = 60};
        enum RegionSpecType     {rst_None, rst_lessThanMin, rst_betweenMinAndMax, rst_greaterThanMax};

    protected:
        RegionSpecType          xRegionSpec;
        RegionSpecType          yRegionSpec;
        DSC_RealMSSpecBase      regionXMin;
        DSC_RealMSSpecBase      regionXMax;
        DSC_RealMSSpecBase      regionYMin;
        DSC_RealMSSpecBase      regionYMax;

        bool                    shadeRegion;
        bool                    shadeIsColorMap;
        FuncObjRef              colorMapObjRef;       // color map
        int                     shadeCMIndex;
        int                     shadePen;

        bool                    labelRegion;
        int                     labelPen;
        double                  labelXPosition;  // 0 to 100 along X limits
        double                  labelYPosition;  // 0 to 100 along Ylimits
        int                     labelOffset;    // pixels from position
        HorizontalTextAlignment halign;
        VerticalTextAlignment   valign;
        PSC_Font                labelFont;
        char                    regionLabel[maxLabelLen];

        bool                    stippleRegion;
        PSC_StippleSpec         regionStipple;


        bool                    frameRegion;
        PSC_LineSpec            frameLine;

        //  used in GL
        double                  regionXMinVal;
        double                  regionXMaxVal;
        double                  regionYMinVal;
        double                  regionYMaxVal;

        const DC_ColorMap*      colorMap;           // color map

    private:
        bool            checkXMin;
        bool            checkXMax;
        bool            checkYMin;
        bool            checkYMax;

        bool            needXMin;
        bool            needXMax;
        bool            needYMin;
        bool            needYMax;

    public:
                        PFO_ShadeRegion(PlotDefC& assPlt);
                        PFO_ShadeRegion(const PFO_ShadeRegion& a, PlotDefC& assPlt);
                        ~PFO_ShadeRegion();

        PFO_ShadeRegion&   operator= (const PFO_ShadeRegion& a);

                        //  FuncObjC virtuals
        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

                        //  PlotObjC  draw virtuals in derived PFO_ShadeRegionGL
        Limit3D         GetPlotObjLimits();


    private:
        void            InitCommon();                         // common to normal and copy constructor
        void            LocalCopy(const PFO_ShadeRegion& a);  // common to copy constructor and = operator
};

#endif // !PFO_SHADEREGION_H

