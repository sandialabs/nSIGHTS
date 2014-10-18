///////////////////////////////////////////////////////////////////////////////////
//
// A_Sequence.cpp
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


#include "stdafx.h"

#include <genClass/U_Str.h>
#include <genClass/DO_XYData.h>
#include <genPlotClass/PD_2DXY.h>

#include <genApp/C_AppMenu.h>
#include <genApp/C_PlotPage.h>

#include <genPFO/PFO_DataLabels.h>

#include <objDFO/DFO_CreateXYArray.h>
#include <objDFO/DFO_XYDualScaleTransform.h>
#include <objDFO/DFO_XYScaleTransform.h>
#include <objDFO/DFO_XYIntegrate.h>
#include <objDFO/DFO_XYReduction.h>
#include <objDFO/DFO_XYMath.h>
#include <objPFO/PFO_2DXYData.h>
#include <objPFO/PFO_2DAnalytic.h>

#include <nsDFO/DFO_ExtractSequenceInterval.h>
#include <nsDFO/DFO_Derivative.h>
#include <nsDFO/DFO_PulseNormalize.h>
#include <nsDFO/DFO_TimeProcess.h>

#include <Var/G_Sequence.h>
#include <Var/G_DataCapture.h>

#include <App/nPreRoot.h>

#include <Auto/A_Main.h>
#include <Auto/A_Sequence.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace nsAutoSetup  {

SequenceAutoSpec::SequenceAutoSpec(int nS)
  : nSpec(nS)
{
  doExtract = false;
  for (int i = 0; i < maxSpec; i++)
  {
    doSpec[i] = false;
    specID[i] = "n/a";
  }

}

bool SequenceAutoSpec::IsEnabled(int indx) const
{
  return indx < nSpec;
}

bool SequenceAutoSpec::DoSpec(int indx) const
{
  return doSpec[indx] && IsEnabled(indx);
}


enum  {fDiag    = 0,
       fFDDiag  = 1,
       fHorner  = 2,
       fBourdet   = 3,
             fAnalytic  = 4};

    FlowSequenceAutoSpec::FlowSequenceAutoSpec() : SequenceAutoSpec(5)
{
  specID[fDiag]   = "Std Diagnostic";
  specID[fFDDiag]   = "FD Diagnostic";
  specID[fHorner]   = "Horner";
  specID[fBourdet]  = "Bourdet";
  specID[fAnalytic] = "Analytic on plots";
}

bool FlowSequenceAutoSpec::IsEnabled(int indx) const
{
  if (indx >= nSpec)
    return false;

  switch (indx) {
  case fDiag    :
    return true;
  case fFDDiag  :
    return true;
  case fHorner  :
    return true;
  case fBourdet   :
    return true;
  case fAnalytic  :
    return  DoSpec(fDiag) ||
            DoSpec(fFDDiag)||
            DoSpec(fHorner) ||
            DoSpec(fBourdet);
  }

  return false;
}

enum  {hDecline   = 0,
       hDiag1     = 1,
       hDiag2     = 2,
             hAnalytic      = 3};

    HistSequenceAutoSpec::HistSequenceAutoSpec() : SequenceAutoSpec(4)
{
  specID[hDecline]    = "Decline Plot";
  specID[hDiag1]      = "Diag #1 Plot";
  specID[hDiag2]      = "Diag #2 Plot";
  specID[hAnalytic]   = "Analytic on plots";
}

bool HistSequenceAutoSpec::IsEnabled(int indx) const
{
  if (indx >= nSpec)
    return false;

  switch (indx) {
  case hDecline   :
    return true;
  case hDiag1     :
    return true;
  case hDiag2     :
    return true;
  case hAnalytic    :
    return DoSpec(hDecline) || DoSpec(hDiag1) || DoSpec(hDiag2);
  }

  return false;
}

enum  {psRameyA  = 0,
       psRameyB  = 1,
       psRameyC  = 2,
       psRameyFD   = 3,
       psDeconv  = 4,
             psAnalytic  = 5};

    PulseSequenceAutoSpec::PulseSequenceAutoSpec() : SequenceAutoSpec(6)
{
  specID[psRameyA]  = "Ramey A Plot";
  specID[psRameyB]  = "Ramey B Plot";
  specID[psRameyC]  = "Ramey C Plot";
  specID[psRameyFD] = "Ramey FD Plot";
  specID[psDeconv]  = "Deconvolution Plot";
  specID[psAnalytic]  = "Analytic on plots";
}

bool PulseSequenceAutoSpec::IsEnabled(int indx) const
{
  if (indx >= nSpec)
    return false;

  switch (indx) {
  case psRameyA   :
    return true;
  case psRameyB   :
    return true;
  case psRameyC   :
    return true;
  case psRameyFD  :
    return true;
  case psDeconv   :
    return true;
  case psAnalytic :
    return DoSpec(psRameyA)
           || DoSpec(psRameyB) || DoSpec(psRameyC)
           || DoSpec(psRameyFD)|| DoSpec(psDeconv) ;
  }

  return false;
}

//  declarations and initial values

FlowSequenceAutoSpec  flowSpec;
HistSequenceAutoSpec  histSpec;
PulseSequenceAutoSpec pulseSpec;
PulseSequenceAutoSpec slugSpec;

bool          testDesignMode = false;


void AutoSequenceCheckOK()
{
  sequence.SequencesOK();
  dataCapture.DataCaptureOK();

  flowSpec.Init();
  histSpec.Init();
  pulseSpec.Init();
  slugSpec.Init();

  for ( int i = 0; i < singleSequenceData.Size(); i++)
    if (singleSequenceData.IsValid(i))
    {
      Sequence& currSeq = singleSequenceData.GetRef(i);
      if (!currSeq.useInAutoSetup)
        continue;

      switch (currSeq.seqType) {
      case Sequence::stFlow : {
        flowSpec.opOK = true;
        flowSpec.seqCount++;
        break;
      }
      case Sequence::stHistory : {
        histSpec.opOK = true;
        histSpec.seqCount++;
        break;
      }
      case Sequence::stPulse : {
        pulseSpec.opOK = true;
        pulseSpec.seqCount++;
        break;
      }
      case Sequence::stSlug : {
        slugSpec.opOK = true;
        slugSpec.seqCount++;
        break;
      }
      case Sequence::stPacker : {
        break;
      }
      }
    }
}


static int          ncreateSimXY;
static int          ncreateFldXY;
static DFO_CreateXYArray* simCreateXY;
static DFO_CreateXYArray* fldCreateXY;

static void AddToXYArray(FuncObjC* simObj, FuncObjC* fldObj, bool systemObject = false)
{
  if (ncreateSimXY < DFO_CreateXYArray::maxXYDataDesc)
  {
    DFO_CreateXYArray::XYDataDesc& nextXY = simCreateXY->xyData[ncreateSimXY++];

    nextXY.doXY = true;
    nextXY.xyObjRef = FuncObjRef(*simObj);
    if (systemObject)
      CopyString(nextXY.newID, simObj->GetID(), DC_XYData::dataIDLen);
    else
      CopyString(nextXY.newID, simID, DC_XYData::dataIDLen);
  }

  if ((!systemObject) && (ncreateFldXY < DFO_CreateXYArray::maxXYDataDesc))
  {
    DFO_CreateXYArray::XYDataDesc& nextXY = fldCreateXY->xyData[ncreateFldXY++];

    nextXY.doXY = true;
    nextXY.xyObjRef = FuncObjRef(*fldObj);
    CopyString(nextXY.newID, fieldID, DC_XYData::dataIDLen);
  }
}

