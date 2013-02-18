///////////////////////////////////////////////////////////////////////////////////
//
// C_Text.h
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
// DESCRIPTION:  Platform independent and application independent base claseess and
//              support classes.
//              Base class for reading/writing text formatted data (file or clipboard)
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_TEXT_H
#define C_TEXT_H

#include <genClass/SC_RealFormat.h>
#include <genClass/SC_IntArray.h>
#include <genClass/SC_DoubleArray.h>

// generic class for reading/writing text -- used as base for text file and clipboard text
class TextC {

protected:
  SC_RealFormat            doubleConvFormat;
  char                     solidTextDelimChar;
  bool                     newLineStart;

public:
  // exception class
  class   TextError {
  public:
    enum        {textFileErrLen = 80};
    char        errMsg[textFileErrLen];
    TextError(const char* eMsg);

  };


public:
  TextC();
  ~TextC();

  // virtuals to be supported by all implementations
  virtual void   Close() = 0;

  virtual void            WriteLine()                        = 0;
  virtual void            WriteText(const char* fText)       = 0; // text followed by end line may contain blanks
  virtual void            WriteSolidText(const char* fText)  = 0; // text with no blanks followed by white space

  virtual void            ReadText(char* fText, int maxLen)  = 0;
  virtual void            ReadSolidText(char* fText)         = 0;
  virtual void            CheckTextError(const char* errMsg) = 0;  // throws error if last read was bad
  virtual bool            IsEOF()                            = 0;  // last read hit EOF


  // basic types
  void                    WriteBool(bool fBool);
  void                    WriteInt(int fInt);
  void                    WriteDouble(const double& fDouble);
  void                    WritePointer(const void* fPtr);

  // use strstream and may be slow -- can be overridden
  virtual bool            ReadBool();
  virtual int             ReadInt();
  virtual double          ReadDouble();
  virtual void*           ReadPointer();

  // permutations on basic types
  void                    WriteTextWithTrailingBlanks(const char* fText); // trailing blanks converted to underscores
  void                    WriteQuotedText(const char* fText);             // text inside quotes, followed by EOL
  void                    WriteQuotedSolidText(const char* fText);        // text inside quotes
  void                    WriteNullDouble(const double& fDouble);         // if real is null, writes null

  void                    ReadTextWithTrailingBlanks(char* fText, int maxLen);
  double                  ReadNullDouble();  // checks for 'null' converts to null real

  //  file header stuff
  void            WriteFileHeader(const char* headerText, int majVer, int minVer);
  // writes file header followed by version data

  void            ReadFileHeader(const char*  headerToMatch,
                                 int&   majVer,
                                 int&   minVer,
                                 const char*  fileDesc);
  //  reads and compares, throws an error if no match on text
  //  fileDesc is used in constructing error message

  //  member access
  void            SetRealConvFormat(const SC_RealFormat& inForm)
  {doubleConvFormat = inForm;}
  void            SetDelimChar(char inChar) {solidTextDelimChar = inChar;}
  void            SupressDelim() {newLineStart = true;}

  //  generic writing routines
  void            WriteIntArray(const SC_IntArray& intData,
                                bool               writeSize,
                                int                lineModulus = 20);  // 0 for no WriteLine
  void            WriteMappedIntArray(const SC_IntArray&    intData,
                                      const SC_IntArray&    validMap,
                                      int                   lineModulus = 20); // 0 for no WriteLine
  void            WriteDoubleArray(const SC_DoubleArray& dblData,
                                   bool                  writeSize,
                                   int                   lineModulus = 20); // 0 for no WriteLine
  void            WriteMappedDoubleArray(const SC_DoubleArray& dblData,
                                         const SC_IntArray&    validMap,
                                         int                   lineModulus = 20); // 0 for no WriteLine


  //  generic reading routines
  void            ReadFileName(char* fName);
  void            ReadIntArray(SC_IntArray& intData,
                               bool         readSize);
  void            ReadMappedIntArray( SC_IntArray&       dblData,
                                      const SC_IntArray& validMap);
  void            ReadDoubleArray(SC_DoubleArray& dblData,
                                  bool            readSize);
  void            ReadMappedDoubleArray(SC_DoubleArray&     dblData,
                                        const SC_IntArray&  validMap);

  //  error checking
  void            ThrowTextError(const char* errMsg);
  // throws exception
};


#endif // C_TEXT_H

