///////////////////////////////////////////////////////////////////////////////////
//
// NPreInterfaceDesc.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "NPreInterfaceDesc.h"
#include <Dlg/Dialogs.h>

#include <Var/G_Control.h>
#include <Var/G_Parameter.h>
#include <Var/G_Sequence.h>
#include <Var/G_DataCapture.h>
#include <Var/G_OutputFiles.h>
#include <osMain/G_Sample.h>
#include <osMain/G_Optimize.h>
#include <osMain/G_Vary.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

NodeDescription nodes[];
NodeDescription nodesConfiguration[];
NodeDescription nodesSequence[];
NodeDescription nodesParameter[];
NodeDescription nodesPointsParameter[];
NodeDescription nodesPointsParameterOpt[];
NodeDescription nodesSimOutput[];
NodeDescription nodesFitSpecification[];
NodeDescription nodesOptimization[];
NodeDescription nodesSampling[];
NodeDescription nodesVary[];
NodeDescription nodesSamplingCorrelationMatrix[];
NodeDescription nodesOutputFileSetup[];
NodeDescription nodesPlotSetup[];

NodeDescription* GetNodesPointsParameters()
{
    if (control.IsOptimization())
        return nodesPointsParameterOpt;
    return nodesPointsParameter;
}

NodeDescription nodesRoot[] =
{
    { "nPre",       0, NULL, nodes},
    { NULL }
};

NodeDescription nodes[] =
{
    { "Configuration",              0, NULL, nodesConfiguration, &control},
    { "Wells and Output",           1, NULL, nodesSimOutput, &dataCapture},
    { "Sequences",                  2, NULL, nodesSequence, &sequence},
    { "Parameters",                 3, NULL, nodesParameter, &parameters},
    { "f(x)Points Parameters",      4, NULL, NULL,  &pointParameters},
    { "Fit Selection",              5, NULL, nodesFitSpecification, NULL},
    { "Optimization",               6, NULL, nodesOptimization, &optimize},
    { "Sampling",                   7, NULL, nodesSampling, NULL, },
    { "Suite/Range",                8, NULL, nodesVary, &vary},
    { "Output Files",               9, NULL, nodesOutputFileSetup, &outputFiles},
    { "Plots & Data Processing",    10, NULL, nodesPlotSetup, NULL},
    { "f(x) Points Parameter",      4, RUNTIME_CLASS(DlgNoPointsParameter), NULL  },
    { "Suite/Range",                8, RUNTIME_CLASS(DlgNoVaryParameter), NULL },
    { NULL }
};

NodeDescription nodesConfiguration[] =
{
    {"Main",            0,  RUNTIME_CLASS(DlgConfigMain),       NULL, &control },
    {"Liquid",          1,  RUNTIME_CLASS(DlgConfigLiquid),     NULL, &control },
    {"Gas",             2,  RUNTIME_CLASS(DlgConfigGas),        NULL, &control },
    {"Matrix",          3,  RUNTIME_CLASS(DlgConfigMatrix),     NULL, &control },
    {"Default Units",   4,  RUNTIME_CLASS(DlgConfigUnits),      NULL, &control },
    {"Curve Files",     5,  RUNTIME_CLASS(DlgConfigCurveFiles), NULL, &control },
    {"Test Description",6,  RUNTIME_CLASS(DlgConfigDescription),NULL, &control },
    { NULL }
};

NodeDescription nodesSimOutput[] =
{
    {"Main",                    0,  RUNTIME_CLASS(DlgDataCaptureMain),  NULL, &dataCapture},
    {"Production Restart",      0,  RUNTIME_CLASS(DlgDataCaptureProdRestart),   NULL, &dataCapture},
    {"Superposition",           0,  RUNTIME_CLASS(DlgSuperpositionTab), NULL, &dataCapture},
    { NULL }
};


NodeDescription nodesSequence[] =
{
    {"Time-Base",           0,  RUNTIME_CLASS(DlgSeqTimeBase),      NULL, &sequence  },
    {"Sequences",           1,  RUNTIME_CLASS(DlgSeqMain),          NULL, &sequence},
    {"TZ Curves",           2,  RUNTIME_CLASS(DlgSeqTestZoneCurve), NULL, &sequence },
    {"Dynamic Time Step",   3,  RUNTIME_CLASS(DlgSeqDynamicStep),   NULL, &sequence},
    {"Partial Run",         4,  RUNTIME_CLASS(DlgSeqPartialRun),    NULL, &sequence},
    { NULL }
};

