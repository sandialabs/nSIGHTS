///////////////////////////////////////////////////////////////////////////////////
//
// NPreInterfacemView.cpp
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


#include "StdAfx.h"

#include <genMFC/ProjectTreeView.h>
#include <genMFC/ProjectView.h>

#include <App/nPreRoot.h>
#include <App/nPreAppMenu.h>
#include <Var/G_Control.h>
#include <genApp/C_MenuPage.h>

#include <osDFO/DFO_FitBase.h>
#include <osMain/G_Optimize.h>

#include "NPreInterfaceApp.h"
#include "NPreInterfaceMView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



NPreMViewNode::NPreMViewNode(InterfaceNode* pParent, int nIdent,
                             NodeDescription* pDescription,
                             nPreRoot& projectRoot) :
  MViewNode(pParent, nIdent, pDescription, projectRoot)
{
}

NPreMViewNode::~NPreMViewNode()
{
}

void NPreMViewNode::BeforeShow(bool bShow)
{
  if (bShow)
  {
    // reset all FO
    nPreRootData::PrepareAllSystemFO(false, false);
  }

}
void NPreMViewNode::AfterShow(bool bShow)
{
  if (bShow)
  {
    optimize.UpdateStatus();   // update output vars
  }

}


//////////////////////////////////////////////////////////////////////////////////////////////
FieldDataMViewNode::FieldDataMViewNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
  NPreMViewNode(pParent, nIdent, pDescription, fieldDataRoot)
{
}


//////////////////////////////////////////////////////////////////////////////////////////////
FitSpecMViewNode::FitSpecMViewNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
  NPreMViewNode(pParent, nIdent, pDescription, fitSpecRoot)
{
}

void FitSpecMViewNode::AfterShow(bool bShow)
{
  if (bShow) {
    optimize.UpdateStatus();   // update output vars
    optimize.OptimizeOK();
    DFO_FitBase::useFitsForOptimization = control.IsOptimization();
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////
SequenceMViewNode::SequenceMViewNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
  NPreMViewNode(pParent, nIdent, pDescription, sequenceRoot)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////
ProcessingSetupMViewNode::ProcessingSetupMViewNode(InterfaceNode* pParent, int nIdent, NodeDescription* pDescription) :
  NPreMViewNode(pParent, nIdent, pDescription, processingSetupRoot)
{
}
