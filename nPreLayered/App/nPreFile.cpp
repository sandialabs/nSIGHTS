///////////////////////////////////////////////////////////////////////////////////
//
// nPreFile.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClass/C_FuncObj.h>

#include <genApp/C_MenuRoot.h>
#include <genApp/C_MenuPage.h>

#ifndef ADCONSOLEAPP
#include <genPlotClass/C_PlotDef.h>
#include <genApp/C_PlotPage.h>
#include <genApp/C_AppMenu.h>
#include <genApp/ProjectUtil.h>
#include <genInterface/InterfaceContext.h>
#endif

#include <genApp/C_ExposedObj.h>

#include <Var/G_PreConfigFile.h>

#include <Var/G_Control.h>
#include <Var/G_Layer.h>
#include <Var/G_Parameter.h>
#include <Var/G_Sequence.h>
#include <Var/G_DataCapture.h>

#include <Var/G_OutputFiles.h>
#include <Var/G_ProfileOutput.h>
#include <Var/G_XYOutput.h>

#include <Sim/G_SimulatorRadiiOutput.h>

#include <osMain/G_Optimize.h>
#include <osMain/G_Sample.h>
#include <osMain/G_Vary.h>
#include <osMain/G_OptRange.h>
#include <osMain/osConfigFile.h>

#include <iostream>

#include "nPreRoot.h"
#include "nPreFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void  nPreFileOperations::NewFile()
{
  control.SetToInitial();
  layerSpec.SetToInitial();
  parameters.SetToInitial();
  sequence.SetToInitial();
  dataCapture.SetToInitial();
  outputFiles.SetToInitial();
  vary.SetToInitial();
  optimize.SetToInitial();
  sample.SetToInitial();

#ifndef ADCONSOLEAPP
  MenuRootC::DestroyAll();
#endif

  fieldDataRoot.SetDefault();
  sequenceRoot.SetDefault();
  fitSpecRoot.SetDefault();
  processingSetupRoot.SetDefault();

#ifndef ADCONSOLEAPP
  projectUtilities::SetModifiedFlag(false);
#endif
}

// ------------------------------------------  file header

static const char*  fileID  = "nSIGHTS nPre Configuration File";
//enum    {majorVersionNum = 0};
//enum    {majorVersionNum = 1};  // added optimize vars
//enum    {majorVersionNum = 2};  // added sample vars
//enum    {majorVersionNum = 3};  // added range/vary vars
//enum    {majorVersionNum = 4};  // new object
//enum    {majorVersionNum = 5};  // interface context as group
//enum    {majorVersionNum = 6};  // added read ver to os vars
//enum    {majorVersionNum = 7};  // UI reconfigure, drop sampling root
enum    {majorVersionNum = 8};    // add layer spec


static void WriteCFGHeader()
{
  WriteFileHeader(fileID, majorVersionNum, 0);

  WriteAppGlobals();
}


static void ReadCFGHeader()
{
  ReadFileHeader(fileID, currentCfgFileReadMajorVersion, currentCfgFileReadMinorVersion,
                 "nPre configuration");

  if (currentCfgFileReadMajorVersion < 0)
    InputError("Bad version #");

  if (currentCfgFileReadMajorVersion < 4)
  {
    prevVerSystemObjects.Alloc(10);

    // fix for old ver of sys obj
    prevVerSystemObjects += &sequenceTimesFO;
    prevVerSystemObjects += &capturedDataFO;
    prevVerSystemObjects += &xyForwardOutputFO;
    prevVerSystemObjects += &profileOutputFO;

    prevVerSystemObjects += &optimizeInput;
    prevVerSystemObjects += &optCompleteOutput;
    prevVerSystemObjects += &optProgressOutput;

    prevVerSystemObjects += &sampResultsFO;
    prevVerSystemObjects += &simulatorRadiiFO;
  }

  // kluge for os versioning
  osUseVersionControl = (currentCfgFileReadMajorVersion > 5);

  ReadAppGlobals();
}

