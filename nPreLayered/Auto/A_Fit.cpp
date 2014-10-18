///////////////////////////////////////////////////////////////////////////////////
//
// A_Fit.cpp
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

#include <genApp/C_AppMenu.h>

#include <genPFO/PFO_DataLabels.h>

#include <objDFO/DFO_XYScaleTransform.h>
#include <nsDFO/DFO_ExtractSequenceInterval.h>

#include <osDPO/DPO_SingleFit.h>
#include <osDPO/DPO_CompositeFit.h>

#include <Var/G_Sequence.h>
#include <osMain/G_Optimize.h>

#include <App/nPreRoot.h>

#include <Auto/A_Main.h>
#include <Auto/A_Fit.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace nsAutoSetup  {

  FitAutoSpec::FitAutoSpec(int nS) : nSpec(nS)
{
  doExtract = false;
  for (int i = 0; i < maxSpec; i++)
  {
    doSpec[i] = false;
    specID[i] = "n/a";
  }

}

bool FitAutoSpec::IsEnabled(int indx) const
{
  if (indx >= nSpec)
    return false;

  int combIndex = nSpec - 1;
  if (indx == combIndex)  // combined
  {
    int nfit = 0;
    for (int i = 0; i < combIndex; i++)
      if (doSpec[i])
        nfit++;
    return (nfit > 1);
  }

  return true;
}

bool FitAutoSpec::DoSpec(int indx) const
{
  return doSpec[indx] && IsEnabled(indx);
}


enum    {cPress     = 0,
         cFlow      = 1,
             cComb      = 2};

  CartesianFitAutoSpec::CartesianFitAutoSpec() : FitAutoSpec(3)
{
  specID[cPress]      = "Pressure";
  specID[cFlow]       = "Flow Rate";
  specID[cComb]       = "Combined";
}

bool CartesianFitAutoSpec::IsEnabled(int indx) const
{
  if (indx >= nSpec)
    return false;

  switch (indx) {
  case cPress :
    return flowFitSpec.opOK || pulseFitSpec.opOK || slugFitSpec.opOK;
  case cFlow  :
    return histFitSpec.opOK;
  case cComb  :
    return doSpec[cPress];
  }

  return false;
}


enum  {flogP    = 0,
       flogdP     = 1,
       fflowD     = 2,
       fHorner    = 3,
       fBourdet   = 4,
       fComb      = 5};

    FlowFitAutoSpec::FlowFitAutoSpec() : FitAutoSpec(6)
{
  specID[flogP]   = "log Pressure";
  specID[flogdP]    = "log dPressure";
  specID[fflowD]    = "flow dimension";
  specID[fHorner]   = "horner";
  specID[fBourdet]  = "bourdet";
  specID[fComb]   = "Combined";
}

bool FlowFitAutoSpec::HornerOrBourdet() const
{
  return (DoSpec(fHorner) || DoSpec(fBourdet)) && (! (DoSpec(flogP) || DoSpec(flogdP) || DoSpec(fflowD)));
}


enum  {hDecline   = 0,
       hDiag1     = 1,
       hDiag2         = 2,
       hComb          = 3};

    HistFitAutoSpec::HistFitAutoSpec() : FitAutoSpec(4)
{
  specID[hDecline]    = "log Flow";
  specID[hDiag1]      = "d2Flow";
  specID[hDiag2]      = "d2InvFlow";
  specID[hComb]   = "Combined";
}


enum  {psPnorm   = 0,
       psPnorm1    = 1,
       psRameyA  = 2,
       psRameyB  = 3,
       psRameyC  = 4,
       psDeconv  = 5,
       psComb      = 6};

    PulseFitAutoSpec::PulseFitAutoSpec() : FitAutoSpec(7)
{
  specID[psPnorm]     = "P(norm)";
  specID[psPnorm1]    = "P(1-norm)";
  specID[psRameyA]    = "Ramey A";
  specID[psRameyB]    = "Ramey B";
  specID[psRameyC]    = "Ramey C";
  specID[psDeconv]    = "Deconvolution";
  specID[psComb]   = "Combined";
}


