///////////////////////////////////////////////////////////////////////////////////
//
// DlgConfigLiquid.cpp
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


// DlgConfigLiquid.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgConfigLiquid.h"

#include <Var/G_Parameter.h>
#include <Var/G_Control.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgConfigLiquid dialog

IMPLEMENT_DYNCREATE(DlgConfigLiquid,CGenericDialog)

DlgConfigLiquid::DlgConfigLiquid(CWnd* pParent /*=NULL*/)
  : CGenericDialog(DlgConfigLiquid::IDD, pParent)
{
  //{{AFX_DATA_INIT(DlgConfigLiquid)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}



BEGIN_MESSAGE_MAP(DlgConfigLiquid, CGenericDialog)
  //{{AFX_MSG_MAP(DlgConfigLiquid)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgConfigLiquid message handlers

BOOL DlgConfigLiquid::OnInitDialog()
{

  m_uenterPermeabilityData.Set(this, IDC_COMBO1, enterPermeabilityData,
                               "Hydraulic Conductivity", "Permeability");
  m_uenterPermeabilityData.AddCallback(this);

  m_uenterSpecificStorageData.Set(this, IDC_COMBO2, enterSpecificStorageData,
                                  "Porosity * Total Compressibility", "Specific Storage");
  m_uenterSpecificStorageData.AddCallback(this);

  m_ucompensateFlowDimensionGeometry.Set(this, IDC_COMBO3, compensateFlowDimensionGeometry);
  m_ucompensateFlowDimensionGeometry.AddCallback(this);

  m_utestZoneVolumeVaries.Set(this, IDC_COMBO4, testZoneVolumeVaries,
                              "Constant", "Varying");
  m_utestZoneVolumeVaries.AddCallback(this);

  m_utestZoneCompressibilityVaries.Set(this, IDC_COMBO5, testZoneCompressibilityVaries,
                                       "Constant", "Varying");
  m_utestZoneCompressibilityVaries.AddCallback(this);

  m_utestZoneTemperatureVaries.Set(this, IDC_COMBO6, testZoneTemperatureVaries,
                                   "Constant", "Varying");
  m_utestZoneTemperatureVaries.AddCallback(this);

  m_udefaultTestZoneTemperature.Set(this, IDC_COMBO7, IDC_EDIT_TEMPERATURE,
                                    &defaultTestZoneTemperature, &SC_DecFormat(2), 1.0, 100.0);

  m_uheadFlag.Set(this, IDC_COMBO8, NSUnitsBase::headFlag, "Pressure", "Head");
  m_uheadFlag.AddCallback(this);

  m_uheadDensity.Set(this, IDC_COMBO9, IDC_EDIT_LIQUID_DENSITY,
                     &NSUnitsBase::headDensity, &SC_DecFormat(4), 100.0, 2000.0);
  m_uheadDensity.m_uRealCtrl.AddCallback(this);

  m_uallowNegativePressure.Set(this, IDC_COMBO10, allowNegativePressure);
  m_uallowNegativePressure.AddCallback(this);

  CGenericDialog::OnInitDialog();

  InitData(true);

  return TRUE;
}


void DlgConfigLiquid::CheckRestrictions()
{
  m_uheadDensity.EnableWindowEx(NSUnitsBase::headFlag);
}

void DlgConfigLiquid::DoCtrlUpdate()
{
  NSUnitsBase::ResetHeadPressureConversions();
  parameters.SetForFlags();
  CheckRestrictions();
}


void DlgConfigLiquid::ResetAllControls()
{
  m_udefaultTestZoneTemperature.DoCtrlReset();
  m_uheadDensity.DoCtrlReset();
  m_uenterPermeabilityData.DoCtrlReset();
  m_uenterSpecificStorageData.DoCtrlReset();
  m_ucompensateFlowDimensionGeometry.DoCtrlReset();
  m_utestZoneVolumeVaries.DoCtrlReset();
  m_utestZoneCompressibilityVaries.DoCtrlReset();
  m_utestZoneTemperatureVaries.DoCtrlReset();
  m_uheadFlag.DoCtrlReset();
}
