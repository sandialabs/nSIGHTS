///////////////////////////////////////////////////////////////////////////////////
//
// NPreInterfaceDesc.h
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
//      Tabbed menu and dialog layout.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef NPREINTERFACEDESC_H
#define NPREINTERFACEDESC_H

#include <genInterface/InterfaceDesc.h>


NodeDescription* GetNodesPointsParameters();

enum MainNodes {
    MN_Configuration        = 0,
    MN_SimOutput            = 1,
    MN_Sequence             = 2,
    MN_Parameter            = 3,
    MN_PointsParameter      = 4,
    MN_FitSpecification     = 5,
    MN_Optimization         = 6,
    MN_Sampling             = 7,
    MN_Vary                 = 8,
    MN_Output_File_Setup    = 9,
    MN_Plot_Setup           = 10,
    MN_PointsParameterAlter = 11,
    MN_VaryAlter            = 12
};

enum ConfigNodes {
    CNFN_Main       = 0,
    CNFN_Liquid     = 1,
    CNFN_Gas        = 2,
    CNFN_Matrix     = 3,
    CNFN_Units      = 4,
    CNFN_CurveFiles = 5,
    CNFN_TestDescription = 6
};

enum SimOutputNodes {
    SON_Main = 0,
    SON_ProductionRestart = 1,
    SON_Superposition = 2
};

enum SequenceNode {
    SEQ_Time_Base           = 0,
    SEQ_Sequences           = 1,
    SEQ_TZ_Curves           = 2,
    SEQ_Dynamic_Time_Step   = 3,
    SEQ_Partial_Run         = 4
};

enum ParameterNodes {
    PARN_Parameters = 0
};


enum FitSpecificationNodes {
    FIT_Select = 0
};

enum OptimizationNodes {
    OPTN_Main = 0,
    OPTN_Tolerances     = 1,
    OPTN_LevMarAlg      = 2,
    OPTN_SimplexAlg     = 3,
    OPTN_SimAnnealAlg   = 4,
    OPTN_SCEMAlg        = 5,
    OPTN_Perturbation   = 6,
    OPTN_CalcVarSel     = 7
};

enum SamplingNodes {
    SMPN_Main = 0,
    SMPN_CorrelationMatrix = 1,
    SMPN_SamplingResults = 2
};

enum VaryNodes {
    VARN_Main = 0
};

enum SamplingCorrelationMatrixNodes {
    SCMN_Parameter = 0,
    SCMN_Sequence_flow_rate = 1,
    SCMN_Sequence_pressure = 2,
    SCMN_Superposition_radii = 3
};

enum OutputFileSetupNodes {
    OFSN_Standard = 0,
    OFSN_Profile = 1,
    OFSN_Range = 2,
    OFSN_Optimization = 3
};


enum PlotSetupNode {
    PS_Field        = 0,
    PS_Sequence     = 1,
    PS_Fit          = 2,
    PS_Post         = 3
};

#endif // _INTERFACEDESC_H

