///////////////////////////////////////////////////////////////////////////////////
//
// C_SingleWell2DConfined.cpp
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
#include <genClass/U_Real.h>

#include <nsClass/nSightConst.h>

#include <Var/G_Parameter.h>
#include <Var/E_Parameter.h>
#include <Var/G_Control.h>
#include <osClass/C_SimErr.h>


#include "C_SingleWell2DConfined.h"


SingleWell2DConfined::SingleWell2DConfined()
{
}
SingleWell2DConfined::~SingleWell2DConfined()
{
}

bool SingleWell2DConfined::SetupForRun(SC_SetupErr&   errData)
{
  if (!SingleWell2D::SetupForRun(errData))
    return false;


  if (!wCap.WorkingCaptureSetup(nodeRadii, staticPressure, constantNodeZ))
    return false;


  return true;
}

void SingleWell2DConfined::UpdateOutput()
{
  wCap.UpdateCapture();
  SingleWellSimulator::UpdateOutput();
}

void SingleWell2DConfined::UpdateTSOutput()
{
  wCap.ApplySpec(currSeqTime.testTime, nodePressure, currSeqTZ);
}

void SingleWell2DConfined::DoAllocateAll()
{
  SingleWell2D::DoAllocateAll();
}

void SingleWell2DConfined::SetRuntimeSize()
{
  SingleWell2D::SetRuntimeSize();

  nodePressure.MatrixSetSize(nverticalNodes, nradialNodes);
}


void SingleWell2DConfined::SetRunParameters()
{
  SingleWell2D::SetRunParameters();
}




void  SingleWell2DConfined::MatSolve(bool& setupRequired)
{
//  DWORD startT = GetTickCount();

  SetupForSolve();

//  DWORD s1 = GetTickCount();

  topVertIndex = nverticalNodes - 1;

  BuildMatrices();
//  DWORD s2 = GetTickCount();

  AddWellModel();
//  DWORD s3 = GetTickCount();

  SetWellBC();
//  DWORD s4 = GetTickCount();

  SetExternalBC();
//  DWORD s5 = GetTickCount();

  Solve2D(ntotalSoln, nverticalNodes);
//  DWORD s6 = GetTickCount();

  UpdatePressures();
//  DWORD s7 = GetTickCount();

  CalculateFlows();

  /*  DWORD s8 = GetTickCount();

    debugF.WriteInt(startT);
    debugF.WriteInt(s1 - startT);
    debugF.WriteInt(s2 - s1);
    debugF.WriteInt(s3 - s2);
    debugF.WriteInt(s4 - s3);
    debugF.WriteInt(s5 - s4);
    debugF.WriteInt(s6 - s5);
    debugF.WriteInt(s7 - s6);
    debugF.WriteInt(s8 - s7);
    debugF.WriteLine();
  */
};

void SingleWell2DConfined::BuildMatrices()
{
  int solnIndx = 0;
  double nextD = 0.0;
  int lastMatIndx = nverticalNodes * 2;

  for (int i = 0; i < npartialEquations; i++)
  {
    double lastD = nextD;

    // radial flow
    if ((i == (npartialEquations - 1)) && (!control.ExtBoundIsFixedP()))
      nextD = 0.0;
    else
      nextD = dTerm[i];
    double sterm = sTerm[i] / deltaT;

    double vterm = qvTerm[i];

    double lastV = 0.0;
    for (int j = 0; j < nverticalNodes; j++)
    {
      // for area of radial flow term and volume of storage term
      double nextB;
      if (j == 0)
      {
        nextB = constantNodeDeltaZ[0] / 2.0;
      }
      else if (j < topVertIndex)
      {
        nextB = (constantNodeDeltaZ[j - 1] + constantNodeDeltaZ[j]) / 2.0;
      }
      else
        nextB = constantNodeDeltaZ[j - 1]  / 2.0;


      // basix matrix terms
      double lastDLocal = lastD * nextB;
      double currSTerm = sterm * nextB;
      double nextDLocal = nextD * nextB;

      double nextV = 0.0;
      if (j < topVertIndex)
        nextV = vterm / constantNodeDeltaZ[j];

      double nextVOffDiag = nextV;
      double lastVOffDiag = lastV;
      double lastDOffDiag = lastDLocal;

      // special processing for fixed head
      int lastDiagOffset = 0;
      int nextDiagOffset = 0;
      if (currentSeqIsFixed)
      {
        // applied at wellbore
        if (i == 0)
        {
          if (control.IsUniformWellModel())
          {
            if (j < firstWellNode)
              nextDiagOffset = nwellNodes;

            if (isWellNode[j])
            {
              lastV = nextV;
              continue;
            }
          }
          else
          {
            if (j < wellNode)
              nextDiagOffset = 1;

            if (j == wellNode)
            {
              lastV = nextV;
              continue;
            }
          }

          // flow to node below
          if (j == (firstWellNode - 1))
          {
            rhsVector[solnIndx] += fixedTZPressure * nextV;
            nextVOffDiag = 0.0;
          }
          else if (j == (lastWellNode + 1))
          {
            rhsVector[solnIndx] += fixedTZPressure * lastV;
            lastVOffDiag = 0.0;
          }
        }
        else if (i == 1)
        {
          if (control.IsUniformWellModel())
          {
            if (j < firstWellNode)
              lastDiagOffset = nwellNodes;

            if (isWellNode[j])
            {
              rhsVector[solnIndx] += fixedTZPressure * lastDLocal;
              lastDOffDiag = 0.0;
            }
          }
          else
          {
            if (j < wellNode)
              lastDiagOffset = 1;

            if (j == wellNode)
            {
              rhsVector[solnIndx] += fixedTZPressure * lastDLocal;
              lastDOffDiag = 0.0;
            }
          }
        }
      }

      matA[lastDiagOffset][solnIndx] = -lastDOffDiag;
      matA[nverticalNodes - 1][solnIndx] = -lastVOffDiag;
      matA[nverticalNodes][solnIndx]
      = lastDLocal + nextDLocal + lastV + nextV + currSTerm;
      matA[nverticalNodes + 1][solnIndx] = -nextVOffDiag;
      matA[lastMatIndx - nextDiagOffset][solnIndx] = -nextDLocal;
      rhsVector[solnIndx] += currSTerm * nodePressure[j][i];

      lastV = nextV;
      solnIndx++;
    }

  }

  ntotalSoln = solnIndx;

}

