///////////////////////////////////////////////////////////////////////////////////
//
// NPreInterfaceApp.cpp
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

#include "stdafx.h"

#include <genClass/U_Real.h>

#include <Var/G_Control.h>
#include <Var/G_Parameter.h>
#include <Var/G_Sequence.h>
#include <Var/G_DataCapture.h>

#include <App/nPreUtil.h>

#include <osMain/C_Sample.h>
#include <osMain/G_Sample.h>
#include <osMain/G_Vary.h>

#include <osDlg/DlgSamplingCorrMatrix.h>

#include "NPreInterfaceMView.h"
#include "NPreInterfaceDesc.h"

#include "NPreInterfaceApp.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

OutlookBar2::OutlookBar2(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    OutlookBar(pParent, nIdent, pDescription)
{
    NodeDescription* pDesc = pDescription->GetNext();
    AddNode(new TabbedFolderNodeConfig(this, MN_Configuration, &pDesc[MN_Configuration]));
    AddNode(new TabbedFolderNodeSimulationOutput(this, MN_SimOutput, &pDesc[MN_SimOutput]));
    AddNode(new TabbedFolderNodeSequence(this, MN_Sequence, &pDesc[MN_Sequence]));
    AddNode(new TabbedFolderNodeParameter(this, MN_Parameter, &pDesc[MN_Parameter]));
    AddNode(new TabbedFolderNodePointsParameter(this, MN_PointsParameter, &pDesc[MN_PointsParameter]));
    AddNode(new TabbedFolderNodeFitSpecification(this, MN_FitSpecification, &pDesc[MN_FitSpecification]));
    AddNode(new TabbedFolderNodeOptimization(this, MN_Optimization, &pDesc[MN_Optimization]));
    AddNode(new TabbedFolderNodeSampling(this, MN_Sampling, &pDesc[MN_Sampling]));
    AddNode(new TabbedFolderNodeVary(this, MN_Vary, &pDesc[MN_Vary]));
    AddNode(new TabbedFolderNodeOutputFileSetup(this, MN_Output_File_Setup, &pDesc[MN_Output_File_Setup]));
    AddNode(new TabbedFolderPlotSetup(this, MN_Plot_Setup, &pDesc[MN_Plot_Setup]));
}

void OutlookBar2::CheckRestrictions()
{
    OutlookPane* pOutlookPane = static_cast<OutlookPane*>(GetParentNode());

    pOutlookPane->EnableItem(0, MN_FitSpecification, control.IsOptimization() || control.IsRange());
    pOutlookPane->EnableItem(0, MN_Optimization, control.IsOptimization());
    pOutlookPane->EnableItem(0, MN_Sampling,     control.IsSampleSim());
    pOutlookPane->EnableItem(0, MN_Vary,     !control.IsSampleSim());

    pOutlookPane->Invalidate();
    pOutlookPane->UpdateWindow();
}

InterfaceNode* OutlookBar2::SelectNode(int nNode)
{
    if (nNode == MN_PointsParameter && nNode != m_nNodeSel) {
        // Points Parameter
        m_aNode[MN_PointsParameter]->DestroyNode();
        if (m_aNode[MN_PointsParameter]->IsCreated()) {
            //m_aNode[MN_PointsParameter].DestroyWindow();
        }
        delete m_aNode[MN_PointsParameter];

        nsParameterGlobals::ParameterArray pointParameters;
        parameters.GetPointsParameters(pointParameters);
        NodeDescription* pDesc = m_pDescription->GetNext();
        if (pointParameters.Size() == 0) {
            m_aNode[MN_PointsParameter] = new DialogNode(this, MN_PointsParameter, &pDesc[MN_PointsParameterAlter]);
        } else {
            TabbedFolderNodePointsParameter* pFolder = new TabbedFolderNodePointsParameter(this, MN_PointsParameter, &pDesc[MN_PointsParameter]);
            pFolder->ResetNodes(&pointParameters);
            m_aNode[MN_PointsParameter] = pFolder;
        }
    }

    if (nNode == MN_Vary && nNode != m_nNodeSel) {
        // vary
        m_aNode[MN_Vary]->DestroyNode();
        if (m_aNode[MN_Vary]->IsCreated()) {
            //m_aNode[MN_PointsParameter].DestroyWindow();
        }
        delete m_aNode[MN_Vary];

        NodeDescription* pDesc = m_pDescription->GetNext();
        if (VaryVar::GetnActive() == 0)
        {
            m_aNode[MN_Vary] = new DialogNode(this, MN_Vary, &pDesc[MN_VaryAlter]);
        }
        else
        {
            TabbedFolderNodeVary* pFolder = new TabbedFolderNodeVary(this, MN_Vary, &pDesc[MN_Vary]);
            m_aNode[MN_Vary] = pFolder;
        }
    }
    return OutlookBar::SelectNode(nNode);
}

//////////////////////////////////////////////////////////////////////////////////
TabbedFolderNodeConfig::TabbedFolderNodeConfig(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    TabbedFolderNode1(pParent, nIdent, pDescription)
{
}

void TabbedFolderNodeConfig::CheckRestrictions()
{
    Enable(CNFN_Liquid, control.IsLiquid());
    Enable(CNFN_Gas, control.IsGas());
    Enable(CNFN_Matrix, control.IsLiquid() && control.IsDual());
    Enable(CNFN_CurveFiles, control.UseCurveFiles());

    Invalidate();
    UpdateWindow();

    GetParentNode()->CheckRestrictions();
}

CWnd* TabbedFolderNodeConfig::ShowNode(bool bShow, CRect* pRect)
{
    CWnd* pWnd = TabbedFolderNode1::ShowNode(bShow, pRect);

    if (bShow) {

        if (simulationType == stOptimization)
            OptVar::SetWasActive();

        if (simulationSubType == sstSample)
            SampVar::SetWasActive();
        else
            VaryVar::SetWasActive();

    }

    return pWnd;
}

//////////////////////////////////////////////////////////////////////////////////
TabbedFolderNodeParameter::TabbedFolderNodeParameter(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    TabbedFolderNode1(pParent, nIdent, pDescription)
{
}

bool TabbedFolderNodeParameter::CreateNode(CWnd* pParentWnd)
{
    parameters.SetForFlags(); // Before InitDialog i.e. UpdateData
    bool bRc = TabbedFolderNode1::CreateNode(pParentWnd);
    CheckRestrictions();
    return bRc;
}

CWnd* TabbedFolderNodeParameter::ShowNode(bool bShow, CRect* pRect)
{
    CWnd* pWnd = TabbedFolderNode1::ShowNode(bShow, pRect);

    if (bShow) {
        parameters.SetForFlags();
        if (m_aNode[PARN_Parameters]->IsCreated()) {
            static_cast<DialogNode*>(m_aNode[PARN_Parameters])->GetDialog()->UpdateData(FALSE);
        }
    }

    return pWnd;
}


//////////////////////////////////////////////////////////////////////////////////
// Points Parameter Stuff
TabbedFolderNodePointsParameter::TabbedFolderNodePointsParameter(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    TabbedFolderNode(pParent, nIdent, pDescription)
{
}

void TabbedFolderNodePointsParameter::ResetNodes(ParameterArray* pointParameters)
{
    for (int i = 0; i < pointParameters->Size(); i++) {
        Parameter* pParameter = (*pointParameters)[i];
        NodeDescription* pItem = new NodeDescriptionRuntime((char *)pParameter->shortName, i, NULL, GetNodesPointsParameters());
        TabbedFolderNode* pTab = new TabbedFolderNodePointsParameterInside(this, i, pItem, pParameter);
        AddNode(pTab);
    }
}

void TabbedFolderNodePointsParameter::CheckRestrictions()
{
}

bool TabbedFolderNodePointsParameter::CreateNode(CWnd* pParentWnd)
{
    return TabbedFolderNode::CreateNode(pParentWnd);
}

//////////////////////////////////////////////////////////////////////////////////
TabbedFolderNodePointsParameterInside::TabbedFolderNodePointsParameterInside(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription,
                                                                             Parameter* pParameter) :
    TabbedFolderNode1(pParent, nIdent, pDescription),
    m_pParameter(pParameter)
{
}


//////////////////////////////////////////////////////////////////////////////////
TabbedFolderNodeFitSpecification::TabbedFolderNodeFitSpecification(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    TabbedFolderNode(pParent, nIdent, pDescription)
{
    NodeDescription* pDesc = pDescription->GetNext();
    AddNode(new DialogNode(this, FIT_Select, &pDesc[FIT_Select]));
}

void TabbedFolderNodeFitSpecification::CheckRestrictions()
{
    bool tabOK = ControlStaticSupport::IsOptimization() || ControlStaticSupport::IsRange();
    EnableTab(FIT_Select, tabOK);

    Invalidate();
    UpdateWindow();
}

//////////////////////////////////////////////////////////////////////////////////
TabbedFolderNodeOptimization::TabbedFolderNodeOptimization(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    TabbedFolderNode(pParent, nIdent, pDescription)
{
    NodeDescription* pDesc = pDescription->GetNext();
    AddNode(new DialogNode(this, OPTN_Main, &pDesc[OPTN_Main]));
    AddNode(new DialogNode(this, OPTN_Tolerances, &pDesc[OPTN_Tolerances]));
    AddNode(new DialogNode(this, OPTN_LevMarAlg, &pDesc[OPTN_LevMarAlg]));
    AddNode(new DialogNode(this, OPTN_SimplexAlg, &pDesc[OPTN_SimplexAlg]));
    AddNode(new DialogNode(this, OPTN_SimAnnealAlg, &pDesc[OPTN_SimAnnealAlg]));
    AddNode(new DialogNode(this, OPTN_SCEMAlg,      &pDesc[OPTN_SCEMAlg]));
    AddNode(new DialogNode(this, OPTN_Perturbation, &pDesc[OPTN_Perturbation]));
    AddNode(new DialogNode(this, OPTN_CalcVarSel,  &pDesc[OPTN_CalcVarSel]));

}

void TabbedFolderNodeOptimization::CheckRestrictions()
{
    EnableTab(OPTN_LevMarAlg, OptimizationControlStatics::UseLevMar());
    EnableTab(OPTN_SimplexAlg, OptimizationControlStatics::UseSimplex());
    EnableTab(OPTN_SimAnnealAlg, OptimizationControlStatics::UseSimAnneal());
    EnableTab(OPTN_SCEMAlg,     OptimizationControlStatics::UseSCEM());

//  EnableTab(OPTN_Perturbation, !control.IsRange());

    Invalidate();
    UpdateWindow();
}

//////////////////////////////////////////////////////////////////////////////////
TabbedFolderNodeVary::TabbedFolderNodeVary(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    TabbedFolderNode(pParent, nIdent, pDescription)
{
    NodeDescription* pDesc = pDescription->GetNext();
    AddNode(new DialogNode(this, VARN_Main, &pDesc[VARN_Main]));
}

void TabbedFolderNodeVary::CheckRestrictions()
{
    EnableTab(VARN_Main, VaryVar::GetnActive() > 0);
}

CWnd* TabbedFolderNodeVary::ShowNode(bool bShow, CRect* pRect)
{
    CWnd* pWnd = TabbedFolderNode::ShowNode(bShow, pRect);

    if (bShow) {
        vary.SetForFlags();
        if (m_aNode[VARN_Main]->IsCreated()) {
            static_cast<DialogNode*>(m_aNode[VARN_Main])->GetDialog()->UpdateData(FALSE);
        }
    }
    return pWnd;
}



//////////////////////////////////////////////////////////////////////////////////
TabbedFolderNodeSampling::TabbedFolderNodeSampling(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    TabbedFolderNode(pParent, nIdent, pDescription)
{
    NodeDescription* pDesc = pDescription->GetNext();
    AddNode(new DialogNode(this, SMPN_Main, &pDesc[SMPN_Main]));
    AddNode(new TabbedFolderNodeSamplingCorrelations(this, SMPN_CorrelationMatrix, &pDesc[SMPN_CorrelationMatrix]));
    AddNode(new DialogNode(this, SMPN_SamplingResults, &pDesc[SMPN_SamplingResults]));
}

void TabbedFolderNodeSampling::CheckRestrictions()
{
    bool corrOK = SampVar::CorrGroupOK(0) || SampVar::CorrGroupOK(1) ||
                  SampVar::CorrGroupOK(2) || SampVar::CorrGroupOK(3);

    EnableTab(SMPN_CorrelationMatrix, SampleControlStatics::specifyCorrelations && corrOK);
    EnableTab(SMPN_SamplingResults, sample.SampleOK());

    Invalidate();
    UpdateWindow();
}

CWnd* TabbedFolderNodeSampling::ShowNode(bool bShow, CRect* pRect)
{
    //  always go to main to avoid pain in corrmatrix etc
    // clear existing
    if (m_nNodeSel > 0)
    {
        TabbedFolderNode::ShowNode(false, pRect);
        m_nNodeSel = 0;
    }
    CWnd* temp = TabbedFolderNode::ShowNode(bShow, pRect);
    if (bShow)
        Show(0);

    return temp;
}

void TabbedFolderNodeSampling::Show(int nIdent)
{
    CheckRestrictions();

    if (nIdent == SMPN_SamplingResults)
    {
        CWaitCursor w;
        sample.Sample();
    }

    TabbedFolderNode::Show(nIdent);

    if (nIdent == SMPN_CorrelationMatrix)
        m_aNode[SMPN_CorrelationMatrix]->CheckRestrictions();
}



TabbedFolderNodeSamplingCorrelations::TabbedFolderNodeSamplingCorrelations(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription)
    : TabbedFolderNode(pParent, nIdent, pDescription)
{
    NodeDescription* pDesc = pDescription->GetNext();

    AddNode(new DialogNodeSamplingCorrGroup(this, SCMN_Parameter, &pDesc[SCMN_Parameter], 0));
    AddNode(new DialogNodeSamplingCorrGroup(this, SCMN_Sequence_flow_rate, &pDesc[SCMN_Sequence_flow_rate], 1));
    AddNode(new DialogNodeSamplingCorrGroup(this, SCMN_Sequence_pressure, &pDesc[SCMN_Sequence_pressure], 2));
    AddNode(new DialogNodeSamplingCorrGroup(this, SCMN_Superposition_radii, &pDesc[SCMN_Superposition_radii], 3));
}



void TabbedFolderNodeSamplingCorrelations::CheckRestrictions()
{
    EnableTab(SCMN_Parameter, SampVar::CorrGroupOK(0));
    EnableTab(SCMN_Sequence_flow_rate, SampVar::CorrGroupOK(1));
    EnableTab(SCMN_Sequence_pressure, SampVar::CorrGroupOK(2));
    EnableTab(SCMN_Superposition_radii, SampVar::CorrGroupOK(3));

    Invalidate();
    UpdateWindow();
}


//////////////////////////////////////////////////////////////////////////////////
TabbedFolderNodeSequence::TabbedFolderNodeSequence(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    TabbedFolderNode(pParent, nIdent, pDescription)
{
    NodeDescription* pDesc = pDescription->GetNext();
    AddNode(new DialogNode(this, SEQ_Time_Base, &pDesc[SEQ_Time_Base]));
    AddNode(new DialogNode(this, SEQ_Sequences, &pDesc[SEQ_Sequences]));
    AddNode(new DialogNode(this, SEQ_TZ_Curves, &pDesc[SEQ_TZ_Curves]));
    AddNode(new DialogNode(this, SEQ_Dynamic_Time_Step, &pDesc[SEQ_Dynamic_Time_Step]));
    AddNode(new DialogNode(this, SEQ_Partial_Run, &pDesc[SEQ_Partial_Run]));
}

CWnd* TabbedFolderNodeSequence::ShowNode(bool bShow, CRect* pRect)
{
    if (bShow)
        prevTimes = sequenceTimesFO.sequenceTimesDC.sequenceStartTimes;


    CWnd* pWnd = TabbedFolderNode::ShowNode(bShow, pRect);

    if (!bShow) {
        parameters.SetForFlags();
        sequence.SetSequenceTimes(false);
        sequenceTimesFO.DoStatusChk();
        if (!prevTimes.SameAs(sequenceTimesFO.sequenceTimesDC.sequenceStartTimes, 1.0E-06))
            nPreUtilities::ExecuteAndReplot(sequenceTimesFO);
    }

    return pWnd;
}


//////////////////////////////////////////////////////////////////////////////////
TabbedFolderNodeSimulationOutput::TabbedFolderNodeSimulationOutput(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    TabbedFolderNode(pParent, nIdent, pDescription)
{
    NodeDescription* pDesc = pDescription->GetNext();
    AddNode(new DialogNode(this, SON_Main, &pDesc[SON_Main]));
    AddNode(new DialogNode(this, SON_ProductionRestart, &pDesc[SON_ProductionRestart]));
    AddNode(new DialogNode(this, SON_Superposition, &pDesc[SON_Superposition]));
}

CWnd* TabbedFolderNodeSimulationOutput::ShowNode(bool bShow, CRect* pRect)
{
    if (bShow)
        dataCaptureUIChange = false;

    CWnd* pWnd = TabbedFolderNode::ShowNode(bShow, pRect);

    if ((!bShow) && dataCaptureUIChange) 
    {
        dataCapture.DataCaptureOK();
        nPreUtilities::ExecuteAndReplot(capturedDataFO);
    }

    return pWnd;
}

//////////////////////////////////////////////////////////////////////////////////
TabbedFolderNodeOutputFileSetup::TabbedFolderNodeOutputFileSetup(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    TabbedFolderNode(pParent, nIdent, pDescription)
{
    NodeDescription* pDesc = pDescription->GetNext();
    AddNode(new DialogNode(this, OFSN_Standard, &pDesc[OFSN_Standard]));
    AddNode(new DialogNode(this, OFSN_Profile, &pDesc[OFSN_Profile]));
    AddNode(new DialogNode(this, OFSN_Range, &pDesc[OFSN_Range]));
    AddNode(new DialogNode(this, OFSN_Optimization, &pDesc[OFSN_Optimization]));
}

void TabbedFolderNodeOutputFileSetup::CheckRestrictions()
{
    EnableTab(OFSN_Standard, !control.IsRange());
    EnableTab(OFSN_Profile, control.IsForward() && control.IsNormal());
    EnableTab(OFSN_Range, control.IsRange());
    EnableTab(OFSN_Optimization, control.IsOptimization() && !control.IsRange());

    SelectFirstEnabledTab();

    Invalidate();
    UpdateWindow();
}

bool TabbedFolderNodeOutputFileSetup::CreateNode(CWnd* pParentWnd)
{
    bool bRc = TabbedFolderNode::CreateNode(pParentWnd);

    CheckRestrictions();

    return bRc;
}

CWnd* TabbedFolderNodeOutputFileSetup::ShowNode(bool bShow, CRect* pRect)
{
    CWnd* pWnd = TabbedFolderNode::ShowNode(bShow, pRect);

    if (bShow) {
        CheckRestrictions();
    }

    return pWnd;
}


TabbedFolderPlotSetup::TabbedFolderPlotSetup(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
    TabbedFolderNode(pParent, nIdent, pDescription)
{
    NodeDescription* pDesc = pDescription->GetNext();

    AddNode(new FieldDataMViewNode(this, PS_Field, &pDesc[PS_Field]));
    AddNode(new SequenceMViewNode(this, PS_Sequence, &pDesc[PS_Sequence]));
    AddNode(new FitSpecMViewNode(this, PS_Fit, &pDesc[PS_Fit]));
    AddNode(new ProcessingSetupMViewNode(this, PS_Post, &pDesc[PS_Post]));
}


void TabbedFolderPlotSetup::CheckRestrictions()
{
    bool fitOK = ControlStaticSupport::IsOptimization() || ControlStaticSupport::IsRange();
    EnableTab(PS_Fit, fitOK);
}