static void AddToFldXYArray(FuncObjC* fldObj)
{
  if (ncreateFldXY < DFO_CreateXYArray::maxXYDataDesc)
  {
    DFO_CreateXYArray::XYDataDesc& nextXY = fldCreateXY->xyData[ncreateFldXY++];

    nextXY.doXY = true;
    nextXY.xyObjRef = FuncObjRef(*fldObj);
    CopyString(nextXY.newID, fieldID, DC_XYData::dataIDLen);
  }
}


static FuncObjRefArray  fieldXYFO; // extract is always index 0 -- except possibly for Horner
static FuncObjRefArray  simXYFO;
static const char*  seqRootID = "Seq";

static  void  AddXYFO(FuncObjC* fieldObj,
                      FuncObjC* simObj,
                      bool    addToOutputArray = true)
{
  fieldXYFO += FuncObjRef(fieldObj);
  simXYFO += FuncObjRef(simObj);

  if (addToOutputArray)
    AddToXYArray(simObj, fieldObj);
}

static  void  AddXYFO(FuncObjC* fieldObj,
                      FuncObjC* simObj,
                      int&    dataIndx,
                      bool    addToOutputArray = true)
{
  dataIndx = fieldXYFO.Size();
  AddXYFO(fieldObj, simObj, addToOutputArray);
}
// for all cases

enum ExtractType {etNormal, etHorner, etBourdetSeq, etBourdetFlow};

static void DoExtract(    int               seqIndx,
                          bool        adjustP,
                          bool        absP,
                          const char*             inPrefix,
                          MenuPageC*      parent,
                          ExtractType     extractType = etNormal)
{
  const char* extractID = wellID;
  if (extractType != etBourdetFlow)
  {
    char pageID[idLen];
    switch (extractType) {
    case etNormal : {
      MakeObjID(pageID, "_process");
      break;
    }
    case etHorner : {
      MakeObjID(pageID, "_Hprocess");
      break;
    }
    case etBourdetSeq : {
      MakeObjID(pageID, "_Bprocess");
      break;
    }
    }
    AddNewDataPage(pageID, parent);
  }
  else
  {
    // bourdet flow always goes to qdat and pdat
    if (currentWells.IsEmpty())
      extractID = "DAT";
    else
      extractID = currentWells[0];
  }


  char inFieldID[80];
  char inSimID[80];
  MakeString(inFieldID, "f", inPrefix, extractID, 80);
  MakeString(inSimID, "s", inPrefix, extractID, 80);

  SetObjRef(typeid(DO_XYData));
  FuncObjRef fieldDATFO = GetMatchingObj(inFieldID);
  FuncObjRef simDATFO = GetMatchingObj(inSimID);

  MakeObjID(fieldID, "_", inFieldID);

  DFO_ExtractSequenceInterval* fieldEXFO = static_cast<DFO_ExtractSequenceInterval*>(AddFieldObject("DPO_ExtractSequenceInterval"));
  fieldEXFO->xyInputDataObjRef = fieldDATFO;
  fieldEXFO->seqTimesObjRef = FuncObjRef(sequenceTimesFO);
  if (extractType == etBourdetFlow)
  {
    // if fQDAT not found use sQDAT - RR 25 Aug 04
    if (fieldDATFO.objRef == 0)
      fieldEXFO->xyInputDataObjRef = simDATFO;

    fieldEXFO->removeDuplicates = true;
    fieldEXFO->setStartAndEnd = true;
    fieldEXFO->startSeqMS.enteredIndexValue = 0;
    fieldEXFO->endSeqMS.enteredIndexValue = seqIndx;
  }
  else
  {
    fieldEXFO->startSeqMS.enteredIndexValue = seqIndx;
    fieldEXFO->endSeqMS.enteredIndexValue = seqIndx;
  }
  fieldEXFO->adjustTimeToStart = true;
  fieldEXFO->adjustPressureToStart = adjustP;
  fieldEXFO->absValuePressure  = absP;

  MakeObjID(simID, "_", inSimID);
  DFO_ExtractSequenceInterval* simEXFO = static_cast<DFO_ExtractSequenceInterval*>(AddSimObject());
  simEXFO->xyInputDataObjRef = simDATFO;

  // extract always first except for bourdet flow data
  if (extractType != etBourdetFlow)
  {
    fieldXYFO.SetResizable(32);
    simXYFO.SetResizable(32);
  }
  AddXYFO(fieldEXFO, simEXFO);  // old comment: bourdet QDAT not added : was in if why?
}

static void AddAnalytic()
{
  MenuObjC* analObj = AddNewObject("PPO_2DAnalytic", "Diag");
  PFO_2DAnalytic& analFO = *(static_cast<PFO_2DAnalytic*>(analObj->objFunction));

  MenuObjC* annoObj = AddNewObject("PPO_DataLabels", "Analysis");
  PFO_DataLabels& annoFO = *(static_cast<PFO_DataLabels*>(annoObj->objFunction));

  PFO_DataLabels::DataLabel& xlLab = annoFO.labelData[0];
  xlLab.doLabel = true;
  xlLab.labelObjRef = FuncObjRef(analFO, 2);

  PFO_DataLabels::DataLabel& ylLab = annoFO.labelData[1];
  ylLab.doLabel = true;
  ylLab.labelObjRef = FuncObjRef(analFO, 3);

  PFO_DataLabels::DataLabel& lLab = annoFO.labelData[2];
  lLab.doLabel = true;
  lLab.labelObjRef = FuncObjRef(analFO, 4);

  PFO_DataLabels::DataLabel& sLab = annoFO.labelData[3];
  sLab.doLabel = true;
  sLab.labelObjRef = FuncObjRef(analFO, 5);

  PFO_DataLabels::DataLabel& mLab = annoFO.labelData[4];
  mLab.doLabel = true;
  mLab.labelObjRef = FuncObjRef(analFO, 6);

  annoFO.ulLocation = Point2D(2.0, 98.0);
  annoFO.frameLegend = true;
  annoFO.legendFont.fontSize = 10.0;

  PlotDefC::ExecuteAndReplot(analFO);
}


// flow sequences

static DFO_Derivative* AddFieldDeriv()
{
  return static_cast<DFO_Derivative*>(AddFieldObject("DPO_Derivative"));
}

static DFO_Derivative* AddSimDeriv(DFO_Derivative* fieldMaster, int inputIndex)
{
  DFO_Derivative* simDeriv = static_cast<DFO_Derivative*>(AddSimObject());
  simDeriv->inputTPObjRef = simXYFO[inputIndex];
  simDeriv->isMaster = testDesignMode;
  simDeriv->masterObjRef = FuncObjRef(*fieldMaster);
  return simDeriv;
}


