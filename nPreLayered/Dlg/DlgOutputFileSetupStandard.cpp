///////////////////////////////////////////////////////////////////////////////////
//
// DlgOutputFileSetupStandard.cpp
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
#include "Resource.h"

#include <genClass/DO_Real.h>
#include <genClass/U_Str.h>

#include <objClass/DO_XYDataArray.h>

#include <Var\G_XYOutput.h>
#include <Var\G_OutputFiles.h>

#include "DlgOutputFileSetupStandard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgOutputFileSetupStandard dialog

IMPLEMENT_DYNCREATE(DlgOutputFileSetupStandard, DlgOutputFileSetupBase)

DlgOutputFileSetupStandard::DlgOutputFileSetupStandard(CWnd* pParent /*=NULL*/)
  : DlgOutputFileSetupBase(DlgOutputFileSetupStandard::IDD, pParent,
                           xyForwardOutputDesc,
                           "Sim XY Output Files (*.nXYSim)|*.nXYSim|All Files (*.*)|*.*|||",
                           "XY output file name", "nXYSim"),
  m_uFOdata(typeid(DO_XYDataArray))
{
  //{{AFX_DATA_INIT(DlgOutputFileSetupStandard)
  // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(DlgOutputFileSetupStandard, DlgOutputFileSetupBase)
  //{{AFX_MSG_MAP(DlgOutputFileSetupStandard)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgOutputFileSetupStandard message handlers


void DlgOutputFileSetupStandard::DoCtrlUpdate()
{
  CheckRestrictions();
}


void DlgOutputFileSetupStandard::CheckRestrictions()
{
  DlgOutputFileSetupBase::CheckRestrictions();
  m_uFOdata.EnableWindow(xyForwardOutputDesc.writeFile);
}

BOOL DlgOutputFileSetupStandard::OnInitDialog()
{
  DlgOutputFileSetupBase::OnInitDialog();

  //  convoluted to get arround internal compiler error
  XYForwardOutputFO& temp = xyForwardOutputFO;
  m_uFOdata.Set(this, IDC_COMBO2, temp.xyDataArrayObjRef);

  CheckRestrictions();
  return TRUE;
}

void DlgOutputFileSetupStandard::ResetAllControls()
{
  DlgOutputFileSetupBase::ResetAllControls();
  m_uFOdata.ResetSelection();
  CheckRestrictions();
}