//  declarations and initial values

CartesianFitAutoSpec  cartesianFitSpec;
FlowFitAutoSpec     flowFitSpec;
HistFitAutoSpec     histFitSpec;
PulseFitAutoSpec    pulseFitSpec;
PulseFitAutoSpec    slugFitSpec;

static int firstQSeq, firstPSeq, lastQSeq, lastPSeq;

void AutoFitCheckOK()
{
  sequence.SequencesOK();

  cartesianFitSpec.Init();
  flowFitSpec.Init();
  histFitSpec.Init();
  pulseFitSpec.Init();
  slugFitSpec.Init();

  firstQSeq = firstPSeq = lastQSeq = lastPSeq = -1;

  for ( int i = 0; i < singleSequenceData.Size(); i++)
    if (singleSequenceData.IsValid(i))
    {
      Sequence& currSeq = singleSequenceData.GetRef(i);
      if (!currSeq.useInAutoSetup)
        continue;

      cartesianFitSpec.opOK = true;
      cartesianFitSpec.seqCount++;

      switch (currSeq.seqType) {
      case Sequence::stFlow : {
        flowFitSpec.opOK = true;
        flowFitSpec.seqCount++;
        break;
      }
      case Sequence::stHistory : {
        if (firstQSeq < 0)
          firstQSeq = i;
        lastQSeq = i;
        histFitSpec.opOK = true;
        histFitSpec.seqCount++;
        break;
      }
      case Sequence::stPulse : {
        pulseFitSpec.opOK = true;
        pulseFitSpec.seqCount++;
        break;
      }
      case Sequence::stSlug : {
        slugFitSpec.opOK = true;
        slugFitSpec.seqCount++;
        break;
      }
      case Sequence::stPacker : {
        break;
      }
      }

      if (currSeq.seqType != Sequence::stHistory)
      {
        if (firstPSeq < 0)
          firstPSeq = i;
        lastPSeq = i;
      }

    }
}


static void STSetup(DFO_XYScaleTransform& currST)
{
  currST.tranOp = DSC_ScaleTransform::tt_Lgt;
  CopyString(currST.outputTypeDesc, "log input", DFO_XYScaleTransform::descLen);
}

static DFO_XYScaleTransform* AddFieldST()
{
  DFO_XYScaleTransform* currST = static_cast<DFO_XYScaleTransform*>(AddFieldObject("DPO_XYScaleTransform"));
  STSetup(*currST);
  return currST;
}

static DFO_XYScaleTransform* AddSimST()
{
  DFO_XYScaleTransform* currST = static_cast<DFO_XYScaleTransform*>(AddSimObject());
  STSetup(*currST);
  return currST;
}

static T_SC_BasicPtrArray<DFO_SingleFit> currFits;

static DFO_SingleFit* SingleFit(const char* dataID,
                                const char* fitID,
                                bool  logY)
{
  SetObjRef(typeid(DO_XYData));

  char tempStr[80];
  const char* uscor = "_";
  if (IsNull(seqID))
    uscor = NULL;

  MakeString(tempStr, uscor, "f", dataID, 80);
  MakeObjID(fieldID, tempStr);
  FuncObjRef fieldDATFO = GetMatchingObj(fieldID);

  MakeString(tempStr, uscor, "s", dataID, 80);
  MakeObjID(simID, tempStr);
  FuncObjRef simDATFO = GetMatchingObj(simID);

  if (logY)
  {
    CopyString(tempStr, fieldID, 80);
    MakeString(fieldID, "log_", tempStr, idLen);
    DFO_XYScaleTransform* logFieldFO = AddFieldST();
    logFieldFO->xyInputObjRef = fieldDATFO;
    fieldDATFO = FuncObjRef(*logFieldFO);

    CopyString(tempStr, simID, 80);
    MakeString(simID, "log_", tempStr, idLen);
    DFO_XYScaleTransform* logSimFO = AddSimST();
    logSimFO->xyInputObjRef = simDATFO;

    simDATFO = FuncObjRef(*logSimFO);
  }

  if (IsNull(seqID))
    CopyString(tempStr, fitID, 80);
  else
    MakeString(tempStr, seqID, "_", fitID, 80);

  DFO_SingleFit* currFit = static_cast<DPO_SingleFit*>(AddNewObject("DPO_SingleFit", tempStr));
  currFit->simResultsObjRef = simDATFO;
  currFit->fieldDataObjRef = fieldDATFO;

  currFits += currFit;

  return currFit;
}

