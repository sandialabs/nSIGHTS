///////////////////////////////////////////////////////////////////////////////////
//
// PFO_SingleConfLimits.h
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
//      Plotting objects for covariance data
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_SINGLECONFLIMITS_H
#define PFO_SINGLECONFLIMITS_H

#include <genClass/C_Common.h>

#include <genClass/DO_XYData.h>
#include <objClass/DO_XYDataArray.h>

#include <genClass/DC_ColorMap.h>

#include <osClass/DC_CovarArray.h>

#include <genPlotClass/PSC_LegendOverride.h>

#include <genPlotClass/DO_SeriesLegendData.h>

#include <genPFO/PFO_ColorBase.h>


class PFO_SingleConfLimits : public PFO_ColorBase  {

    public:
        enum WidthSpec              {wsLine, wsSpecVal, wsSpecPixel, wsPctAvail};

    protected:
        //  input objects
        //FuncObjRef                  covarArrayObjRef;  now inputDataObjRef with ColorBase

        // properties
        int                         xDataIndx;
        bool                        allParameters;

        DC_Covar::ConfidenceLimits  cLimits;

        bool                        plotConfLimits;
        int                         covarLineThk;

        bool                        plotBestEstimate;
        PSC_SymbolSpec              bestEstSymbol;

        WidthSpec                   widthSpec;
        double                      widthSpecValue;
        int                         widthSpecPixel;
        double                      widthSpecPct;

        bool                        plotWithColorMap;
        int                         covarPen;
        //FuncObjRef                    colorMapDataObjRef;
        bool                        colorMapIsIndex;

        PSC_LegendOverride          legendOverride;

        // plotting data
        DC_CovarArray*              covarArrayDC;
        DC_CovarHeader*             covarHeaderDC;

        DC_XYDataArray              covarXYArrayDC; // 2D array of covar points

        //DC_ColorMap*              colorMap;
        SC_DoubleArray              colorData;

        int                         maxParam;
        int                         maxCovar;


    private:
        // output objects
        DO_SeriesLegendData         seriesLegendData;    // output for legend

        DO_XYDataArray              covarXYArrayDO;

    public:
                        PFO_SingleConfLimits(PlotDefC& assPlt);
                        PFO_SingleConfLimits(const PFO_SingleConfLimits& a, PlotDefC& assPlt);
                        ~PFO_SingleConfLimits();

        PFO_SingleConfLimits& operator= (const PFO_SingleConfLimits& a);

                        //  FuncObjC virtuals
        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

                        //  PlotObjC  draw virtuals in derived PFO_SingleConfLimitsGL
        Limit3D         GetPlotObjLimits();

    protected:
        bool            Get2DSingle(int covarIndx,
                                    int paramIndx,
                                    SC_PointArray& val);
        void            FullCopy(const PFO_SingleConfLimits& a);

    private:
        void            InitCommon();                         // common to normal and copy constructor
        void            LocalCopy(const PFO_SingleConfLimits& a);  // common to copy constructor and = operator
};

#endif // !PFO_SINGLECONFLIMITS_H

