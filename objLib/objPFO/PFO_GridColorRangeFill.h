///////////////////////////////////////////////////////////////////////////////////
//
// PFO_GridColorRangeFill.h
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
//    Plots grids as varying colors on a 2D or 3D plot.
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_GRIDCOLORRANGEFILL_H
#define PFO_GRIDCOLORRANGEFILL_H

#include <genPlotClass/PSC_EdgeSpec.h>
#include <genClass/SC_RealFormat.h>

#include <objPFO/PFO_GridColorBase.h>

class PFO_GridColorRangeFill : public PFO_GridColorBase {
  public:
      //  input objects
      PSC_EdgeSpec    edgeSpec;           // block edges
      SC_RealFormat   reportFormat;

  protected:
       //  output data
      SC_CoordArray   assocCoord;
      SC_IntArray     assocNodes;

  private:
      //  for limit checks
      bool            xLin, yLin, zLin;
      Limit3D         triLimits;

  public:
                      PFO_GridColorRangeFill(PlotDefC& assPlt);
                      PFO_GridColorRangeFill(const PFO_GridColorRangeFill& a, PlotDefC& assPlt);

                      ~PFO_GridColorRangeFill();

      PFO_GridColorRangeFill&    operator= (const PFO_GridColorRangeFill& a);

                      //  FuncObjC virtuals
      void            CalcOutput(FOcalcType  calcType);

      // transparency virtuals
      virtual bool    SupportsTransparency() const;
      virtual int     GetnTransObjects() const;
      virtual Coord3D GetTransObjectCoord(int objIndex);
      virtual void    SetupForGetCoord();

  protected:
      bool            TriLimitsOK(int triIndex);
  private:

      void            LocalCopy(const PFO_GridColorRangeFill& a);  // common to copy constructor and = operator
};

#endif // !PFO_GRIDCOLORRANGEFILL_H

