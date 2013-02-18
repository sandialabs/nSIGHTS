///////////////////////////////////////////////////////////////////////////////////
//
// C_SingleWell.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/U_Msg.h>
#include <genClass/C_MessageLog.h>

#include <nsClass/nSightConst.h>

#include <Var/G_Control.h>
#include <Var/G_Sequence.h>
#include <Var/G_DataCapture.h>
#include <Var/G_Parameter.h>
#include <Var/E_Parameter.h>

#include <osClass/C_SimErr.h>

#include "G_SimulatorRadiiOutput.h"
#include "G_ParameterCurveOutput.h"

#include "C_SingleWell.h"

            //  unit conversions
const double SingleWellSimulator::kpaToPa       = 1000.0;   // because GTFM is in kPa
const double SingleWellSimulator::cPToPas       = 0.001;

SingleWellSimulator::SingleWellSimulator()
{
}

SingleWellSimulator::~SingleWellSimulator()
{
}

bool SingleWellSimulator::SetupForRun(SC_SetupErr&  errData)
{
    // make working copies
    // parameters first
    if (!WorkingParameterSetup(wPar, nodeRadii, errData))
        return false;
    // sequences
    if (!WorkingSequenceSetup(*this, wSeq, wPar, errData))
        return false;

    staticPressure = GetStaticPressure();

    if (control.IsLiquid())
    {
        wPar.InitLinearFluidVars(staticPressure);
        kpaConst =  kpaToPa / (gravConst * wPar.GetFluidDensity());
    }
    else
        kpaConst =  kpaToPa;

     return true;
}

void SingleWellSimulator::Execute()
{
    SetRuntimeSize();

    SetRunParameters();

    UpdateFRTable();

    // initialize output
    lastSeqTZ.tsPressure  = staticPressure;
    lastSeqTZ.tzPressure  = staticPressure;
    currSeqTZ.tzPressure  = staticPressure;

    // init at first
    currSeqTZ.wellFlow    = nullReal;
    currSeqTZ.formFlow    = nullReal;
    currSeqTZ.volFlow     = nullReal;
    currSeqTZ.storageFlow = nullReal;

    if (isPartialRun)
    {
        wSeq[partialStartSeqVal].StartSequence(currSeqTime, lastSeqTZ, currSeqTZ);
        InitializeTZData();
        for ( int i = partialStartSeqVal; i <= partialEndSeqVal; i++)
            DoSequenceRun(wSeq[i]);
    }
    else
    {
        wSeq[0].StartSequence(currSeqTime, lastSeqTZ, currSeqTZ);
        InitializeTZData();
        for ( int i = 0; i < wSeq.Size(); i++)
            DoSequenceRun(wSeq[i]);
    }
}

void SingleWellSimulator::UpdateOutput()
{
    capturedDataFO.CalcAndUpdate();
}


bool SingleWellSimulator::AllocateAll(UpdateCB upCB)
{
    updateCB = upCB;

    // TS control flags
    saveOldPressures = sequence.UsesDynamicTS();

    try
    {
        DoAllocateAll();
    }
    catch (SC_Array::AllocationError)
    {
        return false;
    }

    // kluge to set up f(r) calc
    simulatorRadiiFO.PreCalcSetup();

    // and parameter curves
    parameterCurveOutputFO.PreCalcSetup();

    return true;
}

void SingleWellSimulator::DoAllocateAll()
{
    int maxradialNodes = allParam[pN_n_r]->GetIntMaxPar();
    nodeVolume.Alloc(maxradialNodes);
    nodePlanArea.Alloc(maxradialNodes);
    nodeFlowArea.Alloc(maxradialNodes);
    rockMap.Alloc(maxradialNodes);
}


void SingleWellSimulator::SetRuntimeSize()
{
    nradialNodes = wPar[pN_n_r].GetIntData();
    nskinNodes   = wPar[pN_n_s].GetIntData();

    if (nskinNodes > 0)
        fracStartNode   = nskinNodes - 1;
    else
        fracStartNode   = 0;

    nodeVolume.SetSize(nradialNodes);
    nodePlanArea.SetSize(nradialNodes);
    nodeFlowArea.SetSize(nradialNodes - 1);
    rockMap.SetSize(nradialNodes);
}

