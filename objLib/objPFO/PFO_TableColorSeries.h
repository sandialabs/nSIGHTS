///////////////////////////////////////////////////////////////////////////////////
//
// PFO_TableColorSeries.h
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
//    Plotting functional objects for cube, grid, tables and XY data.
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_TABLECOLORSERIES_H
#define PFO_TABLECOLORSERIES_H

#include <genClass/C_Common.h>
#include <genClass/DC_TableData.h>

#include <genPFO/PFO_ColorBase.h>
#include <genPlotClass/PSC_EdgeSpec.h>

#include <objPlotClass/PSC_3DCoordMap.h>


class PFO_TableColorSeries : public PFO_ColorBase  {
  protected:

      // properties
      int                 xDataIndx;
      int                 yDataIndx;
      int                 zDataIndx;  //for 3D plots only
      int                 colorDataIndx;

      //sym properties
      PlotSymbol      pointSymbol;
      int             pointSymSize;
      PSC_EdgeSpec    edgeSpec;           // symbol edges

      bool                plotAllRows;
      int                 lastNRows;
      int                 startRow;
      bool                reversePlotOrder;

      // plotting data
      const DC_TableData* objTableData;       // data to plot

      //  output data
      SC_IntArray     symbolColors;

      bool            autoLimitToData;

      PSC_3DCoordMap coordMap;


  public:
                      PFO_TableColorSeries(PlotDefC& assPlt);
                      PFO_TableColorSeries(const PFO_TableColorSeries& a, PlotDefC& assPlt);
                      ~PFO_TableColorSeries();

      PFO_TableColorSeries& operator= (const PFO_TableColorSeries& a);

      void            SetDefault();

                      //  FuncObjC virtuals
      void            DoStatusChk();
      void            CalcOutput(FOcalcType  calcType);

                      //  PlotObjC  draw virtuals in derived PFO_TableColorSeriesGL
      Limit3D         GetPlotObjLimits();
       void           ViewChangeRecalc();
      //bool          ContainsLinesOnly();

protected:

      void            FullCopy(const PFO_TableColorSeries& a);
      void            MapCoord(Coord3D &pointCoord, double x, double y, double z);
      void            CalcDataColors();

private:
      void            InitCommon();                         // common to normal and copy constructor
      void            LocalCopy(const PFO_TableColorSeries& a);  // common to copy constructor and = operator
      void            SetRangeInView();
};

#endif // !PFO_TABLECOLORSERIES_H

