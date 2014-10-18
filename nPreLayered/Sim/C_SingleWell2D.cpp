///////////////////////////////////////////////////////////////////////////////////
//
// C_SingleWell2D.cpp
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
#include <sstream>

#include <genClass/U_Str.h>

#include <nsClass/nSightConst.h>

#include <osClass/C_SimErr.h>

#include <Var/G_Parameter.h>
#include <Var/E_Parameter.h>
#include <Var/G_Control.h>

#include "C_SingleWell2D.h"

#ifdef LAPACK
#include <genClass/U_Lapack.h>
#endif // LAPACK

SingleWell2D::SingleWell2D()
{
}
SingleWell2D::~SingleWell2D()
{
}

bool SingleWell2D::SetupForRun(SC_SetupErr&   errData)
{
  if (!SingleWellSimulator::SetupForRun(errData))
    return false;

  // required to set nvertical nodes
  Calc2DVerticalGeometry();

  return true;
}



void SingleWell2D::DoAllocateAll()
{
  SingleWellSimulator::DoAllocateAll();

  int maxradialNodes = allParam[pN_n_r]->GetIntMaxPar();
  int maxverticalNodes = allParam[pN_n_v2Dw]->GetIntMaxPar() + 1;
  if (control.IsPartialPenetration())
    maxverticalNodes += allParam[pN_n_v2Du]->GetIntMaxPar() +
                        allParam[pN_n_v2Dl]->GetIntMaxPar();

  nodePressure.FixedMatrixAlloc(maxverticalNodes, maxradialNodes);

  if (saveOldPressures)
    oldNodePressure.FixedMatrixAlloc(maxverticalNodes, maxradialNodes);

  constantNodeZ.Alloc(maxverticalNodes);
  constantNodeDeltaZ.Alloc(maxverticalNodes);
  lastWellPressure.Alloc(maxverticalNodes);
  isWellNode.Alloc(maxverticalNodes + 1);

  int maxTotalNodes = maxradialNodes * (maxverticalNodes + 1);
  matA.FixedMatrixAlloc((maxverticalNodes + 1) * 2 + 1, maxTotalNodes);
#ifdef LAPACK
  ipiv.Alloc(maxTotalNodes);
#else //LAPACK
  matA1.FixedMatrixAlloc(maxverticalNodes + 1, maxTotalNodes);
  matSolveIndx.Alloc(maxTotalNodes);
#endif // LAPACK
  solutionVector.Alloc(maxTotalNodes);
  rhsVector.Alloc(maxTotalNodes);

  qvTerm.Alloc(maxradialNodes);
  dTerm.Alloc(maxradialNodes);
  sTerm.Alloc(maxradialNodes);


}

void SingleWell2D::SetRuntimeSize()
{
  SingleWellSimulator::SetRuntimeSize();

  int nverticalMatSize = nverticalNodes;
//   #ifndef LAPACK
  if (control.IsUnconfined())
    nverticalMatSize++;  // extra for top surface
  int ntotalNodes = nradialNodes * nverticalMatSize;
//   #else // !LAPACK
//    int ntotalNodes = (nradialNodes-1) * nverticalMatSize;
//   #endif // !LAPACK

#ifndef LAPACK
  matA.MatrixSetSize(nverticalMatSize * 2 + 1, ntotalNodes);
  matA1.MatrixSetSize(nverticalMatSize, ntotalNodes);
  matSolveIndx.SetSize(ntotalNodes);
#else // !LAPACK
  ipiv.SetSize(ntotalNodes);
  if (control.IsUnconfined())
    matA.MatrixSetSize((nverticalMatSize-1) * 2 + 1, (nradialNodes)*(nverticalMatSize-1));
  else
    matA.MatrixSetSize(nverticalMatSize * 2 + 1, ntotalNodes);
#endif // !LAPACK
  solutionVector.SetSize(ntotalNodes);
  rhsVector.SetSize(ntotalNodes);
  qvTerm.SetSize(nradialNodes);
  dTerm.SetSize(nradialNodes);
  sTerm.SetSize(nradialNodes);

}


