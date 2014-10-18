///////////////////////////////////////////////////////////////////////////////////
//
// C_SingleWell2DUnconfined.cpp
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


#include "C_SingleWell2DUnconfined.h"


SingleWell2DUnconfined::SingleWell2DUnconfined()
{
}
SingleWell2DUnconfined::~SingleWell2DUnconfined()
{
}

bool SingleWell2DUnconfined::SetupForRun(SC_SetupErr&   errData)
{
  if (!SingleWell2D::SetupForRun(errData))
    return false;


  if (!wCap.WorkingCaptureSetup(nodeRadii, staticPressure,
                                kpaConst, constantNodeZ))
    return false;

  // water table not a well node
  if (lastWellNode == (nverticalNodes - 1))
  {
    lastWellNode--;
    nwellNodes--;
  }

  isWellNode[nverticalNodes - 1] = false;

  return true;
}

void SingleWell2DUnconfined::UpdateOutput()
{
  wCap.UpdateCapture();
  SingleWellSimulator::UpdateOutput();
}

void SingleWell2DUnconfined::UpdateTSOutput()
{
  wCap.ApplySpec(currSeqTime.testTime, currSatThick, nodePressure, currSeqTZ);
}

void SingleWell2DUnconfined::DoAllocateAll()
{
  SingleWell2D::DoAllocateAll();

  int maxradialNodes = allParam[pN_n_r]->GetIntMaxPar();

  int maxverticalNodes = allParam[pN_n_v2Dw]->GetIntMaxPar() + 1;
  if (control.IsPartialPenetration())
    maxverticalNodes += allParam[pN_n_v2Du]->GetIntMaxPar() +
                        allParam[pN_n_v2Dl]->GetIntMaxPar();

  syTerm.Alloc(maxradialNodes);

  currSatThick.Alloc(maxradialNodes);
  lastSatThick.Alloc(maxradialNodes);
  interpPressures.Alloc(maxverticalNodes);
}

void SingleWell2DUnconfined::SetRuntimeSize()
{
  SingleWell2D::SetRuntimeSize();

  nodePressure.MatrixSetSize(nverticalNodes - 1, nradialNodes);

  syTerm.SetSize(nradialNodes);
  currSatThick.SetSize(nradialNodes);
  lastSatThick.SetSize(nradialNodes);
  interpPressures.Alloc(nverticalNodes);
}


void SingleWell2DUnconfined::SetRunParameters()
{
  SingleWell2D::SetRunParameters();

  saturatedThickness = wPar[pF_t].GetData();
  currSatThick.FillToSize(saturatedThickness);
  lastSatThick.FillToSize(saturatedThickness);

  for (int j = 0; j < nradialNodes; j++)
  {
    double skTerm = 0.0;
    if (control.HasSkin() && (j == fracStartNode))
    {
      skTerm = GetRockPar(skinParGroup.syIndx, j);
      skTerm *= lastSkinArea;
    }

    const ParameterGroups& currRock = (*rockMap[j]);
    double sTerm = GetRockPar(currRock.syIndx, j);

    // yield term is for delta distance - no kPA to Pa required
    syTerm[j] = skTerm + sTerm * nodePlanArea[j];
  }

}


double SingleWell2DUnconfined::GetStaticPressure()
{
  double satThick = wPar[pF_t].GetData();
  return satThick * gravConst * wPar[pFl_rho].GetData() / kpaToPa;
}


void  SingleWell2DUnconfined::MatSolve(bool& setupRequired)
{
  SetupForSolve();

  // variable defs changed slightly when 2D confined added
  // these are kluges to map back to previous unconfined usage
  unc_nverticalNodes = nverticalNodes - 1;  // doesn't include water table

  nverticalConstant  = constantNodeZ.Size() - 1;  // last constant node is treated as first vertical
  nverticalVariable  = unc_nverticalNodes - nverticalConstant;
  constantNodeThickness = constantNodeZ.LastIndex();
  topVertIndex = unc_nverticalNodes - 1;

  InterpolatePrevPressures();

  BuildMatrices();

  AddWellModel();

  SetWellBC();

  SetExternalBC();

  Solve2D(ntotalSoln, unc_nverticalNodes + 1);

  UpdatePressures();

  CalculateFlows();
}


