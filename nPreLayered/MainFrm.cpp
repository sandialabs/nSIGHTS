///////////////////////////////////////////////////////////////////////////////////
//
// MainFrm.cpp
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
#include "Resource.h"
#include <GenLibRes.h>

#include <genClass/C_BufFile.h>

#include "UI/NpreInterfaceOutlook.h"

#include <genInterface/InterfaceCtrls.h>

#include <genListClass/LFO_MessageLogListing.h>
#include <genListClassPS/C_ListDefPS.h>

#include <genApp/C_MenuObj.h>
#include <genApp/C_AppMenu.h>
#include <genApp/AppConfigFile.h>

#include <App/nPreRoot.h>
#include <App/nPreFile.h>

// following for listing
#include <Var/G_Control.h>
#include <Var/G_Parameter.h>
#include <Var/G_CalcParameter.h>
#include <Var/G_Sequence.h>
#include <Var/G_DataCapture.h>
#include <Var/G_OutputFiles.h>
#include <osMain/G_Optimize.h>
#include <osMain/G_Sample.h>
#include <osMain/G_Vary.h>
#include <genApp/G_FileNames.h>

#include <Dlg/AutoFieldDlg.h>
#include <Dlg/AutoFitDlg.h>
#include <Dlg/AutoSequenceDlg.h>
#include <Dlg/DlgConfigUnits.h>
#include <Dlg/SaveForNPreXDlg.h>

#include <Run/RunCommon.h>
#include <Run/RunControl.h>

#include <App/nPreFile.h>
#include <App/nPreUtil.h> // Replot

#include "ProgressThreadDlg.h"
#include "nPreDoc.h"
#include "nPreView.h"

#include "MainFrm.h"
#include "nPreDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CProjectFrame)

