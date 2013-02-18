///////////////////////////////////////////////////////////////////////////////////
//
// DlgOutputFileSetupRange.cpp
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

// DlgOutputFileSetupRange.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include <osMain/G_Optimize.h>
#include <Var/G_Control.h>

#include "DlgOutputFileSetupRange.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgOutputFileSetupRange dialog
IMPLEMENT_DYNCREATE(DlgOutputFileSetupRange, DlgOutputFileSetupBase)

DlgOutputFileSetupRange::DlgOutputFileSetupRange(CWnd* pParent /*=NULL*/)
    : DlgOutputFileSetupBase(DlgOutputFileSetupRange::IDD, pParent,
        rangeOutputDesc,
        "Sim Range Output Files (*.nRng)|*.nRng|All Files (*.*)|*.*|||",
        "Range output file name", "nRng")
{
    //{{AFX_DATA_INIT(DlgOutputFileSetupRange)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(DlgOutputFileSetupRange, DlgOutputFileSetupBase)
    //{{AFX_MSG_MAP(DlgOutputFileSetupRange)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgOutputFileSetupRange message handlers



BOOL DlgOutputFileSetupRange::OnInitDialog()
{
    DlgOutputFileSetupBase::OnInitDialog();


    CheckRestrictions();
    return TRUE;
}

void DlgOutputFileSetupRange::ResetAllControls()
{
    DlgOutputFileSetupBase::ResetAllControls();
    CheckRestrictions();
}