void SingleWellSimulator::SetRunParameters()
{
    // reset all DO
    simulatorRadiiFO.ResetCalcPars();

    CalcGeometry();

    // set basic BC stuff
    externalBoundaryTerm = 0.0;
    if (sequence.ContainsSlugBC())
        casingArea = pi * Sqr(wPar[pTZ_r_s].GetData());
    else
        casingArea = 0.0;
}



void SingleWellSimulator::CalcGeometry()
{
    double rWell      = wPar[pTZ_r_w].GetData();
    thicknessAtWell  = wPar[pF_t].GetData();

    double wellArea = 2.0 * pi * rWell * thicknessAtWell;
    if (control.Is2DRadial())
        wellArea /= thicknessAtWell;
    lastA  = wellArea;
    lastR  = rWell;

    lastSkinVolume = 0.0;
    if (control.HasSkin())
    {
        CalcAreaTerms(0, nskinNodes);
        SetRockMap(0, nskinNodes, skinParGroup);
        lastSkinVolume = nodeVolume[nskinNodes - 1];
        lastSkinArea = nodePlanArea[nskinNodes - 1];
    }
    CalcAreaTerms(fracStartNode, nradialNodes);

    SetRockMap(fracStartNode, nradialNodes, formParGroup);
}

void SingleWellSimulator::CalcAreaTerms(int stNode, int endNode)
{
    // array adjust
    endNode--;

    double a1, a2, a12, v1, v2, v12, dv1, dv2;
    dv2 = 0.0;
    for (int j = stNode; j < endNode; j++)
    {
        CalcFlowDCons(j);
        CalcAreaVolume(r1, a1, v1);
        CalcAreaVolume(r12, a12, v12);

        dv1 = fabs(v12 - v1);
        nodeVolume[j] = dv1 + dv2;
        nodeFlowArea[j] = a12;


        if (control.Is2DRadial())
            //plan area is volume divided by thickness - equals below when n=2
            //for 2D, thickness is left out at beginning
            nodePlanArea[j] = nodeVolume[j];
        else
        {
            // node plan area used for leakage calcs only - assume n = 2?
            //note: if you want to use flow dimensions, nodePlanArea can be calculated
            //by dividing the nodeVolume by thicknessAtWell, which is equivalent to below
            //when n = 2
            double startR;
            if (j == stNode)
                startR = r1;
            else
                startR = (r1 + nodeRadii[j - 1]) / 2.0;
            nodePlanArea[j] = pi * (Sqr(r12) - Sqr(startR));
        }

        CalcAreaVolume(r2, a2, v2);
        dv2 = fabs(v2 - v12);

        lastR = r1;
        lastA = a1;
    }

    nodeVolume[endNode] = dv2;
    nodeFlowArea[endNode] = 0.0;
    nodePlanArea[endNode] = pi * (Sqr(r2) - Sqr(r12));
    lastFlowArea = a2;
}


void  SingleWellSimulator::CalcAreaVolume(const double& rVal,
                                      double& aVal,
                                      double& vVal)
{
    aVal = flowCons * pow(rVal, nVal - 1.0);
    vVal = aVal * rVal / nVal;
}

void  SingleWellSimulator::CalcFlowDCons(int nodeNum)
{
    r1   = nodeRadii[nodeNum];
    r2   = nodeRadii[nodeNum + 1];
    r12  = (r1 + r2) / 2.0;
    nVal = wPar[pF_n].GetData(nodeNum);

    if (wPar[pF_n].IsRadius() || compensateFlowDimensionGeometry)
        flowCons = lastA / pow(lastR, nVal - 1.0);
    else
        flowCons = pow(thicknessAtWell, 3.0 - nVal) *
                          2.0 * pow(pi, nVal/2.0) / Gamma(nVal/2.0);
}

void SingleWellSimulator::SetRockMap(int stNode, int endNode, const ParameterGroups& currType)
{
    for (int j = stNode; j < endNode; j++)
        rockMap[j] = &currType;
}



