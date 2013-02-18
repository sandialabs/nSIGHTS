///////////////////////////////////////////////////////////////////////////////////
//
// C_StaticCondensation.cpp
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

#include <nsClass/nSightConst.h>

#include <Var/G_Control.h>
#include <Var/G_Parameter.h>

#include "C_StaticCondensation.h"

StaticCondensation::StaticCondensation() {};
StaticCondensation::~StaticCondensation(){};

void StaticCondensation::DoAllocateAll(int maxSCNodes, int maxMainNodes, bool saveOld)
{
    flowFactor.Alloc(maxMainNodes);
    gaussResults.Alloc(maxMainNodes);
    scaleVector.Alloc(maxMainNodes);

    rhsVector.Alloc(maxSCNodes);
    dMainDiag.Alloc(maxSCNodes);
    dUpperDiag.Alloc(maxSCNodes);
    sDiag.Alloc(maxSCNodes);
    solveMainDiag.Alloc(maxSCNodes);
    solveUpperDiag.Alloc(maxSCNodes);

    sFactor.Alloc(maxSCNodes);

    pressure.FixedMatrixAlloc(maxMainNodes, maxSCNodes);
    if (saveOld)
        prevPressure.FixedMatrixAlloc(maxMainNodes, maxSCNodes);
}

void StaticCondensation::DoSetRuntimeSize(int innmainNodes)
{
    nmainNodes = innmainNodes;

    scaleVector.SetSize(nmainNodes);
    gaussResults.SetSize(nmainNodes);
    flowFactor.SetSize(nmainNodes);

    sFactor.SetSize(nstaticSolnNodes);
    sDiag.SetSize(nstaticSolnNodes);

    dMainDiag.SetSize(nstaticSolnNodes);
    dUpperDiag.SetSize(nstaticSolnNodes);

    solveMainDiag.SetSize(nstaticSolnNodes);
    solveUpperDiag.SetSize(nstaticSolnNodes);

    pressure.MatrixSetSize(nmainNodes, nstaticNodes);
    if (prevPressure.AllocSize() > 0)
        prevPressure.MatrixSetSize(nmainNodes, nstaticNodes);
}

void StaticCondensation::ConstantInit(const double& inStatic,
                                      const double& inkpaConst)
{
    staticPressure = inStatic;
    kpaConst = inkpaConst;
    for (int i = 0; i < pressure.Size(); i++)
    {
        pressure[i].FillToSize(staticPressure);
    }
}

void StaticCondensation::SolveSetup(const double& deltaT)
{
    for (int i = 0; i < nstaticSolnNodes; i++)
    {
        sFactor[i] = sDiag[i] / deltaT;
        solveMainDiag[i] = dMainDiag[i] + sFactor[i];
        solveUpperDiag[i] = dUpperDiag[i];
    }
    solveMainDiag[nstaticSolnNodes - 1] = dMainDiag[nstaticSolnNodes - 1]; // no storage on connecting node
}

void StaticCondensation::GaussElim()
{
    gaussResults[0] = 1.0;
    for (int j = 0; j < (nstaticSolnNodes - 1); j++)
    {
        double temp = solveUpperDiag[j] / solveMainDiag[j];
        solveMainDiag[j + 1] = solveMainDiag[j + 1] - temp * solveUpperDiag[j];
        gaussResults[j + 1] =temp;
    }
}


void StaticCondensation::CalcFlowFactor()
{
    for (int i = 0; i < (nstaticSolnNodes - 1); i++)
    {
        flowFactor[i] = 1.0;
        for (int j  = i + 1; j < nstaticSolnNodes; j++)
            flowFactor[i] *= gaussResults[j];
    }

    double temp = -1.0;
    for (int i = nstaticSolnNodes - 2; i >= 0; i--)
    {
        flowFactor[i] *= temp;
        temp = -temp;
    }
}


void StaticCondensation::CalcQTerm(const double& deltaT)
{
    SolveSetup(deltaT);
    GaussElim();
    CalcFlowFactor();
    qVal = solveMainDiag[nstaticSolnNodes - 1];
}

double StaticCondensation::GetLhsQ(int mainNodeNum)
{
    return qVal * scaleVector[mainNodeNum];
}


