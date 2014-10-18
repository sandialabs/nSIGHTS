///////////////////////////////////////////////////////////////////////////////////
//
// RunControl.cpp
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
#include "Resource.h"

#include <genClass/U_Str.h>

#include <genApp/ProjectUtil.h>

#include <osMain/G_Sample.h>
#include <osMain/G_Optimize.h>
#include <osMain/G_Vary.h>
#include <osMain/G_OptRange.h>

#include <Var/G_Control.h>
#include <Var/G_Parameter.h>
#include <Var/G_Sequence.h>

#include <Var/G_OptOutput.h>
#include <Var/G_ProfileOutput.h>
#include <Var/G_RangeOutput.h>
#include <Var/G_XYOutput.h>
#include <Var/G_OutputFiles.h>

#include <Sim/C_SimCore.h>
#include <Sim/G_SimulatorRadiiOutput.h>

#include "RunCommon.h"
#include "RunControl.h"
#include "MainFrm.h"

#include "ProgressThreadDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static SampVarArray currSamp(false);           // for convenience


class CSimulationThreadDlg : public CProgressThreadDlg
{
public:
  DECLARE_DYNAMIC(CSimulationThreadDlg)

  CSimulationThreadDlg(UINT nProgressDlgID, CWnd* pParent = NULL);

public:
  bool      m_bDoubleLine;
  bool      m_bDoubleColumn;
  bool      m_bMultiCase;
  int       m_ndoubleCol;

protected:
  //{{AFX_DATA(CSimulationThreadDlg)
  //}}AFX_DATA

  virtual void  InitAdjustDialog();
  void      HideWnd(int nID);
  virtual void  SetTimeLabel();

  //{{AFX_VIRTUAL(CSimulationThreadDlg)
protected:
  //}}AFX_VIRTUAL

  //{{AFX_MSG(CSimulationThreadDlg)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

};

IMPLEMENT_DYNAMIC(CSimulationThreadDlg, CProgressThreadDlg)

CSimulationThreadDlg::CSimulationThreadDlg(UINT nProgressDlgID, CWnd* pParent) :
  CProgressThreadDlg(nProgressDlgID, pParent),
  m_bDoubleLine(false),
  m_bMultiCase(true),
  m_bDoubleColumn(false),
  m_ndoubleCol(0)
{
}

BEGIN_MESSAGE_MAP(CSimulationThreadDlg, CProgressThreadDlg)
  //{{AFX_MSG_MAP(CSimulationThreadDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSimulationThreadDlg::HideWnd(int nID)
{
  CWnd* pWnd = GetDlgItem(nID);
  if (pWnd != NULL) {
    pWnd->ShowWindow(SW_HIDE);
  }
}

void CSimulationThreadDlg::InitAdjustDialog()
{
  if ((!m_bMultiCase) && control.Is1DRadial())
  {
    CWnd* pWnd = GetDlgItem(IDC_PROGRESS);
    ASSERT(pWnd);
    pWnd->ShowWindow(SW_HIDE);
  }

  CRect rcDialog;
  GetWindowRect(rcDialog);

  CRect rect;
  if (m_nInfoElements == 0)
  {
    CWnd* pWnd = GetDlgItem(IDC_EDIT1);
    pWnd->GetWindowRect(rect);
    rcDialog.bottom = rect.top;
  }
  else
  {
    int nEditID = IDC_EDIT1 + m_nInfoElements;
    if (m_bDoubleLine)
      nEditID += m_nInfoElements;

    int nStaticID = IDC_STATIC1 + m_nInfoElements;
    int nUnitID = IDC_UNIT1 + m_nInfoElements;

    if (m_bDoubleColumn)
    {
      // kluge
      int maxStaticID = IDC_STATIC1 + m_ndoubleCol;
      int maxEditID = IDC_EDIT1 + m_ndoubleCol;
      if (m_bDoubleLine)
        maxEditID += m_ndoubleCol;
      int maxUnitID = IDC_UNIT1 + m_ndoubleCol;

      // hide remaining
      int i;
      for (i = nStaticID; i < maxStaticID; i++)
        HideWnd(i);
      for (i = nEditID; i < maxEditID; i++)
        HideWnd(i);
      for (i = nUnitID; i < maxUnitID; i++)
        HideWnd(i);
    }
    else
    {

      // Hide next row (if there is such)
      HideWnd(nStaticID);
      HideWnd(nUnitID);
      HideWnd(nEditID);
      if (m_bDoubleLine) {
        HideWnd(nEditID+1);
      }

      CWnd* pWnd = GetDlgItem(nEditID - 1);
      pWnd->GetWindowRect(rect);
      rcDialog.bottom = rect.bottom + 16;
    }
  }
  MoveWindow(rcDialog);
}