void SingleWell2D::SetRunParameters()
{
  SingleWellSimulator::SetRunParameters();

  nodePressure.MatrixFill(staticPressure);
  lastWellPressure.FillToAlloc(staticPressure);

  if (control.IsAnalyticWellModel())
  {
    double rWell    = wPar[pTZ_r_w].GetData();
    double kwellVertical = rWell * rWell  * gravConst * wPar.GetFluidDensity() / (8.0 * 0.001);
    double wellArea = rWell * rWell * pi;
    baseVerticalWellTerm = kwellVertical * wellArea;
  }
  else if (control.IsMultiplierWellModel())
  {
    baseVerticalWellTerm = wPar[pN_kVMult].GetData();
  }
  else
  {
    baseVerticalWellTerm = 0.0;
  }

  for (int j = 0; j < nradialNodes; j++)
  {
    const ParameterGroups& currRock = (*rockMap[j]);

    double dterm;
    if (enterPermeabilityData)
      dterm = GetRockPar(currRock.kIndx, j) /
              GetFluidPar(pFl_mu);
    else
      dterm = GetRockPar(currRock.hkIndx, j) /
              (gravConst * GetFluidPar(pFl_rho));

    if (j < (nradialNodes - 1))
    {
      /*dTerm[j] =  dterm * (nodeRadii[j] + nodeRadii[j + 1]) * pi * kpaToPa
              / (nodeRadii[j + 1] - nodeRadii[j]);*/
      dTerm[j] =  dterm * nodeFlowArea[j] * kpaToPa
                  / (nodeRadii[j + 1] - nodeRadii[j]);
    }


    double vkterm = 0.0;
    if (control.HasSkin() && (j == fracStartNode))
    {
      if (control.IsVerticalAnisotropic())
      {
        if (enterPermeabilityData)
          vkterm = GetRockPar(skinParGroup.kvIndx, j) / GetFluidPar(pFl_mu);
        else
          vkterm = GetRockPar(skinParGroup.hkvIndx, j) /
                   (gravConst * GetFluidPar(pFl_rho));
      }
      else
      {
        if (enterPermeabilityData)
          vkterm = GetRockPar(skinParGroup.kIndx, j) /
                   GetFluidPar(pFl_mu);
        else
          vkterm = GetRockPar(skinParGroup.hkIndx, j) /
                   (gravConst * GetFluidPar(pFl_rho));
      }
      vkterm *= lastSkinArea;
    }


    double vterm;
    if (control.IsVerticalAnisotropic())
    {
      if (enterPermeabilityData)
        vterm = GetRockPar(currRock.kvIndx, j) / GetFluidPar(pFl_mu);
      else
        vterm = GetRockPar(currRock.hkvIndx, j) /
                (gravConst * GetFluidPar(pFl_rho));
    }
    else
    {
      vterm = dterm;
    }

    qvTerm[j] = (vkterm + vterm * nodePlanArea[j]) * kpaToPa;


    double skTerm = 0.0;
    if (control.HasSkin() && (j == fracStartNode))
    {
      if (enterSpecificStorageData)
        skTerm = GetRockPar(skinParGroup.ssIndx, j)
                 / (gravConst * GetFluidPar(pFl_rho));
      else
        skTerm = GetRockPar(skinParGroup.cIndx, j)
                 +  GetRockPar(skinParGroup.phiIndx, j) *
                 GetFluidPar(pFl_C);
      skTerm *= lastSkinArea;
    }

    double sterm;
    if (enterSpecificStorageData)
      sterm = GetRockPar(currRock.ssIndx, j)
              / (gravConst * GetFluidPar(pFl_rho));
    else
      sterm = GetRockPar(currRock.cIndx, j)
              +  GetRockPar(currRock.phiIndx, j) *
              GetFluidPar(pFl_C);

    sTerm[j] = (skTerm + sterm * nodePlanArea[j]) * kpaToPa;
  }

  dTerm.LastIndex() = 0.0;

  if (control.ExtBoundIsFixedP())
  {
    externalBoundaryTerm = -dTerm[nradialNodes - 2];
    externalBoundaryTerm *= staticPressure;
  }



}

void SingleWell2D::SaveCurrentPressure()
{
  oldNodePressure = nodePressure;
}

void SingleWell2D::RestoreCurrentPressure()
{
  nodePressure  = oldNodePressure;
}