BEGIN_MESSAGE_MAP(CMainFrame, CProjectFrame)
  //{{AFX_MSG_MAP(CMainFrame)
  ON_WM_CREATE()
  ON_COMMAND(ID_FILE_SAVECOMMANDLINE, OnSaveCommandLine)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVECOMMANDLINE, OnUpdateSaveCommandLine)
  ON_COMMAND(ID_LIST_CURRENT, OnListCurrent)
  ON_COMMAND(ID_LIST_ERROR, OnListError)
  ON_COMMAND(ID_LIST_ALL, OnListAll)
  ON_COMMAND(ID_LIST_CALCULATED, OnListCalculated)
  ON_COMMAND(ID_LIST_ALLERRORS, OnListAllErrors)
  ON_COMMAND(ID_LIST_MESSAGES, OnListMessages)
  ON_COMMAND(ID_AUTO_FIELD, OnAutoField)
  ON_COMMAND(ID_AUTO_SEQUENCE, OnAutoSequence)
  ON_COMMAND(ID_AUTO_FIT, OnAutoFit)
  ON_COMMAND(ID_RUN_VERBOSE, OnRunVerbose)
  ON_COMMAND(ID_RUN_MINIMAL, OnRunMinimal)
  ON_COMMAND(ID_RUN_NOTHREADS, OnRunNoThreads)
  ON_COMMAND(ID_RUN_OPTFINAL, OnRunOptFinal)

  ON_COMMAND(ID_RUN_MINIMIZE_MAIN, OnRunMinimizeMain)
  ON_UPDATE_COMMAND_UI(ID_RUN_MINIMIZE_MAIN, OnUpdateRunMinimizeMain)
  ON_COMMAND(ID_VIEW_CONTROL_BAR, OnViewControlBar)
  ON_UPDATE_COMMAND_UI(ID_VIEW_CONTROL_BAR, OnUpdateViewControlBar)
  ON_WM_ACTIVATE()
  ON_WM_SYSCOMMAND()
  //}}AFX_MSG_MAP
  ON_COMMAND(ID_FILE_UPDATEPATHS, CProjectFrame::OnFilePathUpdate)

  ON_COMMAND(ID_UIX_LARGEICON, COutlookFrame::OnUixLargeicon)
  ON_UPDATE_COMMAND_UI(ID_UIX_LARGEICON, COutlookFrame::OnUpdateUixLargeicon)
  ON_COMMAND(ID_UIX_SMALLICON, COutlookFrame::OnUixSmallicon)
  ON_UPDATE_COMMAND_UI(ID_UIX_SMALLICON, COutlookFrame::OnUpdateUixSmallicon)
  ON_COMMAND(ID_HELP_FINDER, CProjectFrame::OnHelpFinder)

  // replot processing
  ON_MESSAGE(WM_REPLOT, COutlookFrame::OnReplot)

  ON_COMMAND(ID_OBJECT_APPLY, CProjectFrame::OnObjectApply)
  ON_COMMAND(ID_OBJECT_COPY, CProjectFrame::OnObjectCopy)
  ON_COMMAND(ID_OBJECT_COPYPAGE, CProjectFrame::OnObjectCopyPage)
  ON_COMMAND(ID_OBJECT_DELETE, CProjectFrame::OnObjectDelete)
  ON_COMMAND(ID_OBJECT_DUPLICATE, CProjectFrame::OnObjectDuplicate)
  ON_COMMAND(ID_OBJECT_PASTE, CProjectFrame::OnObjectPaste)
  ON_COMMAND(ID_OBJECT_LISTCONNECTIONS, CProjectFrame::OnObjectListConnections)
  ON_COMMAND(ID_OBJECT_CREATEPLOTFOLDER, CProjectFrame::OnCreatePlotFolder)

  ON_UPDATE_COMMAND_UI(ID_OBJECT_APPLY, CProjectFrame::OnUpdateObjectApply)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_COPY, CProjectFrame::OnUpdateObjectCopy)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_COPYPAGE, CProjectFrame::OnUpdateObjectCopyPage)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_DELETE, CProjectFrame::OnUpdateObjectDelete)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_DUPLICATE, CProjectFrame::OnUpdateObjectDuplicate)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_PASTE, CProjectFrame::OnUpdateObjectPaste)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_LISTCONNECTIONS, CProjectFrame::OnUpdateObjectListConnections)
  ON_UPDATE_COMMAND_UI(ID_OBJECT_CREATEPLOTFOLDER, CProjectFrame::OnUpdateCreatePlotFolder)

  ON_COMMAND(ID_PAGE_DUPLICATE, CProjectFrame::OnPageDuplicate)
  ON_COMMAND(ID_PAGE_DELETE, CProjectFrame::OnPageDelete)
  ON_COMMAND(ID_PAGE_COPYCURRENT, CProjectFrame::OnPageCopyCurrent)
  ON_COMMAND(ID_PAGE_COPYALL, CProjectFrame::OnPageCopyAll)
  ON_COMMAND(ID_PAGE_PASTE, CProjectFrame::OnPagePaste)
  ON_COMMAND(ID_PAGE_CLEARALL, CProjectFrame::OnPageClearAll)
  ON_COMMAND(ID_PAGE_LISTALLCONNECTIONS, CProjectFrame::OnPageListAllConnections)
  ON_COMMAND(ID_PAGE_COLLAPSETREE, CProjectFrame::OnPageCollapseAll)


  ON_UPDATE_COMMAND_UI(ID_PAGE_DUPLICATE, CProjectFrame::OnUpdatePageDuplicate)
  ON_UPDATE_COMMAND_UI(ID_PAGE_DELETE, CProjectFrame::OnUpdatePageDelete)
  ON_UPDATE_COMMAND_UI(ID_PAGE_COPYCURRENT, CProjectFrame::OnUpdatePageCopyCurrent)
  ON_UPDATE_COMMAND_UI(ID_PAGE_COPYALL, CProjectFrame::OnUpdatePageCopyAll)
  ON_UPDATE_COMMAND_UI(ID_PAGE_PASTE, CProjectFrame::OnUpdatePagePaste)
  ON_UPDATE_COMMAND_UI(ID_PAGE_CLEARALL, CProjectFrame::OnUpdatePageClearAll)
  ON_UPDATE_COMMAND_UI(ID_PAGE_LISTALLCONNECTIONS, CProjectFrame::OnUpdatePageListAllConnections)
  ON_UPDATE_COMMAND_UI(ID_PAGE_COLLAPSETREE, CProjectFrame::OnUpdatePageCollapseAll)

  ON_COMMAND(ID_PAGE_NEWDATA, CProjectFrame::OnPageNewData)
  ON_COMMAND(ID_PAGE_NEW_3DXYZ, CProjectFrame::OnPageNew3dXYZ)
  ON_COMMAND(ID_PAGE_NEW_2DXY, CProjectFrame::OnPageNew2dXY)
  ON_COMMAND(ID_PAGE_NEW_COMPOSITE, CProjectFrame::OnPageNewComposite)
  ON_COMMAND(ID_PAGE_BRING_PLOT_TO_TOP, CProjectFrame::OnPageBringPlotToTop)
  ON_COMMAND(ID_WINDOW_LIST, CProjectFrame::OnWindowList)
  ON_UPDATE_COMMAND_UI(ID_WINDOW_LIST, CProjectFrame::OnUpdateWindowList)

  ON_COMMAND(ID_VIEW_SETTINGS, CProjectFrame::OnViewSettings)
  ON_COMMAND(ID_MINIMIZE_ALL_PLOTS1, CProjectFrame::OnMinimizeAllPlots)
  ON_UPDATE_COMMAND_UI(ID_MINIMIZE_ALL_PLOTS1, CProjectFrame::OnUpdateMinimizeAllPlots)

  // Global help commands
  ON_COMMAND(ID_HELP_FINDER, CProjectFrame::OnHelpFinder)
  ON_COMMAND(ID_HELP, CProjectFrame::OnHelp)
  ON_COMMAND(ID_CONTEXT_HELP, CProjectFrame::OnContextHelp)
  ON_COMMAND(ID_DEFAULT_HELP, CProjectFrame::OnHelpFinder)

  // Call nPre from menu
  ON_COMMAND_RANGE(ID_NPRE_0, ID_NPRE_10, OnNpreEditor)
  ON_UPDATE_COMMAND_UI_RANGE(ID_NPRE_0, ID_NPRE_10, OnUpdateNpreEditor)

  // set listing sensitivity
  ON_UPDATE_COMMAND_UI_RANGE(ID_LIST_CURRENT, ID_LIST_MESSAGES, OnUpdateListEditor)

  // set auto setup sensitivity
  ON_UPDATE_COMMAND_UI_RANGE(ID_AUTO_FIELD, ID_AUTO_FIT, OnUpdateAutoEditor)

  // run final sensitivity
  ON_UPDATE_COMMAND_UI(ID_RUN_OPTFINAL, OnUpdateRunOptFinal)



