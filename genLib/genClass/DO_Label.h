///////////////////////////////////////////////////////////////////////////////////
//
// DO_Label.h
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
//              wrap data classes DC_XXX: base class for various label types
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DO_LABEL_H
#define DO_LABEL_H

#include <genClass/C_Common.h>
#include <genClass/C_DataObj.h>

class SC_RealFormat;

class DO_Label : public DataObjC {
private:
  static char* typeDesc;

public:
  enum        {maxLabelLen = 40};
  char        dataTypeLabel[maxLabelLen];
  char        dataValueLabel[maxLabelLen];
public:
  DO_Label();
  DO_Label(const DO_Label& a);
  ~DO_Label(){};
  DO_Label&   operator= (const DO_Label& a);

  virtual void Reset(const char* typeLabel);   // clears all data and sets type label

  // DataObjC virtuals
  char*       GetType() const {return typeDesc;}
  bool        DataOK() {return ((dataTypeLabel[0] != '\0') && (dataValueLabel[0] != '\0'));}

  // local
  const char* GetTypeLabel() {return dataTypeLabel;}
  const char* GetValueLabel() {return dataValueLabel;}
  void        SetTypeLabel(const char inType[]);
  void        SetValueLabel(const char inValue[]);
  void        SetValueLabel(const double& inValue);
  void        SetValueLabel(const double& inValue,
                            const SC_RealFormat& rForm);
  void        SetValueLabel(int  nValue,
                            const char valDesc[]);
  void        SetFileValueLabel(const char inFname[]);   // special case for files -- removes path


  virtual const char* GetID();

private:
  // ******************************* standard methods
  void    LocalCopy(const DO_Label& a);        // common part of constructor and = operator
  void    LocalInitialize();                   // resets all non-class member variables

protected:
  //      locals and all super classes -- none right now
  void    FullCopy(const DO_Label&  a) {LocalCopy(a);}
  void    FullInitialize()             {LocalInitialize();}

};


#endif // DO_LABEL_H

