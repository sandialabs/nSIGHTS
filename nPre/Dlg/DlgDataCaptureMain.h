///////////////////////////////////////////////////////////////////////////////////
//
// DlgDataCaptureMain.h
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

#if !defined(AFX_DLGDATACAPTUREMAIN_H__A1005FE7_E082_11D3_BCCC_00104B4C74FD__INCLUDED_)
#define AFX_DLGDATACAPTUREMAIN_H__A1005FE7_E082_11D3_BCCC_00104B4C74FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDataCaptureMain.h : header file
//

#include <genInterface/GenericDialog.h>

#include <genClass/SC_StringArray.h>
#include <genClass/SC_IntArray.h>

#include <genGrid/GridCtrlCell.h>

class DataCaptureGridCtrl : public CGridCtrlCell
{
private:
    int     m_currDCap;     // selected Dcap for popup actions

public:
    // strings for combos -- set on first entry
    static  SC_StringArray  captureTypeStrings;
    static  SC_StringArray  pressureCapTypeStrings;
    static  SC_StringArray  flowCapTypeStrings;
    static  SC_StringArray  productionCapTypeStrings;
    static  SC_StringArray  testZoneCapTypeStrings;
    static  SC_StringArray  waterTableCapTypeStrings;


public:
            DataCaptureGridCtrl();

    void    ResetDataCaptureGrid(); // rebuilds grid for current settings

protected:
    virtual void FinishEdit(const char* szText, int nValue, int nLastChar);

private:
    void    InsertDataCapture(int index);
    void    DeleteDataCapture(int index);
    void    UpdateModifiedRows();

protected:
    //{{AFX_MSG(DataCaptureGridCtrl)
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    //}}AFX_MSG

    afx_msg void OnPopupCommand(UINT nMenuID);

    DECLARE_MESSAGE_MAP()
};


class DlgDataCaptureMain : public CGenericDialog
{
    DECLARE_DYNCREATE(DlgDataCaptureMain)
// Construction
public:
    DlgDataCaptureMain(CWnd* pParent = NULL);   // standard constructor
    virtual ~DlgDataCaptureMain();

// Dialog Data
    //{{AFX_DATA(DlgDataCaptureMain)
    enum { IDD = IDD_DCAP_MAIN };
        // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    DataCaptureGridCtrl m_grid;

    virtual void        ResetAllControls();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(DlgDataCaptureMain)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation

protected:

    // Generated message map functions
    //{{AFX_MSG(DlgDataCaptureMain)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //AFX_DLGDATACAPTUREMAIN_H__A1005FE7_E082_11D3_BCCC_00104B4C74FD__INCLUDED_
