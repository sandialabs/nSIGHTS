///////////////////////////////////////////////////////////////////////////////////
//
// C_SingleWell1D.cpp
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
#include <nsClass/nSightConst.h>

#include <Var/G_Parameter.h>
#include <Var/E_Parameter.h>
#include <Var/G_Control.h>

#include "C_SingleWell1D.h"


SingleWell1D::SingleWell1D()
{
}
SingleWell1D::~SingleWell1D()
{
}

bool SingleWell1D::SetupForRun(SC_SetupErr&   errData)
{
  if (!SingleWellSimulator::SetupForRun(errData))
    return false;

  double xyRatio = 1.0;
  double xyAngle = 0.0;
  if (control.IsHorizontalAnisotropic())
  {
    xyRatio = wPar[pF_KxyRatio].GetData();
    xyAngle = wPar[pF_KxyAngle].GetData();
  }

  if (!wCap.WorkingCaptureSetup(nodeRadii, staticPressure, xyRatio, xyAngle))
    return false;


  return true;
}

void SingleWell1D::UpdateOutput()
{
  wCap.UpdateCapture();
  SingleWellSimulator::UpdateOutput();
}

void SingleWell1D::DoAllocateAll()
{
  SingleWellSimulator::DoAllocateAll();

  int maxradialNodes = allParam[pN_n_r]->GetIntMaxPar();

  nodePressure.Alloc(maxradialNodes);

  if (saveOldPressures)
    oldNodePressure.Alloc(maxradialNodes);

  dGeomTerm.Alloc(maxradialNodes);
  sGeomTerm.Alloc(maxradialNodes);
}



void SingleWell1D::SetRuntimeSize()
{
  SingleWellSimulator::SetRuntimeSize();

  nodePressure.SetSize(nradialNodes);
  if (saveOldPressures)
    oldNodePressure.SetSize(nradialNodes);

  dGeomTerm.SetSize(nradialNodes);
  sGeomTerm.SetSize(nradialNodes);
}

void SingleWell1D::SetRunParameters()
{
  SingleWellSimulator::SetRunParameters();

  for (int j = 0; j < dGeomTerm.UpperBound(); j++)
  {
    dGeomTerm[j] = nodeFlowArea[j] / (nodeRadii[j + 1] - nodeRadii[j]);
    sGeomTerm[j] = nodeVolume[j];
  }
  dGeomTerm.LastIndex() = 0.0;
  sGeomTerm.LastIndex() = nodeVolume.LastIndex();

  //  pressure initialization
  nodePressure.FillToSize(staticPressure);
}

void SingleWell1D::UpdateTSOutput()
{
  wCap.ApplySpec(currSeqTime.testTime, nodePressure, currSeqTZ);
}

void SingleWell1D::SaveCurrentPressure()
{
  oldNodePressure = nodePressure;
}

void SingleWell1D::RestoreCurrentPressure()
{
  nodePressure  = oldNodePressure;
}

void SingleWell1D::SequenceSetup()
{
  nequations = nradialNodes;
  if (control.ExtBoundIsFixedP())
    nequations--;
  if (currentSeqIsFixed)
    nequations--;
  else
  {
    lastSeqTZ.tzPressure = currSeqTZ.tzPressure;
    nodePressure[0] = GetLastWellPressure();  // FB182 detected error
  }
}


void SingleWell1D::ScaleGeomTerms(const double& dCons,
                                  const double& sCons)
{
  //  scale by constants
  for (int j = 0; j < nradialNodes; j++)
  {
    dGeomTerm[j] *= dCons;
    sGeomTerm[j] *= sCons;
  }
  sLastSkinGeom = lastSkinVolume * sCons;
}