void CSimulationThreadDlg::SetTimeLabel()
{
  if (m_bMultiCase || control.Is2DRadial()) {
    // Estimated time remaining xx:xx:xx
    if (!IsReady()) {
      return;
    }

    CWnd* pWnd = GetDlgItem(IDC_ELAPSED_TIME);
    if (pWnd)
    {
//    ASSERT(pWnd);

      CTimeSpan ts = CTime::GetCurrentTime() - m_timeStart;
      CString sText = ts.Format("Time elapsed   %D:%H:%M:%S");
      if (m_nStep != 0) {
        LONG nSecondsRemaining = LONG(double(ts.GetTotalSeconds()) / m_nStep * (m_nSteps-m_nStep));
        CTimeSpan ts2(nSecondsRemaining);
        CString sText2 = ts2.Format("   Est. time remaining   %D:%H:%M:%S");
        sText += sText2;
      }
      pWnd->SetWindowText(sText);
    }
  }
  else
  {
    CProgressThreadDlg::SetTimeLabel();
  }
}

static CProgressThreadDlg* progressDlg = NULL;

static void SendReplotMsg()
{
  if (progressDlg != NULL && progressDlg->IsReady()) {

    COutlookFrame::DoReplot("nPre");

    /*    if (replotThreadEvent == NULL)
        {
          long hInst = long(AfxGetInstanceHandle());
          CString sEvent;
          sEvent.Format("nPre%ld",hInst );
          replotThreadEvent = ::CreateEvent(NULL, FALSE, FALSE, sEvent);
        }

            ::SendNotifyMessage(hWndSendReplot, WM_REPLOT, NULL, NULL);

        // don't resume until replot is complete
        ::WaitForSingleObject(replotThreadEvent, INFINITE);
    */
  }
}


static int          nforwardRun;
static int          forwardUpdateNum;
static int          forwardUpdateInterval = 100;
static bool         forwardEditsSet;
static bool         singleForward2D;

// this will get called every time step
static void ForwardUpdateProc()
{
  if (!singleForward2D)
  {
    forwardUpdateNum++;
    if (forwardUpdateNum < forwardUpdateInterval)
      return;
    forwardUpdateNum = 0;
  }
  else
    nforwardRun++;  // actually time step

  if (progressDlg == NULL)
  {
    char tempStr[80];
    char iStr[80];
    IntToString(nforwardRun, iStr, 40);
    MakeString(tempStr, "Run #" , iStr, 80);
    GenAppInfoMsg("NoThreads", tempStr);
    return; // no threads run
  }

  // we need to check for cancel button status and if found throw an exception
  if (progressDlg->HasBeenCancelled()) {
    throw SimError("Simulation has been cancelled", SimError::seUserAbort);
  }

  progressDlg->SetProgressStep(nforwardRun);

  if (!forwardEditsSet)
  {
    CString sValue;
    SC_SciFormat rf(5);

    for (int i = 0; i < progressDlg->GetNumberInfoElements(); i++) {
      if (control.IsSampleSim())
      {
        rf.RealToString(currSamp[i]->GetFinalSampleVal(), sValue);
      }
      else
      {
        rf.RealToString(orderedVary[i]->GetFinalVaryVal(), sValue);
      }
      progressDlg->SetEdit(i, sValue);
    }
    forwardEditsSet = true;
  }
}

static bool RunForwardCase(RunnableC& gtfmRun,
                           int       caseIndex)
{
  forwardUpdateNum = 0;
  forwardEditsSet = false;
  nforwardRun++;

  if (control.IsSampleSim())
  {
    SampVar::SetTrialIndex(caseIndex);
  }
  else
  {
    if (orderedVary.IsNotEmpty())
      vary.SetVaryIndexes(caseIndex);
  }

  SC_SetupErr errData;
  if (!gtfmRun.SetupForRun(errData))
  {
    GenAppErrorMsg("RunSetup", errData.GetErrorText());
    return false;
  }

  try
  {

    if (progressDlg == NULL)
    {
      char msgStr[80];
      ConcatInt(caseIndex, "Case", msgStr, 4, 80);
      GenAppInfoMsg("NoThreads", msgStr);
    }

    xyForwardOutputFO.SetCaseIndex(caseIndex);
    gtfmRun.Execute();
    gtfmRun.FinalUpdate();
  }
  catch (SimError& err)
  {
    GenAppErrorMsg("RunTime", err.errStr);
    return false;
  }

  SendReplotMsg();
  return true;
}

static bool simOK;

static void ForwardRangeReplot()
{
  // update plots after each iteration if necessary
  if (optProgressOutput.DownstreamRequired() || simulatorRadiiFO.DownstreamRequired())
    SendReplotMsg();
}


static void DoAllForwardCases()
{
  nforwardRun = 0;

  RunnableC* gtfmRun = SimulatorCore::GetSimulator();

  gtfmRun->RegisterFinalFlag(xyForwardOutputFO.doingFinalUpdate);

  if (!gtfmRun->AllocateAll(ForwardUpdateProc))
  {
    GenAppErrorMsg("RunAlloc", "allocation failure");
    simOK = false;
  }
  else
  {
    int maxRuns = GetnForwardRuns();
    simOK = true;
    for (int i = 0; simOK && (i < maxRuns); i++)
    {
      simOK = RunForwardCase(*gtfmRun, i);
      if (simOK && control.IsRange())
      {
        optRangeOutputFO.AddForwardRun();
        if (optRangeOutputFO.DownstreamRequired())
        {
          optRangeOutputFO.CalcAndUpdate();
          SendReplotMsg();
        }
      }
    }
  }

  delete gtfmRun;

}