void SingleWellSimulator::UpdateFRTable()
{

//  **************************************************************************************
//
//  This whole section is something of a kluge, however it is the most straightforward way
//  to get the f(r) table output.
//
//  **************************************************************************************


    int ntableCols = 5;
    SC_IntArray radPar(wPar.Size());
    radPar.SetEmpty();

    for (int i = 0; i < wPar.Size(); i++)
        if (wPar[i].IsRadius())
        {
            ntableCols++;
            radPar += i;
        }

    DC_TableData& radTabDC = simulatorRadiiFO.simulatorRadiiDC;
    radTabDC.Alloc(ntableCols, nradialNodes, 40);

    // reset titles
    int colNum = 0;
    radTabDC.SetTitle(colNum++, "Radius");
    radTabDC.SetTitle(colNum++, "FlowArea");
    radTabDC.SetTitle(colNum++, "NodeVol");
    radTabDC.SetTitle(colNum++, "CondArea");
    radTabDC.SetTitle(colNum++, "StoreArea");
    for (int i = 0; i < radPar.Size(); i++)
        radTabDC.SetTitle(colNum++, allParam[radPar[i]]->shortName);

    SC_DoubleMatrix& radTab = radTabDC.dataTable;
    radTab.SetSize(ntableCols);
    for (int i = 0; i < ntableCols; i++)
        radTab[i].SetSize(nradialNodes);

    double lastR, currR, dr2;
    double nextR = nodeRadii[0];
    double dr1 = 0.0;

    static const int tabUnits[] = {uDist, uPerm, uVol, uFlow, uDist};
                                        // rad, area, vol,  m/s * m^2, 1/m * m^2

    for (int j = 0; j < nradialNodes; j++)
    {
        // node radius related
        currR = nextR;
        if (j > 0)
        {
            dr1 = (currR - lastR) / 2.0;
        }

        if (j < (nradialNodes - 1))
        {
            nextR = nodeRadii[j + 1];
            dr2 = (nextR - currR) / 2.0;
        }
        else
            dr2 = 0.0;


        radTab[0][j] = currR;

        // flow area
        if (j < (nradialNodes - 1))
        {
            radTab[1][j] = nodeFlowArea[j];
        }
        else
        {
            radTab[1][j] = lastFlowArea;
        }

        // node vol
        radTab[2][j] = nodeVolume[j];

        // cond area
        radTab[3][j] = GetFrCondVal(j) * radTab[1][j];

        // store area
        radTab[4][j] = GetFrStoreVal(j) * radTab[2][j] / (dr1 + dr2);

        lastR = currR;


        int nextCol = 5;

        // unit conversions
        for (int i = 0; i < nextCol; i++)
        {
            UnitIndex currU(tabUnits[i]);
            radTab[i][j] = currU.MetricToUser(radTab[i][j]);
        }


        // and f(R) param
        for (int i = 0; i < radPar.Size(); i++)
        {
            WorkingParameter& currPar = wPar[radPar[i]];
            UnitIndex parUnits = allParam[radPar[i]]->parVal;
            radTab[nextCol++][j] = parUnits.MetricToUser(currPar.GetData(j));
        }
    }

    simulatorRadiiFO.CalcAndUpdate();

}


double  SingleWellSimulator::GetStaticPressure()
{
    return wPar[pF_Pf].GetData();
}

double SingleWellSimulator::GetRockPar(int parIndx,
                                         int nodeNum)
{
    return wPar[parIndx].GetData(nodeNum);
}

double SingleWellSimulator::GetFluidPar(int parIndx)
{
    return wPar[parIndx].GetData();
}

double SingleWellSimulator::GetFrCondVal(int nodeNum)
{
    const ParameterGroups& currRock = (*rockMap[nodeNum]);

    double dTerm;
    if (enterPermeabilityData)
        dTerm = GetRockPar(currRock.kIndx, nodeNum) * (gravConst * GetFluidPar(pFl_rho))
                 / GetFluidPar(pFl_mu);
    else
        dTerm = GetRockPar(currRock.hkIndx, nodeNum);
    return dTerm;
}


double SingleWellSimulator::GetFrStoreVal(int nodeNum)
{
    const ParameterGroups& currRock = (*rockMap[nodeNum]);
    double sTerm;
    if (enterSpecificStorageData)
        sTerm = GetRockPar(currRock.ssIndx, nodeNum);
    else
        sTerm = gravConst * GetFluidPar(pFl_rho) *
                    (GetRockPar(currRock.cIndx, nodeNum)
                        +  GetRockPar(currRock.phiIndx, nodeNum) *
                            GetFluidPar(pFl_C));

    return sTerm;
}