void SingleWell2DUnconfined::InterpolatePrevPressures()
{
  // interpolate pressures in variable zone onto new grid
  if (unc_nverticalNodes < 2)
    return;

  // one vertical column at a time
  for (int i = 0; i < nradialNodes; i++)
  {
    double newdB = (currSatThick[i] - constantNodeThickness) / double(nverticalVariable);
    double olddB = (lastSatThick[i] - constantNodeThickness) / double(nverticalVariable);
    for (int j = 0; j < nverticalVariable; j++)
      interpPressures[j] = nodePressure[j + nverticalConstant][i];

    if (newdB < olddB)
    {
      double nextNew = newdB;
      double nextOld = 0.0;
      for (int j = 1; j < nverticalVariable; j++)
      {
        double dP = interpPressures[j] - interpPressures[j - 1];
        nodePressure[j + nverticalConstant][i] = interpPressures[j - 1] + dP / olddB * (nextNew - nextOld);
        nextNew += newdB;
        nextOld += olddB;
      }
    }
    else
    {
      double nextNew = newdB;
      double nextOld = olddB;
      for (int j = 1; j < nverticalVariable; j++)
      {
        double dP = -interpPressures[j];
        if (j < (nverticalVariable - 1))
          dP += interpPressures[j + 1];
        else
          dP += lastSatThick[i] * kpaConst;

        nodePressure[j + nverticalConstant][i] = interpPressures[j] + dP / olddB * (nextNew - nextOld);
        nextNew += newdB;
        nextOld += olddB;
      }
    }

  }
}

void SingleWell2DUnconfined::BuildMatrices()
{
  int solnIndx = 0;
  double nextD = 0.0;
  variableBasicB = 0.0;
  int lastMatIndx = (unc_nverticalNodes + 1) * 2;

  for (int i = 0; i < npartialEquations; i++)
  {
    double lastD = nextD;
    double lastVariableBasicB = variableBasicB;

    // radial flow
    if ((i == (npartialEquations - 1)) && (!control.ExtBoundIsFixedP()))
      nextD = 0.0;
    else
      nextD = dTerm[i];
    double sterm = sTerm[i] / deltaT;

    // per node thickness at this radius for variable nodes
    double totalVariableB = (currSatThick[i] - constantNodeThickness);
    variableBasicB = totalVariableB / double(nverticalVariable);

    double vterm = qvTerm[i];

    // top node will always be in variable zone, so can be set here for thickness
    double vbterm = vterm / (variableBasicB * kpaConst);

    double yterm = syTerm[i] / deltaT;


    double lastV = 0.0;
    for (int j = 0; j < unc_nverticalNodes; j++)
    {
      // for area of radial flow term and volume of storage term
      double nextB, lastB;

      if (j == 0)
      {
        // first vertical node
        if (nverticalConstant > 0)
        {
          // constant
          nextB = constantNodeDeltaZ[0] / 2.0;
          lastB = nextB;
        }
        else
        {
          if (unc_nverticalNodes == 1)
          {
            // simple
            nextB = variableBasicB;
            lastB = lastVariableBasicB;
          }
          else
          {
            // normal
            nextB = variableBasicB / 2.0;
            lastB = lastVariableBasicB / 2.0;
          }
        }
      }
      else if (j < nverticalConstant)
      {
        // in constant zone
        nextB = (constantNodeDeltaZ[j - 1] + constantNodeDeltaZ[j]) / 2.0;
        lastB = nextB;
      }
      else if (j == nverticalConstant)
      {
        // at interface
        nextB = (variableBasicB + constantNodeDeltaZ[j - 1]) / 2.0;
        lastB = (lastVariableBasicB + constantNodeDeltaZ[j - 1]) / 2.0;
      }
      else if (j == topVertIndex)
      {
        // at top
        nextB = variableBasicB * 1.5;
        lastB = lastVariableBasicB * 1.5;
      }
      else
      {
        // in variable zone
        nextB = variableBasicB;
        lastB = lastVariableBasicB;
      }

      // basix matrix terms
      double lastDLocal = lastD * lastB;
      double currSTerm = sterm * nextB;
      double nextDLocal = nextD * nextB;

      // vertical flow has to be divided by thickness
      double nextBVert = variableBasicB;
      if (j < nverticalConstant)
        nextBVert = constantNodeDeltaZ[j];
      double nextV = vterm / nextBVert;

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
      matA[unc_nverticalNodes][solnIndx] = -lastVOffDiag;
      matA[unc_nverticalNodes + 1][solnIndx]
      = lastDLocal + nextDLocal + lastV + nextV + currSTerm;
      if (j < topVertIndex)
        matA[unc_nverticalNodes + 2][solnIndx] = -nextVOffDiag;
      else
        matA[unc_nverticalNodes + 2][solnIndx] = -vbterm;
      matA[lastMatIndx - nextDiagOffset][solnIndx] = -nextDLocal;

      rhsVector[solnIndx] += currSTerm * nodePressure[j][i];

      lastV = nextV;
      solnIndx++;
    }

    // then vertical flow for water table thick
    if (currentSeqIsFixed && (i == 0) && (lastWellNode == (unc_nverticalNodes - 1)))
    {
      rhsVector[solnIndx] += fixedTZPressure * lastV;
    }
    else
    {
      matA[unc_nverticalNodes][solnIndx] = -lastV;
    }

    matA[unc_nverticalNodes + 1][solnIndx] = yterm + vbterm;
    rhsVector[solnIndx] += yterm * currSatThick[i];
    solnIndx++;

  }

  ntotalSoln = solnIndx;
}