ULONG ForwardProgressFunction(void* pData, CProgressThreadDlg* pProgressDlg)
{
  progressDlg = pProgressDlg;
  DoAllForwardCases();

  return 0; // close progress dialog
}


static void ForwardRun(CSimulationThreadDlg& dlg)
{
  COutlookFrame::StartReplotProcessing();

  int nRuns = GetnForwardRuns();
  dlg.m_bMultiCase = nRuns > 1;
  singleForward2D = (nRuns == 1) && control.Is2DRadial();
  if (singleForward2D)
    nRuns = sequence.GetTotalNTimeStep();

  if (!dlg.ExecuteFunctionWithProgressDialog(ForwardProgressFunction,
      "Simulation", nRuns, NULL, projectUtilities::GetCalculationPriority()))
  {
    // thread hasn't been created
    // Produce warning message and run with wait cursor
    CWaitCursor w;
    DoAllForwardCases();
  }

  COutlookFrame::EndReplotProcessing();

  progressDlg = NULL;

  if (simOK)                        // OK to update output
  {
    xyForwardOutputFO.SimulationComplete();
    profileOutputFO.SimulationComplete();
    rangeOutputControl.SimulationComplete();

    CTimeSpan ts = dlg.m_timeEnd - dlg.m_timeStart;
    CString sText = ts.Format("Completed successfully. Elapsed time   %D:%H:%M:%S");
    GenAppInfoMsg("Simulation", sText);
  }
  else
  {
    xyForwardOutputFO.SimulationAbort();
    profileOutputFO.SimulationAbort();
    rangeOutputControl.SimulationAbort();
  }
}


static void ForwardVerbose()
{
  CSimulationThreadDlg dlg(IDD_X_NORMPROGRESS, AfxGetMainWnd());

  int nInfoElements;
  if (control.IsSampleSim())
    nInfoElements = SampVar::GetnActive();
  else
    nInfoElements = orderedVary.Size();

  if (nInfoElements > 8)              // max info elements in resource
    nInfoElements = 8;

  dlg.SetElements(nInfoElements, nInfoElements);

  for (int i = 0; i < nInfoElements; i++) {
    if (control.IsSampleSim())
    {
      dlg.SetLabelText(i, currSamp[i]->GetLongID());
      // kluge
      SampVarUnits* unitSamp = dynamic_cast<SampVarUnits*>(currSamp[i]);
      if (unitSamp)
      {
        dlg.SetUnitText(i, UnitsBase::GetUnitString(unitSamp->unitIndex.unitType,
                        unitSamp->unitIndex.GetConvIndex()));
      }
    }
    else
    {
      dlg.SetLabelText(i, orderedVary[i]->GetLongID());
      VaryVarUnits* unitVary = dynamic_cast<VaryVarUnits*>(orderedVary[i]);
      if (unitVary)
      {
        dlg.SetUnitText(i, UnitsBase::GetUnitString(unitVary->unitIndex.unitType,
                        unitVary->unitIndex.GetConvIndex()));
      }
    }
  }

  // debug single non-lin runs
  gasDebugFlag = orderedVary.IsEmpty();

  ForwardRun(dlg);
}


static void ForwardNoThreads()
{
  progressDlg = NULL;
  int nRuns = GetnForwardRuns();

#ifdef _DEBUG
  gasDebugFlag = true;
#endif

  DoAllForwardCases();

  if (simOK)                        // OK to update output
  {
    xyForwardOutputFO.SimulationComplete();
    profileOutputFO.SimulationComplete();
    rangeOutputControl.SimulationComplete();
  }
  else
  {
    xyForwardOutputFO.SimulationAbort();
    profileOutputFO.SimulationAbort();
    rangeOutputControl.SimulationAbort();
  }
}

static void ForwardMinimal()
{
  CSimulationThreadDlg dlg(IDD_X_NORMPROGRESS, AfxGetMainWnd());
  dlg.SetElements(0, 0);
  ForwardRun(dlg);
}

//
// ************************************************************************** optimization
//

static int  noptRun;


// common to all opt update procs ....
static bool OptimizeUpdateStart()
{
  if (progressDlg == NULL) {
    return false; // no thread run
  }

  // we need to check for cancel button status and if found throw an exception
  if (progressDlg->HasBeenCancelled()) {
    throw SimError("Optimization has been cancelled", SimError::seUserAbort);
  }

  return true;
}

static void OptimizeUpdateReplot()
{
  // update plots after each iteration if necessary
  if (optProgressOutput.DownstreamRequired() || simulatorRadiiFO.DownstreamRequired())
    SendReplotMsg();
}