void SingleWellSimulator::CalcFluidFormationFactors()
{
    int formKIndex, skinKIndex;
    double formationK;
    if (enterPermeabilityData)
    {
        formKIndex  = formParGroup.kIndx;
        skinKIndex  = skinParGroup.kIndx;
        formationK = GetRockPar(formKIndex, 0) * gravConst * GetFluidPar(pFl_rho)
                        / GetFluidPar(pFl_mu);
    }
    else
    {
        formKIndex  = formParGroup.hkIndx;
        skinKIndex  = skinParGroup.hkIndx;
        formationK = GetRockPar(formKIndex, 0);
    }

    double formationS;
    if (enterSpecificStorageData)
        formationS = GetRockPar(formParGroup.ssIndx, 0);
    else
    {
        formationS =  gravConst * GetFluidPar(pFl_rho) *
                (GetRockPar(formParGroup.cIndx, 0) +
                    +  GetRockPar(formParGroup.phiIndx, 0) * GetFluidPar(pFl_C));
    }

    double transmissivity =  formationK * wPar[pF_t].GetData();
    double storativity =  formationS * wPar[pF_t].GetData();
    double diffusivity = formationK /formationS;

    UnitIndex tranConv(uTran);
    simulatorRadiiFO.transmissivityDO.SetCalcValue(tranConv.MetricToUser(transmissivity));
    simulatorRadiiFO.diffusivityDO.SetCalcValue(tranConv.MetricToUser(diffusivity));
    simulatorRadiiFO.storativityDO.SetCalcValue(storativity);

    if (control.HasSkin())
        CalcSkinFactor(skinKIndex, formKIndex);
}


void SingleWellSimulator::CalcSkinFactor(int skIndex, int fkIndex)
{
    if (!control.HasSkin())
        GenAppInternalError("SingleWellSimulator::CalcSkinFactor");

    double st = wPar[pS_t].GetData();
    double rw = wPar[pTZ_r_w].GetData();
    double skTerm = GetRockPar(skIndex, 0);
    double fkTerm = GetRockPar(fkIndex, fracStartNode);
    double sFactor = (fkTerm / skTerm - 1.0) * log((rw + st) / rw);
    simulatorRadiiFO.skinFactorDO.SetCalcValue(sFactor);
}

double  SingleWellSimulator::GetWstoreCompressibility() // overriden for gas
{
    return lastSeqTZ.tzComp;
}


double SingleWellSimulator::GetLastWellPressure()
{
    return lastSeqTZ.tzPressure;
}

double SingleWellSimulator::GetWellBCRhs()
{
    currSeqData->GetTZvals(currSeqTime, currSeqTZ);

    switch (currSeqData->wellboreStorage)
    {
        case FlowHistBase::wsNone : {
                tzWstoreTerm = 0.0;
                break;
        }
        case FlowHistBase::wsOpen : {
                tzWstoreTerm = casingArea / (gravConst * lastSeqTZ.tzDens);
                if (!simulatorRadiiFO.openholeWellboreStorageDO.valueSet)
                {
                    UnitIndex wsConv(uWbor);
                    simulatorRadiiFO.openholeWellboreStorageDO.SetCalcValue(wsConv.MetricToUser(tzWstoreTerm));
                }
                break;
        }
        case FlowHistBase::wsIsolated : {
                tzWstoreTerm = GetWstoreCompressibility() * currSeqTZ.tzVol;
                if (!simulatorRadiiFO.isolatedWellboreStorageDO.valueSet)
                {
                    UnitIndex wsConv(uWbor);
                    simulatorRadiiFO.isolatedWellboreStorageDO.SetCalcValue(wsConv.MetricToUser(tzWstoreTerm));
                }
                break;
        }
    }
    tzWstoreTerm *= kpaToPa / deltaT;

    if (currentSeqIsFixed)
        return 0.0;

    double tzRhsBTerm = currSeqTZ.wellFlow; // 0.0 for all but spec flow
    tzLhsChng  = false;

    if (accountForTZVol && (currSeqTZ.tzVol != lastSeqTZ.tzVol))
    {
        tzRhsBTerm -= (currSeqTZ.tzVol - lastSeqTZ.tzVol) / deltaT;
        tzLhsChng  = true;
    }
    if (accountForTZTemp)
        tzRhsBTerm += wPar[pFl_TX].GetData() * currSeqTZ.tzVol *
                            (currSeqTZ.tzTemp - lastSeqTZ.tzTemp) / deltaT;

    if (accountForTZComp && (currSeqTZ.tzComp != lastSeqTZ.tzComp))
        tzLhsChng  = true;

     tzRhsBTerm += tzWstoreTerm * GetLastWellPressure();

     return tzRhsBTerm;
}