void SingleWell2DConfined::AddWellModel()
{
  verticalWellTerm = 0.0;
  if (control.IsUniformWellModel())
    return;

  double lastBWell = 1.0;  // to avoid / 0
  verticalWellTerm = baseVerticalWellTerm;
  if (control.IsMultiplierWellModel())
    verticalWellTerm *= qvTerm[0];

  int solnIndx = 0;
  for (int i = 0; i < nverticalNodes; i++)
  {
    // distance up to next node
    double bwell = constantNodeDeltaZ[i];

    double lastVWell = verticalWellTerm / lastBWell;
    double nextVWell = verticalWellTerm / bwell;

    double lastVWellOffDiag = lastVWell;
    double nextVWellOffDiag = nextVWell;

    if (isWellNode[i])
    {
      if (currentSeqIsFixed)
      {
        if (i == wellNode)
        {
          lastBWell = bwell;
          continue;
        }

        if (i == (wellNode + 1))
        {
          rhsVector[solnIndx] += fixedTZPressure * lastVWell;
          lastVWellOffDiag = 0.0;
        }
      }

      if ((i > 0) && isWellNode[i - 1])
      {
        // down to last well node
        matA[nverticalNodes - 1][solnIndx] -= lastVWellOffDiag;
        matA[nverticalNodes][solnIndx] += lastVWell;
      }

      if ((i < topVertIndex) && isWellNode[i + 1])
      {
        // up to next well node
        matA[nverticalNodes][solnIndx] += nextVWell;
        matA[nverticalNodes + 1][solnIndx] -= nextVWellOffDiag;
      }
    }

    solnIndx++;
    lastBWell = bwell;
  }
}

void SingleWell2DConfined::SetWellBC()
{
  if (currentSeqIsFixed)
    return;

  double nodeLHS = GetWellBCLhs();
  double nodeRHS = GetWellBCRhs();
  if (control.IsUniformWellModel())
  {
    nodeLHS /= double(nwellNodes - 1);
    nodeRHS /= double(nwellNodes - 1);

    for (int i = firstWellNode; i <= lastWellNode; i++)
    {
      if ((i > firstWellNode) && (i < lastWellNode))
      {
        matA[nverticalNodes][i] += nodeLHS;
        rhsVector[i] += nodeRHS;
      }
      else
      {
        matA[nverticalNodes][i] += nodeLHS / 2.0;
        rhsVector[i] += nodeRHS / 2.0;
      }
    }
  }
  else
  {
    matA[nverticalNodes][wellNode] += nodeLHS;
    rhsVector[wellNode] += nodeRHS;
  }
}

void SingleWell2DConfined::SetExternalBC()
{
  if (!control.ExtBoundIsFixedP())
    return;

  int stSoln = ntotalSoln - nverticalNodes;

  for (int i = 0; i < nverticalNodes; i++)
  {
    // same logic as before
    double nextB;
    if (i == 0)
    {
      nextB = constantNodeDeltaZ[0] / 2.0;
    }
    else if (i < topVertIndex)
    {
      nextB = (constantNodeDeltaZ[i - 1] + constantNodeDeltaZ[i]) / 2.0;
    }
    else
    {
      nextB = constantNodeDeltaZ[i - 1] / 2.0;
    }

    rhsVector[stSoln++] -= externalBoundaryTerm * nextB;
  }
}


