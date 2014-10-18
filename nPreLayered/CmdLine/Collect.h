///////////////////////////////////////////////////////////////////////////////////
//
// Collect.h
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


#ifndef COLLECT_H
#define COLLECT_H


namespace nsCollect  {

class ProcessData {
public:
  int   nPID;
  int   processResults;
  int   startIndex;
  int   endIndex;
  bool  processComplete;

  char  processID[40];
  char  xyFile[stdFileStrLen];
  char  optFile[stdFileStrLen];
  char  rngFile[stdFileStrLen];

  char  sStr[20];
  char  eStr[20];

  static T_SC_Array<ProcessData> spawnData;

public:
  ProcessData();

  void  SetFiles(int runIndex);

  void  UpdateFiles();

  void  DeleteFiles();

  void  ReplaceExtension(char* outfName,
                         const char* baseFName,
                         const char* processID,
                         const char* extension);

private:

  static void DoDelete(char* delfName);

};

// Windows spawn
extern void   CollectXY();

extern void   CollectOpt();

extern void   CollectRange();

};

//  assuming that if it's included, we want to use it and no name clashes with other nSight
using namespace nsCollect;


#endif