static DFO_XYDualScaleTransform* AddFieldST()
{
  return static_cast<DFO_XYDualScaleTransform*>(AddFieldObject("DPO_XYDualScaleTransform"));
}

static DFO_XYDualScaleTransform* AddSimST()
{
  return static_cast<DFO_XYDualScaleTransform*>(AddSimObject());
}


static void FlowDiagnosticData(int& dataIndx)
{
  char tempID[idLen];
  MakeString(tempID, "dP", wellID, "/dlnt", idLen);

  MakeObjID(tempID);
  DFO_Derivative* fieldPdFO = AddFieldDeriv();
  fieldPdFO->derivativeDC.derivativeType = Derivative::dt_dY_dlnX;
  fieldPdFO->inputTPObjRef = fieldXYFO[0];

  DFO_Derivative* simPdFO = AddSimDeriv(fieldPdFO, 0);

  AddXYFO(fieldPdFO, simPdFO, dataIndx);
}


static void FlowFDDiagnosticData(int pdIndx, int& dataIndx)
{
  char tempID[idLen];
  MakeString(tempID, "d2logP", wellID, "/d2logT", idLen);

  //  second derivative
  MakeObjID(tempID);
  DFO_Derivative* fieldPd2FO = AddFieldDeriv();
  fieldPd2FO->derivativeDC.derivativeType = dt_dlog10Y_dlog10X;
  fieldPd2FO->derivativeDC.offsetY = 1.0E-10;
  fieldPd2FO->inputTPObjRef = fieldXYFO[pdIndx];
  DFO_Derivative* simPd2FO = AddSimDeriv(fieldPd2FO, pdIndx);

  // transform
  MakeString(tempID, "-2d2logP", wellID, "/d2logt+2", idLen);
  MakeObjID(tempID);

  DFO_XYDualScaleTransform* field2FO = AddFieldST();
  field2FO->xyInputObjRef = FuncObjRef(*fieldPd2FO);
  field2FO->yscaleMS.enteredRealValue = -2.0;
  field2FO->yoffsetMS.enteredRealValue = 2.0;
  DFO_XYDualScaleTransform* sim2FO = AddSimST();
  sim2FO->xyInputObjRef = FuncObjRef(*simPd2FO);

  // next
  AddXYFO(field2FO, sim2FO, dataIndx);
}

static DFO_TimeProcess* AddFieldHorner(int seqIndex)
{
  DFO_TimeProcess* fldHorner = static_cast<DFO_TimeProcess*>(AddFieldObject("DPO_TimeProcess"));
  if ((seqIndex > 0) && singleSequenceData.IsValid(seqIndex - 1))
  {
    Sequence& currSeq = singleSequenceData.GetRef(seqIndex - 1);
    fldHorner->constantTMS.enteredRealValue = currSeq.seqLength.GetUserVal();
  }
  fldHorner->inputTPObjRef = fieldXYFO[0];
  return fldHorner;
}

static DFO_TimeProcess* AddSimHorner(DFO_TimeProcess* fieldMaster)
{
  DFO_TimeProcess* simHorner = static_cast<DFO_TimeProcess*>(AddSimObject());
  simHorner->inputTPObjRef = simXYFO[0];
  simHorner->isMaster = testDesignMode;
  simHorner->masterObjRef = FuncObjRef(*fieldMaster);
  return simHorner;
}

static void FlowHornerData(int seqIndex, int& dataIndx)
{
  char tempID[idLen];
  MakeString(tempID, "Horn(", wellID, ")", idLen);

  MakeObjID(tempID);
  DFO_TimeProcess* fieldHorner = AddFieldHorner(seqIndex);
  DFO_TimeProcess*  simHorner = AddSimHorner(fieldHorner);

  AddXYFO(fieldHorner, simHorner, dataIndx);
}

static DFO_TimeProcess* AddFieldBourdet(int bourdetQIndx)
{
  DFO_TimeProcess* fldBourdet = static_cast<DFO_TimeProcess*>(AddFieldObject("DPO_TimeProcess"));
  fldBourdet->timeProcessDC.timeProcessType = TimeProcess::tpBourdetSuper;
  fldBourdet->timeProcessDC.addReplaceLastQ = false;
  fldBourdet->inputTQObjRef = fieldXYFO[bourdetQIndx];
  fldBourdet->inputTPObjRef = fieldXYFO[0];

  return fldBourdet;
}

static DFO_TimeProcess* AddSimBourdet(DFO_TimeProcess* fieldMaster,
                                      int bourdetQIndx)
{
  DFO_TimeProcess* simBourdet = static_cast<DFO_TimeProcess*>(AddSimObject());
  simBourdet->inputTQObjRef = simXYFO[bourdetQIndx];
  simBourdet->inputTPObjRef = simXYFO[0];

  return simBourdet;
}

static void FlowBourdetData(int& dataIndx)
{
  int qinputDataIndx = fieldXYFO.UpperBound();

  // set up reduce - no reduction performed
  MakeObjID("Reduce");
  DFO_XYReduction* fldRed = static_cast<DFO_XYReduction*>(AddFieldObject("DPO_XYReduction"));
  fldRed->xyInputObjRef = fieldXYFO[qinputDataIndx];
  fldRed->dataReductionOperation = DFO_XYReduction::reSkip;
  fldRed->pointSkipInterval = 0;

  DFO_XYReduction* simRed = static_cast<DFO_XYReduction*>(AddSimObject());
  simRed->xyInputObjRef = simXYFO[qinputDataIndx];

  int bourdetQIndx;
  AddXYFO(fldRed, simRed, bourdetQIndx, false);

  char tempID[idLen];
  MakeString(tempID, "Bourdet(", wellID, ")", idLen);

  MakeObjID(tempID);
  DFO_TimeProcess* fieldBourdet = AddFieldBourdet(bourdetQIndx);
  DFO_TimeProcess* simBourdet = AddSimBourdet(fieldBourdet, bourdetQIndx);

  AddXYFO(fieldBourdet, simBourdet, false);

  MakeString(tempID, "dP", wellID, "/dBt", idLen);

  MakeObjID(tempID);
  DFO_Derivative* fieldPdFO = AddFieldDeriv();
  fieldPdFO->derivativeDC.derivativeType = Derivative::dt_dY_dX;
  fieldPdFO->derivativeDC.derivativeCalcType = Derivative::dctLinPctWindow;
  fieldPdFO->derivativeDC.useSuperTime = true;
  fieldPdFO->timeProcessObjRef = fieldBourdet;
  fieldPdFO->inputTPObjRef = fieldXYFO[0];

  DFO_Derivative* simPdFO = AddSimDeriv(fieldPdFO, 0);
  simPdFO->timeProcessObjRef = simBourdet;

  AddXYFO(fieldPdFO, simPdFO, dataIndx);

}



static void MakeSeqPlotID(char  idStr[],
                          const char* suffix)
{
  if (currentWells.Size() > 1)
    MakeString(idStr, wellID, "_", seqID, suffix, idLen);
  else
    MakeString(idStr, seqID, suffix, idLen);
}