// common to single & multiple run verbose opt
static void VerboseOptimizeCommon(Optimizer& opt)
{
  int nInfo = progressDlg->GetNumberInfoElements();
  if (nInfo > opt.noptVars) // this should be unnecessary as m_nInfoElements should be set correctly
    nInfo = opt.noptVars;

  CString sValueBest; // best
  CString sValueCurr;
  for (int i = 0; i < nInfo; i++)
  {
    OptVar* tmpOpt = static_cast<OptVar*>(opt.varsToOptimize[i]);  // should be safe for all nSights opt vars
    OptVarUnits* unitOpt = dynamic_cast<OptVarUnits*>(tmpOpt);
    if (unitOpt)
    {
      unitOpt->unitIndex.GetUserString(opt.bestParEst[i], sValueBest);         // best
      unitOpt->unitIndex.GetUserString(tmpOpt->GetParEstimate(), sValueCurr);  // current
    }
    else
    {
      SC_SciFormat tmp(6);
      tmp.RealToString(opt.bestParEst[i], sValueBest);
      tmp.RealToString(tmpOpt->GetParEstimate(), sValueCurr);
    }

    progressDlg->SetEdit(i*2,   sValueBest);
    progressDlg->SetEdit(i*2+1, sValueCurr);
  }

  // ResourceID               Value                   Type
  //
  // IDC_OPT_MESSAGE          opt.opMessage           const char*
  progressDlg->ShowEditField(IDC_OPT_MESSAGE, opt.opMessage);

  // IDC_NSIMULATION          opt.runCount            int
  CString sNumberOfSimulations;
  sNumberOfSimulations.Format("%d", opt.runCount);
  progressDlg->ShowEditField(IDC_NSIMULATION, sNumberOfSimulations);

  CString sNumberOfIterations;
  sNumberOfIterations.Format("%d", opt.optIterCount);
  progressDlg->ShowEditField(IDC_NITERATION, sNumberOfIterations);

  OptimizeUpdateReplot();
}


// for multiple verbose runs
static void MultVerboseOptimizeUpdateProc(Optimizer& opt)
{
  if (!OptimizeUpdateStart())
    return;

  progressDlg->SetProgressStep(noptRun);

  VerboseOptimizeCommon(opt);
}

// for single verbose runs
static void SnglVerboseOptimizeUpdateProc(Optimizer& opt)
{
  if (!OptimizeUpdateStart())
    return;

  SC_SciFormat rForm(5);
  char tmpStr[40];
  // IDC_BESTFIT              opt.bestFitVal          double      in sci notation 5 dec
  rForm.RealToString(opt.bestFitVal, tmpStr, 40);
  progressDlg->ShowEditField(IDC_BESTFIT, tmpStr);

  // IDC_CURRFIT              opt.currFitVal          double      in sci notation 5 dec
  rForm.RealToString(opt.currFitVal, tmpStr, 40);
  progressDlg->ShowEditField(IDC_CURRFIT, tmpStr);

  // tolerances
  rForm.RealToString(opt.currFitTolerance, tmpStr, 40);
  progressDlg->ShowEditField(IDC_FIT_TOLERANCE, tmpStr);

  rForm.RealToString(opt.currParameterTolerance, tmpStr, 40);
  progressDlg->ShowEditField(IDC_PARAMETER_TOLERANCE, tmpStr);

  rForm.RealToString(opt.currErrorTolerance, tmpStr, 40);
  progressDlg->ShowEditField(IDC_ERROR_TOLERANCE, tmpStr);

  VerboseOptimizeCommon(opt);
}


static void MinimalOptimizeUpdateProc(Optimizer& opt)
{
  if (!OptimizeUpdateStart())
    return;

  progressDlg->SetProgressStep(noptRun);

  // update plots after each iteration if necessary
  OptimizeUpdateReplot();
}

