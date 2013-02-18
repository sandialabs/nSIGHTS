///////////////////////////////////////////////////////////////////////////////////
//
// DlgConfigUnits.h
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
//      nPre dialog.
//
///////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGCONFIGUNITS_H__7AD71FB6_D783_11D3_BCC9_00104B4C74FD__INCLUDED_)
#define AFX_DLGCONFIGUNITS_H__7AD71FB6_D783_11D3_BCC9_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConfigUnits.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DlgConfigUnits dialog

#include <genInterface/GenericDialog.h>
#include <genCtrl/ComboIntCtrl.h>

#include <Var/C_Units.h>

class DlgConfigUnits : public CGenericDialog
{
    DECLARE_DYNCREATE(DlgConfigUnits)
/// Construction
public:
    DlgConfigUnits(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(DlgConfigUnits)
    enum { IDD = IDD_CFG_UNITS };
        // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(DlgConfigUnits)
    //}}AFX_VIRTUAL

// Implementation
protected:
    void InitUnit(int nID, int unitType);

    virtual void    CheckRestrictions();
    virtual void    ResetAllControls();


protected:
    enum            {maxUnitsCtrl = 20};        // max units - 1 (u_none not required)
    CComboIntCtrl   m_unitCtrl[maxUnitsCtrl];

    // Generated message map functions
    //{{AFX_MSG(DlgConfigUnits)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONFIGUNITS_H__7AD71FB6_D783_11D3_BCC9_00104B4C74FD__INCLUDED_)

