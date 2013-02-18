///////////////////////////////////////////////////////////////////////////////////
//
// nPre.cpp
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
//      Defines the class behaviors for the application.
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nPre.h"

#include "MainFrm.h"
#include "nPreDoc.h"
#include "nPreView.h"

#include <genApp/DefaultObjects.h>
#include <genApp/C_MenuRoot.h>
#include <genApp/G_Version.h>

#include <genGrid/GridCtrlBase.h>

#include <genUIExt/HyperLink.h>
#include <genClassPS/PS_Color.h>

#include <App/nPreRoot.h>
#include <App/nPreAppMenu.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <genClassPS/PS_FormContainer.h>
#include <genInterface/InterfaceOutlook.h>

FormContainer* FormContainer::GetFormContainer()
{
    return COutlookFrame::GetFormContainer();
}



/////////////////////////////////////////////////////////////////////////////
// CNPreApp

BEGIN_MESSAGE_MAP(CNPreApp, CWinApp)
    //{{AFX_MSG_MAP(CNPreApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
    //}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    // Standard print setup command
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNPreApp construction

CNPreApp::CNPreApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNPreApp object

CNPreApp theApp;


// mView recalc CB
static void RecalcCallback(FuncObjC* dummy)
{
    // reset time labels
    appDefaultObjects::sysInfoObj.DoStatusChk();

}

/////////////////////////////////////////////////////////////////////////////
// CNPreApp initialization

BOOL CNPreApp::InitInstance()
{
    CGridCtrlBase::RegisterClass();

    VERIFY( AfxInitRichEdit() );

    SetRegistryKey(_T("nSights"));

    LoadStdProfileSettings();  // Load standard INI file options (including MRU)

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.

    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CNPreDoc),
        RUNTIME_CLASS(CMainFrame),       // main SDI frame window
        RUNTIME_CLASS(CNPreView));
    AddDocTemplate(pDocTemplate);

    // Enable DDE Execute open
    EnableShellOpen();
    RegisterShellFileTypes(TRUE);

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    //  mView initialization
    MviewColorSetup();                              // basic color setup
    FuncObjC::AddRecalcCallback(RecalcCallback);    // add the recalc callback

    //  mView menu initialization
    nPreAppMenu.InitAppData(ID_PULLMENU_START);

    MenuRootC::currTreeObj = NULL;

    ChangeToHtmlHelp();

    // Dispatch commands specified on the command line
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;

    // The one and only window has been initialized, so show and update it.
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    AppMenuC::SetMenuBarSensitive();

    return TRUE;
}

void CNPreApp::ChangeToHtmlHelp()
{
    // Change the extension for the default help file
    CString strHelpFile = m_pszHelpFilePath;
    int nLength = StringLength(m_pszHelpFilePath);
    // HLP => CHM
    strHelpFile.Replace(".HLP", ".chm");
    free((void*)m_pszHelpFilePath);
    m_pszHelpFilePath = _tcsdup(strHelpFile);

    EnableHtmlHelp( );
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    CHyperLink  m_hyperMail;
    CHyperLink  m_hyperWWW;

// Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
    CString verText = "Version: "; 
    verText += appVersionGlobals::versionID;
    DDX_Text(pDX, IDD_VERSION, verText);
    CString verDate = "Date: ";
    verDate += appVersionGlobals::versionDate;
    DDX_Text(pDX, IDD_DATE, verDate);
    CString verQA = "QA Status: ";
    verQA += appVersionGlobals::versionQA;
    DDX_Text(pDX, IDD_QASTATUS, verQA);
    DDX_Control(pDX, IDC_HYPER_MAIL, m_hyperMail);
    DDX_Control(pDX, IDC_HYPER_WWW,  m_hyperWWW);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_hyperMail.SetURL(_T("mailto:rmrober@sandia.gov?Subject=nSIGHTS"));
    m_hyperWWW.SetURL(_T("http://www.sandia.gov"));

    return TRUE;
}

// App command to run the dialog
void CNPreApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CNPreApp message handlers

void CNPreApp::OnUpdateFileSave(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(true);
}

void CNPreApp::OnUpdateFileSaveAs(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(true);
}

#include <genCtrl/RealCtrl.h>

LRESULT CNPreApp::ProcessWndProcException(CException* e, const MSG* pMsg)
{
    if (dynamic_cast<CRangeException*>(e) != NULL) {
        static_cast<CMainFrame*>(AfxGetMainWnd())->m_pInterface->RestoreContext(true);
        e->Delete();
        return 0L;
    }

    return CWinApp::ProcessWndProcException(e, pMsg);
}
