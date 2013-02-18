///////////////////////////////////////////////////////////////////////////////////
//
// DC_DataLimit.h
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
//              framework: Specification for mapping real values to integer intervals
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_DATALIMIT_H
#define DC_DATALIMIT_H

class SC_DoubleArray;
class SC_IntArray;

class DC_DataLimit {
public:
  bool    logRange;
  double  minLimit;
  double  maxLimit;

private:
  mutable double  actMin;
  mutable double  actMax;
  mutable double  dAct;

public:
  DC_DataLimit(){logRange = false; minLimit = 0.0; maxLimit = 1.0;}
  DC_DataLimit(const DC_DataLimit& a);
  ~DC_DataLimit() {};
  DC_DataLimit&   operator=(const DC_DataLimit& a);

  void    InitLimit(const double& inVal);     // initializes min and max to in
  void    AddToLimit(const double& inVal);    // expands range to fit minVal
  void    InitLimit(const SC_DoubleArray& inData);
  void    AddToLimit(const SC_DoubleArray& inData);

  void    ResetLogLimit();    // resets minLim to 6 OM less than max lim
                              // if logRange and minLim <= 0.0

  bool    MapDataToRange(const SC_DoubleArray& inData,
                         SC_IntArray&   outMap,
                         bool           clipToRange,
                         int            maxMap) const;
  // uses range to convert real data to map entries in range 0 to maxMap - 1
  // if clip is true, values outside range are set to -1, otherwise
  // below range are set to 0 and above range to maxMap -1
  // returns true if at least one value is mapped within range

  //      for single color case -- just checks for clips & nulls
  bool    MapDataToRange(const SC_DoubleArray& inData,
                         SC_IntArray&   outMap,
                         bool           clipToRange) const;

  double  GetActValue(int mapIndx) const {return actMin + double(mapIndx) * dAct;}
  double  GetMinAct() const {return actMin;}
  double  GetMaxAct() const {return actMax;}
  double  GetdAct()   const {return dAct;}

  bool    TransformValueForRange(double& inVal,
                                 bool    clipToRange) const;

private:
  void    LocalCopy(const DC_DataLimit& a);

};



#endif // DC_DATALIMIT_H