void SingleWell2DConfined::UpdatePressures()
{
  int solnIndx = 0;
  for (int i = 0; i < npartialEquations; i++)
  {
    if (currentSeqIsFixed && (i == 0))
    {
      for (int j = 0; j < nverticalNodes; j++)
        if ((j < firstWellNode) || (j > lastWellNode))
          nodePressure[j][0] = solutionVector[solnIndx++];
    }
    else
      for (int j = 0; j < nverticalNodes; j++)
        nodePressure[j][i] = solutionVector[solnIndx++];
  }

  if (currentSeqIsFixed)
  {
    for (int j = firstWellNode; j <= lastWellNode; j++)
      nodePressure[j][0] = fixedTZPressure;
  }
  else
  {
    /*    if (control.IsUniformWellModel())
        {
          double pSum = 0.0;
          for (int i = firstWellNode; i <= lastWellNode; i++)
            pSum += nodePressure[i][0];
          currSeqTZ.tzPressure = pSum / double(nwellNodes);
        }
        else */
    currSeqTZ.tzPressure = nodePressure[wellNode][0];
  }
}

void SingleWell2DConfined::CalculateFlows()
{

  // formation flow
  //  = radial term + well above + well below + storage at node
  // need to find area and vert distance terms
  // TODO - fix for uniform

  double flowVertUp = 0.0;
  double flowVertDown = 0.0;
  double flowRadial = 0.0;
  double flowStorage = 0.0;

  if (control.IsUniformWellModel())
  {
    if (lastWellNode < (nverticalNodes - 1))
      flowVertUp = qvTerm[0] / constantNodeDeltaZ[lastWellNode]
                   * (nodePressure[lastWellNode][0] - nodePressure[lastWellNode + 1][0]);
    if (firstWellNode > 0)
      flowVertDown = qvTerm[0] / constantNodeDeltaZ[firstWellNode - 1]
                     * (nodePressure[firstWellNode][0] - nodePressure[firstWellNode - 1][0]);

    double dArea;
    for (int j = firstWellNode; j <= lastWellNode; j++)
    {
      if (j == 0)
      {
        dArea = constantNodeDeltaZ[0] / 2.0;
      }
      else if (j == (nverticalNodes - 1))
      {
        dArea = constantNodeDeltaZ[j - 1]  / 2.0;
      }
      else
      {
        dArea = (constantNodeDeltaZ[j - 1] + constantNodeDeltaZ[j]) / 2.0;
      }

      flowRadial += dTerm[0] * dArea
                    * (nodePressure[j][0] - nodePressure[j][1]);

      flowStorage += sTerm[0] / deltaT * dArea
                     * (nodePressure[j][0] - lastWellPressure[j]);

    }
  }
  else
  {

    double dArea, dVertUp, dVertDown;
    if (wellNode == 0)
    {
      dVertDown = 0.0;
      dArea = constantNodeDeltaZ[0] / 2.0;
      dVertUp = constantNodeDeltaZ[0];
    }
    else if (wellNode <  (nverticalNodes - 1))
    {
      dArea = (constantNodeDeltaZ[wellNode - 1] + constantNodeDeltaZ[wellNode]) / 2.0;
      dVertUp = constantNodeDeltaZ[wellNode];
      dVertDown = constantNodeDeltaZ[wellNode - 1];
    }
    else if (wellNode == (nverticalNodes - 1))
    {
      dArea = constantNodeDeltaZ[wellNode - 1]  / 2.0;
      dVertUp = 0.0;
      dVertDown = constantNodeDeltaZ[wellNode - 1];
    }
    else
      GenAppInternalError("SimulatorCore::CalculateFlows()");


    if (wellNode < (nverticalNodes - 1))
      flowVertUp = (verticalWellTerm + qvTerm[0]) / dVertUp
                   * (currSeqTZ.tzPressure - nodePressure[wellNode + 1][0]);
    if (wellNode > 0)
      flowVertDown = qvTerm[0] / dVertDown
                     * (currSeqTZ.tzPressure - nodePressure[wellNode - 1][0]);
    flowRadial = dTerm[0] * dArea
                 * (currSeqTZ.tzPressure - nodePressure[wellNode][1]);

    flowStorage = sTerm[0] / deltaT * dArea
                  * (currSeqTZ.tzPressure - lastSeqTZ.tzPressure);
  }

  currSeqTZ.formFlow  = flowVertUp + flowVertDown + flowRadial + flowStorage;


}





