///////////////////////////////////////////////////////////////////////////////////
//
// C_SingleWell1DLinear.cpp
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
#include <genClass/U_Real.h>
#include <genClass/U_Matrix.h>
#ifdef LAPACK
#include <genClass/U_Lapack.h>
#endif // LAPACK

#include <nsClass/nSightConst.h>
#include <osClass/C_SimErr.h>

#include <Var/G_Control.h>
#include <Var/G_Parameter.h>
#include <Var/G_Sequence.h>
#include <Var/E_Parameter.h>

#include "G_SimulatorRadiiOutput.h"

#include "C_SingleWell1DLinear.h"


SingleWell1DLinear::SingleWell1DLinear()
{
}
SingleWell1DLinear::~SingleWell1DLinear()
{
}

bool SingleWell1DLinear::SetupForRun(SC_SetupErr&   errData)
{
    if (!SingleWell1D::SetupForRun(errData))
        return false;

    return true;
}

void SingleWell1DLinear::DoAllocateAll()
{
    SingleWell1D::DoAllocateAll();

    int maxradialNodes = allParam[pN_n_r]->GetIntMaxPar();

    fracDDiag.Alloc(maxradialNodes);
    fracDUpper.Alloc(maxradialNodes);
    fracSolveDiag.Alloc(maxradialNodes);
    fracSolveUpper.Alloc(maxradialNodes);

    fracSVector.Alloc(maxradialNodes);
    fracSSolve.Alloc(maxradialNodes);
    fracWork.Alloc(maxradialNodes);
    fracRhs.Alloc(maxradialNodes);

    fracSVector.Alloc(maxradialNodes);

    dConstantTerm.Alloc(maxradialNodes);
    sConstantTerm.Alloc(maxradialNodes);
    dTermIsConstant.Alloc(maxradialNodes);
    sTermIsConstant.Alloc(maxradialNodes);

    if (control.IsDual())
        matrix.AllocateAll(maxradialNodes, saveOldPressures);

    if (control.IsLeaky())
    {
        leakArea.Alloc(maxradialNodes);
        if (leakageType == ltUpperLower)
        {
            upperLeak.AllocateAll(maxradialNodes, saveOldPressures);
            lowerLeak.AllocateAll(maxradialNodes, saveOldPressures);
        }
        else
        {
            singleLeak.AllocateAll(maxradialNodes, saveOldPressures);
        }
    }
}

void SingleWell1DLinear::SetRuntimeSize()
{
    SingleWell1D::SetRuntimeSize();

    fracDDiag.SetSize(nradialNodes);
    fracDUpper.SetSize(nradialNodes);
    fracSolveDiag.SetSize(nradialNodes);
    fracSolveUpper.SetSize(nradialNodes);

    fracSVector.SetSize(nradialNodes);
    fracSSolve.SetSize(nradialNodes);
    fracWork.SetSize(nradialNodes);
    fracRhs.SetSize(nradialNodes);

    fracSVector.SetSize(nradialNodes);

    dConstantTerm.SetSize(nradialNodes);
    sConstantTerm.SetSize(nradialNodes);
    dTermIsConstant.SetSize(nradialNodes);
    sTermIsConstant.SetSize(nradialNodes);

    scStartNode = fracStartNode + 1;

    if (control.IsDual())
        matrix.SetRuntimeSize(wPar, nradialNodes);

    if (control.IsLeaky())
    {
        leakArea.SetSize(nradialNodes);
        if (leakageType == ltUpperLower)
        {
            upperLeak.SetRuntimeSize(wPar, nradialNodes);
            lowerLeak.SetRuntimeSize(wPar, nradialNodes);
        }
        else
        {
            singleLeak.SetRuntimeSize(wPar, nradialNodes);
        }
    }

}