static void AddToComposite(DFO_CompositeFit& compFit,
                           DFO_SingleFit*    fitToAdd)
{
  if (fitToAdd == 0)
    return;

  for (int i = 0; i < DFO_CompositeFit::maxFitData; i++)
  {
    DFO_CompositeFit::FitData& nextFit = compFit.fitData[i];
    if (!nextFit.useFit)
    {
      nextFit.useFit = true;
      nextFit.inputFitObjRef = FuncObjRef(fitToAdd);
      break;
    }
  }
}

static DFO_CompositeFit* CompositeFit(const char* fitID)
{
  DFO_CompositeFit* compFit = static_cast<DPO_CompositeFit*>(AddNewObject("DPO_CompositeFit", fitID));
  for (int i = 0; i < currFits.Size(); i++)
    AddToComposite(*compFit, currFits[i]);
  return compFit;
}


static void MakeFitPage(MenuPageC* parent)
{
  char pageID[idLen];
  MakeObjID(pageID, "_fit");

  // delete old
  fitSpecRoot.DeletePage(pageID, parent);

  // make new
  AddNewDataPage(pageID, parent);
}

static int firstAutoSequence;
static int lastAutoSequence;

static void CartesianExtract(const char* datID)
{
  SetObjRef(typeid(DO_XYData));

  MakeObjID(fieldID, "f", datID, wellID);
  FuncObjRef fieldDATFO = GetMatchingObj(fieldID);

  MakeObjID(simID, "s", datID, wellID);
  FuncObjRef simDATFO = GetMatchingObj(simID);

  // make extract objects
  MakeObjID(fieldID, "fCart", datID, wellID);
  DFO_ExtractSequenceInterval* fieldEXFO = static_cast<DFO_ExtractSequenceInterval*>(AddFieldObject("DPO_ExtractSequenceInterval"));
  fieldEXFO->xyInputDataObjRef = fieldDATFO;
  fieldEXFO->seqTimesObjRef = FuncObjRef(sequenceTimesFO);
  fieldEXFO->setStartAndEnd = true;
  fieldEXFO->startSeqMS.enteredIndexValue = firstAutoSequence;
  fieldEXFO->endSeqMS.enteredIndexValue = lastAutoSequence;

  MakeObjID(simID, "sCart", datID, wellID);
  DFO_ExtractSequenceInterval* simEXFO = static_cast<DFO_ExtractSequenceInterval*>(AddSimObject());
  simEXFO->xyInputDataObjRef = simDATFO;
  simEXFO->startSeqMS.indexIsMaster = false;
  simEXFO->startSeqMS.indexMasterObjRef = FuncObjRef(fieldEXFO, 0);
  simEXFO->endSeqMS.indexIsMaster = false;
  simEXFO->endSeqMS.indexMasterObjRef = FuncObjRef(fieldEXFO, 1);
}


