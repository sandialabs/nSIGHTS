///////////////////////////////////////////////////////////////////////////////////
//
// PS_PSOutputSetup.h
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
//      Platform specific implementation of genPlotClass codes
//  adds Windows and OpenGL specific code.  Includes
//  operating system independent implementation of
//  2D and 3D OpenGL functionality (platform specific
//  in this case refers to the OpenGL renderer).
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PS_PSOutputSetup_H__2A6DDBA0_16C8_11D4_8A67_00500495B539__INCLUDED_)
#define AFX_PS_PSOutputSetup_H__2A6DDBA0_16C8_11D4_8A67_00500495B539__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PS_PSOutputSetup.h : header file
//

#include <genPlotClass/PC_PSOutData.h>
#include <genMFC/HelpDialog.h>

/////////////////////////////////////////////////////////////////////////////
// PS_PSOutputSetup dialog

class PS_PSOutputSetup : public CHelpDialog
{
    // Construction
public:
    PS_PSOutputSetup(CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
    //{{AFX_DATA(PS_PSOutputSetup)
    enum { IDD = IDD_PS_FILE_OUTPUT };
    CString m_szFullPathName;
    double  m_fBottom;
    double  m_fGammaCorrect;
    double  m_fHeight;
    double  m_fLineWidthMult;
    double  m_fLeftMargin;
    double  m_fRight;
    double  m_fTextMult;
    double  m_fTopMargin;
    double  m_fWidth;
    double  m_fZBufferMult;
    int     m_iFileFormat;
    int     m_iOrientation;
    int     m_iArialAsHelvetica;
    int     m_inextPSIncrement;
    //}}AFX_DATA

public:
    void SetValues(PC_PSOutData& values);
    void GetValues(PC_PSOutData& values);

public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(PS_PSOutputSetup)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(PS_PSOutputSetup)
    afx_msg void OnBrowse();
    afx_msg void OnAccept();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    // Help support
protected:
    virtual DWORD GetHelpMap();
    virtual CString GetHelpName();

private:
    static DWORD m_dwHelpMap[];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PS_PSOutputSetup_H__2A6DDBA0_16C8_11D4_8A67_00500495B539__INCLUDED_)

