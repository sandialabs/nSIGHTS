///////////////////////////////////////////////////////////////////////////////////
//
// DC_PenSet.h
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
//              framework: Defines pen set consisting of 24 separate colors
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_PENSET_H
#define DC_PENSET_H

#include <genClass/C_Common.h>
#include <genClass/SC_ColorSpec.h>


class DC_PenSet {
public:
  enum            {maxPenSet = 24};  // changed from 8 - 27 Jun 99
                                     // changed from 12 - Apr 2001
  enum            {maxPenLabelLen = 40};

  SC_ColorSpec    penColor[maxPenSet];
  char            penLabel[maxPenSet][maxPenLabelLen];

public:
  DC_PenSet();
  DC_PenSet(const DC_PenSet& a);
  ~DC_PenSet() {};
  DC_PenSet&      operator= (const DC_PenSet& a);

  SC_ColorSpec    GetColor(int penIndx) const;
  char*           GetLabel(int penIndx);

  void            SetDefault();   // sets to basic pen set

private:
  void            LocalCopy(const DC_PenSet& a);
};



#endif // DC_PENSET_H