static void FlowDiagPlot(int dataIndx)
{
  char tempID[idLen];
  MakeSeqPlotID(tempID, "_diag");

  AddNewXYPage(seqRootID, tempID, false, false);

  PFO_2DXYData* currXY = AddXYDataPFO("P");
  SetXYDisplay(*currXY, 0, fieldXYFO[0], ps_FCircle, "Measured pressure");
  SetXYDisplay(*currXY, 1, simXYFO[0], "Simulated pressure");

  currXY = AddXYDataPFO("dlnP");
  SetXYDisplay(*currXY, 0, fieldXYFO[dataIndx], ps_FSquare, "dP(Field)");
  SetXYDisplay(*currXY, 1, simXYFO[dataIndx], "dP(Simulation)");

  RecalcPage();
  if (flowSpec.DoSpec(fAnalytic))
    AddAnalytic();
}


static void FlowFDDiagPlot(int dataIndx)
{
  char tempID[idLen];
  MakeSeqPlotID(tempID, "_FDdiag");

  AddNewXYPage(seqRootID, tempID, false, true);

  PFO_2DXYData* currXY = AddXYDataPFO("FDdP");

  SetXYDisplay(*currXY, 0, fieldXYFO[dataIndx], ps_FCircle, "Field -2 * d2P/dlnt + 2");
  SetXYDisplay(*currXY, 1, simXYFO[dataIndx], "Simulated -2 * d2P/dlnt + 2");

  RecalcPage();
  if (flowSpec.DoSpec(fAnalytic))
    AddAnalytic();
}

static void FlowHornerPlot(int dataIndx)
{
  char tempID[idLen];
  MakeSeqPlotID(tempID, "_Horner");

  AddNewXYPage(seqRootID, tempID, false, true);

  PFO_2DXYData* currXY = AddXYDataPFO("Horner");

  SetXYDisplay(*currXY, 0, fieldXYFO[dataIndx], ps_FCircle, "Field Horner");
  SetXYDisplay(*currXY, 1, simXYFO[dataIndx], "Simulated Horner");

  RecalcPage();
  if (flowSpec.DoSpec(fAnalytic))
    AddAnalytic();
}

static void FlowBourdetPlot(int dataIndx)
{
  char tempID[idLen];
  MakeSeqPlotID(tempID, "_Bdiag");

  AddNewXYPage(seqRootID, tempID, false, false);

  PFO_2DXYData* currXY = AddXYDataPFO("P");
  SetXYDisplay(*currXY, 0, fieldXYFO[0], ps_FCircle, "Measured pressure");
  SetXYDisplay(*currXY, 1, simXYFO[0], "Simulated pressure");

  currXY = AddXYDataPFO("dlnP");
  SetXYDisplay(*currXY, 0, fieldXYFO[dataIndx], ps_FSquare, "dBP(Field)");
  SetXYDisplay(*currXY, 1, simXYFO[dataIndx], "dBP(Simulation)");

  RecalcPage();
  if (flowSpec.DoSpec(fAnalytic))
    AddAnalytic();

}


// history sequences

static void HistDiag1Data(const char* prefix, int& dataIndx)
{
  char tempID[idLen];
  MakeString(tempID, "dlog", prefix, wellID, "/dlogt", idLen);

  MakeObjID(tempID);
  DFO_Derivative* fieldPdFO = AddFieldDeriv();
  fieldPdFO->derivativeDC.derivativeType = Derivative::dt_dlog10Y_dlog10X;
  fieldPdFO->inputTPObjRef = fieldXYFO[0];

  DFO_Derivative* simPdFO = AddSimDeriv(fieldPdFO, 0);

  MakeString(tempID, "2dlog", prefix, wellID, "/dlogt+2", idLen);
  MakeObjID(tempID);
  DFO_XYDualScaleTransform* field2FO = AddFieldST();
  field2FO->xyInputObjRef = FuncObjRef(*fieldPdFO);
  field2FO->yscaleMS.enteredRealValue = 2.0;
  field2FO->yoffsetMS.enteredRealValue = 2.0;

  DFO_XYDualScaleTransform* sim2FO = AddSimST();
  sim2FO->xyInputObjRef = FuncObjRef(*simPdFO);

  AddXYFO(field2FO, sim2FO, dataIndx);
}


static void HistDiag2Data(const char* prefix, int& dataIndx)
{
  char tempID[idLen];
  MakeString(tempID, prefix, wellID, "inv", idLen);

  // q inverse
  MakeObjID(tempID);
  DFO_XYDualScaleTransform* fieldIFO = AddFieldST();
  fieldIFO->xyInputObjRef = fieldXYFO[0];
  fieldIFO->yST.tranOp = DSC_ScaleTransform::tt_Inv;

  DFO_XYDualScaleTransform* simIFO = AddSimST();
  simIFO->xyInputObjRef = simXYFO[0];

  // first derivative
  MakeString(tempID, "d", prefix, wellID, "inv/dlogt", idLen);

  MakeObjID(tempID);
  DFO_Derivative* fieldPdFO = AddFieldDeriv();
  fieldPdFO->derivativeDC.derivativeType = dt_dY_dlog10X;
  fieldPdFO->inputTPObjRef = FuncObjRef(*fieldIFO);

  DFO_Derivative* simPdFO = AddSimDeriv(fieldPdFO, 0);
  simPdFO->inputTPObjRef = FuncObjRef(*simIFO);

  // second derivative
  MakeString(tempID, "d2", prefix, wellID, "inv/dlogt", idLen);
  MakeObjID(tempID);
  DFO_Derivative* fieldPd2FO = AddFieldDeriv();
  fieldPd2FO->derivativeDC.derivativeType = dt_dlog10Y_dlog10X;
  fieldPd2FO->derivativeDC.offsetY = 1.0E-10;
  fieldPd2FO->inputTPObjRef = FuncObjRef(*fieldPdFO);

  DFO_Derivative* simPd2FO = AddSimDeriv(fieldPd2FO, 0);
  simPd2FO->inputTPObjRef = FuncObjRef(*simPdFO);

  // scaled
  MakeString(tempID, "-2d2", prefix, wellID, "inv/dlogt+2", idLen);
  MakeObjID(tempID);
  DFO_XYDualScaleTransform* field2FO = AddFieldST();
  field2FO->xyInputObjRef = FuncObjRef(*fieldPd2FO);
  field2FO->yscaleMS.enteredRealValue = -2.0;
  field2FO->yoffsetMS.enteredRealValue = 2.0;

  DFO_XYDualScaleTransform* sim2FO = AddSimST();
  sim2FO->xyInputObjRef = FuncObjRef(*simPd2FO);

  AddXYFO(field2FO, sim2FO, dataIndx);
}


