///////////////////////////////////////////////////////////////////////////////////
//
// DC_XYCDF.h
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
// DESCRIPTION: Platform independent and application independent base classes and
//              support classes.
//              Data Classes (DC) that define basic types of data used within the
//              framework: Statistical Cumulative Distribution Functions (CDF)
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_XYCDF_H
#define DC_XYCDF_H

#include <genClass/DC_XYData.h>
#include <genClass/SC_SetupErr.h>

class DC_XYCDF : public DC_XYData {
public:
  // generic quantile types & support
  enum QuantileType     {qtWeibull, qtHazen, qtExcel, qtGeneric};
  enum                          {nquantileType = 4};
  static const char*    quantileStrs[nquantileType];

private:
  bool                  isValid;
  bool                  isLog;   // if true xData is log of CDF
public:
  DC_XYCDF();
  DC_XYCDF(const DC_XYCDF& a);
  ~DC_XYCDF();

  DC_XYCDF&  operator=(const DC_XYCDF& a);

  inline bool   IsValid() const {return isValid;}
  inline bool   IsLog() const   {return isLog;}

  void          SetLog()        {isLog = true;}
  void          SetLinear()     {isLog = false;}
  void          SetValid()  {isValid = true;}

  bool          SetupOK(SC_SetupErr& errData);
  // assumes that isLog is correct, and if true that xData has
  // already been logged

  double        GetProbability(const double& dataVal,
                               bool          nullOutSideLimits = true) const;
  // if isLog, log is taken before lookup

  double        GetDataVal(const double& pVal,
                           bool          nullOutSideLimits = true) const;
  // if is log, result is exp(lookup)

  double        GetLimitsMean(const double& minP,
                              const double& maxP) const;
  // returns mean of CDF data >= minP and < maxP
  // is isLog then is log mean
  // returns null if no points in range

  // quantile support
  static bool   QuantileOK(QuantileType qType,
                         const double&  genericVal);

  void          SetCDFQuantile(QuantileType  qType,
                               const double& genericVal);
  // assumes X data already set and N is valid


private:

  double        CDFInterp(const double&         theVal,
                          const SC_DoubleArray& aVec,
                          const SC_DoubleArray& bVec,
                          bool                  nullOutsideLimts) const;

};

#endif //DC_XYCDF_H