void SingleWell1DLinear::SetRunParameters()
{
    SingleWell1D::SetRunParameters();

    double fVolCons = 1.0;
    if (control.IsDual())
    {
        SetRockMap(fracStartNode, nradialNodes, fracParGroup);

        double mVolFactor = wPar[pM_V].GetData();
        fVolCons = 1.0 - mVolFactor;
        for (int j = 0; j < nradialNodes; j++)
        {
            dGeomTerm[j] *= fVolCons;
            sGeomTerm[j] *= fVolCons;
        }

        matrix.ConstantInit(staticPressure, kpaConst);
        matrix.SetParameters(wPar, matParGroup, sGeomTerm);

        // for output calcs only
        double mk     = CalcKTerm(matParGroup);
        double mss    = CalcStoreTerm(matParGroup);
        double malpha;
        if (enterDualAlpha)
            malpha = wPar[pM_Alpha].GetData();
        else
        {
            if (dualGeometryIsPrismatic)
                malpha = 12.0 / Sqr(wPar[pM_t].GetData());  // slab thick
            else
                malpha = 15.0 / Sqr(wPar[pM_d].GetData()/2.0);  // sphere diameter * ???
        }
        // calcs for output
        double fracK  = CalcKTerm(fracParGroup) * (1.0 - mVolFactor);  // for analytic compatibility
        double calcLambda = malpha * mk / fracK * Sqr(wPar[pTZ_r_w].GetData());
        simulatorRadiiFO.dualLambdaDO.SetCalcValue(calcLambda);

        double fracT = fracK * wPar[pF_t].GetData(0);
        simulatorRadiiFO.dualFractureTDO.SetCalcValue(fracT);

        double fracSS  = CalcStoreTerm(fracParGroup) * (1.0 - mVolFactor);
        double calcOmega = fracSS / (fracSS + mss * mVolFactor);
        simulatorRadiiFO.dualOmegaDO.SetCalcValue(calcOmega);
    }

    if (control.IsLeaky())
    {
        for (int j = 0; j < nradialNodes; j++)
             leakArea[j] = nodePlanArea[j] * fVolCons;

        if (leakageType != ltUpperLower)
        {
            singleLeak.ConstantInit(staticPressure, kpaConst);
            singleLeak.SetParameters(wPar, slParGroup, leakArea);
        }
        else
        {
            lowerLeak.ConstantInit(staticPressure, kpaConst);
            lowerLeak.SetParameters(wPar, llParGroup, leakArea);

            upperLeak.ConstantInit(staticPressure, kpaConst);
            upperLeak.SetParameters(wPar, ulParGroup, leakArea);
        }
    }

    ScaleGeomTerms(kpaToPa, kpaToPa);


    allAreConstant = true;
    dTermIsConstant.FillToSize(false);
    sTermIsConstant.FillToSize(false);

    if (isPartialRun)
        parameterTime = wSeq[partialStartSeqVal].startTime;
    else
        parameterTime = wSeq[0].startTime;

    for (int j = 0; j < nradialNodes; j++)
    {
        lastWasConstant = true;
        double nextD = GetDTerm(j);
        dConstantTerm[j] = nextD;
        dTermIsConstant[j] = lastWasConstant;
        if (!lastWasConstant)
            allAreConstant = false;

        lastWasConstant = true;
        double sterm = GetSTerm(j);
        sConstantTerm[j] = sterm;
        sTermIsConstant[j] = lastWasConstant;
        if (!lastWasConstant)
            allAreConstant = false;
    }

    BuildMatrices();

    if (!control.IsDual())
        CalcFluidFormationFactors();

}


void SingleWell1DLinear::BuildMatrices()
{
    double lastD = 0.0;

    for (int j = 0; j < nradialNodes; j++)
    {
        double nextD = GetDTerm(j);
        fracDDiag[j] = nextD + lastD;
        fracDUpper[j] = -nextD;
        lastD = nextD;

        fracSVector[j] = GetSTerm(j);
    }

    wellDTerm = GetDTerm(0);
    fracDDiag.LastIndex() = GetDTerm(nradialNodes - 2);

    if (control.ExtBoundIsFixedP())
    {
        externalBoundaryTerm = -fracDDiag.LastIndex();
        externalBoundaryTerm *= staticPressure;
    }


}

double  SingleWell1DLinear::CalcStoreTerm(const ParameterGroups& parGroup)
{
    return wPar.CalcStoreTerm(parGroup);
}

double SingleWell1DLinear::CalcKTerm(const ParameterGroups& parGroup)
{
    return wPar.CalcKTerm(parGroup);
}



double SingleWell1DLinear::GetRockPar(int   parIndx,
                                           int  nodeNum)
{
    return LinGetRockPar(parIndx, nodeNum);
}

double SingleWell1DLinear::GetFluidPar(int parIndx)
{
    return LinGetFluidPar(parIndx);
}


double SingleWell1DLinear::LinGetRockPar(int parIndx,
                                         int nodeNum)
{
    if (wPar[parIndx].IsFt())
        lastWasConstant = false;

    return wPar[parIndx].GetData(nodeNum, parameterTime);
}

double SingleWell1DLinear::LinGetFluidPar(int parIndx)
{
    if (wPar[parIndx].IsFt())
        lastWasConstant = false;

    return wPar[parIndx].GetData(parameterTime);
}