static void HistDeclinePlot(bool isObs)
{
  char tempID[idLen];
  MakeSeqPlotID(tempID, "_decline");

  AddNewXYPage(seqRootID, tempID, false, false);
  if (isObs)
  {
    PFO_2DXYData* currXY = AddXYDataPFO("P");
    SetXYDisplay(*currXY, 0, fieldXYFO[0], ps_FSquare, "Measured OW pressure ");
    SetXYDisplay(*currXY, 1, simXYFO[0], "Simulated OW pressure");
  }
  else
  {
    PFO_2DXYData* currXY = AddXYDataPFO("Q");
    SetXYDisplay(*currXY, 0, fieldXYFO[0], ps_FSquare, "Measured flow rate");
    SetXYDisplay(*currXY, 1, simXYFO[0], "Simulated flow rate");
  }


  RecalcPage();
  if (histSpec.DoSpec(hAnalytic))
    AddAnalytic();
}

static void HistDiag1Plot(const char* prefix, int dataIndx)
{
  char tempID[idLen];
  MakeSeqPlotID(tempID, "_Diag1");

  AddNewXYPage(seqRootID, tempID, false, true);

  MakeString(tempID, "d", prefix, wellID, "inv/dlogt", idLen);
  PFO_2DXYData* currXY = AddXYDataPFO(tempID);

  MakeString(tempID, "-2dlog", prefix, wellID, "/dlogt + 2 measured", idLen);
  SetXYDisplay(*currXY, 0, fieldXYFO[dataIndx], ps_FSquare, tempID);

  MakeString(tempID, "-2dlog", prefix, wellID, "/dlogt + 2 simulated", idLen);
  SetXYDisplay(*currXY, 1, simXYFO[dataIndx], tempID);

  RecalcPage();
  if (histSpec.DoSpec(hAnalytic))
    AddAnalytic();
}


static void HistDiag2Plot(const char* prefix, int dataIndx)
{
  char tempID[idLen];
  MakeSeqPlotID(tempID, "_Diag2");

  AddNewXYPage(seqRootID, tempID, false, true);

  MakeString(tempID, "FDd2", prefix, wellID, idLen);
  PFO_2DXYData* currXY = AddXYDataPFO("FDd2Q");

  MakeString(tempID, "Field d2", prefix, wellID, "inv/dlogt", idLen);
  SetXYDisplay(*currXY, 0, fieldXYFO[dataIndx], ps_FCircle, tempID);

  MakeString(tempID, "Simulated d2", prefix, wellID, "inv/dlogt", idLen);
  SetXYDisplay(*currXY, 1, simXYFO[dataIndx], tempID);

  RecalcPage();
  if (flowSpec.DoSpec(fAnalytic))
    AddAnalytic();
}


// pulse slug sequences
static void NormPulse(const Sequence& psSeq, bool regNorm, int& dataIndex)
{
  if (regNorm)
    MakeObjID("P(norm)");
  else
    MakeObjID("P(1-norm)");

  DFO_PulseNormalize* fieldPnFO = static_cast<DFO_PulseNormalize*>(AddFieldObject("DPO_PulseNormalize"));
  if (regNorm)
  {
    fieldPnFO->pulseNormalizationType = DFO_PulseNormalize::pntNormal;
  }
  else
  {
    fieldPnFO->pulseNormalizationType = DFO_PulseNormalize::pntOneMinusNormal;
  }

  double initPressure;
  if (psSeq.seqType == Sequence::stPulse)
    initPressure = psSeq.pulseData.initialPressure.GetUserVal();
  else
    initPressure = psSeq.slugData.initialPressure.GetUserVal();

  fieldPnFO->xyInputObjRef = fieldXYFO[0];
  fieldPnFO->p0_MS.enteredRealValue = initPressure;
  fieldPnFO->pI_MS.valueIsMaster = false;
  fieldPnFO->pI_MS.valueMasterObjRef = FuncObjRef(capturedDataFO);


  DFO_PulseNormalize* simPnFO = static_cast<DFO_PulseNormalize*>(AddSimObject());
  simPnFO->xyInputObjRef = simXYFO[0];

  AddXYFO(fieldPnFO, simPnFO, dataIndex);
}


static void RameyBCFDData(int inputIndex, int& outputIndex)
{
  DFO_Derivative* fieldPdFO = AddFieldDeriv();
  fieldPdFO->derivativeDC.derivativeType = Derivative::dt_dlog10Y_dlog10X;
  fieldPdFO->derivativeDC.absOutputY = true;
  fieldPdFO->inputTPObjRef = fieldXYFO[inputIndex];

  DFO_Derivative* simPdFO = AddSimDeriv(fieldPdFO, inputIndex);

  AddXYFO(fieldPdFO, simPdFO, outputIndex);
}


static void RameyAData(int inputIndex, int& outputIndex)
{
  MakeObjID("dP(norm)");
  DFO_Derivative* fieldPdFO = AddFieldDeriv();
  fieldPdFO->derivativeDC.derivativeType = Derivative::dt_dY_dlog10X;
  fieldPdFO->derivativeDC.absOutputY = true;
  fieldPdFO->inputTPObjRef = fieldXYFO[inputIndex];

  DFO_Derivative* simPdFO = AddSimDeriv(fieldPdFO, inputIndex);

  AddXYFO(fieldPdFO, simPdFO, outputIndex);
}


static void RameyFDData(int inputIndex, int& outputIndex)
{
  MakeObjID("2dlogP(norm)");
  DFO_XYDualScaleTransform* field2FO = AddFieldST();
  field2FO->xyInputObjRef = fieldXYFO[inputIndex];
  field2FO->yscaleMS.enteredRealValue = 2.0;

  DFO_XYDualScaleTransform* sim2FO = AddSimST();
  sim2FO->xyInputObjRef = simXYFO[inputIndex];

  AddXYFO(field2FO, sim2FO, outputIndex);
}


static void DeconvolvData(int inputIndex, int normIndex,
                          int& deconvOutputIndex, int& deconvDerivOutputIndex)
{
  MakeObjID("Integrate");
  DFO_XYIntegrate* fldIntegrate = static_cast<DFO_XYIntegrate*>(AddFieldObject("DPO_XYIntegrate"));
  fldIntegrate->xyInputObjRef = fieldXYFO[inputIndex];

  DFO_XYIntegrate* simIntegrate = static_cast<DFO_XYIntegrate*>(AddSimObject());
  simIntegrate->xyInputObjRef = simXYFO[inputIndex];

  MakeObjID("Decon");
  DFO_XYMath* fldMath = static_cast<DFO_XYMath*>(AddFieldObject("DPO_XYMath"));
  fldMath->dataInputAObjRef = fldIntegrate;
  fldMath->dataInputBObjRef = fieldXYFO[normIndex];
  fldMath->mathOp= DFO_MathBase::ot_Divide;
  fldMath->operateOnX = false;

  DFO_XYMath* simMath = static_cast<DFO_XYMath*>(AddSimObject());
  simMath->dataInputAObjRef = simIntegrate;
  simMath->dataInputBObjRef = simXYFO[normIndex];

  MakeObjID("dP(deconv)");
  DFO_Derivative* fieldPdFO = AddFieldDeriv();
  fieldPdFO->derivativeDC.derivativeType = Derivative::dt_dY_dlnX;
  fieldPdFO->derivativeDC.absOutputY = true;
  fieldPdFO->windowPctSpanMS.enteredRealValue = 2.0;
  fieldPdFO->inputTPObjRef = fldMath;

  DFO_Derivative* simPdFO = AddSimDeriv(fieldPdFO, 0);
  simPdFO->inputTPObjRef = simMath;

  AddXYFO(fldMath, simMath, deconvOutputIndex);
  AddXYFO(fieldPdFO, simPdFO, deconvDerivOutputIndex);
}


