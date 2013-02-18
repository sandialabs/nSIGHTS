///////////////////////////////////////////////////////////////////////////////////
//
// C_SingleWell1DNonLinear.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <sstream>

#include <genClass/U_Real.h>

#ifdef LAPACK
#include <genClass/U_Lapack.h>
#endif // LAPACK

#include <nsClass/nSightConst.h>
#include <osClass/C_SimErr.h>

#include <Var/G_Control.h>
#include <Var/G_Parameter.h>
#include <Var/E_Parameter.h>

#include "G_SimulatorRadiiOutput.h"

#include "C_SingleWell1DNonLinear.h"


SingleWell1DNonLinear::SingleWell1DNonLinear()
{
}
SingleWell1DNonLinear::~SingleWell1DNonLinear()
{
}

bool SingleWell1DNonLinear::SetupForRun(SC_SetupErr&    errData)
{
    if (!SingleWell1D::SetupForRun(errData))
        return false;
     return true;
}

void SingleWell1DNonLinear::DoAllocateAll()
{
    SingleWell1D::DoAllocateAll();

    int maxradialNodes = allParam[pN_n_r]->GetIntMaxPar();

    nrRhs.Alloc(maxradialNodes);
    nrDx.Alloc(maxradialNodes);
    nrNewX.Alloc(maxradialNodes);
    nrWork.Alloc(maxradialNodes);
    nrInitRhs.Alloc(maxradialNodes);
    nrMat.FixedMatrixAlloc(3, maxradialNodes);

    dLinearTerm.Alloc(maxradialNodes);
    sLinearTerm.Alloc(maxradialNodes);
    dTermIsLinear.Alloc(maxradialNodes);
    sTermIsLinear.Alloc(maxradialNodes);
}


void SingleWell1DNonLinear::SetRuntimeSize()
{
    SingleWell1D::SetRuntimeSize();

    nrRhs.SetSize(nradialNodes);
    nrDx.SetSize(nradialNodes);
    nrNewX.SetSize(nradialNodes);
    nrWork.SetSize(nradialNodes);
    nrInitRhs.SetSize(nradialNodes);
    nrMat.MatrixSetSize(3, nradialNodes);

    dLinearTerm.SetSize(nradialNodes);
    sLinearTerm.SetSize(nradialNodes);
    dTermIsLinear.SetSize(nradialNodes);
    sTermIsLinear.SetSize(nradialNodes);
}


void SingleWell1DNonLinear::SetRunParameters()
{
    SingleWell1D::SetRunParameters();

    nrTolP = wPar[pN_P_tol].GetData();

}

void SingleWell1DNonLinear::SetDSTerms()
{
    // set d & s data and non linear flags
    for (int j = 0; j < nradialNodes; j++)
    {
        lastWasNonLinear = false;
        dTermIsLinear[j] = false;
        dLinearTerm[j]   = NLGetDTerm(j, 0.0, 0.0);
        dTermIsLinear[j] = !lastWasNonLinear;

        lastWasNonLinear = false;
        sTermIsLinear[j] = false;
        sLinearTerm[j]   = NLGetSTerm(j, 0.0);
        sTermIsLinear[j] = !lastWasNonLinear;
    }
}
// just used for factor calcs

double SingleWell1DNonLinear::GetRockPar(int    parIndx,
                                           int  nodeNum)
{
    return NLGetRockPar(parIndx, nodeNum, staticPressure);
}

double SingleWell1DNonLinear::GetFluidPar(int parIndx)
{
    return NLGetFluidPar(parIndx, staticPressure);
}


double  SingleWell1DNonLinear::NLGetRockPar(int         parIndx,
                                            int         nodeNum,
                                            const double&   pVal)
{
    lastWasNonLinear = lastWasNonLinear || wPar[parIndx].IsFP();
    return wPar[parIndx].GetData(nodeNum, pVal);
}

double SingleWell1DNonLinear::NLGetFluidPar(int         parIndx,
                                                    const double&   pVal)
{
    lastWasNonLinear = lastWasNonLinear || wPar[parIndx].IsFP();
    return wPar[parIndx].GetData(pVal);
}


double  SingleWell1DNonLinear::NLCalcSTerm(    int      nodeNum,
                                         const double&  pN)
{
    return NLGetSTerm(nodeNum, pN) * pN / deltaT;
}


double SingleWell1DNonLinear::NLWellDTerm(const double& nextP)
{
    return NLCalcDTerm(0, NLGetWellPressure(), nextP);
}

double SingleWell1DNonLinear::NLExternalDTerm(const double& nodeN_1P)
{
    return NLCalcDTerm(nradialNodes - 1, nodeN_1P, NLGetExternalPressure());
}


