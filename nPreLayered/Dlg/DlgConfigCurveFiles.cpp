///////////////////////////////////////////////////////////////////////////////////
//
// DlgConfigCurveFiles.cpp
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


// DlgConfigCurveFiles.cpp : implementation file
//

#include "stdafx.h"
#include "nPre.h"
#include "DlgConfigCurveFiles.h"

#include <genClass/U_Str.h>

#include <Var/G_CurveFiles.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// DlgConfigCurveFiles dialog


IMPLEMENT_DYNCREATE(DlgConfigCurveFiles, CGenericDialog)

DlgConfigCurveFiles::DlgConfigCurveFiles(CWnd* pParent /*=NULL*/)
  : CGenericDialog(DlgConfigCurveFiles::IDD, pParent)
{
  //{{AFX_DATA_INIT(DlgConfigCurveFiles)
  //}}AFX_DATA_INIT
}



BEGIN_MESSAGE_MAP(DlgConfigCurveFiles, CGenericDialog)
  //{{AFX_MSG_MAP(DlgConfigCurveFiles)
  ON_BN_CLICKED(IDC_RELOAD, OnButtonReload)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgConfigCurveFiles message handlers


void DlgConfigCurveFiles::OnButtonReload()
{
  testZoneBCCurveFile.LoadCurvesFromFile();
  frParameterCurveFile.LoadCurvesFromFile();
  fPParameterCurveFile.LoadCurvesFromFile();
  ftParameterCurveFile.LoadCurvesFromFile();
}


void DlgConfigCurveFiles::DoCtrlUpdate(int reason)
{
  switch (reason) {
  case 0 : {
    testZoneBCCurveFile.LoadCurvesFromFile();
    break;
  }
  case 1 : {
    fPParameterCurveFile.LoadCurvesFromFile();
    break;
  }
  case 2 : {
    frParameterCurveFile.LoadCurvesFromFile();
    break;
  }
  case 3 : {
    ftParameterCurveFile.LoadCurvesFromFile();
    break;
  }
  }
}

BOOL DlgConfigCurveFiles::OnInitDialog()
{
  CGenericDialog::OnInitDialog();

  SetControlInfo(IDC_EDIT_BOUNDARY, resizeHorizontal);
  SetControlInfo(IDC_EDIT_FP, resizeHorizontal);
  SetControlInfo(IDC_EDIT_FR, resizeHorizontal);
  SetControlInfo(IDC_EDIT_FT, resizeHorizontal);
  SetControlInfo(IDC_BUTTON_BND, anchoreRight);
  SetControlInfo(IDC_BUTTON_FP, anchoreRight);
  SetControlInfo(IDC_BUTTON_FR, anchoreRight);
  SetControlInfo(IDC_BUTTON_FT, anchoreRight);

  const char* fileFilter = "Curve Files (*.nCRV)|*.nCRV|All Files (*.*)|*.*|||";
  const char* defExt = "CRV";

  m_utestZoneBCCurveFile.Set(this, IDC_EDIT_BOUNDARY, IDC_BUTTON_BND, testZoneBCCurveFile.fileName,
                             fileFilter, "Test Zone Curve File", defExt, false);
  m_utestZoneBCCurveFile.AddCallback(this, 0);

  m_ufPParameterCurveFile.Set(this, IDC_EDIT_FP, IDC_BUTTON_FP, fPParameterCurveFile.fileName,
                              fileFilter, "f(P) Parameter Curve File", defExt, false);
  m_ufPParameterCurveFile.AddCallback(this, 1);

  m_ufrParameterCurveFile.Set(this, IDC_EDIT_FR, IDC_BUTTON_FR, frParameterCurveFile.fileName,
                              fileFilter, "f(r) Parameter Curve File", defExt, false);
  m_ufrParameterCurveFile.AddCallback(this, 2);

  m_uftParameterCurveFile.Set(this, IDC_EDIT_FT, IDC_BUTTON_FT, ftParameterCurveFile.fileName,
                              fileFilter, "f(t) Parameter Curve File", defExt, false);
  m_uftParameterCurveFile.AddCallback(this, 3);

  return TRUE;
}
