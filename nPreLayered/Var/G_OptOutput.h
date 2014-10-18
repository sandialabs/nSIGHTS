///////////////////////////////////////////////////////////////////////////////////
//
// G_OptOutput.h
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


#ifndef G_OPTOUTPUT_H
#define G_OPTOUTPUT_H

#include <osClass/DC_OptSimResults.h>
#include <osClass/IO_OptSimResults.h>
#include <osMain/C_Optimize.h>

#include <Var/C_FileOutput.h>


//  namespace containing main output file variables
namespace nsOutputFiles  {

//  useage as singleton -- put in class for clarity
class OptOutputControl : public FileOutputControl {
private:
  int     ncasePar;
  int     maxCase;
  int     currCaseIndex;
  bool    runHeaderWritten;
  bool    caseHeaderWritten;

public:
  OptOutputControl() {};
  ~OptOutputControl() {};

  // sets  ncasePar, maxCase, checks for compatibility on add
  virtual bool    OutputSetup(bool& addOK);
  virtual void    SimulationComplete();

  void      NextCaseSetup(int caseIndex);
  void      NextFitComplete(      bool       fitAborted,
                                  const Optimizer& opt);
  bool      NextCaseComplete();

protected:
  virtual void    Cleanup();  // common to both exit routines

private:
  void    SetRunDesc(OptSimResultsHeader&  runDesc);
  void  SetupCaseHeader(const Optimizer& opt);
  bool    FirstFitWriteOK(const Optimizer& opt);
};

// optimization output
extern IO_OptSimResults     optOutputFile;
extern OptOutputControl     optOutputControl;


};


//  assuming that if it's included, we want to use it and no name clashes with other nSight
using namespace nsOutputFiles;


#endif