void SingleWell2D::AddConstantVerticalNodes(int     nnode,
    const double&  interval)
{
  // assume node at start of interval already set and not counted in nnode
  double dInt = interval/ double(nnode);
  double startZ = constantNodeZ.LastIndex();
  double currInt =  startZ + dInt;
  for (int i = 1; i < nnode; i++)
  {
    constantNodeZ += currInt;
    constantNodeDeltaZ += dInt;
    currInt += dInt;
  }
  // avoid multiple round-offs
  constantNodeZ += startZ + interval;
  constantNodeDeltaZ += dInt;
}


void SingleWell2D::Calc2DVerticalGeometry()
{
  constantNodeZ.SetEmpty();
  constantNodeDeltaZ.SetEmpty();

  double formThick = wPar[pF_t].GetData();
  nwellNodes = wPar[pN_n_v2Dw].GetIntData();

  int nabove = 0;
  int nbelow = 0;

  // bottom is always constant
  constantNodeZ += 0.0;

  if (!control.IsPartialPenetration())
  {
    // if confined then so is rest
    if (control.IsConfined())
      AddConstantVerticalNodes(nwellNodes - 1, formThick);
  }
  else
  {
    const double minThick = 0.001;

    double wellDist = wPar[pTZ_pl].GetData();

    if (control.IsBottomPartialPenetration())
    {
      // add well
      AddConstantVerticalNodes(nwellNodes - 1, wellDist);

      nabove = wPar[pN_n_v2Du].GetIntData();
      nverticalNodes += nabove;

      if (control.IsConfined())
      {
        // and nodes above well
        double aboveDist = formThick - wellDist;
        AddConstantVerticalNodes(nabove, aboveDist);
      }
    }
    else if (control.IsMiddlePartialPenetration())
    {
      nbelow = wPar[pN_n_v2Dl].GetIntData();
      double belowDist = wPar[pTZ_pbo].GetData();


      // below nodes
      AddConstantVerticalNodes(nbelow, belowDist);

      // add well
      AddConstantVerticalNodes(nwellNodes - 1, wellDist);

      nabove = wPar[pN_n_v2Du].GetIntData();
      if (control.IsConfined())
      {
        // and nodes above well
        double aboveDist = formThick - wellDist - belowDist;
        AddConstantVerticalNodes(nabove, aboveDist);
      }
    }
    else if (control.IsTopPartialPenetration())
    {
      nbelow = wPar[pN_n_v2Dl].GetIntData();
      double belowDist = formThick - wellDist;
      AddConstantVerticalNodes(nbelow, belowDist);
      if (control.IsConfined())
      {
        // and well nodes
        AddConstantVerticalNodes(nwellNodes - 1, wellDist);
      }
    }
  }

  nverticalNodes = nwellNodes + nabove + nbelow;

  isWellNode.Alloc(nverticalNodes);
  isWellNode.FillToAlloc(false);

  firstWellNode = nbelow;
  lastWellNode = nbelow;
  wellNode = nbelow;
  for (int i = wellNode; i < wellNode + nwellNodes; i++)
    isWellNode[i] = true;

  if (control.IsUniformWellModel())
  {
    firstWellNode = nbelow;
    lastWellNode = nbelow + nwellNodes - 1;
    wellNode = (firstWellNode + lastWellNode) / 2;
  }

}


void SingleWell2D::SequenceSetup()
{
  if (!currentSeqIsFixed)
  {
    lastSeqTZ.tzPressure = currSeqTZ.tzPressure;
    for (int j = firstWellNode; j <= lastWellNode; j++)
    {
      lastWellPressure[j] = currSeqTZ.tzPressure;
      nodePressure[j][0] = currSeqTZ.tzPressure;
    }
  }
}

void SingleWell2D::SetupForSolve()
{
  matA.MatrixFill(0.0);
  rhsVector.FillToAlloc(0.0);

  fixedTZPressure = currSeqTZ.tzPressure;

  // global nEquations is wrong for 2D case ...
  npartialEquations = nradialNodes;
  if (control.ExtBoundIsFixedP())
    npartialEquations--;

}