static void RameyAPlot(int normIndx, int pdIndex)
{
  char tempID[idLen];
  MakeObjID(tempID, "_RameyA");

  AddNewXYPage(seqRootID, tempID, false, true);
  PD_2DXY* currPlot = static_cast<PD_2DXY*>(static_cast<PlotPageC*>(autoPage)->GetPlot());

  currPlot->yAxis.axisLinMin = 0.0;
  currPlot->yAxis.axisLinMax = 1.0;
  currPlot->yAxis.autoScaleAxes = false;

  PFO_2DXYData* currXY = AddXYDataPFO("P(norm)");
  SetXYDisplay(*currXY, 0, fieldXYFO[normIndx], ps_FCircle, "Normalized measured pressure");
  SetXYDisplay(*currXY, 1, simXYFO[normIndx], "Normalized simulated pressure");

  currXY = AddXYDataPFO("dP(norm)");
  SetXYDisplay(*currXY, 0, fieldXYFO[pdIndex], ps_FSquare, "Measured dP(norm)/dlogt");
  SetXYDisplay(*currXY, 1, simXYFO[pdIndex], "Simulated dP(norm)/dlogt");
}

static void RameyBPlot(int normIndx, int pdIndex)
{
  char tempID[idLen];
  MakeObjID(tempID, "_RameyB");

  AddNewXYPage(seqRootID, tempID, false, false);

  PFO_2DXYData* currXY = AddXYDataPFO("P(norm)");
  SetXYDisplay(*currXY, 0, fieldXYFO[normIndx], ps_FCircle, "Normalized measured pressure");
  SetXYDisplay(*currXY, 1, simXYFO[normIndx], "Normalized simulated pressure");

  currXY = AddXYDataPFO("dlogP(norm)");
  SetXYDisplay(*currXY, 0, fieldXYFO[pdIndex], ps_FSquare, "Measured dlogP(norm)/dlogt");
  SetXYDisplay(*currXY, 1, simXYFO[pdIndex], "Simulated dlogP(norm)/dlogt");

}
static void RameyCPlot(int normIndx, int pdIndex)
{
  char tempID[idLen];
  MakeObjID(tempID, "_RameyC");

  AddNewXYPage(seqRootID, tempID, false, false);

  PFO_2DXYData* currXY = AddXYDataPFO("P(1 - norm)");
  SetXYDisplay(*currXY, 0, fieldXYFO[normIndx], ps_FCircle, "1 - Normalized measured pressure");
  SetXYDisplay(*currXY, 1, simXYFO[normIndx], "1 - Normalized simulated pressure");

  currXY = AddXYDataPFO("dlogP(1-norm)");
  SetXYDisplay(*currXY, 0, fieldXYFO[pdIndex], ps_FSquare, "Measured dlogP(1-norm)/dlogt");
  SetXYDisplay(*currXY, 1, simXYFO[pdIndex], "Simulated dlogP(1-norm)/dlogt");
}

static void RameyFDPlot(int dataIndx)
{
  char tempID[idLen];
  MakeObjID(tempID, "_RameyFD");

  AddNewXYPage(seqRootID, tempID, false, false);

  PFO_2DXYData* currXY = AddXYDataPFO("2dlogP(norm)");
  SetXYDisplay(*currXY, 0, fieldXYFO[dataIndx], ps_FCircle, "2.0 * measured dlogP(norm)/dlogt");
  SetXYDisplay(*currXY, 1, simXYFO[dataIndx], "2.0 * simulated dlogP(norm)/dlogt");
}



static void DeconvolvePlot(int deconvIndx, int deconvDerivIndx)
{
  char tempID[idLen];
  MakeObjID(tempID, "_Deconv");

  AddNewXYPage(seqRootID, tempID, false, false);

  PFO_2DXYData* currXY = AddXYDataPFO("dP(deconv)");
  SetXYDisplay(*currXY, 0, fieldXYFO[deconvIndx], ps_FCircle, "measured deconvolved");
  SetXYDisplay(*currXY, 1, simXYFO[deconvIndx], "simulated deconvolved");
  SetXYDisplay(*currXY, 2, fieldXYFO[deconvDerivIndx], ps_FCircle, "measured dP(deconvolved)/dlnt");
  SetXYDisplay(*currXY, 3, simXYFO[deconvDerivIndx], "simulated dP(deconvolved)/dlnt");
}

static void AddPSAnalytic(const PulseSequenceAutoSpec& psSpec)
{
  RecalcPage();
  if (psSpec.DoSpec(psAnalytic))
    AddAnalytic();
}


static void DeleteSequencePage(bool inSpec, const char* inSuffix, MenuPageC* parent)
{
  if (inSpec)
  {
    char tempID[idLen];
    MakeObjID(tempID, inSuffix);
    sequenceRoot.DeletePage(tempID, parent);
  }
}


static void PSSetup(const PulseSequenceAutoSpec& inSpec,
                    const Sequence&        currSeq)
{
  int normIndex, norm1Index, linDerivIndex, logDerivIndex, log1DerivIndex, log2DerivIndex, deconvIndex, deconvDerivIndex;

  if (inSpec.DoSpec(psRameyA) || inSpec.DoSpec(psRameyB)
      || inSpec.DoSpec(psRameyFD) || inSpec.DoSpec(psDeconv))
    NormPulse(currSeq, true, normIndex);

  if (inSpec.DoSpec(psRameyC) || inSpec.DoSpec(psDeconv))
    NormPulse(currSeq, false, norm1Index);

  if (inSpec.DoSpec(psRameyA))
    RameyAData(normIndex, linDerivIndex);

  if (inSpec.DoSpec(psRameyB) || inSpec.DoSpec(psRameyFD))
  {
    MakeObjID("dlogP(norm)");
    RameyBCFDData(normIndex, logDerivIndex);
  }
  if (inSpec.DoSpec(psRameyC))
  {
    MakeObjID("dlogP(1-norm)");
    RameyBCFDData(norm1Index, log1DerivIndex);
  }
  if (inSpec.DoSpec(psRameyFD))
    RameyFDData(logDerivIndex, log2DerivIndex);

  if (inSpec.DoSpec(psDeconv))
    DeconvolvData(normIndex, norm1Index, deconvIndex, deconvDerivIndex);

  RecalcPage();

  if (inSpec.DoSpec(psRameyA))
  {
    RameyAPlot(normIndex, linDerivIndex);
    AddPSAnalytic(inSpec);
  }
  if (inSpec.DoSpec(psRameyB))
  {
    RameyBPlot(normIndex, logDerivIndex);
    AddPSAnalytic(inSpec);
  }
  if (inSpec.DoSpec(psRameyC))
  {
    RameyCPlot(norm1Index, log1DerivIndex);
    AddPSAnalytic(inSpec);
  }
  if (inSpec.DoSpec(psRameyFD))
  {
    RameyFDPlot(log2DerivIndex);
    AddPSAnalytic(inSpec);
  }
  if (inSpec.DoSpec(psDeconv))
  {
    DeconvolvePlot(deconvIndex, deconvDerivIndex);
    AddPSAnalytic(inSpec);
  }
}