void SingleWell2DUnconfined::AddWellModel()
{
  verticalWellTerm = 0.0;
  if (control.IsUniformWellModel())
    return;

  // add well conductivities
  variableBasicB = (currSatThick[0] - constantNodeThickness) / double(nverticalVariable);

  verticalWellTerm = baseVerticalWellTerm;
  if (control.IsMultiplierWellModel())
    verticalWellTerm *= qvTerm[0];

  double lastBWell = 1.0;  // to avoid / 0
  int solnIndx = 0;

  for (int i = 0; i < unc_nverticalNodes; i++)
  {
    // distance up to next node
    double bwell = variableBasicB;
    if (i < nverticalConstant)
      bwell = constantNodeDeltaZ[i];

    double lastVWell = verticalWellTerm / lastBWell;
    double nextVWell = verticalWellTerm / bwell;

    double lastVWellOffDiag = lastVWell;

    if (isWellNode[i])
    {
      if (currentSeqIsFixed)
      {
        if (i == wellNode)
        {
          lastBWell = bwell;
          continue;
        }
        else if (i == (wellNode + 1))
        {
          rhsVector[solnIndx] += fixedTZPressure * lastVWell;
          lastVWellOffDiag = 0.0;
        }
      }

      if ((i > 0) && isWellNode[i - 1])
      {
        // down to last well node
        matA[unc_nverticalNodes][solnIndx] -= lastVWellOffDiag;
        matA[unc_nverticalNodes + 1][solnIndx] += lastVWell;
      }

      if ((i < topVertIndex) && isWellNode[i + 1])
      {
        // up to next well node
        matA[unc_nverticalNodes + 1][solnIndx] += nextVWell;
        matA[unc_nverticalNodes + 2][solnIndx] -= nextVWell;

        //  if water table is included in well (see below)
        //      if (i < topVertIndex)
        //        matA[unc_nverticalNodes + 2][solnIndx] -= nextVWellOffDiag;
        //      else
        //        matA[unc_nverticalNodes + 2][solnIndx] -= nextVWellOffDiag / kpaConst;
      }
    }
    else
    {
      if (i > wellNode)
        break;
    }

    solnIndx++;
    lastBWell = bwell;
  }

// add connection of water table to well
// this is intuitively correct, but ruins early time type curve match
//  if (isWellNode[topVertIndex])
//  {
//    double lastVWell = verticalWellTerm / lastBWell;
//    matA[unc_nverticalNodes][solnIndx] -= lastVWell;
//    matA[unc_nverticalNodes + 1][solnIndx] += lastVWell / kpaConst;
//  }

}
void SingleWell2DUnconfined::SetWellBC()
{
  if (currentSeqIsFixed)
    return;

  double nodeLHS = GetWellBCLhs();
  double nodeRHS = GetWellBCRhs();
  if (control.IsUniformWellModel())
  {
    // LHS & RHS should be scaled by contributing distance
    variableBasicB = (currSatThick[0] - constantNodeThickness) / double(nverticalVariable);
    double wellLength = 0.0;
    for (int i = firstWellNode; i < lastWellNode; i++)
    {
      if (i < nverticalConstant)
      {
        // in constant zone
        wellLength += constantNodeDeltaZ[i];
      }
      else
      {
        // at interface
        wellLength += variableBasicB;
      }
    }

    nodeLHS /= wellLength;
    nodeRHS /= wellLength;

    for (int i = firstWellNode; i <= lastWellNode; i++)
    {
      double aboveLength, belowLength;
      GetVerticalEdgeLengths(i, aboveLength, belowLength);

      if (i < lastWellNode)
      {
        aboveLength /= 2.0;
        matA[unc_nverticalNodes + 1][i] += nodeLHS * aboveLength;
        rhsVector[i] += nodeRHS * aboveLength;
      }

      if (i > firstWellNode)
      {
        belowLength /= 2.0;
        matA[unc_nverticalNodes + 1][i] += nodeLHS * belowLength;
        rhsVector[i] += nodeRHS * belowLength;
      }
    }
  }
  else
  {
    matA[unc_nverticalNodes + 1][wellNode] += nodeLHS;
    rhsVector[wellNode] += nodeRHS;
  }
}