END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
  m_nPulldownMenuFrom = ID_PULLMENU_START;
  m_nPulldownMenuTo = ID_PULLMENU_END;
  m_nWindowMenuStart = ID_WINDOW_MENU_START;
  m_nWindowMenuEnd = ID_WINDOW_MENU_END;
  m_nWindowMenuPos = 9;
  m_nTemplateMenuStart = ID_TEMPLATE_MENU_START;
  m_nTemplateMenuEnd = ID_TEMPLATE_MENU_END;
  m_nTemplateMenuPos = 6;
  AppMenuC::SetTemplateFileExtension("nPre");
}

CMainFrame::~CMainFrame()
{
  DestroyInterface();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  mainToolbarID = IDR_MAINFRAME;
  if (COutlookFrame::OnCreate(lpCreateStruct) == -1)
    return -1;

  nPreUtilities::LoadSettings();
  AppMenuC::SetMenuBarSensitive();

  return 0;
}

BOOL CMainFrame::DestroyWindow()
{
  nPreUtilities::SaveSettings();
  return COutlookFrame::DestroyWindow();
}



BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
  if (!COutlookFrame::PreCreateWindow(cs))
    return FALSE;

  return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
  m_imaLarge.Create(IDB_IMAGELIST, 32, 0, RGB(128,128,128));
  m_imaSmall.Create(IDB_SMALL_IMAGELIST, 16, 0, RGB(0,128,128));

  return COutlookFrame::OnCreateClient(lpcs, pContext);
}

