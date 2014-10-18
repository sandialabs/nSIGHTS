///////////////////////////////////////////////////////////////////////////////////
//
// nPreRoot.cpp
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


#include "StdAfx.h"

#ifndef ADCONSOLEAPP
#include <genPlotClass/C_PlotDef.h>
#include <genApp/C_AppMenu.h>
#endif

#include <genApp/C_MenuPage.h>
#include <genApp/C_ExposedObj.h>

#include <osMain/G_OptRange.h>
#include <osMain/G_Optimize.h>

#include <Var/G_Control.h>
#include <Var/G_DataCapture.h>
#include <Var/G_Sequence.h>

#include "nPreRoot.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace nPreRootData  {

nPreRoot    fieldDataRoot("Field Data", "Fld", "Field Data");
nPreRoot    sequenceRoot("Sequence", "Seq", "Sequence - Process/Plot");
nPreRoot    fitSpecRoot("Fit Specification", "Fit", "Fit Specification - Graphics");
nPreRoot    processingSetupRoot("Run-Time","Run", "Run-Time");


void PrepareAllSystemFO(bool clearOptTables, bool doRecalc)
{
  sequence.SetSequenceTimes(false);
  dataCapture.DataCaptureOK();
  optRangeOutputFO.AllocOutputIDs(control.IsOptimization());

  if (clearOptTables)
  {
    optCompleteOutput.Setup();
    optProgressOutput.Setup();
  }

  if (!doRecalc)
    return;

  FuncObjArray recalcObj;

  recalcObj += &sequenceTimesFO;
  recalcObj += &capturedDataFO;
  recalcObj += &optRangeOutputFO;
  recalcObj += &optCompleteOutput;
  recalcObj += &optProgressOutput;

  FuncObjC::CalcSelectedObjects(foc_PortOnly, recalcObj);

#ifndef ADCONSOLEAPP
  PlotDefC::ReplotAfterCalc();
  ExposedObjC::UpdateAllExposed();
#endif

}



nPreRoot::nPreRoot(const char* rID, const char* sID, const char* ibD) :
  MenuRootC(rID, sID, ibD)
{
}

void nPreRoot::SetDefault()
{
#ifndef ADCONSOLEAPP
  SetDefaultDataPage();
#endif
}

bool nPreRoot::ReadFromFile()
{
  // base class
  MenuRootC::ReadFromFile();

#ifndef ADCONSOLEAPP
  ClearOldDefaults();
#endif
  return true;
}


}; // end NS