double SingleWell1DLinear::GetDTerm(int nodeNum)
{
    if (dTermIsConstant[nodeNum])
        return dConstantTerm[nodeNum];

    const ParameterGroups& currRock = (*rockMap[nodeNum]);

    double dTerm;
    if (enterPermeabilityData)
        dTerm = LinGetRockPar(currRock.kIndx, nodeNum) /
                    LinGetFluidPar(pFl_mu);
    else
        dTerm = LinGetRockPar(currRock.hkIndx, nodeNum) /
                    (gravConst * LinGetFluidPar(pFl_rho));

    if (horizontalAnisotropy)
        dTerm *= sqrt(1.0 / wPar[pF_KxyRatio].GetData());

    return dTerm * dGeomTerm[nodeNum];
}

double SingleWell1DLinear::GetSTerm(int nodeNum)
{
    if (sTermIsConstant[nodeNum])
        return sConstantTerm[nodeNum];

    double skTerm = 0.0;
    if (control.HasSkin() && (nodeNum == fracStartNode))
    {
        if (enterSpecificStorageData)
            skTerm = LinGetRockPar(skinParGroup.ssIndx, nodeNum)
                        / (gravConst * LinGetFluidPar(pFl_rho));
        else
            skTerm = LinGetRockPar(skinParGroup.cIndx, nodeNum)
                        +  LinGetRockPar(skinParGroup.phiIndx, nodeNum) *
                            LinGetFluidPar(pFl_C);
        skTerm *= sLastSkinGeom;
    }

    const ParameterGroups& currRock = (*rockMap[nodeNum]);
    double sTerm;
    if (enterSpecificStorageData)
        sTerm = LinGetRockPar(currRock.ssIndx, nodeNum)
                    / (gravConst * LinGetFluidPar(pFl_rho));
    else
        sTerm = LinGetRockPar(currRock.cIndx, nodeNum)
                    +  LinGetRockPar(currRock.phiIndx, nodeNum) *
                        LinGetFluidPar(pFl_C);

    return skTerm + sTerm * sGeomTerm[nodeNum];
}



// ******************************************* linear stuff

int  SingleWell1DLinear::SolveNodeOffset()
{
    if (currentSeqIsFixed)
        return 1;
    return 0;
}


void SingleWell1DLinear::AddSCLhsTerm(StaticCondensation& inSC)
{
    inSC.CalcQTerm(deltaT);
    int indxOff = SolveNodeOffset();
    for (int i = scStartNode - indxOff; i < nequations; i++)
        fracSolveDiag[i] += inSC.GetLhsQ(i + indxOff);
}

void SingleWell1DLinear::AddSCRhsTerm(StaticCondensation& inSC)
{
    int indxOff = SolveNodeOffset();
    for (int i = scStartNode - indxOff; i < nequations; i++)
        fracRhs[i] += inSC.GetRhsQ(i + indxOff);
}

void SingleWell1DLinear::SCGaussSolve(StaticCondensation& inSC)
{
    for (int i = scStartNode; i < nradialNodes; i++)
        inSC.GaussSolve(i,  nodePressure[i]);
}

void  SingleWell1DLinear::LinearMatSetup()
{
    int indxOff = SolveNodeOffset();
    for (int i = 0; i < nequations; i++)
    {
          int nodeIndx = i + indxOff;
          fracSSolve[i]   = fracSVector[nodeIndx] / deltaT;
          fracSolveDiag[i] = fracDDiag[nodeIndx] + fracSSolve[i];
          fracSolveUpper[i] = fracDUpper[nodeIndx];
    }

    if (control.IsDual())
        AddSCLhsTerm(matrix);


    if (control.IsLeaky())
    {
        if (leakageType == ltUpperLower)
        {
            AddSCLhsTerm(upperLeak);
            AddSCLhsTerm(lowerLeak);
        }
        else
            AddSCLhsTerm(singleLeak);
    }

    if (!currentSeqIsFixed)
    {
        fracSolveDiag[0] = fracDDiag[0] + fracSSolve[0] + GetWellBCLhs();
    }


}


