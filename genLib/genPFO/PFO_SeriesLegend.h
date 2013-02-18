///////////////////////////////////////////////////////////////////////////////////
//
// PFO_SeriesLegend.h
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
//      creates legend box showing line types and symbols with legend.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_SERIESLEGEND_H
#define PFO_SERIESLEGEND_H

#include <genPlotClass/DO_SeriesLegendData.h>
#include <genPlotClass/C_AnnoObj.h>


class PFO_SeriesLegend : public AnnoObjC {
    public:

        class SeriesLegend {
            public:
                FuncObjRef              seriesObjRef;
                bool                    doSeries;
                DO_SeriesLegendData*    dataSource;

            public:
                            SeriesLegend() {doSeries = false; dataSource = 0;}
                            SeriesLegend(const SeriesLegend& a) {seriesObjRef = a.seriesObjRef;
                                                       doSeries = a.doSeries;
                                                       dataSource = 0;}

                SeriesLegend&   operator= (const SeriesLegend& a);

        };

        enum        {maxSeriesLegend = 12};

        //  input objects
        SeriesLegend        legendData[maxSeriesLegend];
        double              seriesSize;
        int                 seriesNpts;

    public:
                        PFO_SeriesLegend(PlotDefC& assPlt);
                        PFO_SeriesLegend(const PFO_SeriesLegend& a, PlotDefC& assPlt);
                        ~PFO_SeriesLegend();

        PFO_SeriesLegend& operator= (const PFO_SeriesLegend& a);

        void            SetDefault();

                        //  FuncObjC virtuals
        void            DoStatusChk();
                        //  PlotObjC  draw virtuals in derived PFO_SeriesLegendGL

    protected:
        void            FullCopy(const PFO_SeriesLegend& a);  // called by copy in derived class

    private:
        void            InitCommon();                         // common to normal and copy constructor
        void            LocalCopy(const PFO_SeriesLegend& a);  // common to copy constructor and = operator
};

#endif // !PFO_SERIESLEGEND_H