static void WriteFileBody(bool forCmdLine)
{
  control.WriteToFile();
  layerSpec.WriteToFile();
  parameters.WriteToFile();
  sequence.WriteToFile();
  dataCapture.WriteToFile();
  optimize.WriteToFile();
  vary.WriteToFile();
  sample.WriteToFile();               // after all G_s with sample
  outputFiles.WriteToFile();

  fieldDataRoot.WriteToFile(forCmdLine);
  sequenceRoot.WriteToFile(forCmdLine);
  fitSpecRoot.WriteToFile(forCmdLine);
  processingSetupRoot.WriteToFile(forCmdLine);

#ifndef ADCONSOLEAPP
  interfaceContext.WriteToFile();
#endif

}


static void ReadFileBody(nPreRoot&  dummyRoot)
{
  control.ReadFromFile();
    //std::cout << "control OK" << std::endl;
  sample.PreReadSetup();              // before any sample vars
  if (currentCfgFileReadMajorVersion > 7)
    layerSpec.ReadFromFile();
  parameters.ReadFromFile();
  sequence.ReadFromFile();
  dataCapture.ReadFromFile();

  if (currentCfgFileReadMajorVersion > 0)
    optimize.ReadFromFile();
  if (currentCfgFileReadMajorVersion > 2)
    vary.ReadFromFile();
  if (currentCfgFileReadMajorVersion > 1)
    sample.ReadFromFile();
  outputFiles.ReadFromFile();

  fieldDataRoot.ReadFromFile();
  sequenceRoot.ReadFromFile();
  fitSpecRoot.ReadFromFile();
  if (currentCfgFileReadMajorVersion < 7)
    dummyRoot.ReadFromFile();
  processingSetupRoot.ReadFromFile();

  sequence.SetForFlags();
  parameters.SetForFlags();
  dataCapture.SetForFlags();
  outputFiles.SetForFlags();
  optimize.SetForFlags();
  vary.SetForFlags();

#ifndef ADCONSOLEAPP
  interfaceContext.ReadFromFile(currentCfgFileReadMajorVersion < 5);
#endif
}


bool nPreFileOperations::OpenFile(const char fName[])
{
  bool currPagesDeleted = false;
  bool fileReadOK = false;

  // will clean up when it leaves scope
  nPreRoot dummyRoot("Sampling", "Smp", "Sampling - Graphics");
  try
  {
    appConfigFile::OpenAsFile(fName, true);

    //  header
    ReadCFGHeader();

    //  clear all
    control.SetToInitial();
    layerSpec.SetToInitial();
    parameters.SetToInitial();
    sequence.SetToInitial();
    dataCapture.SetToInitial();
    outputFiles.SetToInitial();
    optimize.SetToInitial();
    vary.SetToInitial();
    sample.SetToInitial();
    optRangeOutputFO.SetToInitial();

#ifndef ADCONSOLEAPP
    MenuRootC::DestroyAll();
#endif

    currPagesDeleted = true;

    ReadFileBody(dummyRoot);
    fileReadOK = true;
  }
  catch (TextC::TextError re) {
    GenAppErrorMsg("OpenFile", re.errMsg);
  }
  appConfigFile::Close();


  if ( currPagesDeleted)
  {
    FuncObjC::UpdateAll();
    if (fileReadOK)
    {
      GenAppClearMsgs();
#ifndef ADCONSOLEAPP
      PlotDefC::InitializeAllPlots();
#endif
      FuncObjC::CalcAllObjects(foc_Full);

#ifndef ADCONSOLEAPP
      PlotDefC::ReplotAfterCalc();
      ExposedObjC::UpdateAllExposed();
      PlotPageC::MinimizeSubplots();
      MenuRootC::UpdateAllUI();
#endif
    }

  }

  DeallocFuncObjList();
  return fileReadOK;
}

void nPreFileOperations::SaveFile(const char fName[],
                                  bool forCmdLine)
{
#ifndef ADCONSOLEAPP
  // update menus
  AfxGetMainWnd()->SetFocus();
#endif
  try
  {
    appConfigFile::OpenAsFile(fName, false);
    WriteCFGHeader();
    WriteFileBody(forCmdLine);
#ifndef ADCONSOLEAPP
    projectUtilities::SetModifiedFlag(false);
#endif
  }
  catch (TextC::TextError re) {
    GenAppErrorMsg("OpenFile", re.errMsg);
  }
  appConfigFile::Close();
}
