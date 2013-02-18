///////////////////////////////////////////////////////////////////////////////////
//
// C_BufFile.h
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
// DESCRIPTION: Platform independent and application independent base classes
//              and support classes.
//              Reads/writes binary formatted files
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_BUFFILE_H
#define C_BUFFILE_H

#ifdef MSCVS2005
#include <stdlib.h> // big kluge for 2GB files
#endif
#include <fstream>

#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_IntArray.h>
#include <genClass/SC_BoolArray.h>
#include <genClass/SC_StringArray.h>


class BufFileC {
private:
  static const float nullFloat;

#ifdef MSCVS2005
  typedef long long  BUFSIZETYPE;  // VC2005 - files > 2GB (in theory)
  FILE*              bufferFile;
#else
  typedef long       BUFSIZETYPE;    // otherwise 2GB only
  std::fstream*      bufferFile;
#endif
  BUFSIZETYPE        bufferSize;
  char*              buffer;

  int                currRecNum;
  int                currByteNum;
  bool               openWasOK;

  bool               entireFileRead;

protected:
  bool               realsAreFloats;

public:
  // exception class
  class   BufferFileError {
  public:
    enum    {bufferFileErrLen = 80};
    char        errMsg[bufferFileErrLen];
  public:
    BufferFileError(const char* eMsg);
  };


public:
  //  constructors and destructors
  BufFileC(int bufSize);
  ~BufFileC();

  void       OpenFile(const char fName[],
                      bool forRead,
                      bool readOnly = false);  // opens file with "rb" attribute if forRead - allows read when open for write by other prog - FB335

  void       CloseFile();

  int        GetBufferSize() {return int(bufferSize);} //ned - limited to 2GB - jda
  int        GetCurrentByteNum() {return currByteNum;}  //ned

  int        GetCurrentRec() {return currRecNum;}
  void       WriteAbsolute(int recNum);
  void       WriteCurrent();  // writes current

  void       ReadAbsolute(int recNum);
  void       ReadNextRecord();
  bool       IsEOF();
  char       Peek();


  void       WriteSequentialBytes(const void* dataAdr,
                                  int   nbytes);

  void       ReadSequentialBytes(void* dataAdr,
                                 int   nbytes);

  void       FlushBuffer(int& nextRecNum);
  void       FlushBuffer();  // throws away recnum

  void       BufferReset(int recNum);

  void       BufferError(const char* errMsg);            // throws exception

  void       WriteDoublesAsFloats() {realsAreFloats = true;}

  //  generic writing routines
  void       WriteString(const char* fText, int strSize);
  void       WriteBool(bool fBool);
  void       WriteInt(int fInt);
  void       WriteLong(long fLong);
  void       WriteDouble(const double& fDouble);
  // written as double even if realsAreFloats is true
  void       WriteReal(const double& fReal);
  // written as double if realsAreFloats is false
  // written as float if realsAreFloats is true


  //  genericreading routines
  void       ReadString(char* fText, int strSize);
  void       ReadBool(bool& fBool);
  bool       ReadBool();
  void       ReadInt(int& fInt);
  int        ReadInt();
  void       ReadLong(long& fLong);
  void       ReadDouble(double& fDouble);
  double     ReadDouble();
  void       ReadReal(double& fReal);
  // reads double if realsAreFloats is false
  // reads floats and converts if realsAreFloats is true

  //  std file header processing
  //  headersare alwas written to record 0
  void       WriteStdFileHeader(const char* headerID,
                                int   headerSize,
                                int   majorVerNum,
                                int   minorVerNum);

  void       ReadStdFileHeader(const char* headerID,
                               int   headerSize,
                               int&  majorVerNum,
                               int&  minorVerNum);
  // throws exception if ID in file does not match header ID


  // std DoubleArray
  void       WriteRealArray(const SC_DoubleArray& fRealArray,
                            int&            nextRec);
  // as for WriteReal
  // writes fRealArray.Size() elements, flushes buffer
  // returns nextRec
  void       ReadRealArray(SC_DoubleArray& fRealArray, int nRead);
  // as for ReadReal

  //      thefollowing were revised post mView for nSights and are consistent
  //      buffer record
  void       WriteIntArray(const SC_IntArray&  intVals,    // writes Size, followed by Size ints
                           int&                nextRec);   // followed by flush buffer
  void       WriteIntArray(const SC_IntArray&  intVals);   // writes Size, followed by Size ints - no flush

  void       WriteMappedIntArray(const SC_IntArray&    intData,
                                 const SC_IntArray&    validMap);
  void       ReadIntArray(SC_IntArray&  intVals);
  void       ReadMappedIntArray(SC_IntArray&       dblData,
                                const SC_IntArray& validMap);
  
  void       WriteDoubleArray(const SC_DoubleArray&  dblVals,    // writes Size, followed by Size doubles
                              int&                   nextRec);   // followed by flush buffer
  void       WriteDoubleArray(const SC_DoubleArray&  dblVals);   // writes Size, followed by Size doubles - no flush
  void       WriteMappedDoubleArray(const SC_DoubleArray& dblData,
                                    const SC_IntArray&    validMap);
  
  
  void       ReadDoubleArray(SC_DoubleArray&  dblVals);
  void       ReadMappedDoubleArray(SC_DoubleArray& dblData,
                                   const SC_IntArray&    validMap);
  
  
  void       WriteDoubleMatrix(const SC_DoubleMatrix&  dblVals,   // writes Size, followed by Size doubleArrays
                               int&                    nextRec);  // followed by flush buffer
  void       WriteDoubleMatrix(const SC_DoubleMatrix&  dblVals);  // writes Size, followed by Size doubleAtrrays - no flush
  void       ReadDoubleMatrix(SC_DoubleMatrix&  dblVals);


  void       WriteStringArray(const SC_StringArray&  strVals,   // writes Size, followed by Size Strings
                              int&                   nextRec);  // followed by flush buffer
  void       WriteStringArray(const SC_StringArray&  strVals);  // writes Size, followed by Size Strings - no flush
  void       ReadStringArray(SC_StringArray&  strVals);

  void       WriteBoolArray(const SC_BoolArray&  boolVals,  // writes Size, followed by Size ints
                            int&                 nextRec);  // followed by flush buffer
  void       WriteBoolArray(const SC_BoolArray&  boolVals); // writes Size, followed by Size ints - no flush
  void       ReadBoolArray(SC_BoolArray&  boolVals);   // writes Size, followed by Size ints - no flush



  // to Unix text file conversion
  // reads entire file, changes CR to blank, writes entire file
  static void WindowsToUnix(const char* fileName);  // base method

  static void WindowsToUnix(bool  convertToUnix, // calls base if flag true
                           const char* fileName);

protected:
  void       CheckBufferError(const char* errMsg);

};


#endif // C_BUFFILE_H