static void MakeCartesianFit(MenuPageC* parent)
{
  MakeFitPage(parent);
  SetToNull(seqID);

  // reset wells
  int maxLoop = 1;
  if (MultiWellIDS())
    maxLoop = currentWells.Size();

  currFits.SetResizable(maxLoop + 1);
  char fitID[80], dataID[80];
  for (int i = 0; i < maxLoop; i++)
  {
    wellID = currentWells[i];

    bool doPressure = processObsWellPressureData;
    if (i == 0)
      doPressure = processTestZonePressureData;

    if (cartesianFitSpec.DoSpec(0) && doPressure)
    {
      CartesianExtract("P");
      MakeString(fitID, "Cart_", wellID, "_P", 80);
      MakeString(dataID, "CartP", wellID, 80);
      SingleFit(dataID, fitID, false);
    }

    if ((i == 0) && cartesianFitSpec.DoSpec(1) && processTestZoneFlowData)
    {
      CartesianExtract("Q");
      MakeString(fitID, "Cart_", wellID, "_Q", 80);
      MakeString(dataID, "CartQ", wellID, 80);
      SingleFit(dataID, fitID, false);
    }
  }

  if (cartesianFitSpec.DoSpec(2) && (currFits.Size() > 1))
    CompositeFit("CartAll");
}

static void MakeFlowFit(MenuPageC* parent,
                        int      seqIndex)
{
  if ((seqIndex == 0) && flowFitSpec.HornerOrBourdet())
    return;

  MakeFitPage(parent);

  char tempID[idLen];

  currFits.SetResizable(8);
  if (flowFitSpec.DoSpec(flogP))
  {
    MakeString(tempID, "P", wellID, idLen);
    SingleFit(tempID, "log P", true);
  }

  if (flowFitSpec.DoSpec(flogdP))
  {
    MakeString(tempID, "dP", wellID, "/dlnt", idLen);
    SingleFit(tempID, "log dP", true);
  }

  if (flowFitSpec.DoSpec(fflowD))
  {
    MakeString(tempID, "-2d2logP", wellID, "/d2logt+2", idLen);
    SingleFit(tempID, "FD", false);
  }
  if ((seqIndex > 0) && flowFitSpec.DoSpec(fHorner))
  {
    MakeString(tempID, "Horn(", wellID, ")", idLen);
    SingleFit(tempID, "Horner", false);
  }
  if ((seqIndex > 0) && flowFitSpec.DoSpec(fBourdet))
  {
    MakeString(tempID, "dP", wellID, "/dBt", idLen);
    SingleFit(tempID, "Bourdet", true);
  }

  if (flowFitSpec.DoSpec(fComb))
  {
    MakeString(tempID, "P", wellID, "_combined", idLen);
    CompositeFit(tempID);
  }
}

static void MakeHistoryFit(const char* prefix, MenuPageC* parent)
{
  MakeFitPage(parent);
  char tempID[idLen], fitID[idLen];

  currFits.SetResizable(8);
  if (histFitSpec.DoSpec(hDecline))
  {
    MakeString(tempID, prefix, wellID, idLen);
    MakeString(fitID, "log", prefix, wellID, idLen);
    SingleFit(tempID, fitID, true);
  }

  if (histFitSpec.DoSpec(hDiag1))
  {
    MakeString(tempID, "dlog", prefix, wellID, "/dlogt", idLen);
    SingleFit(tempID, tempID, false);
  }

  if (histFitSpec.DoSpec(hDiag2))
  {
    MakeString(tempID, "d2", prefix, wellID, "inv/dlogt", idLen);
    SingleFit(tempID, tempID, false);
  }

  if (histFitSpec.DoSpec(hComb))
  {
    MakeString(tempID, prefix, wellID, "_combined", idLen);
    CompositeFit(tempID);
  }
}

static void MakePulseSlugFit(const PulseFitAutoSpec& inSpec, MenuPageC* parent)
{
  MakeFitPage(parent);

  currFits.SetResizable(8);
  if (inSpec.DoSpec(psPnorm))
  {
    SingleFit("P(norm)", "P(norm)", true);
  }

  if (inSpec.DoSpec(psPnorm1))
  {
    SingleFit("P(1-norm)", "P(1-norm)", true);
  }

  if (inSpec.DoSpec(psRameyA))
  {
    SingleFit("dP(norm)", "RameyA", true);
  }

  if (inSpec.DoSpec(psRameyB))
  {
    SingleFit("dlogP(norm)", "RameyB", true);
  }

  if (inSpec.DoSpec(psRameyC))
  {
    SingleFit("dlogP(1-norm)", "RameyC", true);
  }
  if (inSpec.DoSpec(psDeconv))
  {
    SingleFit("dP(deconv)", "Deconvolution", true);
  }

  if (inSpec.DoSpec(psComb))
    CompositeFit("Combined");
}