static bool RunOptCase(const char* runID,
                       int      caseIndex,
                       int&       ncaseOK)
{
  optOutputControl.NextCaseSetup(caseIndex);
  if (control.IsRange())
  {
    optRangeOutputFO.FirstOptRunSetup();
  }

  xyForwardOutputFO.SetCaseIndex(caseIndex);

  int fitNumber = 0;
  for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
  {
    if (optimizeInput.inputFits[i].optimizeOnFit)
    {
      const char* fitID = optimizeInput.inputFits[i].fitObject.objRef->GetID();

      char titleStr[80];
      MakeString(titleStr, "Optimization: ", runID, " Fit: ", fitID, 80);
      if (progressDlg != NULL)
        progressDlg->ChangeTitle(titleStr);
#ifndef _DEBUG
      else
        GenAppInfoMsg("NoThreads", titleStr);
#endif

      xyForwardOutputFO.SetFitData(fitID, fitNumber);

      int lastNrows = optCompleteOutput.GetNRows();
      try
      {
        optPack.MultiStageOptRun(i, caseIndex, fitNumber);

        optOutputControl.NextFitComplete(false, *optPack.finalStageOpt);

        ncaseOK++;

        if (control.IsRange())
        {
          optRangeOutputFO.AddOptRun(*optPack.finalStageOpt, true);
          if (optRangeOutputFO.DownstreamRequired())
          {
            optRangeOutputFO.CalcAndUpdate();
            SendReplotMsg();
          }
        }
      }
      catch (SimError& err)
      {
        // abort - rethrow
        if (err.IsUserAbort())
        {
          // kluge to reset on abort
          ParameterSupport::ResetPointsParameters();
          throw err;
        }

        char failPct[40];
        SC_DecFormat rf(2);
        rf.RealToString(double(ncaseOK) / double(noptRun) * 100.0, failPct, 40);

        char failMsg[80];
        MakeString(failMsg, err.errStr, " -- ", failPct, "% fail", 80);
        // show message
        GenAppErrorMsg(fitID, failMsg);

//                if ((ratioOK < 0.50) && (noptRun > 10))
//                    throw SimError("more than 50% failure rate");

        BadOptFinish(lastNrows);
        optPack.lastErr = err;

        if (control.IsRange())
        {
          optRangeOutputFO.AddOptRun(*optPack.finalStageOpt, false);
          if (optRangeOutputFO.DownstreamRequired())
          {
            optRangeOutputFO.CalcAndUpdate();
            SendReplotMsg();
          }
        }
      }

      fitNumber++;

      if (!OptimizationControlStatics::updateParameterEstimates)
        ParameterSupport::ResetPointsParameters();

      noptRun++;

      if (optCompleteOutput.DownstreamRequired())
        SendReplotMsg();
    }
  }

  return optOutputControl.NextCaseComplete();
}

static bool RunPerturbedOpt()
{
  int nper = OptimizationControlStatics::nPerturb;
  optPack.firstStageOpt->PerturbationSetup();
  try
  {
    int nperturbOK = 0;
    for (int j = 0; j < nper; j++)
    {
      xyForwardOutputFO.SetCaseIndex(j);

      char runID[80];
      CopyString(runID, "Pert# ", 80);
      ConcatInt(j + 1, runID, 3, 80);

      if (!RunOptCase(runID, j, nperturbOK))
        return false;
    }
  }
  catch (SimError& err)
  {
    GenAppErrorMsg("PertOptRun", err.errStr);
    return false;
  }

  return true;
}

static bool RunSingleOpt()
{
  bool retVal = true;

  try
  {
    optOutputControl.NextCaseSetup(0);
    int fitNumber = 0;
    for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
    {
      if (optimizeInput.inputFits[i].optimizeOnFit)
      {
        const char* fitID = optimizeInput.inputFits[i].fitObject.objRef->GetID();

        char titleStr[80];
        MakeString(titleStr, "Optimization Fit ID: ", fitID, 80);
        if (progressDlg != NULL)
          progressDlg->ChangeTitle(titleStr);

        xyForwardOutputFO.SetFitData(fitID, fitNumber);

        int lastNrows = optCompleteOutput.GetNRows();
        try
        {
          optPack.MultiStageOptRun(i, 0, 0);
          optOutputControl.NextFitComplete(false, *optPack.finalStageOpt);

        }
        catch (SimError& err)
        {
          // abort - rethrow
          if (err.IsUserAbort())
          {
            // kluge to reset on abort
            ParameterSupport::ResetPointsParameters();
            throw err;
          }
          GenAppErrorMsg(fitID, err.errStr);

          BadOptFinish(lastNrows);
          // otherwise  on to next fit

          retVal = false;
          optPack.lastErr = err;
        }

        if (!OptimizationControlStatics::updateParameterEstimates)
          ParameterSupport::ResetPointsParameters();

        if (optCompleteOutput.DownstreamRequired())
          SendReplotMsg();

        fitNumber++;
        noptRun++;
      }
    }

    retVal = optOutputControl.NextCaseComplete() && retVal;

  }
  catch (SimError& err)
  {
    GenAppErrorMsg("SnglOptRun", err.errStr);
    retVal = false;
  }

  return retVal;
}


static bool RunSampledOpt()
{
  int nsamp = SampVar::GetnTrials();
  try
  {
    int nsampOK = 0;
    for (int j = 0; j < nsamp; j++)
    {
      char runID[80];
      ConcatInt(j + 1, "Sample#", runID, 3, 80);

      SampVar::SetTrialIndex(j);
      if (!RunOptCase(runID, j, nsampOK))
        return false;
    }
  }
  catch (SimError& err)
  {
    GenAppErrorMsg("SampOptRun", err.errStr);
    return false;
  }
  return true;
}


static bool RunVaryingOpt()
{
  try
  {
    int nvaryOK = 0;
    int nvary = vary.GetnVaryCombinations();
    for (int i = 0; i < nvary; i++)
    {
      vary.SetVaryIndexes(i);

      char runID[80];
      ConcatInt(i + 1, "Vary#", runID, 3, 80);

      if (!RunOptCase(runID, i, nvaryOK))
      {
        rangeOutputControl.SimulationComplete();
        return false;
      }
    }
    rangeOutputControl.SimulationComplete();
  }
  catch (SimError& err)
  {
    GenAppErrorMsg("VaryOptRun", err.errStr);
    rangeOutputControl.SimulationAbort();
    return false;
  }

  return true;
}