double SingleWellSimulator::GetWellBCLhs()
{
    return  tzWstoreTerm;
}


void SingleWellSimulator::AddMessageVal(const char* txt, const double& cval)
{
    using namespace std;
    ostringstream ostr;
    ostr.setf(ios::scientific, ios::floatfield);
    ostr << txt << " "  << cval << ends;
    MessageLogC::AddOtherMessage("SimCore", ostr.str().c_str());
}


void SingleWellSimulator::DoNextTimeStep(bool& setupRequired)
{
    deltaT = currSeqTime.UpdateTimes();
    GetWellBCRhs();
    if (tzLhsChng)
        setupRequired = true;

    MatSolve(setupRequired);

    currSeqTZ.storageFlow = tzWstoreTerm * (currSeqTZ.tzPressure - lastSeqTZ.tzPressure);
    currSeqTZ.volFlow     = (currSeqTZ.tzVol - lastSeqTZ.tzVol) / deltaT;

    // well flow calculation depends upon seq type
    switch (currSeqData->seqType)
    {
        case Sequence::stFlow  : {  // OK as is
            break;
        }
        case Sequence::stHistory  : {
            // sum of all other flows
            currSeqTZ.wellFlow = currSeqTZ.formFlow + currSeqTZ.volFlow + currSeqTZ.storageFlow;
            break;
        }
        case Sequence::stPulse  : {
            currSeqTZ.wellFlow = 0.0;   // this is a change from GTFM - but makes sense
            break;
        }
        case Sequence::stSlug  : {
            currSeqTZ.wellFlow = -currSeqTZ.storageFlow;
            break;
         }
    }
}

void SingleWellSimulator::SequenceSetup()
{
}
void SingleWellSimulator::SequenceComplete()
{
}

void SingleWellSimulator::UpdateTZData()
{
    lastSeqTZ = currSeqTZ;
}

void SingleWellSimulator::InitializeTZData()
{
    lastSeqTZ = currSeqTZ;
}

void SingleWellSimulator::DoSequenceRun(WorkingSequence& currSeq)
{
    currSeqData = &currSeq;
    currSeq.StartSequence(currSeqTime, lastSeqTZ, currSeqTZ);

    accountForTZTemp    = currSeq.isThermal;
    accountForTZVol     = testZoneVolumeVaries && (currSeq.wellboreStorage == FlowHistBase::wsIsolated);
    accountForTZComp    = testZoneCompressibilityVaries && (currSeq.wellboreStorage == FlowHistBase::wsIsolated);
    currentSeqIsFixed   =  (currSeq.seqType == Sequence::stHistory);

    SequenceSetup();

    UpdateTSOutput();

    currSeq.DynamicSetup();
    if (currSeq.IsDynamicTS())
        SaveCurrentPressure();

    bool setupRequired = true;
    tstepNum = 0;

    do
    {
        tstepNum++;
        if (currSeq.IsDynamicTS())
        {
            while (true)
            {
                DoNextTimeStep(setupRequired);
                if (!currSeq.DynamicRestart(currSeqTime, currSeqTZ))
                    break;
                setupRequired = true;
            }

            SaveCurrentPressure();
        }
        else
            DoNextTimeStep(setupRequired);

        UpdateTSOutput();
        updateCB();

        if (currSeq.IsComplete(currSeqTime))
            break;

        setupRequired = currSeq.SetNextTimeStep(currSeqTime);
        UpdateTZData();

    } while (true);

    // update last
    UpdateTZData();
    if (currentSeqIsFixed)
        lastSeqTZ.tsPressure = currSeqTZ.tzPressure;

    SequenceComplete();
}

