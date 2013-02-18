///////////////////////////////////////////////////////////////////////////////////
//
// PFO_EnterXYOnPlot.h
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
//    Allows entry/editing of XY data with a mouse.
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_ENTERXYONPLOT_H
#define PFO_ENTERXYONPLOT_H

#include <genClass/C_Common.h>

#include <genClass/DO_XYData.h>
#include <genClass/DO_Real.h>

#include <genPlotClass/PSC_SymbolSpec.h>
#include <genPlotClass/C_ActiveObj.h>


class PFO_EnterXYOnPlot : public ActiveObjC {

  private:
      DO_XYData           xyDataDO;
      bool                useInputXY;     // kluge -- permanently set to false

  protected:
      DO_Real             lastSelectedXDO;
      DO_Real             lastSelectedYDO;

      enum                        PointStatusType {pstCurrent, pstEntered, pstDeleted};
      T_SC_Array<PointStatusType> pointStatus;

      DC_XYData           xyDataDC;         // current XY data

      PSC_SymbolSpec      currentPtSymbol;
      PSC_SymbolSpec      enteredPtSymbol;
      PSC_SymbolSpec      deletedPtSymbol;

      int                 currLinePen;

  public:
      FuncObjRef          inputXYObjRef;  // only used in UI --


  public:
                          PFO_EnterXYOnPlot(PlotDefC& assPlt);
                          PFO_EnterXYOnPlot(const PFO_EnterXYOnPlot& a, PlotDefC& assPlt);
                          ~PFO_EnterXYOnPlot();

      PFO_EnterXYOnPlot&  operator= (const PFO_EnterXYOnPlot& a);

                          //  FuncObjC virtuals
      void                DoStatusChk();
      void                CalcOutput(FOcalcType  calcType);

                          //  PlotObjC virtuals
      Limit3D             GetPlotObjLimits();

      void                UpdateFrom(const DC_XYData& newXY);

  protected:
      void                DeleteType(PointStatusType typeToDelete);

  private:
      void                InitCommon();                       // common to normal and copy constructor
      void                LocalCopy(const PFO_EnterXYOnPlot& a);  // common to copy constructor and = operator




};

#endif // !PFO_ENTERXYONPLOT_H

