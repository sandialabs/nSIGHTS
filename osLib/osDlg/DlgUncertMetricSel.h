///////////////////////////////////////////////////////////////////////////////////
//
// DlgUncertMetricSel.h
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

#ifndef DLGUNCERTMETRICSEL_H
#define DLGUNCERTMETRICSEL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DlgUncertMetricSel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DlgUncertMetricSel dialog

#include <genInterface/InterfaceCtrls.h>
#include <genInterface/GenericDialog.h>

#include <genCtrl/CheckBoxCtrl.h>
#include <genCtrl/ComboFOCtrl.h>

#include <osMain/G_Sample.h>

#include "osLibResource.h"

class DlgUncertMetricSel : public CGenericDialog
{
   DECLARE_DYNCREATE(DlgUncertMetricSel)
   // Construction
public:
   DlgUncertMetricSel(CWnd* pParent = NULL);   // standard constructor
   ~DlgUncertMetricSel();   // cleanup heap

   // Dialog Data
   //{{AFX_DATA(DlgUncertMetricSel)
   enum { IDD = IDD_UNCERT_METRICSEL};
   //}}AFX_DATA

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(DlgUncertMetricSel)
protected:
   //}}AFX_VIRTUAL

   // Implementation
protected:
   virtual void    CheckRestrictions();
   virtual void    ResetAllControls();
   virtual void    DoCtrlUpdate();

private:

   CCheckBoxCtrl   m_metricSelected[UncertInput::maxMetricSel];
   CComboFOCtrl*   m_metricObject[UncertInput::maxMetricSel];  // allocated on heap because of constructor issues

protected:
   // Generated message map functions
   //{{AFX_MSG(DlgUncertMetricSel)
   virtual BOOL OnInitDialog();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONFIGOPTIMIZATION_H__7AD71FB9_D783_11D3_BCC9_00104B4C74FD__INCLUDED_)

