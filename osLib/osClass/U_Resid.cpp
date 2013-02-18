///////////////////////////////////////////////////////////////////////////////////
//
// U_Resid.cpp
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
//      basic residual utilities (standardization, normal distribution calculations).
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/C_Common.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>
#include <genClass/SC_Statistics.h>

#include <osClass/U_Resid.h>


namespace osResidualCalcSupport {

   void StandardizeResiduals(SC_DoubleArray& residData)
   {
       SC_Statistics statCalc;
       statCalc.CalcStatistics(residData, false);

       double mean = statCalc.realResults[SC_Statistics::soMean];
       double stdDev = statCalc.realResults[SC_Statistics::soStdDev];

       for (int i = 0; i < residData.Size(); i++)
           residData[i] = (residData[i] - mean) / stdDev;
   }

   double CalcNormal(const double& xVal)
   {
       return exp(-Sqr(xVal)/2.0)/sqrt(2.0 * pi);
    //    calc_norm := EXP(-SQR(x_val)/2.0)/SQRT(2.0 * real_constants.pi);
   }


   static const double normLim = 5.0;

   double CalcCumulativeNormal(const double& xVal)
   {
       const int nnormalPoints = 300;

       double dx = (xVal + normLim) / double(nnormalPoints - 1);
       if (dx < 0.0)
           return 0.0;

       double cnormX = 0.0;
       double x = -normLim;

       for (int i = 0; i < nnormalPoints; i++)
       {
           cnormX += CalcNormal(x) * dx;
           x += dx;
       }

       return cnormX;
   }


   bool StandardNormalDistribution(DC_XYData&    outputDist,
                                   int           ndistPts,
                                   bool          cumulative,
                                   const double& minX,
                                   const double& maxX)
   {
       if (!outputDist.Alloc(ndistPts))
           return false;


       double calcMin = -normLim;
       double calcMax = normLim;
       if (calcMin < minX)
           calcMin = minX;
       if (calcMax > maxX)
           calcMax = maxX;
       double cnormX;
       if (cumulative)
           cnormX = CalcCumulativeNormal(calcMin);


       double dx = (calcMax - calcMin) / double(ndistPts - 1);
       double x = calcMin;

       for (int i = 0; i < ndistPts; i++)
       {
           outputDist.xData[i] = x;
           if (cumulative)
           {
               cnormX += CalcNormal(x) * dx;
               outputDist.yData[i] = cnormX;
           }
           else
               outputDist.yData[i] = CalcNormal(x);
           x += dx;
       }

       outputDist.SetSize(ndistPts);
       return true;
   }


}; // end NS