static int FitPageExists(MenuPageC* parent)
{
  char tempID[idLen];
  MakeObjID(tempID, "_fit");
  if (fitSpecRoot.PageExists(tempID, parent))
    return 1;

  return 0;
}

static void DeleteFitPage(MenuPageC* parent)
{
  char tempID[idLen];
  MakeObjID(tempID, "_fit");
  fitSpecRoot.DeletePage(tempID, parent);
}


static void AddFitPages(MenuPageC* parent, bool isObs)
{

  bool doPressure = processTestZonePressureData;
  bool doFlow  = processTestZoneFlowData;
  if (isObs)
  {
    doPressure = processObsWellPressureData;
    doFlow = false;
  }

  for (int i = 0; i < singleSequenceData.Size(); i++)
    if (singleSequenceData.IsValid(i))
    {
      Sequence& currSeq = singleSequenceData.GetRef(i);
      if (!currSeq.useInAutoSetup)
        continue;

      CopyString(seqID, currSeq.seqID, idLen);

      switch (currSeq.seqType) {
      case Sequence::stFlow : {

        if (doPressure && flowFitSpec.ExtractOK())
        {
          MakeFlowFit(parent, i);
        }
        break;
      }
      case Sequence::stHistory : {

        if (doFlow && histFitSpec.ExtractOK())
        {
          const char* prefix = "Q";
          if (isObs)
            prefix = "P";
          MakeHistoryFit(prefix, parent);
        }
        break;
      }
      case Sequence::stPulse : {
        if ((!isObs) && doPressure && pulseFitSpec.ExtractOK())
        {
          MakePulseSlugFit(pulseFitSpec, parent);
        }
        break;
      }
      case Sequence::stSlug : {

        if ((!isObs) && doPressure && slugFitSpec.ExtractOK())
        {
          MakePulseSlugFit(slugFitSpec, parent);
        }
        break;
      }
      case Sequence::stPacker : {
        break;
      }
      }
    }
}


static void DeleteFitPages(MenuPageC* parent, bool isObs)
{
  for ( int i = 0; i < singleSequenceData.Size(); i++)
    if (singleSequenceData.IsValid(i))
    {
      Sequence& currSeq = singleSequenceData.GetRef(i);
      if (!currSeq.useInAutoSetup)
        continue;

      CopyString(seqID, currSeq.seqID, idLen);

      bool processDel = false;
      switch (currSeq.seqType) {
      case Sequence::stFlow : {
        processDel = flowFitSpec.ExtractOK();
        break;
      }
      case Sequence::stHistory : {
        processDel =  histFitSpec.ExtractOK();
        break;
      }
      case Sequence::stPulse : {
        processDel = (!isObs) && pulseFitSpec.ExtractOK();
        break;
      }
      case Sequence::stSlug : {
        processDel = (!isObs) && slugFitSpec.ExtractOK();
        break;
      }
      case Sequence::stPacker : {
        break;
      }
      }
      if (processDel)
        DeleteFitPage(parent);
    }

}


