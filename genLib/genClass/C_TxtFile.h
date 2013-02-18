///////////////////////////////////////////////////////////////////////////////////
//
// C_TxtFile.h
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
// DESCRIPTION:  Platform independent and application independent base classes and
//              support classes.
//              Derived from C_Text for file I/O
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_TXTFILE_H
#define C_TXTFILE_H

#include <fstream>
#include <genClass/C_Text.h>
#include <genClass/SC_StringArray.h>

class TxtFileC : public TextC{
private:
  std::fstream*   txtFile;
  bool            openWasOK;

public:
  TxtFileC();
  TxtFileC(const char* fName, bool openForRead); // opens the named file for reading or writing
  ~TxtFileC();

  //  manual open & close file
  void            OpenFile(const char* fName, bool openForRead);
  void            CloseFile();  // closes files after open with name

  //  TextC virtuals
  virtual void    Close();        // calls close file
  virtual bool    IsEOF();


  virtual void    WriteLine();
  virtual void    WriteText(const char* fText);       // text followed by end line may contain blanks
  virtual void    WriteSolidText(const char* fText);  // text with no blanks followed by white space

  void            WriteTextIncBlank(const char* fText);   //same as WriteText but allows blank lines
  void            WriteStringArray(const SC_StringArray fText);  //each string is followed by an endline

  virtual void    ReadText(char* fText, int maxLen);
  virtual void    ReadSolidText(char* fText);
  virtual void    CheckTextError(const char* errMsg);

  // overrides on base class
  virtual bool    ReadBool();
  virtual int     ReadInt();
  virtual double  ReadDouble();

  // for ignoring all predefined routines
  std::fstream*   GetStream() {return txtFile;}

};


#endif // C_TXTFILE_H

