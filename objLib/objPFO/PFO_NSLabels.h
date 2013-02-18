///////////////////////////////////////////////////////////////////////////////////
//
// PFO_NSLabels.h
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

#ifndef PFO_NSLABELS_H
#define PFO_NSLABELS_H

#include <genClass/C_Common.h>
#include <genClass/DO_LabelArray.h>

#include <genPlotClass/C_PlotObj.h>

class PFO_NSLabels : public PlotObjC  {
  protected:
      //  input object
      FuncObjRef                labelsObjRef;       // labels geometry object
      DO_LabelArray*            labelsObj;

  public:
                      PFO_NSLabels(const char* pltType, PlotDefC& assPlt);
                      PFO_NSLabels(const PFO_NSLabels& a, PlotDefC& assPlt);
                      ~PFO_NSLabels();

      PFO_NSLabels&   operator= (const PFO_NSLabels& a);

                      //  FuncObjC virtuals
      void            DoStatusChk();
      void            CalcOutput(FOcalcType  calcType);

                      //  PlotObjC virtuals
      Limit3D         GetPlotObjLimits();
                      //  PlotObjC  draw virtuals in PS derived

  private:
      void            InitCommon();                       // common to normal and copy constructor
      void            LocalCopy(const PFO_NSLabels& a);  // common to copy constructor and = operator
};

#endif // !PFO_NSLABELS_H