void CMainFrame::ResetInterface(CDocument* pDocument, bool bRestoreContext)
{
  COutlookFrame::ResetInterface(pDocument, bRestoreContext);

  // Set CNPreView as an active view - so messages can be routed to the document (Save, SaveAs)
  m_pViewActive = GetNPreView();
}

void CMainFrame::CreateInterface()
{
  ASSERT(m_pInterface == NULL);
  m_pInterface = new NPreInterfaceOutlook(NULL, 0, nodesRoot, interfaceContext);

  COutlookFrame::CreateInterface();
}

void CMainFrame::OnNpreEditor(UINT nID)
{
  int nIndex = nID - ID_NPRE_0;
  FolderNode* pFolderNode = static_cast<FolderNode*>(m_pInterface->GetOutlookPane()->GetNode(0));
  m_pInterface->ShowNode(pFolderNode->SelectNode(nIndex));
}

void CMainFrame::OnUpdateNpreEditor(CCmdUI* pCmdUI)
{
  if (pCmdUI->m_nID == ID_NPRE_5) {// fit specification
    pCmdUI->Enable(control.IsOptimization() || control.IsRange());
    return;
  }
  if (pCmdUI->m_nID == ID_NPRE_6) {// Optimization
    pCmdUI->Enable(control.IsOptimization());
    return;
  }
  if (pCmdUI->m_nID == ID_NPRE_7) {// Sampling
    pCmdUI->Enable(control.IsSampleSim());
    return;
  }
  if (pCmdUI->m_nID == ID_NPRE_8) {// Suite/Range
    pCmdUI->Enable(!control.IsSampleSim());
    return;
  }
  pCmdUI->Enable();
}

void CMainFrame::OnUpdateListEditor(CCmdUI* pCmdUI)
{
  if (pCmdUI->m_nID == ID_LIST_CURRENT) {

    ListFactory* pListFact = NULL;
    if (m_pInterface != NULL)
    {
      pListFact = m_pInterface->GetListFactory();
    }

    pCmdUI->Enable(pListFact != NULL);
    return;
  }
  if (pCmdUI->m_nID == ID_LIST_ERROR) {

    bool noErrors = true;

    if (m_pInterface != NULL)
    {
      ListFactory* pListFact = m_pInterface->GetListFactory();
      if (pListFact)
      {
        ErrorListObjC* currErr = pListFact->GetErrorListObject();
        if (currErr)
          noErrors = currErr->ErrorCheckOK();
      }
    }

    pCmdUI->Enable(!noErrors);
    return;
  }

  if (pCmdUI->m_nID == ID_LIST_CALCULATED)
  {
    bool calcOK = parameters.ParametersOK();
    if (!control.IsSampleSim())
      calcOK = calcOK && vary.VaryOK();
    pCmdUI->Enable(calcOK);
    return;
  }

  pCmdUI->Enable();
}

void CMainFrame::OnUpdateAutoEditor(CCmdUI* pCmdUI)
{
  bool opOK = false;
  if (pCmdUI->m_nID == ID_AUTO_FIELD)
  {
    opOK = dataCapture.DataCaptureOK();
  }
  if (pCmdUI->m_nID == ID_AUTO_SEQUENCE)
  {
    opOK = dataCapture.DataCaptureOK() && sequence.SequencesOK();
  }

  if (pCmdUI->m_nID == ID_AUTO_FIT)
  {
    opOK = (control.IsOptimization() || control.IsRange()) &&
           dataCapture.DataCaptureOK() && sequence.SequencesOK();
  }

  pCmdUI->Enable(opOK);
}

void CMainFrame::LoadTemplateFile(const char* tfName)
{
  CNPreDoc* docPtr =CNPreDoc::GetDoc();
  if (docPtr != NULL)
  {
    if (!docPtr->SaveModified())
      return;
    docPtr->OnOpenTemplateDocument(tfName);
  }
}

bool CMainFrame::RunSetup()
{
  // update menus
  AfxGetMainWnd()->SetFocus();

  // reset all FO
  nPreRootData::PrepareAllSystemFO(true);

  bool fileError;
  if (!CheckInput(fileError))
  {
    if (fileError)
      return false;

    ListAllErrors();
    return false;
  }

  if (nPreUtilities::bRunWithMinimizedMain)
  {
    AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
  }

  return true;
}

