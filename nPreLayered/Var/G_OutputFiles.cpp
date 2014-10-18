///////////////////////////////////////////////////////////////////////////////////
//
// G_OutputFiles.cpp
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

#include <genApp/G_FileNames.h>

#include <osMain/G_Optimize.h>
#include <osMain/G_Vary.h>

#include <Var/G_PreConfigFile.h>

#include <Var/G_Control.h>
#include <Var/G_DataCapture.h>
#include <Var/G_Sequence.h>

#include <Var/G_OptOutput.h>
#include <Var/G_ProfileOutput.h>
#include <Var/G_RangeOutput.h>
#include <Var/G_XYOutput.h>
#include <Var/G_mViewOutput.h>

#include <Var/G_OutputFiles.h>

namespace nsOutputFiles  {


// declarations
OutputFileDesc              xyForwardOutputDesc(xyForwardOutputFile.fileName);
OptOutputFileDesc           optOutputDesc(optOutputFile.fileName);
ProfileOutputFileDesc   profileOutputDesc(profileOutputFile.fileName);
RangeOutputFileDesc         rangeOutputDesc(rangeOutputFile.fileName);
mViewFileDesc       mviewOutputDesc;

OutputFilesStaticSupport    outputFiles;
OutputFilesErrorListing   outputFilesErrorListing;


//  *************************** OutputFileDesc

OutputFileDesc::OutputFileDesc(char* inF)
  : fileName(inF)
{
  SetToInitial("dummy");
  appFileNameData.AddTo(fileName);
}

void OutputFileDesc::SetToInitial(const char* defFname)
{
  writeFile = false;
  CopyString(fileName, defFname, stdFileStrLen);
  addExisting = true;
  CopyString(runID, "Run#1", runIDLen);
}


//  *************************** OptOutputFileDesc

OptOutputFileDesc::OptOutputFileDesc(char* inF)
  : OutputFileDesc(inF)
{
  // virtual set to initial called by base class -- nothing to do here
}

void OptOutputFileDesc::SetToInitial(const char* defFname)
{
  OutputFileDesc::SetToInitial(defFname);
  writeResiduals  = false;
  writeCovariance = true;
  writeJacobian   = false;
}

//  *************************** ProfileOutputFileDesc

ProfileOutputFileDesc::ProfileOutputFileDesc(char* inF)
  : OutputFileDesc(inF)
{
  // virtual set to initial called by base class -- nothing to do here
}

void ProfileOutputFileDesc::SetToInitial(const char* defFname)
{
  OutputFileDesc::SetToInitial(defFname);
  startSeq          = 0;
  endSeq            = -1;
  timeStepModulus   = 1;
  radiusNodeModulus = 1;
}

//  *************************** RangeOutputFileDesc

RangeOutputFileDesc::RangeOutputFileDesc(char* inF)
  : OutputFileDesc(inF)
{
  // virtual set to initial called by base class -- nothing to do here
}

void RangeOutputFileDesc::SetToInitial(const char* defFname)
{
  OutputFileDesc::SetToInitial(defFname);
}


mViewFileDesc::mViewFileDesc()
{
  SetToInitial();
  appFileNameData.AddTo(geomFileName);
  appFileNameData.AddTo(propFileName);
}

void mViewFileDesc::SetToInitial()
{
  writeFile = false;
  CopyString(geomFileName, "nSightsLayered.mGEO", stdFileStrLen);
  CopyString(propFileName, "nSightsLayered.mPRP", stdFileStrLen);
}

appFuncObjGlobals::AppFuncArray OutputFilesStaticSupport::outFileObj("Output");


OutputFilesListing::OutputFilesListing()
  : OSListObj("nPreOutputFiles")
{
}


void OutputFilesListing::CreateListing()
{
  StdTitle("OutputFiles");

  if (!control.IsRange())
  {
    ListFileDesc(xyForwardOutputDesc, "XY Forward Output");
    if (xyForwardOutputDesc.writeFile)
    {
      xyForwardOutputFO.DoStatusChk();
      AddStdText("Output data",   xyForwardOutputFO.xyDataArrayObjRef.objRef->GetID());
    }
  }

  if (control.IsOptimization() && (!control.IsRange()))
  {
    ListFileDesc(optOutputDesc, "Optimization Output");
    if (optOutputDesc.writeFile)
    {
      AddYesNo("Write residuals ?", optOutputDesc.writeResiduals);
      if (optimize.HasCovariance())
      {
        AddYesNo("Write Jacobian ?", optOutputDesc.writeJacobian);
        AddYesNo("Write covariance matrices?", optOutputDesc.writeCovariance);
      }
    }
  }

  if (control.IsRange())
  {
    ListFileDesc(rangeOutputDesc, "Range Output");
  }

  if (control.IsForward() && control.IsNormal())
  {
    ListFileDesc(profileOutputDesc, "Profile Output");

    if (profileOutputDesc.writeFile)
    {

      int startSeq = profileOutputDesc.startSeq;
      if ((startSeq < 0) || (startSeq >= sequence.GetNSingleSequence()))
        startSeq = 0;

      int endSeq = profileOutputDesc.endSeq;
      if ((endSeq < 0) || (endSeq >= sequence.GetNSingleSequence()) || (endSeq < startSeq))
        endSeq = sequence.GetNSingleSequence() - 1;

      if (singleSequenceData.IsValid(startSeq))
        AddStdText("Start sequence ", singleSequenceData.GetRef(startSeq).seqID);
      else
        AddStdText("Start sequence ", "invalid ??");

      if (singleSequenceData.IsValid(endSeq))
        AddStdText("End sequence ", singleSequenceData.GetRef(endSeq).seqID);
      else
        AddStdText("End sequence ", "invalid ??");

      AddStdInt("Time step modulus ", profileOutputDesc.timeStepModulus);
      AddStdInt("Node radius modulus ", profileOutputDesc.radiusNodeModulus);
    }
  }

  if (control.Is2DRadial())
  {
    SubTitle("mView Output Files");
    AddYesNo("Write file ?", mviewOutputDesc.writeFile);
    if (mviewOutputDesc.writeFile)
    {
      AddStdText("Geometry file", mviewOutputDesc.geomFileName);
      AddStdText("Property file", mviewOutputDesc.propFileName);
    }
  }
}

void  OutputFilesListing::ListFileDesc(const OutputFileDesc& desc,
                                       const char*       title)
{
  SubTitle(title);
  AddYesNo("Write file ?", desc.writeFile);
  if (desc.writeFile)
  {
    AddStdText("File name", desc.fileName);
    AddStdText("Run ID",  desc.runID);
    AddBoolText("If file exists", desc.addExisting, "Update", "Overwrite");
  }
}

OutputFilesErrorListing::OutputFilesErrorListing()
  :ErrorListObjC("OutputFiles")
{
}

void OutputFilesErrorListing::CreateListing()
{
  StdTitle("Simulation Output Files Setup Errors");

  SC_SetupErr filErr;
  const char* fileType;
  if (xyForwardOutputDesc.writeFile)
  {
    fileType = "XY Output ";
    FileDescErrorCheck(fileType, xyForwardOutputDesc);

    xyForwardOutputFO.DoStatusChk();
    if (xyForwardOutputFO.GetStatus() == os_Bad)
    {
      AddError("output object setup bad ???");
    }
    if (xyForwardOutputFO.GetStatus() == os_NotSet)
    {
      AddError("input data object setup bad");
    }
    if (control.IsOptimization())
    {
      if (optimize.doOptPerturbations && (!orderedVary.IsEmpty()))
      {
        AddError("can not write forward file for vary & perturbed");
      }
    }
  }

  if (optOutputDesc.writeFile)
  {
    fileType = "Opt Output ";
    FileDescErrorCheck(fileType, optOutputDesc);

    // best place for this ??
    if (control.IsSampleSim() && (optimize.doOptPerturbations))
    {
      filErr.SetInternalError(fileType, ":cannot do optimization perturbations with sampling");
      AddError(filErr);
    }
    else if ((!orderedVary.IsEmpty()) && (optimize.doOptPerturbations))
    {
      filErr.SetInternalError(fileType, ":cannot do optimization perturbations with suite/range parameters");
      AddError(filErr);
    }
  }

  if (profileOutputDesc.writeFile)
  {
    fileType = "Profile Output ";
    FileDescErrorCheck(fileType, profileOutputDesc);

    profileOutputFO.DoStatusChk();
    if (profileOutputFO.GetStatus() == os_Bad)
    {
      AddError("profile output object setup bad ???");
    }
    else if (profileOutputFO.GetStatus() == os_NotSet)
    {
      AddError("profile input data object setup bad");
    }

    if (control.IsOptimization() || (!control.IsNormal()))
    {
      AddError("can not write profile file for optimization, range, or sampling");
    }
  }

  if (rangeOutputDesc.writeFile)
  {
    fileType = "Range Output ";
    FileDescErrorCheck(fileType, rangeOutputDesc);

    // best place for this ??
    if (control.IsOptimization() && optimize.doOptPerturbations)
    {
      filErr.SetInternalError(fileType, ":cannot do optimization perturbations with range");
      AddError(filErr);
    }
  }

  if (control.Is2DRadial() && mviewOutputDesc.writeFile)
  {
    if (control.IsOptimization() || (!control.IsNormal()))
    {
      AddError("can not write mView files file for optimization, range, or sampling");
    }
  }

}


void OutputFilesErrorListing::FileDescErrorCheck(const char*           fileType,
    const OutputFileDesc& desc)
{
  SC_SetupErr fileErr;
  if (desc.writeFile)
  {
    if (IsNull(desc.fileName))
    {
      fileErr.SetInternalError(fileType, ":file name not set");
      AddError(fileErr);
    }
  }
}


//  ********************* OutputFilesStaticSupport

OutputFilesStaticSupport::OutputFilesStaticSupport()
{
  SetToInitial();

}

void OutputFilesStaticSupport::SetToInitial()
{
  xyForwardOutputDesc.SetToInitial("StdResults.nXYSim");
  xyForwardOutputFO.xyDataArrayObjRef   = FuncObjRef(capturedDataFO);
  xyForwardOutputFO.staticPressureObjRef  = FuncObjRef(capturedDataFO);
  xyForwardOutputFO.seqTimesObjRef    = FuncObjRef(sequenceTimesFO);

  optOutputDesc.SetToInitial("OptResults.nOpt");

  profileOutputDesc.SetToInitial("ProfileResults.nPro");
  profileOutputFO.seqTimesObjRef      = FuncObjRef(sequenceTimesFO);
  profileOutputFO.staticPressureObjRef  = FuncObjRef(capturedDataFO);

  rangeOutputDesc.SetToInitial("RangeResults.nRng");

  mviewOutputDesc.SetToInitial();
  mViewOutputFO.staticPressureObjRef = FuncObjRef(capturedDataFO);
}


void OutputFilesStaticSupport::SetForFlags()
{
  if (control.IsRange())
    xyForwardOutputDesc.writeFile = false;
  if (control.IsForward() || control.IsRange())
    optOutputDesc.writeFile = false;
  if (!control.IsRange())
    rangeOutputDesc.writeFile = false;
  if (control.IsOptimization() || (!control.IsNormal()))
  {
    profileOutputDesc.writeFile = false;
    mviewOutputDesc.writeFile = false;
  }
}

bool OutputFilesStaticSupport::CheckOutputFile(FileOutputControl& outputToCheck,
    bool&        errorDisplayed,
    const char*      outputDesc)
{
  bool addOK = true;
  if (outputToCheck.OutputSetup(addOK))
    return true;

  char errStr[80];
  if (addOK)
  {
    MakeString(errStr, outputDesc, " unanticipated file check error -- memory allocation ??", 80);
  }
  else
  {
    MakeString(errStr, outputDesc, " file incompatible -- cannot append", 80);
  }

  GenAppErrorMsg("FileCheck", errStr);
  errorDisplayed = true;

  return false;
}

bool OutputFilesStaticSupport::OutputFilesOK(bool&  errorDisplayed)
{
  errorDisplayed = false;
  if (!outputFilesErrorListing.ErrorCheckOK())
    return false;

  if (xyForwardOutputDesc.writeFile &&
      (!CheckOutputFile(xyForwardOutputFO, errorDisplayed, "XY")))
    return false;

  if (profileOutputDesc.writeFile &&
      (!CheckOutputFile(profileOutputFO, errorDisplayed, "Profile")))
    return false;

  if (optOutputDesc.writeFile &&
      (!CheckOutputFile(optOutputControl, errorDisplayed, "Optimiztion")))
    return false;

  if (rangeOutputDesc.writeFile &&
      (!CheckOutputFile(rangeOutputControl, errorDisplayed, "Range")))
    return false;

  return true;
}


void OutputFilesStaticSupport::InitAppGlobals()
{
  outFileObj.AddTo(&xyForwardOutputFO);
  outFileObj.AddTo(&profileOutputFO);
  appFuncObjGlobals::appFuncData += &outFileObj;
}

const char* OutputFilesStaticSupport::groupHeader = "OutputFiles";
const int  OutputFilesStaticSupport::majorVersion = 4;  // mView
//const int  OutputFilesStaticSupport::majorVersion = 3;  // range modification
//const int  OutputFilesStaticSupport::majorVersion = 2;  // global func
//const int  OutputFilesStaticSupport::majorVersion = 1;  // added other file output
//const int  OutputFilesStaticSupport::majorVersion = 0;
const int  OutputFilesStaticSupport::minorVersion = 0;

void OutputFilesStaticSupport::WriteToFile()
{
  WriteGroupHeader(groupHeader, majorVersion, minorVersion);

  // global objects
  WriteGlobalFunc(xyForwardOutputFO);
  WriteGlobalFunc(profileOutputFO);

  WriteFileDesc(xyForwardOutputDesc);
  WriteFuncObjRef(xyForwardOutputFO.xyDataArrayObjRef);

  // added opt ver 1
  WriteFileDesc(optOutputDesc);
  WriteBool(optOutputDesc.writeResiduals);
  WriteBool(optOutputDesc.writeJacobian);
  WriteBool(optOutputDesc.writeCovariance);
  WriteLine();

  WriteFileDesc(profileOutputDesc);
  WriteInt(profileOutputDesc.startSeq);
  WriteInt(profileOutputDesc.endSeq);
  WriteInt(profileOutputDesc.timeStepModulus);
  WriteInt(profileOutputDesc.radiusNodeModulus);
  WriteLine();

  WriteFileDesc(rangeOutputDesc);
  WriteLine();

  // v4 - mView files
  WriteBool(mviewOutputDesc.writeFile);
  WriteLine();
  WriteText(mviewOutputDesc.geomFileName);
  WriteText(mviewOutputDesc.propFileName);

}

bool OutputFilesStaticSupport::ReadFromFile()
{
  ReadGroupHeader(groupHeader, majorVersion);
  if (currentObjReadMajorVersion > 1)
  {
    ReadGlobalFunc(xyForwardOutputFO);
    ReadGlobalFunc(profileOutputFO);
  }

  ReadFileDesc(xyForwardOutputDesc);
  ReadFuncObjRef(xyForwardOutputFO.xyDataArrayObjRef);

  if (currentObjReadMajorVersion > 0)
  {
    ReadFileDesc(optOutputDesc);
    optOutputDesc.writeResiduals    = ReadBool();
    optOutputDesc.writeJacobian     = ReadBool();
    optOutputDesc.writeCovariance   = ReadBool();

    ReadFileDesc(profileOutputDesc);
    profileOutputDesc.startSeq          = ReadInt();
    profileOutputDesc.endSeq            = ReadInt();
    profileOutputDesc.timeStepModulus   = ReadInt();
    profileOutputDesc.radiusNodeModulus = ReadInt();

    ReadFileDesc(rangeOutputDesc);
    if (currentObjReadMajorVersion < 3)
    {
      bool writeFitValues       = ReadBool();
      bool writeFitComponents   = ReadBool();
      bool writeOptimizeResults = ReadBool();
      bool writeCovariance      = ReadBool();
    }
  }

  if (currentObjReadMajorVersion > 3)
  {
    mviewOutputDesc.writeFile = ReadBool();
    ReadFileName(mviewOutputDesc.geomFileName);
    ReadFileName(mviewOutputDesc.propFileName);
  }

  return true;
}


void OutputFilesStaticSupport::WriteFileDesc(const OutputFileDesc& desc)
{
  WriteBool(desc.writeFile);
  WriteBool(desc.addExisting);
  WriteLine();
  WriteText(desc.fileName);
  WriteText(desc.runID);
}

void  OutputFilesStaticSupport::ReadFileDesc(OutputFileDesc& desc)
{
  desc.writeFile = ReadBool();
  desc.addExisting = ReadBool();
  ReadFileName(desc.fileName);
  ReadText(desc.runID, OutputFileDesc::runIDLen);
}



};  // end namespace