static void DummyUpdateProc()
{
}

static bool optOK;

static Optimizer::UpdateCB  optUpdateProc;


static void DoAllOptRuns()
{
  noptRun = 0;
  optCompleteOutput.Setup();
  optCompleteOutput.CalcDownStreamObjects();  // clear dependent input

  RunnableC* gtfmRun = SimulatorCore::GetSimulator() ;
  gtfmRun->RegisterFinalFlag(xyForwardOutputFO.doingFinalUpdate);

  optPack.SetOptimizers();

  optOK = false;
  bool dummy;
  if (!gtfmRun->AllocateAll(DummyUpdateProc))
  {
    GenAppErrorMsg("OptInit", "simulator allocation failure");
  }
  else if (!optPack.Initialize(optUpdateProc, *gtfmRun))
  {
    // error msg displayed in optPack
  }
  else if (!optOutputControl.OutputSetup(dummy))
  {
    GenAppErrorMsg("OptInit", "output setup failure");
  }
  else
  {
    if (OptimizationControlStatics::doOptPerturbations)
    {
      optOK = RunPerturbedOpt();
    }
    else if (control.IsSampleSim())
    {
      optOK = RunSampledOpt();
    }
    else if (orderedVary.IsEmpty())
    {
      optOK = RunSingleOpt();
    }
    else
    {
      optOK = RunVaryingOpt();
    }

    xyForwardOutputFO.SimulationComplete();

    // redisplay error message
    if (optPack.lastErr.IsAppError())
      GenAppErrorMsg("Last fit", optPack.lastErr.errStr);

  }
//  optPack.ClearOptimizers();
  delete gtfmRun;

}

static bool verboseOptRun;  // a bit of a kluge

ULONG OptProgressFunction(void* pData, CProgressThreadDlg* pProgressDlg)
{
  progressDlg = pProgressDlg;

  DoAllOptRuns();

  if (verboseOptRun && progressDlg != NULL && progressDlg->IsReady()) {
    progressDlg->ShowEditField(IDC_OPT_MESSAGE,   "Finished");
  }

  return verboseOptRun; // don't close progress dialog
}


static void OptimizationRun(CSimulationThreadDlg& dlg)
{
  COutlookFrame::StartReplotProcessing();

  if (!dlg.ExecuteFunctionWithProgressDialog(OptProgressFunction,
      "Optimization", GetnOptFits(), NULL, projectUtilities::GetCalculationPriority()))
  {
    // thread hasn't been created
    // so we run just with wait cursor
    CWaitCursor w;
    DoAllOptRuns();
  }

  COutlookFrame::EndReplotProcessing();
  progressDlg = NULL;

  if (optOK)                          // OK to update output
  {
    CTimeSpan ts = dlg.m_timeEnd - dlg.m_timeStart;
    CString sText = ts.Format("Completed successfully. Elapsed time   %D:%H:%M:%S");
    GenAppInfoMsg("Optimize", sText);
  }
}

static void OptVerbose()
{
  bool bMultiRun = GetnOptFits() > 1;
  verboseOptRun = true;

  int maxInfoElements = 12;  // # of fields in normal verbose
  bool veryVerbose = false;

  UINT nIDTemplate;
  if (bMultiRun)
    nIDTemplate = IDD_X_MULTOPTVERBOSE;
  else if (OptVar::GetnActive() > maxInfoElements)
  {
    nIDTemplate = IDD_X_SNGLOPTVERYVERBOSE;
    maxInfoElements = 24;
    veryVerbose = true;
  }
  else
    nIDTemplate = IDD_X_SNGLOPTVERBOSE;

  CSimulationThreadDlg dlg(nIDTemplate, AfxGetMainWnd());

  int nInfoElements = OptVar::GetnActive();
  if (nInfoElements > maxInfoElements)
    nInfoElements = maxInfoElements;


  dlg.SetElements(nInfoElements, nInfoElements * 2);
  dlg.m_bDoubleLine = true;
  dlg.m_bMultiCase = bMultiRun;

  if (veryVerbose)
  {
    dlg.m_bDoubleColumn = true;
    dlg.m_ndoubleCol = 24;
  }

  // reset titles
  OptVarArray currOpts;
  char unitStr[40];
  for (int i = 0; i < currOpts.Size(); i++)
  {
    OptVarUnits* unitOpt = dynamic_cast<OptVarUnits*>(currOpts[i]);
    if (unitOpt)
      unitOpt->unitIndex.MakeUnitString(unitStr, 40, true);
    else
      CopyString(unitStr, "pts ", 40);

    if (veryVerbose)
      dlg.SetLabelText(i, currOpts[i]->GetShortID());
    else
      dlg.SetLabelText(i, currOpts[i]->GetLongID());
    dlg.SetUnitText(i, unitStr);
  }

  if (bMultiRun)
  {
    optUpdateProc = MultVerboseOptimizeUpdateProc;
    OptimizationRun(dlg);
  }
  else
  {
    optUpdateProc = SnglVerboseOptimizeUpdateProc;
    OptimizationRun(dlg);
  }
}


