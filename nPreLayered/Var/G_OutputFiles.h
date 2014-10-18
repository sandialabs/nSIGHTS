///////////////////////////////////////////////////////////////////////////////////
//
// G_OutputFiles.h
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract
// DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
// retains certain rights in this software.
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


#ifndef G_OUTPUTFILE_H
#define G_OUTPUTFILE_H

#include <genApp/C_CfgFileObj.h>
#include <genApp/G_AppObj.h>

#include <osMain/C_OSListObj.h>

#include <Var/C_FileOutput.h>

#include <genListClass/C_ErrorListObj.h>
#include <genListClass/C_ListFactory.h>


namespace nsOutputFiles  {

//  ***************  basic file description/control
class OutputFileDesc  {
public:
  enum      {runIDLen = 40};

  bool      writeFile;
  char*     fileName;
  bool      addExisting;
  char      runID[runIDLen];

public:
  OutputFileDesc(char* inF);
  virtual void    SetToInitial(const char* defFname);
};


//  ***************  optimization output
class OptOutputFileDesc : public OutputFileDesc {
public:
  //  properties for opt files
  bool            writeResiduals;
  bool            writeJacobian;
  bool            writeCovariance;

public:
  OptOutputFileDesc(char* inF);
  virtual void    SetToInitial(const char* defFname);
};


//  ***************  range output
class RangeOutputFileDesc : public OutputFileDesc {

public:
  RangeOutputFileDesc(char* inF);
  virtual void    SetToInitial(const char* defFname);
};


//  ***************  profile output
class ProfileOutputFileDesc : public OutputFileDesc {
public:
  //  properties for pro files
  int             startSeq;
  int             endSeq;
  int             timeStepModulus;
  int             radiusNodeModulus;

public:
  ProfileOutputFileDesc(char* inF);
  virtual void    SetToInitial(const char* defFname);
};


//  ***************  mview output - different than others
class mViewFileDesc {
public:
  bool      writeFile;
  char      geomFileName[stdFileStrLen];;
  char      propFileName[stdFileStrLen];;
public:
  mViewFileDesc();
  void      SetToInitial();
};



//  ***************  all output files

class OutputFilesListing : public OSListObj {
public:
  OutputFilesListing();
  ~OutputFilesListing() {};

  virtual void  CreateListing();

private:
  void    ListOutputFiles();
  void    ListFileDesc(const OutputFileDesc& desc,
                       const char*      title);
};

class OutputFilesErrorListing : public ErrorListObjC {
public:
  OutputFilesErrorListing();
  ~OutputFilesErrorListing() {};

  virtual void  CreateListing();

private:
  void        FileDescErrorCheck(const char*      fileType,
                                 const OutputFileDesc& desc);
};

extern OutputFilesErrorListing  outputFilesErrorListing;

class OutputFilesStaticSupport : public ListFactory,
  public CfgFileObj {
private:
  // object versioning support
  static const char*  groupHeader;
  static const int  majorVersion;
  static const int  minorVersion;
  static appFuncObjGlobals::AppFuncArray outFileObj;

public:
  OutputFilesStaticSupport();     // constructor initializes all parameters and vars
  ~OutputFilesStaticSupport() {};

  static void   InitAppGlobals(); // must be called once after creation and before use
  static void     SetToInitial();     // for file New
  static void     SetForFlags();      // modifies settings
  static bool   OutputFilesOK(bool& errorDisplayed);    // pre-run check -- should be last check called

  virtual void    WriteToFile();
  virtual bool    ReadFromFile();

  virtual ListObjC*   CreateListObject() {
    return new OutputFilesListing();
  }
  virtual ErrorListObjC*  GetErrorListObject() {
    return &outputFilesErrorListing;
  }

private:
  void    WriteOutputFiles();
  void    ReadOutputFiles();

  void    WriteFileDesc(const OutputFileDesc& desc);
  void    ReadFileDesc(OutputFileDesc& desc);

  static bool CheckOutputFile(FileOutputControl& outputToCheck,
                              bool&       errorDisplayed,
                              const char*     outputDesc);

};


extern OutputFileDesc     xyForwardOutputDesc;
extern OptOutputFileDesc    optOutputDesc;
extern RangeOutputFileDesc    rangeOutputDesc;
extern ProfileOutputFileDesc    profileOutputDesc;
extern mViewFileDesc      mviewOutputDesc;

extern OutputFilesStaticSupport outputFiles;

};


//  assuming that if it's included, we want to use it and no name clashes with other nSight
using namespace nsOutputFiles;


#endif
