///////////////////////////////////////////////////////////////////////////////////
//
// DlgSeqPartialRun.cpp
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

// DlgSeqPartialRun.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include <genClass/U_Str.h>
#include <Var/G_Sequence.h>

#include "DlgSeqPartialRun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgSeqPartialRun dialog

IMPLEMENT_DYNCREATE(DlgSeqPartialRun, CGenericDialog)

DlgSeqPartialRun::DlgSeqPartialRun(CWnd* pParent /*=NULL*/)
   : CGenericDialog(DlgSeqPartialRun::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgSeqPartialRun)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void DlgSeqPartialRun::DoDataExchange(CDataExchange* pDX)
{
   CGenericDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(DlgSeqPartialRun)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgSeqPartialRun, CGenericDialog)
    //{{AFX_MSG_MAP(DlgSeqPartialRun)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgSeqPartialRun message handlers


BOOL DlgSeqPartialRun::OnInitDialog()
{
    CGenericDialog::OnInitDialog();
    m_uisPartialRun.Set(this, IDC_COMBO1, isPartialRun,
            "Full Test", "Partial Test");
    m_uisPartialRun.AddCallback(this);

    m_upartialStartSeqVal.Set(this, IDC_COMBO2, &partialStartSeqVal);
    m_upartialStartSeqVal.AddCallback(this, 0);

    m_upartialEndSeqVal.Set(this, IDC_COMBO3, &partialEndSeqVal);
    m_upartialEndSeqVal.AddCallback(this, 1);

    InitData(false);

    return TRUE;
}

void DlgSeqPartialRun::CheckRestrictions()
{
    m_upartialEndSeqVal.EnableWindow(isPartialRun && (sequence.GetNSequence() > 0));
    m_upartialStartSeqVal.EnableWindow(isPartialRun && (sequence.GetNSequence() > 0));
}

void DlgSeqPartialRun::DoCtrlUpdate(int reason)
{
    SC_StringArray seqIDs;
    sequence.GetSequenceIDs(seqIDs);
    if ((reason == 0) && (partialStartSeqVal >= 0))
        CopyString(partialStartSeqID, seqIDs[partialStartSeqVal], Sequence::seqIDLen);
    if ((reason == 1) && (partialEndSeqVal >= 0))
        CopyString(partialEndSeqID, seqIDs[partialEndSeqVal], Sequence::seqIDLen);
}

void DlgSeqPartialRun::ResetAllControls()
{
    m_uisPartialRun.DoCtrlReset();

    SC_StringArray seqIDs;
    sequence.GetSequenceIDs(seqIDs);
    if (!sequence.GetSequenceIndex(partialStartSeqID, partialStartSeqVal))
        partialStartSeqVal = -1;
    if (!sequence.GetSequenceIndex(partialEndSeqID, partialEndSeqVal))
        partialStartSeqVal = -1;
    m_upartialEndSeqVal.NewStrings(seqIDs);
    m_upartialStartSeqVal.NewStrings(seqIDs);
}

