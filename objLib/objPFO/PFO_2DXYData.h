///////////////////////////////////////////////////////////////////////////////////
//
// PFO_2DXYData.h
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
//    Plots data from one or more XY data sets using symbols and/or lines on a
// 2D XY plot.
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_2DXYDATA_H
#define PFO_2DXYDATA_H

#include <genClass/C_Common.h>
#include <genClass/DO_XYData.h>

#include <genPlotClass/PSC_SeriesSpec.h>
#include <genPlotClass/PSC_LegendOverride.h>
#include <genPlotClass/DO_SeriesLegendData.h>

#include <genPlotClass/C_PlotObj.h>

class PFO_2DXYData : public PlotObjC  {
  public:

      class XYSeries : public PSC_SeriesSpec, public PSC_LegendOverride {
          public:
              bool                doSeries;
              FuncObjRef          xyDataObjRef;
              const DC_XYData*    xyDataDC;

                          XYSeries();
                          XYSeries(const XYSeries& a);
              XYSeries&   operator= (const XYSeries& a);
              bool        OKtoPlot();

          private:
              void         LocalCopy(const XYSeries& a);  // common to copy constructor and = operator

      };

      enum        {maxXYSeries = 6};


  public:
      XYSeries            xySeries[maxXYSeries];
      PC_Report*          secondReport;

  private:
      // output objects
      DO_SeriesLegendData seriesLegendData;    // output for legend

  public:
                      PFO_2DXYData(PlotDefC& assPlt);
                      PFO_2DXYData(const PFO_2DXYData& a, PlotDefC& assPlt);
                      ~PFO_2DXYData();

      PFO_2DXYData& operator= (const PFO_2DXYData& a);

      void            SetDefault();

                      //  FuncObjC virtuals
      void            DoStatusChk();
      void            CalcOutput(FOcalcType  calcType);

                      //  PlotObjC  draw virtuals in derived PFO_2DXYDataGL
      Limit3D         GetPlotObjLimits();
      bool            ContainsLinesOnly();

  protected:
      void            FullCopy(const PFO_2DXYData& a);  // called by copy in derived class
      void            SetReportTitles();

  private:
      void            InitCommon();                         // common to normal and copy constructor
      void            LocalCopy(const PFO_2DXYData& a);  // common to copy constructor and = operator
};

#endif // !PFO_2DXYDATA_H

