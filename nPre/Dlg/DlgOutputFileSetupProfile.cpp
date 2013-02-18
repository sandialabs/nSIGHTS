///////////////////////////////////////////////////////////////////////////////////
//
// DlgOutputFileSetupProfile.cpp
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

// DlgOutputFileSetupProfile.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include <Var/G_Sequence.h>

#include "DlgOutputFileSetupProfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgOutputFileSetupProfile dialog
IMPLEMENT_DYNCREATE(DlgOutputFileSetupProfile, DlgOutputFileSetupBase)

DlgOutputFileSetupProfile::DlgOutputFileSetupProfile(CWnd* pParent /*=NULL*/)
    : DlgOutputFileSetupBase(DlgOutputFileSetupProfile::IDD, pParent,
        profileOutputDesc,
        "Sim Profile Output Files (*.nPro)|*.nPro|All Files (*.*)|*.*|||",
        "Profile output file name", "nPro")
{
    //{{AFX_DATA_INIT(DlgOutputFileSetupProfile)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(DlgOutputFileSetupProfile, DlgOutputFileSetupBase)
    //{{AFX_MSG_MAP(DlgOutputFileSetupProfile)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgOutputFileSetupProfile message handlers


void DlgOutputFileSetupProfile::DoCtrlUpdate()
{
    CheckRestrictions();
}


void DlgOutputFileSetupProfile::CheckRestrictions()
{
    DlgOutputFileSetupBase::CheckRestrictions();

    m_ustartSeq.EnableWindow(profileOutputDesc.writeFile && (sequence.GetNSequence() > 0));
    m_uendSeq.EnableWindow(profileOutputDesc.writeFile && (sequence.GetNSequence() > 0));
    m_utimeStepModulus.EnableWindow(profileOutputDesc.writeFile);
    m_uradiusNodeModulus.EnableWindow(profileOutputDesc.writeFile);
}


BOOL DlgOutputFileSetupProfile::OnInitDialog()
{
    DlgOutputFileSetupBase::OnInitDialog();

    m_ustartSeq.Set(this, IDC_COMBO2, &profileOutputDesc.startSeq);
    m_uendSeq.Set(this, IDC_COMBO3, &profileOutputDesc.endSeq);
    m_utimeStepModulus.Set(this, IDC_EDIT5, profileOutputDesc.timeStepModulus, 1, 10);
    m_uradiusNodeModulus.Set(this, IDC_EDIT6, profileOutputDesc.radiusNodeModulus, 1, 10);

    CheckRestrictions();
    return TRUE;
}

void DlgOutputFileSetupProfile::ResetAllControls()
{
    DlgOutputFileSetupBase::ResetAllControls();

    SC_StringArray seqIDs;
    sequence.GetSequenceIDs(seqIDs);
    m_ustartSeq.NewStrings(seqIDs);
    m_uendSeq.NewStrings(seqIDs);

    CheckRestrictions();
}

