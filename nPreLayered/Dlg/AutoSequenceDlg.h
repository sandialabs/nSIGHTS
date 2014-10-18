///////////////////////////////////////////////////////////////////////////////////
//
// AutoSequenceDlg.h
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


#if !defined(AFX_AUTOSequenceDLG_H__6107A9B6_2A25_11D4_BCE1_00104B4C74FD__INCLUDED_)
#define AFX_AUTOSequenceDLG_H__6107A9B6_2A25_11D4_BCE1_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAutoSequenceDlg dialog

#include <osClass/C_OptVarUnits.h>
#include <genCtrl/CheckBoxCtrl.h>
#include <genCtrl/ComboBoolCtrl.h>
#include <Auto/A_Sequence.h>


class AutoSequenceUI :  public CCtrlUpdate {
public:
  CCheckBoxCtrl   m_udoExtract;
  CCheckBoxCtrl   m_udoSpec[SequenceAutoSpec::maxSpec];
  bool      categoryOK;

private:
  SequenceAutoSpec& aSpec;

public:
  AutoSequenceUI(SequenceAutoSpec& inSpec) : aSpec(inSpec) {};
  ~AutoSequenceUI() {};

  void            Set(CWnd* pParent, int& idStart);
  virtual void  DoCtrlUpdate();
  virtual void    CheckRestrictions();
  bool            IsOK() {
    return categoryOK && aSpec.opOK && aSpec.doExtract;
  }

};



class CAutoSequenceDlg : public CDialog, public CCtrlUpdate
{
private:
  AutoSequenceUI   m_uflowSpec;
  AutoSequenceUI   m_uhistSpec;
  AutoSequenceUI   m_upulseSpec;
  AutoSequenceUI   m_uslugSpec;

  CCheckBoxCtrl   m_uprocessTestZonePressureData;
  CCheckBoxCtrl   m_uprocessTestZoneFlowData;
  CCheckBoxCtrl   m_uprocessObsWellPressureData;
  CCheckBoxCtrl   m_utestDesignMode;

  bool obsOK;

public:
  CAutoSequenceDlg(CWnd* pParent = NULL);   // standard constructor

  virtual void    CheckRestrictions();
  virtual void  DoCtrlUpdate();

  //{{AFX_DATA(CAutoSequenceDlg)
  enum {IDD = IDD_AUTOSEQUENCE};
  //}}AFX_DATA

  //{{AFX_VIRTUAL(CAutoSequenceDlg)
  //}}AFX_VIRTUAL

protected:

  //{{AFX_MSG(CAutoSequenceDlg)
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAMETEROPTIMIZATIONDLG_H__6107A9B6_2A25_11D4_BCE1_00104B4C74FD__INCLUDED_)
