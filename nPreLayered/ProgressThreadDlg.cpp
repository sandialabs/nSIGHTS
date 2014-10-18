///////////////////////////////////////////////////////////////////////////////////
//
// ProgressThreadDlg.cpp
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



#include "stdafx.h"
#include "resource.h"

#include "ProgressThreadDlg.h"

#include <genClass/U_Str.h> // Replot

#include <App/nPreUtil.h> // Replot

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Use to pass a structure to the worker threads
class CProgressThreadInfo
{
public:
  CProgressThreadInfo();

  CProgressThreadDlg*   m_pStatusDlg;  // Used to allow the UI to be updated during operation
  ProgressFunctionType* m_pfnFunction; // Function to call to do the work
  void*         m_pData;       // data to pass to the caller function
};

CProgressThreadInfo::CProgressThreadInfo()
{
  m_pStatusDlg = NULL;
  m_pfnFunction = NULL;
  m_pData = NULL;
}

IMPLEMENT_DYNAMIC(CProgressThreadDlg, CDialog)

CProgressThreadDlg::CProgressThreadDlg(UINT_PTR nProgressDlgID, CWnd* pParent)
  : CDialog((UINT) nProgressDlgID, pParent)
{
  //{{AFX_DATA_INIT(CProgressThreadDlg)
  //}}AFX_DATA_INIT
  m_nInfoElements = 0;
  m_nEditElements = 0;
  m_nSteps = 100;
  m_nStep = 0;
  m_bReady = false;
  m_bCancelled = false;
  m_bOkToClose = false;
  m_uTimerID = 0;
}

CProgressThreadDlg::~CProgressThreadDlg()
{
}

void CProgressThreadDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CProgressThreadDlg)
  DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProgressThreadDlg, CDialog)
  //{{AFX_MSG_MAP(CProgressThreadDlg)
  ON_BN_CLICKED(IDC_CANCEL, OnCancelled)
  ON_WM_TIMER()
  ON_COMMAND(IDC_MINIMIZE, OnMinimize)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CProgressThreadDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  // initialise the progress control and remember the start time
  m_ctrlProgress.SetRange(0, m_nSteps);
  m_ctrlProgress.SetPos(m_nStep);
  m_timeOfCreation = CTime::GetCurrentTime();
  m_bReady = true;

  if (!m_sCaption.IsEmpty()) {
    SetWindowText( m_sCaption );
  }

  InitAdjustDialog();

  // set labes and units
  CString sLabel, sUnit;
  for (int i = 0; i < m_nInfoElements; i++) {
    CWnd* pWnd = GetDlgItem(IDC_STATIC1+i);
    ASSERT(pWnd);
    sLabel = m_aLabel[i];
    pWnd->SetWindowText(sLabel);

    pWnd = GetDlgItem(IDC_UNIT1+i);
    ASSERT(pWnd);
    sUnit = m_aUnit[i];
    pWnd->SetWindowText(sUnit);
  }

  // set timer
  m_uTimerID = SetTimer(0x451, 1000, NULL);

  m_pThread->ResumeThread();

  return true;
}

void CProgressThreadDlg::SetElements(int nInfoElements, int nEditElements)
{
  m_nInfoElements = nInfoElements;
  m_nEditElements = nEditElements;
  m_aLabel.SetSize(nInfoElements);
  m_aUnit.SetSize(nInfoElements);
}

int CProgressThreadDlg::GetNumberInfoElements()
{
  return m_nInfoElements;
}

void CProgressThreadDlg::SetLabelText(int i, const CString& sLabel)
{
  if (i < m_nInfoElements)
    m_aLabel.SetAt(i, sLabel);
}

void CProgressThreadDlg::SetUnitText(int i, const CString& sUnit)
{
  if (i < m_nInfoElements)
    m_aUnit.SetAt(i, sUnit);
}

//
// called from different thread
void CProgressThreadDlg::SetEdit(int i, const char* szEdit)
{
  if ((i >= m_nEditElements) || (!IsReady())) {
    return;
  }

  int nID = IDC_EDIT1 + i;
  HWND hWnd = ::GetDlgItem(m_hWnd, nID);
  //CWnd* pWnd = GetDlgItem(IDC_EDIT1+i);
//  ASSERT(hWnd);
  if (hWnd)
    ::SetWindowText(hWnd, szEdit);
}

void CProgressThreadDlg::ShowEditField(int nID, const char* szText)
{
  if (!IsReady()) {
    return;
  }

  HWND hWnd = ::GetDlgItem(m_hWnd, nID);
//  ASSERT(hWnd);
  if (hWnd)
    ::SetWindowText(hWnd, szText);
}

void CProgressThreadDlg::InitAdjustDialog()
{
}

