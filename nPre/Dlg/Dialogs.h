///////////////////////////////////////////////////////////////////////////////////
//
// Dialogs.h
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
// DESCRIPTION:
//
//      Include file for all nPre dialogs.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DIALOGS_H
#define DIALOGS_H

#include "Resource.h"

#include <Dlg/DlgDummy.h>
#include <Dlg/DlgConfigMain.h>
#include <Dlg/DlgConfigUnits.h>
#include <Dlg/DlgConfigCurveFiles.h>
#include <Dlg/DlgConfigGas.h>
#include <Dlg/DlgConfigLiquid.h>
#include <Dlg/DlgConfigMatrix.h>
#include <Dlg/DlgConfigDescription.h>

#include <Dlg/DlgSeqTimeBase.h>
#include <Dlg/DlgSeqMain.h>
#include <Dlg/DlgSeqTestZoneCurve.h>
#include <Dlg/DlgSeqDynamicStep.h>
#include <Dlg/DlgSeqPartialRun.h>

#include <Dlg/DlgParParameter.h>

#include <Dlg/DlgNoPointsParameter.h>
#include <Dlg/DlgPointsParametersPointsEntry.h>
#include <Dlg/DlgPointsParametersInterpolation.h>
#include <Dlg/DlgPointsParametersUnitsTransform.h>
#include <Dlg/DlgPointsParametersYOptimization.h>

#include <osDlg/DlgOptimizationMain.h>
#include <osDlg/DlgOptimizationLMAlg.h>
#include <osDlg/DlgOptimizationSimplexAlg.h>
#include <osDlg/DlgOptimizationSimAnnealAlg.h>
#include <osDlg/DlgOptimizationSCEMAlg.h>
#include <osDlg/DlgOptimizationTolerances.h>
#include <osDlg/DlgOptimizationPerturbation.h>
#include <osDlg/DlgOptimizationFitSel.h>
#include <osDlg/DlgOptimizationCalcVarSel.h>

#include <Dlg/DlgDataCaptureMain.h>
#include <Dlg/DlgDataCaptureProdRestart.h>
#include <Dlg/DlgSuperpositionTab.h>

#include <osDlg/DlgSamplingMonteCarlo.h>
#include <osDlg/DlgSamplingCorrMatrix.h>
#include <osDlg/DlgSamplingSamples.h>

#include <Dlg/DlgVaryPriority.h>
#include <Dlg/DlgNoVaryParameter.h>

#include <Dlg/DlgOutputFileSetupStandard.h>
#include <Dlg/DlgOutputFileSetupProfile.h>
#include <Dlg/DlgOutputFileSetupRange.h>
#include <Dlg/DlgOutputFileSetupOptimization.h>

#endif // _DIALOGS_H