void SingleWell2DUnconfined::SetExternalBC()
{
  if (!control.ExtBoundIsFixedP())
    return;

  int stSoln = ntotalSoln - 1 - unc_nverticalNodes;

  if (unc_nverticalNodes == 1)
  {
    rhsVector[stSoln] -= externalBoundaryTerm * saturatedThickness;
  }
  else
  {
    variableBasicB = (saturatedThickness - constantNodeThickness) / double(nverticalVariable);
    for (int i = 0; i < unc_nverticalNodes; i++)
    {
      // same logic as before
      double nextB;
      GetVerticalEdgeLengths(i, nextB);
      // special case for top node - contribution from water table
      if (i == topVertIndex)
        nextB += variableBasicB * 0.5;

      rhsVector[stSoln++] -= externalBoundaryTerm * nextB;
    }
  }
}


void SingleWell2DUnconfined::UpdatePressures()
{
  int solnIndx = 0;
  double minThickness = constantNodeThickness + 0.001;

  // keep last well thickness for flow calcs
  lastWellThick = currSatThick[0];

  for (int i = 0; i < npartialEquations; i++)
  {
    if (currentSeqIsFixed && (i == 0))
    {
      for (int j = 0; j < unc_nverticalNodes; j++)
        if ((j < firstWellNode) || (j > lastWellNode))
          nodePressure[j][0] = solutionVector[solnIndx++];
    }
    else
      for (int j = 0; j < unc_nverticalNodes; j++)
        nodePressure[j][i] = solutionVector[solnIndx++];
    double nextB = solutionVector[solnIndx++];
    // bad things happen if thickness goes -ve ...
    if (nextB < minThickness)
    {
      if (!allowNegativePressure)
        throw SimError("minimum unconfined thickness reached - reduce pumping rate", SimError::seSemiFatal);

      nextB = minThickness;
    }
    currSatThick[i] = nextB;
  }

  if (currentSeqIsFixed)
  {
    for (int j = firstWellNode; j <= lastWellNode; j++)
      nodePressure[j][0] = fixedTZPressure;
  }
  else
  {
    currSeqTZ.tzPressure = nodePressure[wellNode][0];
  }

}