void CProgressThreadDlg::SetTimeLabel()
{
  if (!IsReady()) {
    return;
  }

  CWnd* pWnd = GetDlgItem(IDC_ELAPSED_TIME);
  if (pWnd)
  {
//    ASSERT(pWnd);
    CTimeSpan ts = CTime::GetCurrentTime() - m_timeStart;
    CString sText = ts.Format( "Time elapsed   %H:%M:%S" );
    pWnd->SetWindowText(sText);
  }
}

void CProgressThreadDlg::OnCancel()
{
  if (m_uTimerID != 0) {
    KillTimer(m_uTimerID);
  }

  if (m_bOkToClose) {
    CDialog::OnCancel();
  } else {
    m_bCancelled = true;
  }
}

void CProgressThreadDlg::OnOK()
{
  // deliberately do nothing
}

void CProgressThreadDlg::Close()
{
  m_timeEnd = CTime::GetCurrentTime();
  m_bOkToClose = true;
  PostMessage(WM_CLOSE);
}

void CProgressThreadDlg::OnCancelled()
{
  OnCancel();
}


void CProgressThreadDlg::ChangeTitle(const char* newTitle)
{
  SetWindowText(newTitle);
}


void CProgressThreadDlg::SetCaption(const CString& sCaption)
{
  m_sCaption = sCaption;
}

void CProgressThreadDlg::SetSteps(int nSteps)
{
  m_nSteps = nSteps;
}

void CProgressThreadDlg::SetProgressStep(int nStep)
{

  if (!IsReady()) {
    return;
  }

  if (nStep > m_nSteps) // could possibly happen with dynamic TS
    nStep = m_nSteps;

  // update only in the case of change(any) - it would be slow otherwise
  if (nStep > m_nStep) {
    m_ctrlProgress.SetPos(nStep);
    m_nStep = nStep;
  }
}

void CProgressThreadDlg::OnTimer(UINT_PTR nIDEvent)
{
  ASSERT(nIDEvent == m_uTimerID);
  CDialog::OnTimer(nIDEvent);

  // Eat spurious WM_TIMER messages
  MSG msg;
  while(::PeekMessage(&msg, m_hWnd, WM_TIMER, WM_TIMER, PM_REMOVE));

  SetTimeLabel();
}

void CProgressThreadDlg::SetFinished()
{
  if (m_uTimerID != 0) {
    KillTimer(m_uTimerID);
  }

  // update button
  CWnd* pWnd = GetDlgItem(IDC_CANCEL);
  ASSERT(pWnd);
  pWnd->SetWindowText("OK");

  char endTitle[80];
  MakeString(endTitle, m_sCaption, " Completed", 80);
  ChangeTitle(endTitle);

  // progress bar to end
  SetProgressStep(m_nSteps);

  // final time label
  pWnd = GetDlgItem(IDC_ELAPSED_TIME);
  ASSERT(pWnd);
  m_timeEnd = CTime::GetCurrentTime();
  CTimeSpan ts = m_timeEnd - m_timeStart;
  CString sText = ts.Format("Total elapsed time  %H:%M:%S");
  pWnd->SetWindowText(sText);

  m_bOkToClose = true;
}


UINT ProgressDialogWorkerThread(LPVOID pParam);

bool CProgressThreadDlg::ExecuteFunctionWithProgressDialog(ProgressFunctionType* pfnFunction,
    const CString& sProgressTitle,
    int nSteps,
    void* pData,
    int nPriority)
{
  SetCaption(sProgressTitle);
  SetSteps(nSteps);
  CProgressThreadInfo Info;
  Info.m_pStatusDlg = this;
  Info.m_pfnFunction = pfnFunction;
  Info.m_pData = pData;
  m_pThread = AfxBeginThread(ProgressDialogWorkerThread, &Info, nPriority, 0, CREATE_SUSPENDED);

  // If the thread failed to create itself
  if (m_pThread == NULL) {
    ASSERT(0);
    return false;
  }

  // Start timer
  m_timeStart = CTime::GetCurrentTime();
  m_timeEnd = m_timeStart;

  // bring up the dialog modal (thread will close it for us)
  DoModal();

  return true;
}

UINT ProgressDialogWorkerThread(LPVOID pParam)
{
  CProgressThreadInfo* pInfo = (CProgressThreadInfo*)pParam;

  // Call the user defined function
  if (pInfo->m_pfnFunction(pInfo->m_pData, pInfo->m_pStatusDlg) == 0) {
    // close down the progress dialog
    pInfo->m_pStatusDlg->Close();
  } else {
    pInfo->m_pStatusDlg->SetFinished();
  }

  return 0;
}

void CProgressThreadDlg::OnMinimize()
{
  if (nPreUtilities::bRunWithMinimizedMain)
    ShowWindow(SW_MINIMIZE);
  else
    AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);

}