double StaticCondensation::BasicRHSQ(int mainNodeNum)
{
    SC_DoubleArray& scPressure = pressure[mainNodeNum];
    double temp = 0.0;
    for (int j = 0; j < (nstaticSolnNodes - 1); j++)
        temp += scPressure[j] * flowFactor[j];

    return temp;
}

void StaticCondensation::GaussSolve(      int           mainNodeNum,
                                 const double&          mainNodePressure)
{
    SC_DoubleArray& scPressure = pressure[mainNodeNum];

    for (int j = 0; j < (nstaticSolnNodes - 1); j++)
        rhsVector[j] = sFactor[j] * scPressure[j];
    rhsVector[0] += bTerm;

    for (int j = 1; j < (nstaticSolnNodes - 1); j++)
        rhsVector[j] -= rhsVector[j - 1] * gaussResults[j];

    double pOld = mainNodePressure;
    for (int j = (nstaticSolnNodes - 2); j >= 0; j--)
    {
          double pNew = (rhsVector[j] - pOld * solveUpperDiag[j]) /  solveMainDiag[j];
          scPressure[j] = pNew;
          pOld = pNew;
    }
}

// dynamic TS support

void StaticCondensation::AddToMinMax(double&    minP,
                                     double&    maxP,
                                     int        startNode)
{
    for (int i = startNode; i < nmainNodes; i++)
        pressure[i].AddToMinMax(minP, maxP);
}


void StaticCondensation::SaveCurrentPressure()
{
    prevPressure = pressure;
}

void StaticCondensation::RestoreCurrentPressure()
{
    pressure = prevPressure;
}


// ************************* dual porosity 

void DualPorosity::AllocateAll(int maxMainNodes, bool saveOld)
{
    int maxmatrixNodes = allParam[pN_n_m]->GetIntMaxPar();
    DoAllocateAll(maxmatrixNodes + 1, maxMainNodes, saveOld); 
}

void DualPorosity::SetRuntimeSize(WorkingParameterArray& wPar, int innmainNodes)
{
    nstaticNodes = wPar[pN_n_m].GetIntData();
    nstaticSolnNodes = nstaticNodes + 1; 
    DoSetRuntimeSize(innmainNodes);
}

double DualPorosity::GetRhsQ(int mainNodeNum)
{
    double temp = BasicRHSQ(mainNodeNum);

    return temp * sFactor[0] * scaleVector[mainNodeNum];
}

void DualPorosity::SetParameters(WorkingParameterArray& wPar,
                                  const ParameterGroups& parGroup,
                                  const SC_DoubleArray&   fracNodeVol)

