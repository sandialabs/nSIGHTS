///////////////////////////////////////////////////////////////////////////////////
//
// ProgressThreadDlg.h
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

// PRE-NSIGHTS-FREE DEVELOPMENT HISTORY:
//
// Programmer   : Radim Krampol,  DE&S
//
// Revision History:
//    2000/10/11  Initial implementation
//    2000/11/10  Support of replaceble dialog resource and changed settings of values
//    2000/11/15  Added time elapsed (call to virtual void SetTimeLabel() every second)
//      2001/09/19  Added Minimize button

#ifndef PROGRESS_THREAD_DLG_H_
#define PROGRESS_THREAD_DLG_H_


class CProgressThreadDlg;

// format of the user function which can be called
typedef ULONG (ProgressFunctionType)(void* pData, CProgressThreadDlg* pProgressDlg);

// Progress dialog which will be displayed when thread is executing
class CProgressThreadDlg : public CDialog
{
public:
  DECLARE_DYNAMIC(CProgressThreadDlg)

  CProgressThreadDlg(UINT_PTR nProgressDlgID, CWnd* pParent = NULL);
  virtual ~CProgressThreadDlg();


  // we can either step or go through percentage estimation
  void  SetProgressStep(int nStep);

  //  modified to restrict #edit elements as well as info
  void  SetElements(int nInfoElements, int nEditElements);

  int   GetNumberInfoElements();
  void  SetLabelText(int i, const CString& sLabel);
  void  SetUnitText(int i, const CString& sUnit);

  void  SetEdit(int i, const char* szEdit);
  void  ShowEditField(int nID, const char* szText);

  bool  HasBeenCancelled() const;
  void  Close();

  void  ChangeTitle(const char* newTitle);

  void  SetCaption(const CString& sCaption);
  void  SetSteps(int nSteps);

  bool  IsReady() const;

  void  SetFinished(); // called after process ends and doesn't close dialog

  /* Actual function to call to get your function to work in a separate thread
     with a progress dialog being displayed while it is executing

     Description of parameters taken are:

     pfnFunction:   Function to execute.
     sProgressTitle:  The title that should appear on the progress dialog while your function is executing.
     pData:     Any user defined data that your function will be called with. Similar to the way threads give
            you a single LPVOID parameter.
     nPriority:   The thread priority of the thread pfnFunction will execute at.
            (See Win32 help for more informatiob)
     Return value:  true if the function was successfully executed otherwise false
            i.e. some error occurred.
  */
  bool  ExecuteFunctionWithProgressDialog(ProgressFunctionType* pfnFunction,
                                          const CString& sProgressTitle,
                                          int nSteps = 100,
                                          void* pData = NULL,
                                          int nPriority = THREAD_PRIORITY_NORMAL);

protected:
  //{{AFX_DATA(CProgressThreadDlg)
  CProgressCtrl m_ctrlProgress;
  //}}AFX_DATA

  virtual void OnCancel();
  virtual void OnOK();

  virtual void InitAdjustDialog();
  virtual void SetTimeLabel();

  //{{AFX_VIRTUAL(CProgressThreadDlg)
protected:
  virtual void DoDataExchange(CDataExchange* pDX);
  //}}AFX_VIRTUAL

  //{{AFX_MSG(CProgressThreadDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnCancelled();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnMinimize();
  //}}AFX_MSG
  afx_msg void OnReplot();
  DECLARE_MESSAGE_MAP()


public:
  CTime     m_timeStart;
  CTime     m_timeEnd;

protected:
  int       m_nInfoElements;
  int       m_nEditElements;
  CStringArray  m_aLabel;
  CStringArray  m_aUnit;

protected:
  int       m_nSteps;
  int       m_nStep;
  CTime     m_timeOfCreation;
  bool      m_bReady;
  CString     m_sCaption;
  bool      m_bCancelled;
  bool      m_bOkToClose;

  UINT_PTR    m_uTimerID;
  CWinThread*   m_pThread;
};

inline bool CProgressThreadDlg::HasBeenCancelled() const
{
  return m_bCancelled;
};
inline bool CProgressThreadDlg::IsReady() const
{
  return m_bReady;
};

#endif //_PROGRESS_THREAD_DLG_H_