static void NoThreadsOptimizeUpdateProc(Optimizer& opt)
{
#ifdef _DEBUG
  char iStr[40];
  char tempStr[80];
  IntToString(opt.runCount, iStr, 40);
  MakeString(tempStr, opt.opMessage," Count: ", iStr, 80);
  GenAppInfoMsg("Debug", tempStr);
#endif
}

static void OptNoThreads()
{
  progressDlg = NULL;
  optUpdateProc = NoThreadsOptimizeUpdateProc;
  DoAllOptRuns();
}


static void OptMinimal()
{
  bool bMultiRun = GetnOptFits() > 1;
  verboseOptRun = false;

  CSimulationThreadDlg dlg(IDD_X_NORMPROGRESS, AfxGetMainWnd());
  dlg.SetElements(0, 0);
  dlg.m_bMultiCase = bMultiRun;

  optUpdateProc = MinimalOptimizeUpdateProc;
  OptimizationRun(dlg);
}

static void DoFinalOptRun()
{
  noptRun = 0;
  optCompleteOutput.Setup();
  optCompleteOutput.CalcDownStreamObjects();  // clear dependent input

  RunnableC* gtfmRun = SimulatorCore::GetSimulator();
  gtfmRun->RegisterFinalFlag(xyForwardOutputFO.doingFinalUpdate);

  Optimizer opt;
  optPack.lastOptRun = &opt;

  optOK = false;
  bool dummy;
  if (!gtfmRun->AllocateAll(DummyUpdateProc))
  {
    GenAppErrorMsg("OptInit", "simulator allocation failure");
  }
  else if (!opt.Initialize(optUpdateProc, *gtfmRun))
  {
    GenAppErrorMsg("OptInit", "optimizer initialization failure");
  }
  else if (!optOutputControl.OutputSetup(dummy))
  {
    GenAppErrorMsg("OptInit", "output setup failure");
  }
  else
  {
    optOK = true;
    try
    {
      optOutputControl.NextCaseSetup(0);
      int fitNumber = 0;
      for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
      {
        if (optimizeInput.inputFits[i].optimizeOnFit)
        {
          const char* fitID = optimizeInput.inputFits[i].fitObject.objRef->GetID();

          char titleStr[80];
          MakeString(titleStr, "Optimization Fit ID: ", fitID, 80);
          progressDlg->ChangeTitle(titleStr);

          xyForwardOutputFO.SetFitData(fitID, fitNumber);

          int lastNrows = optCompleteOutput.GetNRows();
          try
          {
            opt.FinalCovarOnly(i);
            optOutputControl.NextFitComplete(false, opt);
          }
          catch (SimError& err)
          {
            // abort - rethrow
            if (err.IsUserAbort())
            {
              // kluge to reset on abort
              ParameterSupport::ResetPointsParameters();
              throw err;
            }

            GenAppErrorMsg(fitID, err.errStr);

            BadOptFinish(lastNrows);
            // otherwise  on to next fit

            optOK = false;
            optPack.lastErr = err;
          }
          if (optCompleteOutput.DownstreamRequired())
            SendReplotMsg();

          noptRun++;
          fitNumber++;
        }
      }

      optOK = optOutputControl.NextCaseComplete() && optOK;

    }
    catch (SimError& err)
    {
      GenAppErrorMsg("SnglOptRun", err.errStr);
      optOK = false;
    }

    xyForwardOutputFO.SimulationComplete();

    // redisplay error message
    if (optPack.lastErr.IsAppError())
      GenAppErrorMsg("Last fit", optPack.lastErr.errStr);
  }

  delete gtfmRun;
}


ULONG FinalOptProgressFunction(void* pData, CProgressThreadDlg* pProgressDlg)
{
  progressDlg = pProgressDlg;

  DoFinalOptRun();

  bool multiRun = optimizeInput.GetNFits() > 1;

  if (multiRun && progressDlg != NULL && progressDlg->IsReady())
  {
    progressDlg->ShowEditField(IDC_OPT_MESSAGE,   "Finished");
  }

  return multiRun;
}


static void FinalOptimizationRun(CSimulationThreadDlg& dlg)
{
  COutlookFrame::StartReplotProcessing();

  if (!dlg.ExecuteFunctionWithProgressDialog(FinalOptProgressFunction,
      "Final Opt", optimizeInput.GetNFits(), NULL, projectUtilities::GetCalculationPriority()))
  {
    // thread hasn't been created
    // so we run just with wait cursor
    CWaitCursor w;
    DoFinalOptRun();
  }

  COutlookFrame::EndReplotProcessing();
  progressDlg = NULL;

  if (optOK)                          // OK to update output
  {
    CTimeSpan ts = dlg.m_timeEnd - dlg.m_timeStart;
    CString sText = ts.Format("Completed successfully. Elapsed time   %D:%H:%M:%S");
    GenAppInfoMsg("Final Opt", sText);
  }
}