void CMainFrame::RunComplete()
{
  if (nPreUtilities::bRunWithMinimizedMain) {
    AfxGetMainWnd()->ShowWindow(SW_RESTORE);
  }

  // need to redisplay current dialog/mView window
  if (m_pInterface != NULL)
    m_pInterface->ShowActive();

}


void CMainFrame::OnRunVerbose()
{
  if (RunSetup())
  {
    RunVerbose();
    RunComplete();
  }

}

void CMainFrame::OnRunMinimal()
{
  if (RunSetup())
  {
    RunMinimal();
    RunComplete();
  }
}

void CMainFrame::OnRunNoThreads()
{
  if (RunSetup())
  {
    RunNoThreads();
    RunComplete();
  }
}

void CMainFrame::OnRunOptFinal()
{
  if (FinalOptOK() && RunSetup())
  {
    RunFinalOpt();
    RunComplete();
  }
}

void CMainFrame::OnRunMinimizeMain()
{
  nPreUtilities::bRunWithMinimizedMain = !nPreUtilities::bRunWithMinimizedMain;
}

void CMainFrame::OnUpdateRunMinimizeMain(CCmdUI* pCmdUI)
{
  pCmdUI->SetCheck(nPreUtilities::bRunWithMinimizedMain);
}


bool CMainFrame::FinalOptOK()
{
  return  control.IsOptimization() && control.IsNormal() &&
          (VaryVar::GetnActive() == 0) &&
          optimize.RequiresChisq() && (!optimize.doOptPerturbations);
}


void CMainFrame::OnUpdateRunOptFinal(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(FinalOptOK());
}


void CMainFrame::OnListCurrent()
{
  ListFactory* pListFact = NULL;

  if (m_pInterface != NULL) {
    pListFact = m_pInterface->GetListFactory();
  }

  if (pListFact != NULL)
  {
    ListObjC* currObj = pListFact->CreateListObject();
    if (currObj)
    {
      ListDefCPS modalListing("Current Menu");
      modalListing.AddTo(currObj);
      DoModalView(modalListing);
      delete currObj;
    }
  }

}

void CMainFrame::OnListError()
{
  ListFactory* pListFact = NULL;

  if (m_pInterface != NULL) {
    pListFact = m_pInterface->GetListFactory();
  }

  if (pListFact != NULL)
  {
    ListObjC* currObj = pListFact->GetErrorListObject();
    if (currObj)
    {
      ListDefCPS modalListing("Current Errors");
      modalListing.AddTo(currObj);
      DoModalView(modalListing);
    }
  }
}



void CMainFrame::OnListAll()
{
  ControlListing    controlListing;
  ParameterListing  parameterListing;
  SequenceListing   sequenceListing;
  DataCaptureListing  dataCaptureListing;
  OutputFilesListing  outputFilesListing;
  OptimizeListing   optimizeListing;
  SampleListing   sampleListing;
  VaryListing     varyListing;

  CalcParameterListing  calcParameterListing;
  PointsParameterListing  pointsParameterListing;
  bool parOK = parameters.ParametersOK();
  bool pointsOK = parameters.HasPointsParameters();

  {
    ListDefCPS modalListing("nPre Setup Listing");

    modalListing.AddTo(&controlListing);
    modalListing.AddTo(&parameterListing);
    if (pointsOK)
      modalListing.AddTo(&pointsParameterListing);
    if (parOK)
      modalListing.AddTo(&calcParameterListing);
    modalListing.AddTo(&sequenceListing);
    modalListing.AddTo(&dataCaptureListing);
    modalListing.AddTo(&outputFilesListing);
    if (control.IsOptimization())
      modalListing.AddTo(&optimizeListing);
    if (control.IsSampleSim())
      modalListing.AddTo(&sampleListing);
    if (!control.IsSampleSim())
      modalListing.AddTo(&varyListing);

    DoModalView(modalListing);
  }
}