NodeDescription nodesParameter[] =
{
    {"Parameters",          0,  RUNTIME_CLASS(DlgParParameter), NULL, NULL },
    { NULL }
};

NodeDescription nodesPointsParameter[] =
{
    {"Point Entry" ,        0,  RUNTIME_CLASS(DlgPointsParametersPointsEntry),    NULL, NULL },
    {"Interpolation",       0,  RUNTIME_CLASS(DlgPointsParametersInterpolation),  NULL, NULL },
    {"Units/Transform",     0,  RUNTIME_CLASS(DlgPointsParametersUnitsTransform), NULL, NULL },
    { NULL }
};

NodeDescription nodesPointsParameterOpt[] =
{
    {"Point Entry" ,        0,  RUNTIME_CLASS(DlgPointsParametersPointsEntry),      NULL, NULL },
    {"Interpolation",       0,  RUNTIME_CLASS(DlgPointsParametersInterpolation),    NULL, NULL },
    {"Units/Transform",     0,  RUNTIME_CLASS(DlgPointsParametersUnitsTransform),   NULL, NULL },
    {"Optimization",        0,  RUNTIME_CLASS(DlgPointsParametersYOptimization),    NULL, NULL },
    { NULL }
};


NodeDescription nodesFitSpecification[] =
{
    {"Fit Selection",       0,  RUNTIME_CLASS(DlgOptimizationFitSel),   NULL},
    { NULL }
};

NodeDescription nodesOptimization[] =
{
    {"Main",                0,  RUNTIME_CLASS(DlgOptimizationMain),         NULL, &optimize},
    {"Tolerances",          0,  RUNTIME_CLASS(DlgOptimizationTolerances),   NULL, &optimize},
    {"L-M",                 0,  RUNTIME_CLASS(DlgOptimizationLMAlg),        NULL, &optimize},
    {"Simplex",             0,  RUNTIME_CLASS(DlgOptimizationSimplexAlg),   NULL, &optimize},
    {"S-A",                 0,  RUNTIME_CLASS(DlgOptimizationSimAnnealAlg), NULL, &optimize},
    {"SCEM",                0,  RUNTIME_CLASS(DlgOptimizationSCEMAlg),      NULL, &optimize},
    {"Perturbation",        0,  RUNTIME_CLASS(DlgOptimizationPerturbation), NULL, &optimize},
    {"Calculated Vars",     0,  RUNTIME_CLASS(DlgOptimizationCalcVarSel),   NULL, &optimize},
    { NULL }
};

NodeDescription nodesSampling[] =
{
    {"Main",                0,  RUNTIME_CLASS(DlgSamplingMonteCarlo), NULL, &sample},
    {"Correlations",        0,  NULL, nodesSamplingCorrelationMatrix, &sample},
    {"Samples",             0,  RUNTIME_CLASS(DlgSamplingSamples), NULL,  &sample},
    { NULL }
};

NodeDescription nodesVary[] =
{
    {"Priority",            0,  RUNTIME_CLASS(DlgVaryPriority), NULL, &vary},
    { NULL }
};


NodeDescription nodesSamplingCorrelationMatrix[] =
{
    {"Parameter",           0,  RUNTIME_CLASS(DlgSamplingCorrMatrix), NULL },
    {"Sequence Q",          0,  RUNTIME_CLASS(DlgSamplingCorrMatrix), NULL },
    {"Sequence P",          0,  RUNTIME_CLASS(DlgSamplingCorrMatrix), NULL },
    {"Superposition R",     0,  RUNTIME_CLASS(DlgSamplingCorrMatrix), NULL },
    { NULL }
};

NodeDescription nodesOutputFileSetup[] =
{
    {"XY Data",             0,  RUNTIME_CLASS(DlgOutputFileSetupStandard),  NULL,  &outputFiles  },
    {"Profile",             0,  RUNTIME_CLASS(DlgOutputFileSetupProfile),   NULL,  &outputFiles },
    {"Range",               0,  RUNTIME_CLASS(DlgOutputFileSetupRange),     NULL,  &outputFiles },
    {"Optimization",        0,  RUNTIME_CLASS(DlgOutputFileSetupOptimization),NULL, &outputFiles },
    { NULL }
};

NodeDescription nodesPlotSetup[] =
{
    {"Field Data",      0,  NULL,   NULL    },
    {"Sequence",        1,  NULL,   NULL },
    {"Fit",             2,  NULL,   NULL},
    {"Runtime",         3,  NULL,   NULL},
    { NULL }
};

