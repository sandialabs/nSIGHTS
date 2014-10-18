///////////////////////////////////////////////////////////////////////////////////
//
// DlgParameterGroup.h
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


#if !defined(AFX_DLGPARAMETERGROUP_H__438F5B31_3077_11D4_BCE4_00104B4C74FD__INCLUDED_)
#define AFX_DLGPARAMETERGROUP_H__438F5B31_3077_11D4_BCE4_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgParameterGroup.h : header file
//

#include <genInterface/GenericDialog.h>
#include <genGrid/GridCtrlCell.h>
#include <genInterface/InterfaceCtrls.h>
#include <Var/G_Parameter.h>

/////////////////////////////////////////////////////////////////////////////
// DlgParameterGroup dialog

class ParameterGridCtrl : public CGridCtrlCell
{
public:
  ParameterGridCtrl();

  void  InitGrid(ParameterPtrArray& parameterArray);

protected:
  virtual void FinishEdit(const char* szText, int nValue, int nLastChar);

  // Generated message map functions
protected:
  //{{AFX_MSG(ParameterGridCtrl)
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

class DlgParameterGroup : public CGenericDialog
{
  DECLARE_DYNCREATE(DlgParameterGroup)
// Construction
public:
  DlgParameterGroup(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  //{{AFX_DATA(DlgParameterGroup)
  enum { IDD = IDD_PAR_PARGROUP };
  // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA
  ParameterGridCtrl m_grid;

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(DlgParameterGroup)
protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:

  // Generated message map functions
  //{{AFX_MSG(DlgParameterGroup)
  virtual BOOL OnInitDialog();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

class DlgNodeParameterGroup : public DlgNode
{
  friend class DlgParameterGroup;
public:
  DlgNodeParameterGroup(InterfaceNode* pParent, int nIdent,
                        NodeDescription* pDescription,
                        ParameterPtrArray& parameterArray);
protected:
  ParameterPtrArray& m_parameterArray;
};

inline DlgNodeParameterGroup::DlgNodeParameterGroup(InterfaceNode* pParent, int nIdent,
    NodeDescription* pDescription,
    ParameterPtrArray& parameterArray) :
  DlgNode(pParent, nIdent, pDescription),
  m_parameterArray(parameterArray)
{
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPARAMETERGROUP_H__438F5B31_3077_11D4_BCE4_00104B4C74FD__INCLUDED_)