void CMainFrame::ListAllErrors()
{
  ListDefCPS modalListing("nPre Error Listing");

  if (!controlErrorListing.ErrorCheckOK())
    modalListing.AddTo(&controlErrorListing);

  if (!parameterErrorListing.ErrorCheckOK())
    modalListing.AddTo(&parameterErrorListing);
  if (!sequenceErrorListing.ErrorCheckOK())
    modalListing.AddTo(&sequenceErrorListing);
  if (!dataCaptureErrorListing.ErrorCheckOK())
    modalListing.AddTo(&dataCaptureErrorListing);
  if (!outputFilesErrorListing.ErrorCheckOK())
    modalListing.AddTo(&outputFilesErrorListing);
  if (control.IsOptimization() && (!optimizeErrorListing.ErrorCheckOK()))
    modalListing.AddTo(&optimizeErrorListing);
  if (control.IsSampleSim() && (!sampleErrorListing.ErrorCheckOK()))
    modalListing.AddTo(&sampleErrorListing);
  if (!control.IsSampleSim() && (!varyErrorListing.ErrorCheckOK()))
    modalListing.AddTo(&varyErrorListing);

  DoModalView(modalListing);
}



void CMainFrame::OnListAllErrors()
{
  ListAllErrors();
}


void CMainFrame::OnListCalculated()
{
  CalcParameterListing calcParameterListing;
  {
    ListDefCPS modalListing("Calculated Parameter Listing");
    modalListing.AddTo(&calcParameterListing);

    DoModalView(modalListing);
  }
}

void CMainFrame::OnListMessages()
{
  LFO_MessageLogListing messageListing;
  {
    ListDefCPS modalListing("Error Message Listing");
    modalListing.AddTo(&messageListing);
    modalListing.SuppressDefaults();
    DoModalView(modalListing);
  }
}


void CMainFrame::OnAutoField()
{
  MenuObjC::CloseOpenMenu();

  CAutoFieldDlg dlg;

  if (dlg.DoModal() == IDOK)
  {
    ResetRoots();
    OnNpreEditor(ID_NPRE_10);
  }
}

void CMainFrame::OnAutoSequence()
{
  MenuObjC::CloseOpenMenu();

  CAutoSequenceDlg dlg;

  if (dlg.DoModal() == IDOK)
  {
    ResetRoots();
    OnNpreEditor(ID_NPRE_10);
  }
}

void CMainFrame::OnAutoFit()
{
  MenuObjC::CloseOpenMenu();

  CAutoFitDlg dlg;

  if (dlg.DoModal() == IDOK)
  {
    ResetRoots();
    OnNpreEditor(ID_NPRE_10);
  }
}

void CMainFrame::OnSaveCommandLine()
{
  MenuObjC::CloseOpenMenu();

  CSaveForNPreXDlg dlg;

  if (dlg.DoModal() == IDOK)
  {
    // slight kluge ....
    char oldConfigFileName[stdFileStrLen];
    char newConfigFileName[stdFileStrLen];
    CopyString(oldConfigFileName, currConfigFileName, stdFileStrLen);
    CopyString(newConfigFileName, currConfigFileName, stdFileStrLen);
    char* extPos = strstr(newConfigFileName, ".nPre");
    // truncate at extPos
    if (extPos != NULL)
      *extPos ='\0';

    // concat new ext
    ConcatString(newConfigFileName, "X.nPre", stdFileStrLen);
    nPreFileOperations::SaveFile(newConfigFileName, true);

    // restore old config
    CopyString(currConfigFileName, oldConfigFileName, stdFileStrLen);

    //return to old paths and remove carriage returns (kluge!!)
    if (writeLinux)
    {
      if (changePaths)
        FileNameArray::ReturnPaths();

      BufFileC::WindowsToUnix(newConfigFileName);

    }//if writeLinux

  }
}

void CMainFrame::OnUpdateSaveCommandLine(CCmdUI* pCmdUI)
{
  bool clOK = !IsNull(currConfigFileName);

  pCmdUI->Enable(clOK);
}


CNPreView* CMainFrame::GetNPreView()
{
  return static_cast<CNPreView*>(m_pInterface->GetView());
}


void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
  COutlookFrame::OnActivate(nState, pWndOther, bMinimized);
}