static int SequencePageExists(bool inSpec, const char* inSuffix, MenuPageC* parent)
{
  if (inSpec)
  {
    char tempID[idLen];
    MakeObjID(tempID, inSuffix);
    if (sequenceRoot.PageExists(tempID, parent))
      return 1;
  }
  return 0;
}


static void DeleteSeqPages(MenuPageC* parent, bool isObs)
{
  bool tzPOK = processTestZonePressureData && (!isObs);
  bool tzQOK = processTestZoneFlowData&& (!isObs);
  bool obsOK = processObsWellPressureData && isObs;

  for ( int i = 0; i < singleSequenceData.Size(); i++)
    if (singleSequenceData.IsValid(i))
    {
      Sequence& currSeq = singleSequenceData.GetRef(i);
      if (!currSeq.useInAutoSetup)
        continue;

      CopyString(seqID, currSeq.seqID, idLen);

      bool processDel = (SequencePageExists(true, "_process", parent) > 0);

      switch (currSeq.seqType) {
      case Sequence::stFlow : {
        if (flowSpec.ExtractOK() && (tzPOK || obsOK))
        {
          DeleteSequencePage(processDel || flowSpec.DoSpec(fDiag), "_diag", parent);
          DeleteSequencePage(processDel || flowSpec.DoSpec(fFDDiag), "_FDdiag", parent);
          bool processHDel = (SequencePageExists(true, "_Hprocess", parent) > 0);
          DeleteSequencePage(processHDel || flowSpec.DoSpec(fHorner), "_Horner", parent);
          DeleteSequencePage(true, "_Hprocess", parent);
          bool processBDel = (SequencePageExists(true, "_Bprocess", parent) > 0);
          DeleteSequencePage(processBDel || flowSpec.DoSpec(fBourdet), "_Bdiag", parent);
          DeleteSequencePage(true, "_Bprocess", parent);
        }
        break;
      }
      case Sequence::stHistory : {
        if (histSpec.ExtractOK() && (tzQOK || obsOK))
        {
          DeleteSequencePage(processDel || histSpec.DoSpec(hDecline), "_decline", parent);
          DeleteSequencePage(processDel || histSpec.DoSpec(hDiag1), "_Diag1", parent);
          DeleteSequencePage(processDel || histSpec.DoSpec(hDiag2), "_Ddiag2", parent);
        }
        break;
      }
      case Sequence::stPulse : {
        if (pulseSpec.ExtractOK() && tzPOK)
        {
          DeleteSequencePage(processDel || pulseSpec.DoSpec(psRameyA) , "_RameyA", parent);
          DeleteSequencePage(processDel || pulseSpec.DoSpec(psRameyB) , "_RameyB", parent);
          DeleteSequencePage(processDel || pulseSpec.DoSpec(psRameyC) , "_RameyC", parent);
          DeleteSequencePage(processDel || pulseSpec.DoSpec(psRameyFD) , "_RameyFD", parent);
          DeleteSequencePage(processDel || pulseSpec.DoSpec(psDeconv) , "_Deconv", parent);
        }
        break;
      }
      case Sequence::stSlug : {
        if (slugSpec.ExtractOK() && tzPOK)
        {
          DeleteSequencePage(processDel || slugSpec.DoSpec(psRameyA) , "_RameyA", parent);
          DeleteSequencePage(processDel || slugSpec.DoSpec(psRameyB) , "_RameyB", parent);
          DeleteSequencePage(processDel || slugSpec.DoSpec(psRameyC) , "_RameyC", parent);
        }
        break;
      }
      case Sequence::stPacker : {
        break;
      }
      }
      DeleteSequencePage(true, "_process", parent);
    }
}


static void AddSeqPages(MenuPageC* parent, bool isObs)
{
  bool tzPOK = processTestZonePressureData && (!isObs);
  bool tzQOK = processTestZoneFlowData&& (!isObs);
  bool obsOK = processObsWellPressureData && isObs;

  // find fPDAT and fQDAT and add to FieldOutput (FB Case84)
  {
    SetObjRef(typeid(DO_XYData));
    MakeString(fieldID, "fP", wellID, 80);
    FuncObjRef fieldDATFO = GetMatchingObj(fieldID);
    if (fieldDATFO.objRef != 0)
      AddToFldXYArray(fieldDATFO.objRef);

    MakeString(fieldID, "fQ", wellID, 80);
    fieldDATFO = GetMatchingObj(fieldID);
    if (fieldDATFO.objRef != 0)
      AddToFldXYArray(fieldDATFO.objRef);
  }


  for ( int i = 0; i < singleSequenceData.Size(); i++)
    if (singleSequenceData.IsValid(i))
    {
      Sequence& currSeq = singleSequenceData.GetRef(i);
      if (!currSeq.useInAutoSetup)
        continue;

      CopyString(seqID, currSeq.seqID, idLen);

      switch (currSeq.seqType) {
      case Sequence::stFlow : {
        if (flowSpec.ExtractOK() && (tzPOK || obsOK))
        {
          if (flowSpec.DoSpec(fDiag) || flowSpec.DoSpec(fFDDiag))
          {
            DoExtract(i, true, true, "P", parent);

            int diagData, fdDiagData;
            if (flowSpec.DoSpec(fDiag) || flowSpec.DoSpec(fFDDiag))
              FlowDiagnosticData(diagData);
            if (flowSpec.DoSpec(fFDDiag))
              FlowFDDiagnosticData(diagData, fdDiagData);

            RecalcPage();

            if (flowSpec.DoSpec(fDiag))
              FlowDiagPlot(diagData);
            if (flowSpec.DoSpec(fFDDiag))
              FlowFDDiagPlot(fdDiagData);
          }

          if ((i > 0) && flowSpec.DoSpec(fHorner))
          {
            DoExtract(i, false, false, "P", parent, etHorner);
            int hornerData;
            FlowHornerData(i, hornerData);
            RecalcPage();
            FlowHornerPlot(hornerData);
          }

          if ((i > 0) && flowSpec.DoSpec(fBourdet) && (currSeq.flowData.valueType == FlowHistBase::vtFixed))
          {
            DoExtract(i, true, true, "P", parent, etBourdetSeq);
            DoExtract(i, false, true, "Q", parent, etBourdetFlow);
            int bourdetData;
            FlowBourdetData(bourdetData);
            RecalcPage();
            FlowBourdetPlot(bourdetData);
          }
        }
        break;
      }
      case Sequence::stHistory : {

        if (histSpec.ExtractOK() && (tzQOK || obsOK))
        {
          const char* prefix = "Q";
          if (isObs)
            prefix = "P";

          DoExtract(i, false, true,  prefix, parent);

          int diag1Index, diag2Index;
          if (histSpec.DoSpec(hDiag1))
            HistDiag1Data(prefix, diag1Index);
          if (histSpec.DoSpec(hDiag2))
            HistDiag2Data(prefix, diag2Index);
          RecalcPage();

          if (histSpec.DoSpec(hDecline))
            HistDeclinePlot(isObs);
          if (histSpec.DoSpec(hDiag1))
            HistDiag1Plot(prefix, diag1Index);
          if (histSpec.DoSpec(hDiag2))
            HistDiag2Plot(prefix, diag2Index);
        }
        break;
      }
      case Sequence::stPulse : {
        if (pulseSpec.ExtractOK() && tzPOK)
        {
          DoExtract(i, false, false, "P", parent);
          PSSetup(pulseSpec, currSeq);
        }
        break;
      }
      case Sequence::stSlug : {
        if (slugSpec.ExtractOK() && tzPOK)
        {
          DoExtract(i, false, false, "P", parent);
          PSSetup(slugSpec, currSeq);
        }
        break;
      }
      case Sequence::stPacker : {
        break;
      }
      }
    }
}