{
    double mVolFactor = wPar[pM_V].GetData();
    double mk     = wPar.CalcKTerm(parGroup);
    double mss    = wPar.CalcStoreTerm(parGroup);

    double malpha;
    if (enterDualAlpha)
        malpha = wPar[pM_Alpha].GetData();

    if (dualGeometryIsPrismatic)
    {
        if (!enterDualAlpha)
            malpha = 12.0 / Sqr(wPar[pM_t].GetData());  // slab thick

        // based on unit volume & unit prism area  -- changed from unit area in 6.20
        // old m_d from unit area scaling (pre 6.20)  m_d_term   :=  m_alpha * m_k * REAL8(n_matrix_nodes) * kpa_const;
        // old m_s from unit area mat_s_term :=  m_spec_store * m_thick / REAL8(n_matrix_nodes) * kpa_const;
        // constant for each node
        double mdTerm =  malpha * mk * double(nstaticNodes) * kpaConst;
        double msTerm =  mss / double(nstaticNodes) * kpaConst;

        for (int j = 0; j < nstaticSolnNodes; j++)
        {
            dMainDiag[j] = mdTerm + mdTerm;
            dUpperDiag[j] = -mdTerm;
            sDiag[j] = msTerm;
        }
        dMainDiag[0] = mdTerm;
        dMainDiag[nstaticSolnNodes - 1] = mdTerm;
    }
    else
    {
        if (!enterDualAlpha)
            malpha = 15.0 / Sqr(wPar[pM_d].GetData()/2.0);  // sphere diameter * ???

        // discretization is based on equal volume for each node
        double matNodeVol = 1.0 / double(nstaticNodes);
        SC_DoubleArray matNodeRadius(nstaticSolnNodes), matFlowRadius(nstaticSolnNodes), matFlowArea(nstaticSolnNodes);

        // determine radii of interfaces
        matNodeRadius[0] = 0.0;
        for (int j = 0; j < nstaticNodes; j++)
        {
            double enclosedVol = double(j + 1) * matNodeVol;

            // sphere volume = 4/3 pi r^3
            // calculate radius for volume
            matFlowRadius[j] = Power(enclosedVol / pi * 0.75, 0.3333333333);

            // area associated with radius
            matFlowArea[j] = 4.0 * pi * Sqr(matFlowRadius[j]);
            // node radius is midway between flow areas
            if (j > 0)
                matNodeRadius[j] = (matFlowRadius[j - 1] + matFlowRadius[j]) / 2.0;
        }

        // radius of outside of sphere at matrix/fracture interface
        matNodeRadius[nstaticNodes] = 0.62035049; // radius of sphere of volume = 1
        double mdTerm =  malpha * mk * kpaConst;
        double msTerm =  mss * matNodeVol * kpaConst;

        double lastD = 0.0;
        for (int j = 0; j < nstaticNodes; j++)
        {
            double nextD = mdTerm * matFlowArea[j] / (matNodeRadius[j + 1] - matNodeRadius[j]);
            dMainDiag[j] = nextD + lastD;
            dUpperDiag[j] = -nextD;
            lastD = nextD;
            sDiag[j] = msTerm;
        }
        dMainDiag[nstaticSolnNodes - 1] = lastD;
        sDiag[nstaticSolnNodes - 1] = msTerm;
    }

    // matrix scale factor based on unit volume
    // s_geom_term was scaled to fracture vol already -- unscale, then scale to matrix
    double volCompFactor = mVolFactor / (1.0 - mVolFactor);
    for (int j = 0; j < nmainNodes; j++)
            scaleVector[j] = fracNodeVol[j] * volCompFactor;
    //  pre 6.2 calculation mat_scale_vector^[j] := s_geom_term^[j + n_plug_nodes] / f_thick;

    bTerm = 0.0;
}

// ************************* leakage

void Leakage::SetParameters(      WorkingParameterArray&    wPar,
                            const ParameterGroups&          parGroup,
                            const SC_DoubleArray&           nodeArea)
{
    double lThick = wPar[parGroup.tIndx].GetData();
    double lk  = wPar.CalcKTerm(parGroup);
    double lss = wPar.CalcStoreTerm(parGroup);
    double dl  = lThick / double(nstaticNodes);

    // based on unit area
    double dTerm = lk / dl * kpaConst;
    double sTerm = lss * dl * kpaConst;

    dMainDiag.SetSize(nstaticSolnNodes);
    dUpperDiag.SetSize(nstaticSolnNodes);
    sDiag.SetSize(nstaticSolnNodes);
    for (int j = 0; j < nstaticSolnNodes; j++)
    {
        dMainDiag[j] = 2.0 * dTerm;
        dUpperDiag[j] = -dTerm;
        sDiag[j] = sTerm;
    }
    dMainDiag.SetLastIndex(dTerm);
    bTerm = dTerm * staticPressure;

    for (int j = 0; j < nmainNodes; j++)
        scaleVector[j] = nodeArea[j];
}

double Leakage::GetRhsQ(int mainNodeNum)
{
    double temp = BasicRHSQ(mainNodeNum);

    temp = temp * sFactor[0] + bTerm * flowFactor[0];

    return temp * scaleVector[mainNodeNum];

}

void Leakage::AllocateAll(int maxMainNodes, bool saveOld)
{
    int maxleakyNodes  = allParam[pN_n_l]->GetIntMaxPar();
    DoAllocateAll(maxleakyNodes, maxMainNodes, saveOld); 
}

void Leakage::SetRuntimeSize(WorkingParameterArray& wPar, int innmainNodes)
{
    nstaticNodes = wPar[pN_n_l].GetIntData();
    nstaticSolnNodes = nstaticNodes; 
    DoSetRuntimeSize(innmainNodes);
}

