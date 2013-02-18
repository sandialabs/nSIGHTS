///////////////////////////////////////////////////////////////////////////////////
//
// PFO_Scatterplot.h
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

#ifndef PFO_SCATTERPLOT_H
#define PFO_SCATTERPLOT_H

#include <genClass/C_Common.h>

#include <genPlotClass/PSC_SeriesSpec.h>
#include <genPlotClass/PSC_LegendOverride.h>
#include <genPlotClass/DO_SeriesLegendData.h>

#include <genPlotClass/C_PlotObj.h>

#include <saClass/DO_SAPlotTable.h>

class PFO_Scatterplot : public PlotObjC {
    public:
    protected:

        //  input objects
        FuncObjRef          scatterDataObjRef;
        FuncObjRef          xgridLineObjRef;
        FuncObjRef          ygridLineObjRef;

        bool                plotXGridLines;
        bool                plotYGridLines;
        bool                plotAxesLabels;

        int                 plotDVIndx;
        int                 plotIVIndx;

        //  properties
        PSC_SymbolSpec      scatterSpec;

        //  input associated data
        const DC_TableData* scatterDataDC;
        const DC_TableData* xGridLinesDC;
        const DC_TableData* yGridLinesDC;

        const char*         dvLabel;
        const char*         ivLabel;

        int                 nplotDV;
        int                 nplotIV;

    private:
        // output objects
        DO_SeriesLegendData seriesLegendData;    // output for legend

    public:
                        PFO_Scatterplot(PlotDefC& assPlt);
                        PFO_Scatterplot(const PFO_Scatterplot& a, PlotDefC& assPlt);
                        ~PFO_Scatterplot();

        PFO_Scatterplot& operator= (const PFO_Scatterplot& a);

        void            SetDefault();

                        //  FuncObjC virtuals
        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

                        //  PlotObjC  draw virtuals in derived PFO_ScatterplotGL
        Limit3D         GetPlotObjLimits();

    private:
        void            InitCommon();                         // common to normal and copy constructor
        void            LocalCopy(const PFO_Scatterplot& a);  // common to copy constructor and = operator
};

#endif // !PFO_SCATTERPLOT_H