void SingleWell2D::UpdateTZData()
{
  SingleWellSimulator::UpdateTZData();
  for (int j = firstWellNode; j <= lastWellNode; j++)
    lastWellPressure[j] = nodePressure[j][0];
}

void SingleWell2D::InitializeTZData()
{
  SingleWellSimulator::InitializeTZData();
  for (int j = firstWellNode; j <= lastWellNode; j++)
    lastWellPressure[j] = currSeqTZ.tzPressure;
}

//TxtFileC debugF("D:/nSights/Matrix Solvers/test_unconfMatrix.out", false);
//TxtFileC debugIter("D:/nSights/Matrix Solvers/test_iter.out", false);

void SingleWell2D::Solve2D(int neqn, int nvert)
{
  int d = 1;  //variable not used here

  //int numCompactCols = nvert + nvert + 1;
  //for (int i = 0; i < neqn; i++)  //loop over rows
  //{
  //  int kk = i - nvert;
  //  int minLoop = IMax(0, -kk);
  //  int maxLoop = IMin(numCompactCols, neqn - kk) - 1;
  //  for (int j = 0; j < numCompactCols; j++)
  //  {
  //    if ((j < minLoop) || (j > maxLoop))
  //      continue;
  //    debugF.WriteInt(i);
  //    debugF.WriteInt(j);
  //    debugF.WriteDouble(matA[j][i]);
  //    debugF.WriteLine();
  //  }
  //}

  try
  {
#ifdef LAPACK

    //Solves Ax=b for KL sub- and KU super-diagonals
    int nrhs = 1;                       //the number of right hand sides (number of columns of b)
    int ldab = 2*nvert+nvert+1;               //leading dimension of AB
    int ldb = neqn;                        //leading dimension of RHS
//        double *B = new double[ldb*nrhs];   //RHS
//        double *AB = new double[ldab*n];    //matrix A in banded form in rows KL+1 to 2*KL+KU+1
//        int *ipiv2 = new int[neqn];
    int info;

//        SC_LapackBandedMatrix C(nvert*2+1, n);

    //On entry, the matrix A in band storage, in columns KL+1 to
    //  2*KL+KU+1; olumns 1 to KL of the array need not be set.
    //  The j-th row of A is stored in the j-th row of the
    //  array AB as follows:
    //  AB(j,KL+KU+1+i-j) = A(i,j) for max(1,j-KU)<=i<=min(N,j+KL)

    //Currently, matA is in  the transpose of the fortran Numerical recipes compact form
    // which is a shifted transpose of LAPACK's form
    //TODO: this is temporary.  Because of the way memory is being allocated, it is difficult to copy straight
    //from the matA buffer to the AB type.  Eventually, matA will be built up correctly the first time.
    /*
            for(int j = 0; j < n; j++)
            {
                B[j]=rhsVector[j];
            }

            //move diagonal over
            for(int i = 0; i < n; i++)
            {
                int indx = i*ldab+kl+ku;
                if((indx > ldab*n-1) || (indx < 0))
                {
                    throw SimError("Unable to solve matrix by banded solver", SimError::seSemiFatal);
                }
                AB[i*ldab+kl+ku]=matA[kl][i];
    //            C[kl][i]=matA[kl][i];
            }
            //super diagonals
            for(int i = 0; i < ku; i++)
            {
                for(int j = 0; j < n-i-1; j++)
                {
                    AB[kl+ku-i-1+(j+i+1)*ldab]=matA[kl+i+1][j];
    //                C[kl+i+1][j]=matA[kl+i+1][j];
                }
            }
            //sub diagonals
            for(int i = 0; i < kl; i++)
            {
                for(int j = 0; j < n-i-1; j++)
                {
                    AB[kl+ku+1+i+j*ldab]=matA[kl-i-1][j+i+1];
    //                C[kl-i-1][j+i+1]=matA[kl-i-1][j+i+1];
                }
            }
    */
    dgbsv_(&neqn, &nvert, &nvert, &nrhs, matA.buffer, &ldab, ipiv.tListData, rhsVector.tListData, &ldb, &info);
    //dgbsv_(&n, &kl, &ku, &nrhs, AB, &ldab, ipiv, B, &ldb, &info);

    if(info != 0)
    {
      throw SimError("Unable to solve matrix by banded solver", SimError::seSemiFatal);
    }

    //copy results back over
    for(int i = 0; i < neqn; i++)
    {
      solutionVector[i]=rhsVector[i];
    }
    /*
            delete [] AB;
            delete [] ipiv2;
            delete [] B;
    */
#else //LAPACK
    if (matrixSolver == msBanded)
    {
//  DWORD s0 = GetTickCount();

      Bandec(matA, neqn, nvert, nvert, matA1, matSolveIndx, d);

//  DWORD s1 = GetTickCount();

      Banbks(matA, neqn, nvert, nvert, matA1, matSolveIndx, rhsVector, solutionVector);

      /*  DWORD s2 = GetTickCount();

        debugF.WriteInt(s0);
        debugF.WriteInt(s1 - s0);
        debugF.WriteInt(s2 - s1);
        debugF.WriteInt(s2);
        debugF.WriteLine();
      */
    }
    else
    {
      //update value
      conjugateVar.n = neqn;
      conjugateVar.numCols = neqn;
      conjugateVar.symmetric = true;
      conjugateVar.reOrder = reorder;

//      DWORD s0 = GetTickCount();
      //fill matrix into sparse form
      if (!Sparse(nvert))
      {
        if ((matrixSolver == msCholesky) || (control.IsConjugateSolver() && solverPreconditioner != spSimple))
          throw SimError("Matrix not symmetric, use another solver", SimError::seSemiFatal);

        conjugateVar.symmetric = false;
        SparseUnSymmetric(neqn, nvert);
      }
      //      DWORD s1 = GetTickCount();

      if (matrixSolver == msCholesky)
      {
        if (!conjugateVar.IsDiagDominant(matA))
          throw SimError("non-positive definite, use another solver", SimError::seSemiFatal);
        conjugateVar.SolveCholesky(solutionVector, rhsVector);
      }
      else
      {
        //temporary intialization of vars for Vaidya
        //conjugateVar.forceDiagDom = false;
        //conjugateVar.stretch_flag = false;  //false;
        //conjugateVar.rnd = 170566;
        //conjugateVar.subgraphs = 1.0;
        conjugateVar.tol = wPar[pN_tol].GetData();

        int i = 1;
        double err = 1.0E30;
        int count = 0;
        while ((i == 1) && (count < 10))
        {
          i = 0;
          err = 1.0E30;

          if (count > 0)
            conjugateVar.tol /= 10.0;

          //solutionVector.FillToSize(0.0);
          count++;
          //      DWORD s2;
          if (matrixSolver == msBiconjugate)
          {
            //for Vaidya
            /*conjugateVar.forceDiagDom = false;
            if (solverPreconditioner == spVaidya)
              if (!conjugateVar.IsDiagDominant(matA))
                conjugateVar.forceDiagDom = true;*/

            conjugateVar.BiConjugateGradient(solutionVector, rhsVector, i, err);
          }
          else if (matrixSolver == msConjugate)
          {
            if (count == 0)
            {
              if (!conjugateVar.IsDiagDominant(matA))
                throw SimError("non-positive definite, use another solver", SimError::seSemiFatal);
            }

            //        s2 = GetTickCount();

            conjugateVar.ConjugateGradient(solutionVector, rhsVector, i, err);
          }
        }

        //      DWORD s3 = GetTickCount();

        if (i > conjugateVar.itmax)
        {
          using namespace std;
          ostringstream ostr;
          ostr.setf(ios::scientific, ios::floatfield);
          ostr << " Matrix solver - no converg tol " << conjugateVar.tol << " err " << err << " # iter " << i << " #loops " << count << ends;
          throw SimError(ostr.str().c_str());
        }
        else
        {
          using namespace std;
          ostringstream ostr;
          ostr.setf(ios::scientific, ios::floatfield);
          ostr << " iter # " << i << " tol " << err << ends;
          GenAppInfoMsg("Matrix Solver", ostr.str().c_str());
        }
      }
    }
#endif //LAPACK

  }
  catch (MathError& merr)
  {
    char errStr[80];
    MakeString(errStr, "2DSolveError:", merr.errStr, 80);
    throw SimError(errStr, SimError::seSemiFatal);
  }

}