void  SingleWell1DLinear::LinearMatSolve()
{
    int indxOff = SolveNodeOffset();
    for (int i = 0; i < nequations; i++)
        fracRhs[i] = fracSSolve[i] * nodePressure[i + indxOff];

    if (control.IsDual())
        AddSCRhsTerm(matrix);

    if (control.IsLeaky())
    {
        if (leakageType == ltUpperLower)
        {
            AddSCRhsTerm(upperLeak);
            AddSCRhsTerm(lowerLeak);
        }
        else
            AddSCRhsTerm(singleLeak);
    }

    fracRhs[nequations - 1] -= externalBoundaryTerm;

    if (currentSeqIsFixed)
        fracRhs[0] += currSeqTZ.tzPressure * wellDTerm;
    else
        fracRhs[0] += GetWellBCRhs();

#ifdef LAPACK

    //for LAPACK, just use the simple tri-diagonal solver.
    int N = nequations;
    int nrhs = 1;
    double *DL = new double[N-1];
    double *D = new double[N];
    double *DU = new double[N-1];
    double *B = new double[N];

    for(int i = 0; i < N-1; i++)
    {
        DL[i]=fracSolveUpper[i];
        DU[i]=fracSolveUpper[i];
        D[i]=fracSolveDiag[i];
        B[i]=fracRhs[i];
    }
    D[N-1]=fracSolveDiag[N-1];
    B[N-1]=fracRhs[N-1];
    int ldb = N;
    int info;

    dgtsv_(&N, &nrhs, DL, D, DU, B, &ldb, &info);

    if(info == 0)
    {
        for(int i = 0; i < N; i++)
        {
            nodePressure[i]=B[i];
        }
    }
    else //info != 0
    {
        throw SimError("SingleWell1DLinear::LinearMatSolve - failed tri-diagonal solve", SimError::seSemiFatal);
    }

    delete [] DL;
    delete [] DU;
    delete [] D;
    delete [] B;

#else // LAPACK
    ThomasSolve(fracSolveDiag, fracSolveUpper, fracWork, fracRhs, nodePressure, nequations);
#endif // LAPACK

    if (currentSeqIsFixed)
    {
        for (int i = nequations; i > 0; i--)
            nodePressure[i] = nodePressure[i - 1];
        nodePressure[0] = currSeqTZ.tzPressure;
    }

    if (control.IsDual())
        SCGaussSolve(matrix);
    if (control.IsLeaky())
    {
        if (leakageType == ltUpperLower)
        {
            SCGaussSolve(upperLeak);
            SCGaussSolve(lowerLeak);
        }
        else
            SCGaussSolve(singleLeak);
    }
}


void  SingleWell1DLinear::MatSolve(bool& setupRequired)  // main driver
{
    if (!allAreConstant)
    {
        parameterTime = currSeqTime.testTime;
        BuildMatrices();
        setupRequired = true;
    }

    if (setupRequired)
    {
        LinearMatSetup();
        setupRequired = false;
    }

    LinearMatSolve();

    currSeqTZ.tzPressure = nodePressure[0];
    currSeqTZ.formFlow = wellDTerm * (currSeqTZ.tzPressure - nodePressure[1]) +
        fracSVector[0] * (currSeqTZ.tzPressure - lastSeqTZ.tzPressure) / deltaT;

    if ((currSeqTZ.tzPressure < 0.0) && (!allowNegativePressure))
        throw SimError("1D linear well sucked dry - reduce pumping rate", SimError::seSemiFatal);
}




double SingleWell1DLinear::GetPChange()
{
    double minP, maxP;

    nodePressure.CalcMinMax(minP, maxP);
    if (control.IsDual())
        matrix.AddToMinMax(minP, maxP, scStartNode);

    if (control.IsLeaky())
    {
        if (leakageType == ltUpperLower)
        {
            upperLeak.AddToMinMax(minP, maxP, scStartNode);
            lowerLeak.AddToMinMax(minP, maxP, scStartNode);
        }
        else
            singleLeak.AddToMinMax(minP, maxP, scStartNode);
    }
    return maxP - minP;
}


void SingleWell1DLinear::SaveCurrentPressure()
{
    SingleWell1D::SaveCurrentPressure();
    if (control.IsDual())
        matrix.SaveCurrentPressure();

    if (control.IsLeaky())
    {
        if (leakageType == ltUpperLower)
        {
            upperLeak.SaveCurrentPressure();
            lowerLeak.SaveCurrentPressure();
        }
        else
            singleLeak.SaveCurrentPressure();
    }
}

void SingleWell1DLinear::RestoreCurrentPressure()
{
    SingleWell1D::RestoreCurrentPressure();
    if (control.IsDual())
        matrix.RestoreCurrentPressure();

    if (control.IsLeaky())
    {
        if (leakageType == ltUpperLower)
        {
            upperLeak.RestoreCurrentPressure();
            lowerLeak.RestoreCurrentPressure();
        }
        else
            singleLeak.RestoreCurrentPressure();
    }
}


