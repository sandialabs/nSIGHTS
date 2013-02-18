///////////////////////////////////////////////////////////////////////////////////
//
// DC_ContourSpec.h
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
//              framework: contour values, colors, and line type
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_CONTOURSPEC_H
#define DC_CONTOURSPEC_H

#include <genClass/C_Graphic.h>
#include <genClass/SC_ColorSpec.h>
#include <genClass/T_SC_Array.h>


class DC_ContourSpec {
public:
  SC_ColorSpec    contourColor;
  int             lineThickness;
  PlotLineType    lineType;
  bool            mitreLines;
  double          contourVal;

public:
  DC_ContourSpec() {lineThickness = 1; contourVal = 1.0; lineType = plt_Solid;}
  DC_ContourSpec(const DC_ContourSpec& a);
  DC_ContourSpec&  operator= (const DC_ContourSpec& a);

private:
  void   LocalCopy(const DC_ContourSpec& a);


};

class DC_ContourSpecArray: public T_SC_Array<DC_ContourSpec>  {
public:
  bool  doLogContours;        // take log of contourVal before contouring

public:
  DC_ContourSpecArray() {};
  DC_ContourSpecArray(const DC_ContourSpecArray& a);
  ~DC_ContourSpecArray() {};

  DC_ContourSpecArray&    operator= (const DC_ContourSpecArray& a);

private:
  void  LocalCopy(const DC_ContourSpecArray& a);

};


#endif // DC_CONTOURSPEC_H

