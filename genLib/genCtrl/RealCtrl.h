///////////////////////////////////////////////////////////////////////////////////
//
// RealCtrl.h
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
// DESCRIPTION: Windows specific code for implementing basic controls with
//              callbacks on resource defined dialog forms.
//              Enter real numeric value
//
///////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_REALCTRL_H__8CC003E0_42F2_11D4_9C49_006008909B72__INCLUDED_)
#define AFX_REALCTRL_H__8CC003E0_42F2_11D4_9C49_006008909B72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RealCtrl.h : header file
//
#include <genClass/SC_RealFormat.h>

#include <genCtrl/UpdateableCtrl.h>
/////////////////////////////////////////////////////////////////////////////
// CRealCtrl window

class CRangeException : public CException
{
};

class CRealCtrl : public CEdit, public CUpdateableCtrl
{
protected:
    SC_RealFormat   m_currFormat;
    bool            m_isLimited;
    double          m_fMinLimit;
    double          m_fMaxLimit;
    double*         m_pfDataVal;

    // Construction
public:
    CRealCtrl();

    //  various sigs
    void        Set(CWnd* pParent, int nIDEdit, double& inDataVal);
    void        Set(CWnd* pParent, int nIDEdit, double& inDataVal,
                    const SC_RealFormat& inFormat);
    void        Set(CWnd* pParent, int nIDEdit, double& inDataVal,
                    const SC_RealFormat& inFormat,
                    const double& inMinLimit, const double& inMaxLimit);

    void        ResetFormat(SC_RealFormat&  inFormat) {m_currFormat = inFormat;}

    // CUpdateableCtrl virtual
    void        DoCtrlReset();


    // Operations
public:
    virtual void    UpdateText();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CRealCtrl)
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~CRealCtrl();

    // Generated message map functions
protected:
    void    RangeMsg();

    //{{AFX_MSG(CRealCtrl)
    afx_msg void OnKillfocus();
    afx_msg void OnSetFocus();
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_REALCTRL_H__8CC003E0_42F2_11D4_9C49_006008909B72__INCLUDED_