// onlinear partial calculations

double  SingleWell1DNonLinear::NLCalcFJ(      int       eqnNum,
                                        const double&   p_i,
                                        const double&   p_j,
                                        const double&   p_k)
{
    int nodeNum = eqnNum;
    if (currentSeqIsFixed)
        nodeNum++;

    double dij;
    if (eqnNum == 0)
        dij = 0.0;
    else
        dij = NLCalcDTerm(nodeNum - 1, p_i, p_j);

    double sj = NLCalcSTerm(nodeNum, p_j);
    if (nodeNum == 0)
    {
        sj += GetWellBCLhs() * p_j;
    }


    double djk;
    if (eqnNum == (nequations - 1))
        djk = 0.0;
    else
        djk = NLCalcDTerm(nodeNum, p_j, p_k);

    return -dij + sj + djk;

}

void SingleWell1DNonLinear::SetDp(const double&     p,
                                  const double&     delP,
                                  const double&     pSum,
                                        double&     pf1,
                                        double&     pf2,
                                        double&     dpf)
{
    double dpTemp = delP;
    if (dpTemp > pSum)
        dpTemp = pSum * 0.1;
    if (dpTemp < 1.0)
        dpTemp = 1.0;
    pf1 = p - dpTemp;
    pf2 = p + dpTemp;
    dpf = dpTemp + dpTemp;
}



double SingleWell1DNonLinear::AnalyticPartialD(   int       nodeNum,
                                                 const double&  pN)  // OK for liquid - override for gas
{
    if (!dTermIsLinear[nodeNum])
        GenAppInternalError("SingleWell1DNonLinear::AnalyticPartialD");

    return dLinearTerm[nodeNum];
}


void SingleWell1DNonLinear::NLSetPartials()
{
    SC_DoubleArray& a = nrMat[0];
    SC_DoubleArray& b = nrMat[1];
    SC_DoubleArray& c = nrMat[2];

    double minP = nrNewX[0];
    double maxP = minP;
    for (int j = 0; j < nequations; j++ )
    {
        if (nrNewX[j] > maxP)
            maxP = nrNewX[j];
        else
            if (nrNewX[j] < minP)
                minP = nrNewX[j];
        nrRhs[j] = nrInitRhs[j];
    }

    if (currentSeqIsFixed)
        nrRhs[0] += NLWellDTerm(nrNewX[0]);
    if (control.ExtBoundIsFixedP())
        nrRhs[nequations - 1] -= NLExternalDTerm(nrNewX[nequations - 1]);

    double delP = (maxP - minP) * 0.01;
    if (delP < 1.0)
        delP = 1.0;

    double p1, p2, p3, pSum;

    bool prevDLin   = false;
    p1 = 0.0;

    for (int j = 0; j < nequations; j++ )
    {
        int prevNodeNum = j - 1;
        int currNodeNum = j;
        if (currentSeqIsFixed)
        {
            prevNodeNum++;
            currNodeNum++;
        }

        if (j > 0)
        {
            p1 = nrNewX[j - 1];
            prevDLin = dTermIsLinear[prevNodeNum];
        }

        p2 = nrNewX[j];
        bool currDLin = dTermIsLinear[currNodeNum];

        if (j < (nequations - 1))
            p3 = nrNewX[j + 1];
        else
            p3 = 0.0;

        pSum = p1 + p2 + p3;

        if (j == 0)
        {
            a[j] = 0.0;
        }
        else
            if (prevDLin)
            {
                a[j] = - AnalyticPartialD(prevNodeNum, p1);
            }
            else
            {
                double pf1, pf2, dpf;
                SetDp(p1, delP, pSum, pf1, pf2, dpf);
                double f1 = NLCalcFJ(j, pf1, p2, p3);
                double f2 = NLCalcFJ(j, pf2, p2, p3);
                a[j]  = (f2 - f1) / dpf;
            }


        if ((currNodeNum > 0) && prevDLin && currDLin && sTermIsLinear[currNodeNum])
        {
            b[j]  =  AnalyticPartialD(prevNodeNum, p2) + AnalyticPartialD(currNodeNum, p2);
            b[j] +=  sLinearTerm[currNodeNum] / deltaT;
        }
        else
        {
            double pf1, pf2, dpf;
            SetDp(p2, delP, pSum, pf1, pf2, dpf);
            double f1 = NLCalcFJ(j, p1, pf1, p3);
            double f2 = NLCalcFJ(j, p1, pf2, p3);

            if ((j == 0) && currentSeqIsFixed)
            {
                f1 = f1 - NLWellDTerm(pf1);
                f2 = f2 - NLWellDTerm(pf2);
            }

            if ((j == (nequations - 1)) && control.ExtBoundIsFixedP())
            {
                f1 = f1 + NLExternalDTerm(pf1);
                f2 = f2 + NLExternalDTerm(pf2);
            }
            b[j] = (f2 - f1) / dpf;
        }


        if (j == (nequations - 1))
        {
            c[j] = 0.0;
        }
        else
            if (currDLin)
            {
                c[j] = - AnalyticPartialD(currNodeNum, p3);
            }
            else
            {
                double pf1, pf2, dpf;
                SetDp(p3, delP, pSum, pf1, pf2, dpf);
                double f1 = NLCalcFJ(j, p1, p2, pf1);
                double f2 = NLCalcFJ(j, p1, p2, pf2);
                c[j] = (f2 - f1) / dpf;
            }

        nrRhs[j] -= NLCalcFJ(j, p1, p2, p3);

    }
}


