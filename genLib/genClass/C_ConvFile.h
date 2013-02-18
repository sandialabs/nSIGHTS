///////////////////////////////////////////////////////////////////////////////////
//
// C_ConvFile.h
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
// DESCRIPTION:  Platform independent and application independent base classes
//              and support classes.
//              Special support for reading and converting ASCII files produced
//              by numeric models.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_CONVFILE_H
#define C_CONVFILE_H

#include <fstream>

#include <genClass/C_Common.h>

class SC_StringArray;
class SC_DoubleArray;
class SC_IntArray;

class ConvFileC {
public:
  const int       maxLineLen;    // also record length for direct access mode

private:
  std::ifstream*   inputF;
  char        currFname[stdFileStrLen];

  enum        {msgStrLen = 80};
  char        infoMsgSrc[msgStrLen];
  char        infoMsgHdr[msgStrLen];
  int         infoMsgMod;
  char        commentChars[40];
  bool        processComments;
  int         ncommentChars;

  bool        isDirect;
  int         directRecordNum;

protected:
  bool        trapEOFseperately;

public:
  int         nlinesRead;
  char*       lastLineRead;           // allocd to maxLineLen+1
  char*       parsedLine;             // allocd to maxLineLen+1
  int         lastLineLen;
  bool        beenParsed;

private:
  enum        {maxParseChars = 40};
  char        parseChars[maxParseChars];      // default parse chars are ' ' and \t

public:
  ConvFileC();                    // defaults to maxLine len = 1024 chars
  ConvFileC(int maxLen);
  ConvFileC(const char* fName);   // defaults to maxLine len = 1024 chars
  ConvFileC(const char* fName, int maxLen);
  ~ConvFileC();

  class   CFReadError {           // general exception class
  public:
    enum {maxErrLen = 80};
    char errMsg[maxErrLen];
  public:
    CFReadError(const char* eMsg);
  };

  class   CFEofError {            // EOF exception class
  public:
    CFEofError() {}
  };


  static bool CFfileExists(const char* fN);  // returns true if file found

  //  most routines below are error trapped and throw exception when called

  void        CFsetMsgStuff(const char*   msgSrc,
                            const char*   msgHdr,
                            int           msgMod);

  const char* CFgetFilename() {return currFname;}

  //      comment processing -- if set lines begining with any of comment chars will be treated
  //      as blank

  void        CFsetComment(const char* inChars);  // if inChars is null no processing is done

  void        CFopenText(const char* fN);
  void        CFsetDirect();
  void        CFsetNormal();
  void        CFreset();
  void        CFcloseText();
  bool        CFreadText();       // returns true if line read was non-blank
  // comments are processed if set
  bool        CFreadText(int minLen);       // returns true if line read was non-blank, pads to minLen chars
  void        CFreadNonBlank();

  bool        CFreadUntil(const char matchStr[],   // must match anywhere in string
                          bool abortEOF = true);   // if false then will return false on EOF

  bool        CFreadUntil(const char matchStr[],   // must match starting at stcol
                          int  stCol,              // stCol is FORTRAN column number
                          bool abortEOF = true);   // if false then will return false on EOF
  // other wise will throw error
  bool        CFreadUntil(const char matchStr[],   // trims stcol to end col and tries match
                          int  stCol,              // stCol is FORTRAN column number
                          int  endCol,             // stCol is FORTRAN column number
                          bool abortEOF = true);   // if false then will return false on EOF
  // other wise will throw error

  int         CFreadUntil(const SC_StringArray& matchStr,  // must match starting at stcol
                          int  stCol,              // stCol is FORTRAN column number
                          bool abortEOF = true);   // if false then will return false on EOF
  // returns -1 if not found and abortEOf = false
  bool        CFstrIsDouble(char    inStr[],
                            double& dVal,
                            bool    throwError = true);

  bool        CFstrIsInt(char inStr[],
                         int& iVal,
                         bool throwError = true);

  void        CFskipLines(int nSkip);             // reads next nSkip lines

  //  col pos stuff

  void        CFgetSubStr(char    inStr[],
                          int     stCol,    // stCol and endCol are FORTRAN column numbers
                          int     endCol);  // ie char array index + 1

  double      CFgetDouble(int     stCol,
                          int     endCol);

  bool        CFisDouble(int     stCol,
                         int     endCol,
                         double& dVal);

  int         CFgetInt(int     stCol,
                       int     endCol);

  bool        CFisInt(int  stCol,
                      int  endCol,
                      int& iVal);

  //  parse char delimited routines
  void        SetParseChars(const char newParseChars[],  // extra parse chars
                            bool useJustNew = false);    // blank and tab are default - not used if = true

  void        ClearParseChars();                     // resets to just blank and tab

  bool        CFgetNextStr(char    inStr[],  // extracts next string from parsed line
                           int     maxLen);  // returns false if str is empty

  double      CFgetNextDouble();             // next double

  int         CFgetNextInt();

  void        CFskipNextEntry(int nskip);

  void        CFresetParseStr();              // resets to initial str

  void        CFextractToCol(int stCol);      // removes first stCol chars

  void        CFreadDoubleArray(SC_DoubleArray& inVec,                // reads to set .Size()
                                    bool                    nullOK);  // nuls allowed
  void        CFreadIntArray(SC_IntArray& inVec);   // reads to set .Size()

  //      streamed parse chars
  double      CFstreamDouble(bool throwError = true); // if false, sets to null
  int         CFstreamInt();
  void        CFstreamText(char* txtVal, int maxLen);

  // other parsing
  void        CFtrim();                          // clears leading and trailing blanks
  bool        CFmatches(const char matchStr[]);  // true if matches exactly
  bool        CFmatchStringFound(const char matchStr[]); // true if matchStr is a sub string


  bool        CFeof();
  void        CFtextError(const char errMsg[]);  // throws exception when called
  void        CFimportOK(int nConv);

  // DXF support
  void        CFreadDXFCoord(Coord3D& readCoord);

private:
  bool        ReadUntilCommon(const char matchStr[],
                              bool abortEOF);

  void        InitCommon();   // common to all constructors

};



#endif // C_CONVFILE_H

