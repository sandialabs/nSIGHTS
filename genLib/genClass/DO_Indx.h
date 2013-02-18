///////////////////////////////////////////////////////////////////////////////////
//
// DO_Indx.h
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
//              lightweight Data Object (DO) classes derived from DataObjC to
//              wrap data classes DC_XXX: base class for various index types
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DO_INDX_H
#define DO_INDX_H

#include <genClass/DO_Label.h>
#include <genClass/SC_IntArray.h>

//  base class for various index types
class DO_Indx : public DO_Label {
private:
  static char* typeDesc;
public:
  //  status flags set in derived class if necessary
  const bool  singleOK;           // default true
  const bool  multipleOK;         // default false

  bool        indexIsSingle;      // default true
  int         indxVal;            // single value
  SC_IntArray indxArray;          // multiple value  -- initialized as resizable 10 if multipleOK

public:
  DO_Indx();          // single true, mult false
  DO_Indx(bool snglOK, bool multOK);
  DO_Indx(const DO_Indx& a);
  ~DO_Indx(){};

  DO_Indx&      operator= (const DO_Indx& a);

  // Data Obj virtuals
  char*         GetType() const {return typeDesc;}
  bool          DataOK();

  // label virtual
  void          Reset(const char* typeLabel);
  void          SetIndexLabel();  // sets to index

  //  access
  inline bool   SingleOK()      const {return singleOK;}
  inline bool   MultipleOK()    const {return multipleOK;}

  inline bool   IsSingle()      const {return indexIsSingle;}
  inline bool   IsMultiple()    const {return !indexIsSingle;}

  void          SetIndexType(bool isSngl);

  inline void   SetIndexValue(int inIndx) {indxVal = inIndx;}
  inline int    GetIndexValue() const {return indxVal;}

  void          SetIndexArray(const SC_IntArray&  inArray);
  const SC_IntArray&  GetIndexArray() const;

private:
  void          LocalCopy(const DO_Indx& a);

};

#endif // DO_INDX_H