void SingleWell1DNonLinear::NonlinearMatSolve()
{
    static const int maxIter = 500;

    int nodeNum = 0;
    if (currentSeqIsFixed)
        nodeNum++;

    for (int j = 0; j < nequations; j++)
    {
        nrInitRhs[j] = NLCalcSTerm(nodeNum, nodePressure[nodeNum]);
        nrNewX[j] = nodePressure[nodeNum];
        nodeNum++;
    }

    if (!currentSeqIsFixed)
        nrInitRhs[0] += GetWellBCRhs();

    for (int i = 0; i <= maxIter; i++)
    {
        nnrIter = i;
        NLSetPartials();
        double errQ = 0.0;
        for (int j = 0; j < nequations; j++)
            errQ += fabs(nrRhs[j]);
        if (errQ < nrTolQ)
            break;

#ifdef LAPACK
        //call general tri-diagonal lapack solver

        int N = nequations;
        int nrhs = 1;
        double *DL = new double[N-1];
        double *D = new double[N];
        double *DU = new double[N-1];
        double *B = new double[N];

        for(int j = 1; j < N; j++)
        {
            DL[j-1]=nrMat[0][j];
            DU[j-1]=nrMat[2][j];
            D[j]=nrMat[1][j];
            B[j]=nrRhs[j];
        }
        D[0]=nrMat[1][0];
        B[0]=nrRhs[0];
        int ldb = N;
        int info;

        dgtsv_(&N, &nrhs, DL, D, DU, B, &ldb, &info);

        if(info == 0)
        {
            for(int j = 0; j < N; j++)
            {
                nrDx[j]=B[j];
            }
        }
        else //info != 0
        {
            GenAppInternalError("SingleWell1DNonLinear::NonlinearMatSolve - lapack tri-diag solve failed");
        }

        delete [] DL;
        delete [] DU;
        delete [] D;
        delete [] B;

#else //LAPACK
        TriDiag(nrMat[0], nrMat[1], nrMat[2], nrRhs, nrDx, nrWork, nequations);
#endif //LAPACK
        double errP = 0.0;
        for (int j = 0; j < nequations; j++)
        {
            errP += fabs(nrDx[j]);
            nrNewX[j] += nrDx[j];
        }
        if (errP < nrTolP)
            break;

        if (gasDebugFlag)
        {
            using namespace std;
            ostringstream ostr;
            ostr.setf(ios::scientific, ios::floatfield);
            ostr << "TS " << tstepNum << " iter # " << i << " Ptol " << errP << " Qtol " << errQ << ends;
            GenAppInfoMsg("NRSolver", ostr.str().c_str());
        }

        if (i == maxIter)
        {
            using namespace std;
            ostringstream ostr;
            ostr.setf(ios::scientific, ios::floatfield);
            ostr << " NR - no convergance "  << " Ptol " << errP << " Qtol " << errQ << ends;
            throw SimError(ostr.str().c_str());
        }
    }

    nodeNum = 0;
    if (currentSeqIsFixed)
        nodePressure[nodeNum++] = NLGetWellPressure();

    for (int j = 0; j < nequations; j++)
        nodePressure[nodeNum++] = nrNewX[j];
}


double SingleWell1DNonLinear::NLCalcFormSTerm(const double& pTZ)
{
    bool useLin = sTermIsLinear[0];
    sTermIsLinear[0] = false;
    double temp = NLCalcSTerm(0, pTZ);
    sTermIsLinear[0] = useLin;
    return temp;
}

double SingleWell1DNonLinear::GetPChange()
{
    double minP, maxP;
    nodePressure.CalcMinMax(minP, maxP);
    return maxP - minP;
}