static void OptFinalUpdateProc(Optimizer& opt)
{
  if (!OptimizeUpdateStart())
    return;

  SC_SciFormat rForm(5);
  char tmpStr[40];
  // IDC_DERIVSPAN              opt.bestFitVal          double      in sci notation 5 dec
  rForm.RealToString(opt.optDerivSpan, tmpStr, 40);
  progressDlg->ShowEditField(IDC_DERIVSPAN, tmpStr);


  int nInfo = progressDlg->GetNumberInfoElements();
  if (nInfo > opt.noptVars) // this should be unnecessary as m_nInfoElements should be set correctly
    nInfo = opt.noptVars;

  CString sValueCurr;
  for (int i = 0; i < nInfo; i++)
  {
    OptVar* tmpOpt = static_cast<OptVar*>(opt.varsToOptimize[i]);  // should be safe for all nSights opt vars
    OptVarUnits* unitOpt = dynamic_cast<OptVarUnits*>(tmpOpt);
    if (unitOpt)
    {
      unitOpt->unitIndex.GetUserString(tmpOpt->GetParEstimate(), sValueCurr);  // current
    }
    else
    {
      SC_SciFormat tmp(6);
      tmp.RealToString(tmpOpt->GetParEstimate(), sValueCurr);
    }

    progressDlg->SetEdit(i, sValueCurr);
  }

  // ResourceID               Value                   Type
  //
  // IDC_OPT_MESSAGE          opt.opMessage           const char*
  progressDlg->ShowEditField(IDC_OPT_MESSAGE, opt.opMessage);

  // IDC_NSIMULATION          opt.runCount            int
  CString sNumberOfSimulations;
  sNumberOfSimulations.Format("%d", opt.runCount);
  progressDlg->ShowEditField(IDC_NSIMULATION, sNumberOfSimulations);

  CString sNumberOfIterations;
  sNumberOfIterations.Format("%d", opt.optIterCount);
  progressDlg->ShowEditField(IDC_NITERATION, sNumberOfIterations);


  progressDlg->SetProgressStep(noptRun);
}


static void OptFinal()
{

  CSimulationThreadDlg dlg(IDD_X_FINALOPT, AfxGetMainWnd());

  int nInfoElements = OptVar::GetnActive();
  if (nInfoElements > 12)
    nInfoElements = 12;

  dlg.SetElements(nInfoElements, nInfoElements);
  dlg.m_bDoubleLine = false;
  dlg.m_bMultiCase = optimizeInput.GetNFits() > 1;

  // reset titles
  OptVarArray currOpts;
  char unitStr[40];
  for (int i = 0; i < currOpts.Size(); i++)
  {
    OptVarUnits* unitOpt = dynamic_cast<OptVarUnits*>(currOpts[i]);
    if (unitOpt)
      unitOpt->unitIndex.MakeUnitString(unitStr, 40, true);
    else
      CopyString(unitStr, "pts ", 40);

    dlg.SetLabelText(i, currOpts[i]->GetLongID());
    dlg.SetUnitText(i, unitStr);
  }

  optUpdateProc = OptFinalUpdateProc;
  FinalOptimizationRun(dlg);

}

static void CommonSetup()
{
  if (control.IsSampleSim())
    SampVar::GetActiveArray(currSamp);

  progressDlg  = NULL;
  gasDebugFlag = false;
  simOK        = false;
}

void nsRunControl::RunVerbose()
{
  GenAppInfoMsg("Simulation", "Started...");

  CommonSetup();
  if (control.IsForward())
  {
    RunMessage("starting forward verbose");
    ForwardVerbose();
    RunMessage("forward complete");
  }
  else
  {
    RunMessage("starting optimization verbose");
    OptVerbose();
    RunMessage("optimization complete");
  }
}

void nsRunControl::RunMinimal()
{
  GenAppInfoMsg("Simulation", "Started...");
  CommonSetup();

  if (control.IsForward())
  {
    RunMessage("starting forward minimal");
    ForwardMinimal();
    RunMessage("forward complete");
  }
  else
  {
    RunMessage("starting optimization minimal");
    OptMinimal();
    RunMessage("optimization complete");
  }
}

void nsRunControl::RunNoThreads()
{
  GenAppInfoMsg("Simulation", "Started...");
  CommonSetup();

  CTime timeStart = CTime::GetCurrentTime();
  CWaitCursor w;
  if (control.IsForward())
  {
    ForwardNoThreads();
  }
  else
  {
    OptNoThreads();
  }

  CTimeSpan ts = CTime::GetCurrentTime() - timeStart;
  CString sText = ts.Format("run complete. Elapsed time   %D:%H:%M:%S");
  GenAppInfoMsg("No Threads", sText);
}


void nsRunControl::RunFinalOpt()
{
  GenAppInfoMsg("Simulation", "Started...");
  CommonSetup();
  RunMessage("starting optimization final");
  OptFinal();
  RunMessage("optimization final complete");
}