void SingleWell2DUnconfined::CalculateFlows()
{
  //  = radial term + well above + well below + storage at node
  // need to find area and vert distance terms

  // base on thickness at start of time step
  variableBasicB = (lastWellThick - constantNodeThickness) / double(nverticalVariable);

  double flowVertUp = 0.0;
  double flowVertDown = 0.0;
  double flowRadial = 0.0;
  double flowStorage = 0.0;

  if (control.IsUniformWellModel())
  {
    if (lastWellNode < (unc_nverticalNodes - 1))
    {
      double lengthAbove, lengthBelow;
      GetVerticalEdgeLengths(lastWellNode, lengthAbove, lengthBelow);
      flowVertUp = qvTerm[0] / lengthAbove
                   * (nodePressure[lastWellNode][0] - nodePressure[lastWellNode + 1][0]);
    }
    else
    {
      // flow to water table - delta H is head
      // top node will always be in variable zone, so can be set here for thickness
      double vbTerm = qvTerm[0] / (variableBasicB * kpaConst);
      double lastWellHead = nodePressure[lastWellNode][0] * kpaConst;
      flowVertUp = vbTerm * (lastWellHead - currSatThick[0]);
    }

    if (firstWellNode > 0)
    {
      double lengthAbove, lengthBelow;
      GetVerticalEdgeLengths(firstWellNode, lengthAbove, lengthBelow);
      flowVertDown = qvTerm[0] / lengthBelow
                     * (nodePressure[firstWellNode][0] - nodePressure[firstWellNode - 1][0]);
    }

    double dArea;
    for (int j = firstWellNode; j <= lastWellNode; j++)
    {
      GetVerticalEdgeLengths(j, dArea);
      if (j == lastWellNode)
        dArea += variableBasicB * 0.5;

      flowRadial += dTerm[0] * dArea
                    * (nodePressure[j][0] - nodePressure[j][1]);

      flowStorage += sTerm[0] / deltaT * dArea
                     * (nodePressure[j][0] - lastWellPressure[j]);
    }
  }
  else
  {
    double lengthAbove, lengthBelow;
    GetVerticalEdgeLengths(wellNode, lengthAbove, lengthBelow);

    if (unc_nverticalNodes > 1)
    {
      flowVertUp = (verticalWellTerm + qvTerm[0]) / lengthAbove
                   * (currSeqTZ.tzPressure - nodePressure[wellNode + 1][0]);
      if (wellNode > 0)
      {
        // not through well edge
        flowVertDown = qvTerm[0] / lengthBelow
                       * (currSeqTZ.tzPressure - nodePressure[wellNode - 1][0]);
      }
    }
    double dArea  = (lengthAbove + lengthBelow)/ 2.0;

    flowRadial = dTerm[0] * dArea
                 * (currSeqTZ.tzPressure - nodePressure[wellNode][1]);

    flowStorage = sTerm[0] / deltaT * dArea
                  * (currSeqTZ.tzPressure - lastSeqTZ.tzPressure);
  }


  currSeqTZ.formFlow  = flowVertUp + flowVertDown + flowRadial + flowStorage;
}

void SingleWell2DUnconfined::GetVerticalEdgeLengths(int    vertNodeIndx,
    double& aboveLength,
    double& belowLength)
{
  aboveLength = variableBasicB;
  belowLength = aboveLength;
  if (vertNodeIndx == 0)
  {
    belowLength = 0.0;
    // first vertical node
    if (nverticalConstant > 0)
      aboveLength = constantNodeDeltaZ[0];
  }
  else if (vertNodeIndx < nverticalConstant)
  {
    aboveLength = constantNodeDeltaZ[vertNodeIndx];
    belowLength = constantNodeDeltaZ[vertNodeIndx - 1];
  }
  else if (vertNodeIndx == nverticalConstant)
  {
    belowLength = constantNodeDeltaZ[vertNodeIndx - 1];
  }
}

void SingleWell2DUnconfined::GetVerticalEdgeLengths(int    vertNodeIndx,
    double& nextB)
{
  double aboveLength, belowLength;
  GetVerticalEdgeLengths(vertNodeIndx, aboveLength, belowLength);
  nextB = (aboveLength + belowLength) / 2.0;
}


void SingleWell2DUnconfined::SaveCurrentPressure()
{
  SingleWell2D::SaveCurrentPressure();
  oldcurrSatThick = currSatThick;
  oldlastSatThick = lastSatThick;
}

void SingleWell2DUnconfined::RestoreCurrentPressure()
{
  SingleWell2D::RestoreCurrentPressure();
  currSatThick = oldcurrSatThick;
  lastSatThick = oldlastSatThick;
}


