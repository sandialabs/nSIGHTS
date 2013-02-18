///////////////////////////////////////////////////////////////////////////////////
//
// DlgOutputFileSetupBase.cpp
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
//      nPre dialog
//
///////////////////////////////////////////////////////////////////////////////////

// DlgOutputFileSetupStandard.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include <genClass/U_Str.h>

#include <Dlg/DlgOutputFileSetupBase.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgOutputFileSetupStandard dialog

DlgOutputFileSetupBase::DlgOutputFileSetupBase(UINT nIDTemplate, CWnd* pParent /*=NULL*/,
                            OutputFileDesc&     fDesc,
                            const char* fileFilter, const char* fileDialogTitle, const char* defExt)
    : CGenericDialog(nIDTemplate, pParent),
        p_fDesc(&fDesc), p_fileFilter(fileFilter), p_fileDialogTitle(fileDialogTitle), p_defExt(defExt)
{
}

IMPLEMENT_DYNAMIC(DlgOutputFileSetupBase, CGenericDialog)

BEGIN_MESSAGE_MAP(DlgOutputFileSetupBase, CGenericDialog)
    //{{AFX_MSG_MAP(DlgOutputFileSetupBase)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgOutputFileSetupBase message handlers


void DlgOutputFileSetupBase::DoCtrlUpdate()
{
    CheckRestrictions();
}


void DlgOutputFileSetupBase::CheckRestrictions()
{
    m_uaddExisting.EnableWindow(p_fDesc->writeFile);
    m_uRunID.EnableWindow(p_fDesc->writeFile);
    m_uFile.EnableWindowEx(p_fDesc->writeFile);
}

BOOL DlgOutputFileSetupBase::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    m_uwriteFile.Set(this, IDC_CHECK1,  p_fDesc->writeFile);
    m_uwriteFile.AddCallback(this);
    m_uaddExisting.Set(this, IDC_COMBO1,  p_fDesc->addExisting, "Overwrite", "Append");
    m_uRunID.Set(this, IDC_EDIT2,  p_fDesc->runID, OutputFileDesc::runIDLen);

    m_uFile.Set(this, IDC_EDIT1, IDC_BUTTON1, p_fDesc->fileName,
                p_fileFilter, p_fileDialogTitle, p_defExt, false);


    return TRUE;
}


void DlgOutputFileSetupBase::ResetAllControls()
{
    m_uwriteFile.DoCtrlReset();
    m_uaddExisting.DoCtrlReset();
    m_uRunID.DoCtrlReset();
    m_uFile.DoCtrlReset();
}

