///////////////////////////////////////////////////////////////////////////////////
//
// PFO_3DTableSeries.h
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
//    Plots three selected columns for a single table using symbols and/or lines
// on a 3D XYZ plot.
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_3DTABLESERIES_H
#define PFO_3DTABLESERIES_H

#include <genClass/C_Common.h>
#include <genClass/DC_TableData.h>

#include <genPlotClass/PSC_3DSeriesSpec.h>
#include <genPlotClass/PSC_LegendOverride.h>
#include <genPlotClass/DO_SeriesLegendData.h>

#include <genPlotClass/C_PlotObj.h>


class PFO_3DTableSeries : public PlotObjC  {
  protected:
      //  input objects
      FuncObjRef          tableDataObjRef;

      // properties
      int                 xDataIndx;
      int                 yDataIndx;
      int                 zDataIndx;
      PSC_3DSeriesSpec     xyzSeries;
      bool                plotAllRows;
      int                 lastNRows;

      //  used in derived
      int                 startRow;

      PSC_LegendOverride   seriesLabel;

      // plotting data
      const DC_TableData*  objTableData;       // data to plot

  private:
      // output objects
      DO_SeriesLegendData seriesLegendData;    // output for legend

  public:
                      PFO_3DTableSeries(PlotDefC& assPlt);
                      PFO_3DTableSeries(const PFO_3DTableSeries& a, PlotDefC& assPlt);
                      ~PFO_3DTableSeries();

      PFO_3DTableSeries& operator= (const PFO_3DTableSeries& a);

      void            SetDefault();

                      //  FuncObjC virtuals
      void            DoStatusChk();
      void            CalcOutput(FOcalcType  calcType);

                      //  PlotObjC  draw virtuals in derived PFO_3DTableSeriesGL
      Limit3D         GetPlotObjLimits();
      bool            ContainsLinesOnly();

  protected:
      void            FullCopy(const PFO_3DTableSeries& a);  // called by copy in derived class

  private:
      void            InitCommon();                         // common to normal and copy constructor
      void            LocalCopy(const PFO_3DTableSeries& a);  // common to copy constructor and = operator
};

#endif // !PFO_3DTABLESERIES_H

