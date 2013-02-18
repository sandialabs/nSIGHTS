///////////////////////////////////////////////////////////////////////////////////
//
// DlgConfigMain.cpp
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
//      Dialog for nPre.
//
///////////////////////////////////////////////////////////////////////////////////

// DlgConfigMain.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgConfigMain.h"

#include <Var/G_Control.h>
#include <Var/G_Parameter.h>
#include <Var/G_Sequence.h>
#include <Var/G_DataCapture.h>
#include <Var/G_OutputFiles.h>
#include <osMain/G_Optimize.h>

#include <genInterface/InterfaceCtrls.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgConfigMain dialog

IMPLEMENT_DYNCREATE(DlgConfigMain,CGenericDialog)

DlgConfigMain::DlgConfigMain(CWnd* pParent /*=NULL*/)
   : CGenericDialog(DlgConfigMain::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgConfigMain)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}



BEGIN_MESSAGE_MAP(DlgConfigMain, CGenericDialog)
    //{{AFX_MSG_MAP(DlgConfigMain)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgConfigMain message handlers

BOOL DlgConfigMain::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    m_usimulationType.AddMappedEntry("Forward", stForward);
    m_usimulationType.AddMappedEntry("Optimization", stOptimization);
    m_usimulationType.Set(this, IDC_COMBO1, (int*)(&simulationType));
    m_usimulationType.AddCallback(this);

    m_usimulationSubType.AddMappedEntry("Normal", sstNormal);
    m_usimulationSubType.AddMappedEntry("Range", sstRange);
    m_usimulationSubType.AddMappedEntry("Sampling", sstSample);
    m_usimulationSubType.Set(this, IDC_COMBO2, (int*)(&simulationSubType));
    m_usimulationSubType.AddCallback(this, 1);


    m_usystemIsLiquid.Set(this, IDC_COMBO3,  systemIsLiquid, "Gas", "Liquid");
    m_usystemIsLiquid.AddCallback(this);

    m_usimulationIs2D.Set(this, IDC_COMBO4,  simulationIs2D, "1D Radial", "2D Radial");
    m_usimulationIs2D.AddCallback(this);

    m_aquiferType.Set(this, IDC_COMBO5, twoDisConfined, "Unconfined", "Confined");
    m_aquiferType.AddCallback(this);

    m_permType.Set(this, IDC_COMBO6, twoDisIsotropic, "Anisotropic", "Isotropic");
    m_permType.AddCallback(this);

    m_wellType.Set(this, IDC_COMBO7, twoDisPartialPenetration, "Full", "Partial");
    m_wellType.AddCallback(this);

    m_partialType.AddMappedEntry("Top", pptTop);
    m_partialType.AddMappedEntry("Middle", pptMiddle);
    m_partialType.AddMappedEntry("Bottom", pptBottom);
    m_partialType.Set(this, IDC_COMBO8,  (int*) (&partialPenetrationType));
    m_partialType.AddCallback(this);

    m_wellModelType.AddMappedEntry("Analytic", wmtAnalytic);
    m_wellModelType.AddMappedEntry("kV Multiplier", wmtMultiplier);
    m_wellModelType.AddMappedEntry("Uniform", wmtUniform);

    m_wellModelType.Set(this, IDC_COMBO9, (int*) (&wellModelType));
    m_wellModelType.AddCallback(this);


    m_udualPorosity.Set(this, IDC_COMBO10,  dualPorosity, "Single", "Dual");
    m_udualPorosity.AddCallback(this);

    m_uleakageType.AddMappedEntry("None", ltNone);
    m_uleakageType.AddMappedEntry("Single", ltSingle);
    m_uleakageType.AddMappedEntry("Upper/Lower", ltUpperLower);
    m_uleakageType.Set(this, IDC_COMBO11,  (int*) (&leakageType));
    m_uleakageType.AddCallback(this);


    m_uhasSkin.Set(this, IDC_COMBO12,  hasSkin);
    m_uhasSkin.AddCallback(this);

    m_uexternalBoundaryIsFixedP.Set(this, IDC_COMBO13,
                        externalBoundaryIsFixedP, "Zero Flow", "Fixed Pressure");

    //m_uuseCurveObjects.Set(this, IDC_COMBO14,
    //                      useCurveObjects, "File", "Objects");
    //m_uuseCurveObjects.AddCallback(this);