void AutoSequenceSetup()
{
  MenuRootC::currTreeObj = NULL;
  sequenceRoot.CloseUI();
  processingSetupRoot.CloseUI();

  autoRoot = &processingSetupRoot;
  processingSetupRoot.DeletePage("Seq_process", 0);
  AddNewDataPage("Seq_process", 0);
  {
    MenuObjC* createXY = AddNewObject("DPO_CreateXYArray", "AutoSimData");
    simCreateXY = static_cast<DFO_CreateXYArray*>(createXY->objFunction);
    createXY = AddNewObject("DPO_CreateXYArray", "AutoFieldData");
    fldCreateXY = static_cast<DFO_CreateXYArray*>(createXY->objFunction);
    ncreateSimXY = 0;
    ncreateFldXY = 0;
    if (dataCapture.DataCaptureOK())
    {
      FuncObjC* pressSimFO = dataCaptureData[0]->GetObjRef();
      AddToXYArray(pressSimFO, 0, true);
      if (processTestZoneFlowData)
      {
        FuncObjC* flowSimFO = dataCaptureData[1]->GetObjRef();
        AddToXYArray(flowSimFO, 0, true);
      }
    }

  }

  autoRoot = &sequenceRoot;

  bool multiWells = MultiWellIDS();

  if (multiWells)
  {
    for (int i = 0; i < currentWells.Size(); i++)
    {
      MenuPageC* currWell = sequenceRoot.FindPage(currentWells[i], 0);
      if (currWell)
      {
        DeleteSeqPages(currWell, i > 0);
        if (currWell->childPages.IsEmpty())
          sequenceRoot.DeletePage(currentWells[i], 0);
      }
    }

    for (int i = 0; i < currentWells.Size(); i++)
    {
      wellID = currentWells[i];
      MenuPageC* currWell = sequenceRoot.FindPage(wellID, 0);
      if (!currWell)
      {
        AddNewDataPage(wellID, 0);
        currWell = autoPage;
      }
      AddSeqPages(currWell, i > 0);
    }

  }
  else
  {
    DeleteSeqPages(0, false);
    wellID = currentWells[0];
    AddSeqPages(0, false);
  }

  RecalcAll();
}


static int DelSequencePagesCount(MenuPageC* parent, bool isObs)
{
  int delCount = 0;

  bool tzPOK = processTestZonePressureData && (!isObs);
  bool tzQOK = processTestZoneFlowData&& (!isObs);
  bool obsOK = processObsWellPressureData && isObs;

  for ( int i = 0; i < singleSequenceData.Size(); i++)
    if (singleSequenceData.IsValid(i))
    {
      Sequence& currSeq = singleSequenceData.GetRef(i);
      if (!currSeq.useInAutoSetup)
        continue;

      CopyString(seqID, currSeq.seqID, idLen);
      bool processDel = (SequencePageExists(true, "_process", parent) > 0);

      switch (currSeq.seqType) {
      case Sequence::stFlow : {
        if (flowSpec.ExtractOK() && (tzPOK || obsOK))
        {
          if (processDel)
            delCount++;
          delCount += SequencePageExists(processDel || flowSpec.DoSpec(fDiag), "_diag", parent);
          delCount += SequencePageExists(processDel || flowSpec.DoSpec(fFDDiag), "_FDdiag", parent);
          bool processHDel = (SequencePageExists(true, "_Hprocess", parent) > 0);
          if (processHDel)
            delCount++;
          delCount += SequencePageExists(processHDel || flowSpec.DoSpec(fHorner), "_Horner", parent);
        }
        break;
      }
      case Sequence::stHistory : {
        if (histSpec.ExtractOK() && (tzQOK || obsOK))
        {
          if (processDel)
            delCount++;
          delCount += SequencePageExists(processDel || histSpec.DoSpec(hDecline), "_decline", parent);
          delCount += SequencePageExists(processDel || histSpec.DoSpec(hDiag1), "_Diag1", parent);
          delCount += SequencePageExists(processDel || histSpec.DoSpec(hDiag2), "_Diag2", parent);
        }
        break;
      }
      case Sequence::stPulse : {
        if (pulseSpec.ExtractOK() && tzPOK)
        {
          if (processDel)
            delCount++;
          delCount += SequencePageExists(processDel || pulseSpec.DoSpec(psRameyA) , "_RameyA", parent);
          delCount += SequencePageExists(processDel || pulseSpec.DoSpec(psRameyB) , "_RameyB", parent);
          delCount += SequencePageExists(processDel || pulseSpec.DoSpec(psRameyC) , "_RameyC", parent);
          delCount += SequencePageExists(processDel || pulseSpec.DoSpec(psRameyFD) , "_RameyFD", parent);
        }
        break;
      }
      case Sequence::stSlug : {
        if (slugSpec.ExtractOK() && tzPOK)
        {
          if (processDel)
            delCount++;

          delCount += SequencePageExists(processDel || slugSpec.DoSpec(psRameyA) , "_RameyA", parent);
          delCount += SequencePageExists(processDel || slugSpec.DoSpec(psRameyB) , "_RameyB", parent);
          delCount += SequencePageExists(processDel || slugSpec.DoSpec(psRameyC) , "_RameyC", parent);
          delCount += SequencePageExists(processDel || slugSpec.DoSpec(psRameyFD) , "_RameyFD", parent);
        }
        break;
      }
      case Sequence::stPacker : {
        break;
      }

      }

    }

  return delCount;
}

int AutoSequenceWillDeletePages()
{
  autoRoot = &processingSetupRoot;
  int delCount = processingSetupRoot.PageExists("Seq_process", 0);

  autoRoot = &sequenceRoot;

  if (MultiWellIDS())
  {
    for (int i = 0; i < currentWells.Size(); i++)
    {
      MenuPageC* currWell = sequenceRoot.FindPage(currentWells[i], 0);
      if (currWell)
        delCount += DelSequencePagesCount(currWell, i > 0);
    }
  }
  else
    delCount += DelSequencePagesCount(0, false);

  return delCount;
}




}; // end NS