void AutoFitSetup()
{
  MenuRootC::currTreeObj = NULL;
  fitSpecRoot.CloseUI();
  autoRoot = &fitSpecRoot;

  firstAutoSequence = lastAutoSequence = -1;

  for (int i = 0; i < singleSequenceData.Size(); i++)
    if (singleSequenceData.IsValid(i))
    {
      Sequence& currSeq = singleSequenceData.GetRef(i);
      if (currSeq.useInAutoSetup)
      {
        if (firstAutoSequence < 0)
          firstAutoSequence = i;
        lastAutoSequence = i;
      }
    }

  bool multiWells = MultiWellIDS();

  // cartesian fit first
  if ((lastAutoSequence >= 0) && cartesianFitSpec.ExtractOK() && (cartesianFitSpec.DoSpec(0) || cartesianFitSpec.DoSpec(1)))
  {
    CopyString(seqID, "AllSeq", idLen);
    MakeCartesianFit(0);
  }

  if (multiWells)
  {
    for (int i = 0; i < currentWells.Size(); i++)
    {
      MenuPageC* currWell = fitSpecRoot.FindPage(currentWells[i], 0);
      if (currWell)
      {
        DeleteFitPages(currWell, i > 0);
        if (currWell->childPages.IsEmpty())
          fitSpecRoot.DeletePage(currentWells[i], 0);
      }
    }

    for (int i = 0; i < currentWells.Size(); i++)
    {
      bool doWell = processTestZonePressureData || processTestZoneFlowData;
      if (i > 0)
        doWell = processObsWellPressureData;

      if (doWell)
      {
        wellID = currentWells[i];
        MenuPageC* currWell = fitSpecRoot.FindPage(wellID, 0);
        if (!currWell)
        {
          AddNewDataPage(wellID, 0);
          currWell = autoPage;
        }
        AddFitPages(currWell, i > 0);
      }
    }
  }
  else
  {
    DeleteFitPages(0, false);
    wellID = currentWells[0];
    AddFitPages(0, false);
  }

  optimize.UpdateStatus();
  RecalcAll();

  // reset fits
  SetObjRef(typeid(DO_FitResults));
  for (int i = 0; i < OptimizeInput::maxOptFitSel; i++)
  {
    if (i < autoObjects.Size())
    {
      optimizeInput.inputFits[i].optimizeOnFit = true;
      optimizeInput.inputFits[i].fitObject = autoObjects[i];
    }
    else
    {
      optimizeInput.inputFits[i].optimizeOnFit = false;
      optimizeInput.inputFits[i].fitObject = autoObjects[0];
    }
  }
}


static int DelFitPagesCount(MenuPageC* parent, bool isObs)
{
  int delCount = 0;

  for (int i = 0; i < singleSequenceData.Size(); i++)
    if (singleSequenceData.IsValid(i))
    {
      Sequence& currSeq = singleSequenceData.GetRef(i);
      if (!currSeq.useInAutoSetup)
        continue;

      CopyString(seqID, currSeq.seqID, idLen);

      bool processDel = false;
      switch (currSeq.seqType) {
      case Sequence::stFlow : {
        processDel = flowFitSpec.ExtractOK();
        break;
      }
      case Sequence::stHistory : {
        processDel =  histFitSpec.ExtractOK();
        break;
      }
      case Sequence::stPulse : {
        processDel = (!isObs) && pulseFitSpec.ExtractOK();
        break;
      }
      case Sequence::stSlug : {
        processDel = (!isObs) && slugFitSpec.ExtractOK();
        break;
      }
      case Sequence::stPacker : {
        break;
      }
      }
      if (processDel && (FitPageExists(parent) > 0))
        delCount++;
    }

  return delCount;
}

int AutoFitWillDeletePages()
{
  autoRoot = &fitSpecRoot;
  int delCount = 0;

  // cartesian
  {
    CopyString(seqID, "AllSeq", idLen);
    bool processDel = (FitPageExists(0) > 0);
    if (processDel && cartesianFitSpec.ExtractOK() && (cartesianFitSpec.DoSpec(0) || cartesianFitSpec.DoSpec(1)))
      delCount++;
  }


  if (MultiWellIDS())
  {
    for (int i = 0; i < currentWells.Size(); i++)
    {
      MenuPageC* currWell = fitSpecRoot.FindPage(currentWells[i], 0);
      if (currWell)
        delCount += DelFitPagesCount(currWell, i > 0);
    }
  }
  else
    delCount += DelFitPagesCount(0, false);

  return delCount;
}




}; // end NS