/*  m_uhorizontalAnisotropy.Set(this, IDC_COMBO15, horizontalAnisotropy, "Isotropic", "Anisotropic");
    m_uhorizontalAnisotropy.AddCallback(this);

    m_uwellXLocation.Set(this, IDC_COMBO16, IDC_EDIT_WELLX, &wellXLocation, &SC_DecFormat(1), -1e7, 1e7);

    m_uwellYLocation.Set(this, IDC_COMBO17, IDC_EDIT_WELLY, &wellYLocation, &SC_DecFormat(1), -1e7, 1e7);
*/
    InitData(true);

    return TRUE;
}


void DlgConfigMain::DoCtrlUpdate(int reason)
{
    if ((simulationType == stForward) && (simulationSubType == sstNormal))
    {
        // kluge on range -> normal, set fixed if no vary var  
        for (int i = 0; i < maxParameters; i++)
        {
            Parameter& currPar = *(allParam[i]);
            if (currPar.IsActive() && currPar.IsVary() && currPar.varyParVal.suiteValues.IsEmpty())
                currPar.SetParameterType(Parameter::ptFixed, false);
        }
    }

    DoCtrlUpdate();
}

void DlgConfigMain::DoCtrlUpdate()
{
    control.SetConsistent();

    sequence.SetForFlags();
    parameters.SetForFlags();
    dataCapture.SetForFlags();
    outputFiles.SetForFlags();
    optimize.SetForFlags();

    CheckRestrictions();
}

void DlgConfigMain::ResetAllControls()
{
    m_usimulationType.DoCtrlReset();
    m_usimulationSubType.DoCtrlReset();
    m_uleakageType.DoCtrlReset();

    m_aquiferType.DoCtrlReset();
    m_permType.DoCtrlReset();
    m_wellType.DoCtrlReset();
    m_partialType.DoCtrlReset();
    m_wellModelType.DoCtrlReset();

    m_usystemIsLiquid.DoCtrlReset();
    m_uhasSkin.DoCtrlReset();

    m_uexternalBoundaryIsFixedP.DoCtrlReset();
    m_udualPorosity.DoCtrlReset();
//    m_uuseCurveObjects.DoCtrlReset();

//  m_uhorizontalAnisotropy.DoCtrlReset();
//  m_uwellXLocation.DoCtrlReset();
//  m_uwellYLocation.DoCtrlReset();
}


void DlgConfigMain::CheckRestrictions()
{
    m_usimulationIs2D.EnableWindow(control.IsLiquid());

    m_udualPorosity.EnableWindow(control.IsLiquid() && control.Is1DRadial());
    m_uleakageType.EnableWindow(control.IsLiquid()  && control.Is1DRadial());
    m_aquiferType.EnableWindow(control.IsLiquid() && control.Is2DRadial());

    m_permType.EnableWindow(control.Is2DRadial());
    m_wellType.EnableWindow(control.Is2DRadial());
    m_partialType.EnableWindow(control.Is2DRadial() && control.IsPartialPenetration());
    m_wellModelType.EnableWindow(control.Is2DRadial());

//  m_uhorizontalAnisotropy.EnableWindow(control.IsLiquid() && control.Is1DRadial());
//  m_uwellXLocation.EnableWindowEx(control.IsHorizontalAnisotropic());
//  m_uwellYLocation.EnableWindowEx(control.IsHorizontalAnisotropic());


    CWnd* pParent = GetParent();
    static_cast<DialogNode*>(pParent)->CheckRestrictions();
}


