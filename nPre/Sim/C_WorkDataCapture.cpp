///////////////////////////////////////////////////////////////////////////////////
//
// C_WorkDataCapture.cpp
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
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>
#include <genClass/C_MessageLog.h>

#include <nsClass/nSightConst.h>

#include <Var/G_Control.h>
#include <Var/G_DataCapture.h>
#include <Var/G_ProfileOutput.h>

#include "C_WorkDataCapture.h"


void RadiusPressureExtraction::SetExtraction(const SC_DoubleArray& nodeRadii,
                                             const double&          capRadius)
{
    doScale = false;
    // special cases first
    if (capRadius <= nodeRadii[0])
    {
        radIndex = 0;
        weight = 1.0;
        return;
    }

    if (capRadius >= nodeRadii.LastIndex())
    {
        radIndex = nodeRadii.Size() - 2;
        weight = 0.0;
        return;
    }

    for (int i = 1; i < nodeRadii.Size(); i++)
        if (capRadius < nodeRadii[i])
        {
            radIndex = i - 1;
            weight = 1.0 - (capRadius - nodeRadii[i - 1]) / (nodeRadii[i] - nodeRadii[i - 1]);
            return;
        }

    // ??? ouch
    GenAppInternalError("RadiusPressureExtraction::SetExtraction");
}

void RadiusPressureExtraction::SetExtraction(const SC_DoubleArray& nodeRadii,
                                             const double&          capRadius,
                                              const double&         inStaticP,
                                              const double&         inScaleFactor)
{
    SetExtraction(nodeRadii, capRadius);
    doScale = control.IsHorizontalAnisotropic();
    staticP = inStaticP;
    scaleFactor = inScaleFactor;
}

double  RadiusPressureExtraction::DoExtraction(const SC_DoubleArray& nodePressure) const
{
    double interpP = nodePressure[radIndex] * weight + nodePressure[radIndex + 1] * (1.0 - weight);
    if (!doScale)
        return interpP;

    double delP = interpP - staticP;
    double scaledP = delP / scaleFactor;

    return staticP + scaledP;
}



void WorkingSuperComponent::SetSuper(const SC_DoubleArray& nodeRadii,
                                     const SuperComponent& inData)
{
    superOp   = inData.superOp;
    double currVal;
    switch (inData.superType) {
        case  SuperComponent::stConstant : {
            currVal = inData.constantRad;
            break;
        }
        case  SuperComponent::stOptimized : {
            currVal = inData.optRadVal.GetParEstimate();
            break;
        }
        case  SuperComponent::stSampled : {
            currVal = inData.sampRadVal.GetSampleVal();
            break;
        }
        case  SuperComponent::stVary : {
            currVal = inData.varyRadVal.GetVaryVal();
            break;
        }
    }
    radData.SetExtraction(nodeRadii, currVal);
}


void FixedVerticalPressureExtraction::SetExtraction(const SC_DoubleArray&   nodeRadii,
                                                    const SC_DoubleArray&   fixedZ,
                                                    const double&           capRadius,
                                                          bool              vertNormalized,
                                                    const double&           vertActualOffset,
                                                    const double&           vertNormalizedOffset)
{
    RadiusPressureExtraction::SetExtraction(nodeRadii, capRadius);
    fixedNodeZ = &fixedZ;
    double capZ;
    if (vertNormalized)
        capZ = vertNormalizedOffset * fixedZ.LastIndex();
    else
        capZ = vertActualOffset;
    CalcConstantVert(capZ);
}

void FixedVerticalPressureExtraction::CalcConstantVert(const double& capZ) const
{
    double currZ = 0.0;
    vertBotNodeIndx = 0;

    // at bottom ?
    if (capZ < stdEps)
    {
        vertTopWeight = 0.0;
        return;
    }

    double lastZ = 0.0;
    for (int i = 1; i < fixedNodeZ->Size(); i++)
    {
        const double& nextZ = (*fixedNodeZ)[i];
        if (nextZ > capZ)
        {
            vertTopWeight = (capZ - lastZ) / (nextZ - lastZ);
            return;
        }
        lastZ = nextZ;
        vertBotNodeIndx = i;
    }

    // must be at top
    vertTopWeight = 0.0;
}

double  FixedVerticalPressureExtraction::DoExtraction(const SC_DoubleMatrix& nodePressure) const
{
    double pbot1 = nodePressure[vertBotNodeIndx][radIndex];
    double pbot2 = nodePressure[vertBotNodeIndx][radIndex + 1];
    double pbot = pbot1 * weight + pbot2 * (1.0 - weight);

    // at top ?
    if (vertBotNodeIndx == fixedNodeZ->UpperBound())
        return pbot;

    double  ptop1 = nodePressure[vertBotNodeIndx + 1][radIndex];
    double  ptop2 = nodePressure[vertBotNodeIndx + 1][radIndex + 1];
    double ptop = ptop1 * weight + ptop2 * (1.0 - weight);

    return pbot + (ptop - pbot) * vertTopWeight;
}


void VariableVerticalPressureExtraction::SetExtraction(const SC_DoubleArray& nodeRadii,
                                                  const SC_DoubleArray&  fixedZ,
                                                  const double&         thickToPress,
                                                  const double&         capRadius,
                                                    bool            vertNormalized,
                                              const double&         vertActualOffset,
                                              const double&         vertNormalizedOffset)
{
    RadiusPressureExtraction::SetExtraction(nodeRadii, capRadius);
    normalized = vertNormalized;
    if (normalized)
        offsetVal = vertNormalizedOffset;
    else
        offsetVal =  vertActualOffset;

    fixedNodeZ = &fixedZ;
    thicknessToPressure = thickToPress;
    doFixedZ = true;
    if ((!normalized) && (offsetVal < fixedZ.LastIndex()))
    {
        CalcConstantVert(offsetVal);
    }
    else if (offsetVal < stdEps)
    {
        // all zeroes at bottom
        CalcConstantVert(0.0);
    }
    else
        doFixedZ = false;
}


double VariableVerticalPressureExtraction::DoExtraction(const SC_DoubleArray& uncThickness,
                                                const SC_DoubleMatrix& nodePressure) const
{
    if (doFixedZ)
    {
        return FixedVerticalPressureExtraction::DoExtraction(nodePressure);
    }

    // use one radius for coefficient calc
    double radThick = uncThickness[radIndex];

    // get coeff with 0.0 = bottom and 1.0 = top
    double vertCoeff = offsetVal;
    if (!normalized)
    {
        vertCoeff  =  offsetVal / radThick;
        // special case, above water table, return 0.0
        if (vertCoeff > 1.0)
            return 0.0;
    }
    // actual vert pos (same as offset for !norm)
    double vertPos = vertCoeff * radThick;

    // convenience
    const double& constantThick = fixedNodeZ->LastIndex();
    // inside constant zone ?
    if (vertPos <= constantThick)
    {
        CalcConstantVert(vertPos);
        return FixedVerticalPressureExtraction::DoExtraction(nodePressure);
    }

    // in variable thick zone,
    // recalc coeff for vertical zone only
    vertCoeff = (vertPos - constantThick) / (radThick - constantThick);

    int nconstant = fixedNodeZ->Size();
    int nvariable = nodePressure.Size() + 1 - nconstant;

    double nodeCoeff = 1.0 / double(nvariable);

    // coeff zone for pressure based calc only
    double maxPressureCoeff = 1.0 - nodeCoeff;

    if (vertCoeff <= maxPressureCoeff)
    {
        // in interval for pressure nodes only
        vertBotNodeIndx = int(vertCoeff / nodeCoeff);
        vertTopWeight = vertCoeff - double(vertBotNodeIndx) * nodeCoeff;
        vertBotNodeIndx += nconstant - 1;
        return FixedVerticalPressureExtraction::DoExtraction(nodePressure);
    }

    // above top pressure nodes - need water table pressure
    double  ptop1 = radThick / thicknessToPressure;
    double  ptop2 = uncThickness[radIndex + 1] / thicknessToPressure;

    // at water table, use water table
    if (vertCoeff > 0.99999)
        return ptop1 * weight + ptop2 * (1.0 - weight);

    // in interval between top pressure node and water table
    double pbot1 = nodePressure.LastIndex()[radIndex];
    double pbot2 = nodePressure.LastIndex()[radIndex + 1];

    vertTopWeight = (vertCoeff - maxPressureCoeff) / nodeCoeff;

    double ptop = ptop1 * weight + ptop2 * (1.0 - weight);
    double pbot = pbot1 * weight + pbot2 * (1.0 - weight);
    return pbot + (ptop - pbot) * vertTopWeight;
}


WorkingDataCaptureSpecBase::WorkingDataCaptureSpecBase()
{
}
WorkingDataCaptureSpecBase::~WorkingDataCaptureSpecBase()
{
}

void WorkingDataCaptureSpecBase::SetupSpec(const DataCaptureSpec& inSpec,
                                          const double&          inStatic)

{
    captureType        = inSpec.captureType;
    pressureCapType    = inSpec.pressureCapType;
    flowCapType        = inSpec.flowCapType;
    productionCapType  = inSpec.productionCapType;
    testZoneCapType    = inSpec.testZoneCapType;
    outputUnits        = inSpec.outputUnits;
    dcapStaticPressure = inStatic;
}

double  WorkingDataCaptureSpecBase::ApplySpec(const double&         prevVal,         // prev val of spec for production calc
                                              const double&         deltaT,          // time for production calc
                                              const SeqBoundVals&   tsData)
{
    double capVal;
    switch (captureType)
    {
        case DataCaptureSpec::ctPressure  : {
            switch (pressureCapType)
            {
                case DataCaptureSpec::ctPressure  : {
                    capVal = tsData.tzPressure;
                    break;
                }
                case DataCaptureSpec::pctRadius  : {
                    break;
                }
                case DataCaptureSpec::pctSuperposition  : {
                    break;
                }
            }
            break;
        }
        case DataCaptureSpec::ctFlow  : {
            switch (flowCapType)
            {
                case DataCaptureSpec::fctWell  : {
                    capVal = tsData.wellFlow;
                    break;
                }
                case DataCaptureSpec::fctFormation  : {
                    capVal = tsData.formFlow;
                    break;
                }
                case DataCaptureSpec::fctTestZone  : {
                    capVal = tsData.volFlow;
                    break;
                }
                case DataCaptureSpec::fctWellboreStorage  : {
                    capVal = tsData.storageFlow;
                    break;
                }
            }
            break;
        }
        case DataCaptureSpec::ctProduction  : {
            double flowVal;
            switch (productionCapType)
            {
                case DataCaptureSpec::prctWell  : {
                    flowVal = tsData.wellFlow;
                    break;
                }
                case DataCaptureSpec::prctFormation  : {
                    flowVal = tsData.formFlow;
                    break;
                }
                case DataCaptureSpec::prctTestZone  : {
                    flowVal = tsData.volFlow;
                    break;
                }
            }
            capVal = outputUnits.UserToMetric(prevVal) + flowVal * deltaT;
            break;
        }
        case DataCaptureSpec::ctTestZone : {
            switch (testZoneCapType)
            {
                case DataCaptureSpec::tzctTemperature : {
                    capVal = tsData.tzTemp;
                    break;
                }
                case DataCaptureSpec::tzctCompressibility : {
                    capVal = tsData.tzComp;
                    break;
                }
                case DataCaptureSpec::tzctVolume : {
                    capVal = tsData.tzVol;
                    break;
                }
            }
            break;
        }
        case DataCaptureSpec::ctWaterTable : {
            break;
        }
    }
    return outputUnits.MetricToUser(capVal);
}


WorkingDataCaptureSpec1D::WorkingDataCaptureSpec1D()
{
}
WorkingDataCaptureSpec1D::~WorkingDataCaptureSpec1D()
{
}


void WorkingDataCaptureSpec1D::SetupSpec(const DataCaptureSpec& inSpec,
                                         const double&           inStatic,
                                         const SC_DoubleArray& nodeRadii,
                                         const double&          xyRatio,
                                        const double&           xyAngle)
{
    WorkingDataCaptureSpecBase::SetupSpec(inSpec, inStatic);

    if (captureType == DataCaptureSpec::ctPressure)
    {
        switch (pressureCapType)
        {
            case DataCaptureSpec::ctPressure  : {
                break;
            }
            case DataCaptureSpec::pctRadius  : {
                double capRadius = inSpec.radiusData.GetMetricVal();

                if (control.IsHorizontalAnisotropic())
                {
                    double wellAngle = inSpec.GetObsAngle(); // radians
                    double actualRad  = wellAngle - Radians(xyAngle);

                    double scaleFactor = sqrt(1.0/xyRatio) * Sqr(cos(actualRad)) +
                                         sqrt(xyRatio) * Sqr(sin(actualRad));
                    oneDRadData.SetExtraction(nodeRadii, capRadius, inStatic, scaleFactor);
                }
                else
                    oneDRadData.SetExtraction(nodeRadii, capRadius);

                break;
            }
            case DataCaptureSpec::pctSuperposition  : {
                superData.AllocAndSetSize(inSpec.superData.Size());
                for (int i = 0; i < superData.Size(); i++)
                    superData[i].SetSuper(nodeRadii, *(inSpec.superData[i]));
                break;
            }
        }
    }
}

double  WorkingDataCaptureSpec1D::ApplySpec(const SC_DoubleArray& node1DPressure,
                                          const double&         prevVal,         // prev val of spec for production calc
                                          const double&         deltaT,          // time for production calc
                                          const SeqBoundVals&   tsData)
{
    if (captureType == DataCaptureSpec::ctPressure)
    {
        if (pressureCapType == DataCaptureSpec::pctRadius)
            return outputUnits.MetricToUser(oneDRadData.DoExtraction(node1DPressure));

        if (pressureCapType == DataCaptureSpec::pctSuperposition)
        {
            double capVal = 0.0;
            for (int i = 0; i < superData.Size(); i++)
            {
                double superPress = superData[i].radData.DoExtraction(node1DPressure);
                double deltaP = dcapStaticPressure - superPress;
                switch (superData[i].superOp)
                {
                    case SuperComponent::soAddP : {
                        break;
                    }
                    case SuperComponent::soSubP : {
                        superPress = -superPress;
                        break;
                    }
                    case SuperComponent::soAddDP : {
                        superPress = deltaP;
                        break;
                    }
                    case SuperComponent::soSubDP : {
                        superPress = -deltaP;
                        break;
                    }
                }
                capVal += superPress;
            }
            return outputUnits.MetricToUser(capVal);
        }
    }

    return WorkingDataCaptureSpecBase::ApplySpec(prevVal, deltaT, tsData);
}


WorkingDataCaptureSpec2DUnconfined::WorkingDataCaptureSpec2DUnconfined()
{
}
WorkingDataCaptureSpec2DUnconfined::~WorkingDataCaptureSpec2DUnconfined()
{
}

void WorkingDataCaptureSpec2DUnconfined::SetupSpec(const DataCaptureSpec& inSpec,
                                              const double&          inStatic,
                                       const SC_DoubleArray& nodeRadii,
                                       const SC_DoubleArray& fixedZ,
                                       const double&          thickToPress)
{
    WorkingDataCaptureSpecBase::SetupSpec(inSpec, inStatic);

    double capRadius = inSpec.radiusData.GetMetricVal();
    if (captureType == DataCaptureSpec::ctWaterTable)
    {
        wtRadData.SetExtraction(nodeRadii, capRadius);
    }
    else if (captureType == DataCaptureSpec::ctPressure)
    {
        switch (pressureCapType)
        {
            case DataCaptureSpec::ctPressure  : {
                break;
            }
            case DataCaptureSpec::pctRadius  : {
                pressureRadData.SetExtraction(nodeRadii, fixedZ, thickToPress, capRadius,
                        inSpec.zvalueIsRatio, inSpec.actualZ.GetMetricVal(), inSpec.normalizedZ);
                break;
            }
            case DataCaptureSpec::pctSuperposition  : {
                break;
            }
        }
    }
}

double  WorkingDataCaptureSpec2DUnconfined::ApplySpec(const SC_DoubleArray& uncThickness,
                                          const SC_DoubleMatrix& node2DPressure,
                                          const double&         prevVal,         // prev val of spec for production calc
                                          const double&         deltaT,          // time for production calc
                                          const SeqBoundVals&   tsData)
{
    if ((captureType == DataCaptureSpec::ctPressure) &&
        (pressureCapType == DataCaptureSpec::pctRadius))
        return outputUnits.MetricToUser(pressureRadData.DoExtraction(uncThickness, node2DPressure));

    if (captureType == DataCaptureSpec::ctWaterTable)
        return outputUnits.MetricToUser(wtRadData.DoExtraction(uncThickness));

    return WorkingDataCaptureSpecBase::ApplySpec(prevVal, deltaT, tsData);
}

WorkingDataCaptureSpec2DConfined::WorkingDataCaptureSpec2DConfined()
{
}
WorkingDataCaptureSpec2DConfined::~WorkingDataCaptureSpec2DConfined()
{
}

void WorkingDataCaptureSpec2DConfined::SetupSpec(const DataCaptureSpec& inSpec,
                                              const double&          inStatic,
                                       const SC_DoubleArray& nodeRadii,
                                       const SC_DoubleArray& fixedZ)
{
    WorkingDataCaptureSpecBase::SetupSpec(inSpec, inStatic);

    if (captureType == DataCaptureSpec::ctPressure)
    {
        switch (pressureCapType)
        {
            case DataCaptureSpec::ctPressure  : {
                break;
            }
            case DataCaptureSpec::pctRadius  : {
                double capRadius = inSpec.radiusData.GetMetricVal();
                pressureRadData.SetExtraction(nodeRadii, fixedZ, capRadius,
                        inSpec.zvalueIsRatio, inSpec.actualZ.GetMetricVal(), inSpec.normalizedZ);
                break;
            }
            case DataCaptureSpec::pctSuperposition  : {
                break;
            }
        }
    }
}

double  WorkingDataCaptureSpec2DConfined::ApplySpec(const SC_DoubleMatrix& node2DPressure,
                                                    const   double&         prevVal,         // prev val of spec for production calc
                                                    const double&           deltaT,          // time for production calc
                                                    const SeqBoundVals& tsData)
{
    if ((captureType == DataCaptureSpec::ctPressure) &&
        (pressureCapType == DataCaptureSpec::pctRadius))
        return outputUnits.MetricToUser(pressureRadData.DoExtraction(node2DPressure));

    return WorkingDataCaptureSpecBase::ApplySpec(prevVal, deltaT, tsData);
}




WorkingCaptureDataBase::WorkingCaptureDataBase()
{
}


bool WorkingCaptureDataBase::WorkingCaptureSetup()
{
    nextRestart = 0;
    int ncap = dataCaptureData.Size();

    // reset output data
    currCapturedData = &capturedData;
    for (int i = 0; i < ncap; i++)
    {
        (*currCapturedData)[i].SetEmpty();
    }
    currCapturedData->LastIndex().SetEmpty();

    return true;
}

double WorkingCaptureDataBase::GetDeltaT(const double& testTime)
{
    if ((*currCapturedData)[0].IsEmpty())
        lastMetricTime = testTime;

    UnitIndex timeConv(uTime);
    (*currCapturedData)[0] += timeConv.MetricToUser(testTime);

    double deltaT = testTime - lastMetricTime;

    lastMetricTime = testTime;

    return deltaT;
}


void WorkingCaptureDataBase::ApplySpec(const double& testTime)
{
    for (int i = 0; i < capturedVals.Size(); i++)
    {
        // get ref
        SC_DoubleArray& currVec = (*currCapturedData)[i+1];

        double lastVal = 0.0;   // no production at start
        if (!currVec.IsEmpty())
            lastVal = currVec.LastIndex();

        double newVal = capturedVals[i];

        // for first flow in sequence
        if (RealIsNull(lastVal))
            currVec.SetLastIndex(newVal);

        currVec += newVal;
    }

    if ((nextRestart < productionRestartTimes.Size()) && (!(*currCapturedData)[0].IsEmpty()))
    {
        if (testTime > productionRestartTimes[nextRestart])
        {
            for (int i = 0; i < productionCapture.Size(); i++)
                (*currCapturedData)[productionCapture[i] + 1].SetLastIndex(0.0);

            nextRestart++;
        }
    }
}

void WorkingCaptureDataBase::UpdateCapture()
{
    UnitIndex pConv(uPress);
    capturedDataFO.staticPressureDO.InitLabelAndValue(pConv.MetricToUser(dcapStatic));
}


bool WorkingCaptureData1D::WorkingCaptureSetup(const SC_DoubleArray&    nodeRadii,
                                               const double&            currStatic,
                                               const double&            xyRatio,
                                               const double&            xyAngle)
{
    dcapStatic = currStatic;
    int ncap = dataCaptureData.Size();
    capturedSpec.AllocAndSetSize(ncap);
    capturedVals.AllocAndSetSize(ncap);
    productionCapture.Alloc(ncap);

    // set capture spec
    for (int i = 0; i < ncap; i++)
    {
        capturedSpec[i].SetupSpec(*(dataCaptureData[i]), currStatic, nodeRadii, xyRatio, xyAngle);
        if (capturedSpec[i].captureType == DataCaptureSpec::ctProduction)
            productionCapture += i;
    }

    WorkingCaptureDataBase::WorkingCaptureSetup();

    // slight kluge -- initialize profiles
    SC_DoubleArray dummy;
    profileOutputFO.CaseInit(nodeRadii, dummy, 0.0);

    return true;
}


void WorkingCaptureData1D::ApplySpec(const double&          testTime,
                                   const SC_DoubleArray&    node1DPressure,
                                   const SeqBoundVals&      tsData)
{
    double deltaT = GetDeltaT(testTime);
    for (int i = 0; i < capturedSpec.Size(); i++)
    {
        SC_DoubleArray& currVec = (*currCapturedData)[i+1];
        double lastVal = 0.0;   // no production at start
        if (!currVec.IsEmpty())
            lastVal = currVec.LastIndex();

        capturedVals[i] = capturedSpec[i].ApplySpec(node1DPressure, lastVal, deltaT, tsData);
    }

    WorkingCaptureDataBase::ApplySpec(testTime);
    // slight kluge -- update profiles

    SC_DoubleArray dummyThk;
    SC_DoubleMatrix dummy2D;

    profileOutputFO.CaseAddTimeStep(testTime, node1DPressure, dummyThk, dummy2D);
}



bool WorkingCaptureData2DUnconfined::WorkingCaptureSetup(const SC_DoubleArray&  nodeRadii,
                                             const double&          currStatic,
                                             const double&          currThicknessToPressure, // conversion to calc pressure associated with uncThick
                                             const SC_DoubleArray&  currConstantNodeZ)
{
    dcapStatic = currStatic;

    int ncap = dataCaptureData.Size();
    capturedSpec.AllocAndSetSize(ncap);
    capturedVals.AllocAndSetSize(ncap);
    productionCapture.Alloc(ncap);

    // set capture spec
    for (int i = 0; i < ncap; i++)
    {
        capturedSpec[i].SetupSpec(*(dataCaptureData[i]), currStatic, nodeRadii, currConstantNodeZ, currThicknessToPressure);
        if (capturedSpec[i].captureType == DataCaptureSpec::ctProduction)
            productionCapture += i;
    }

    WorkingCaptureDataBase::WorkingCaptureSetup();

    // slight kluge -- initialize profiles
    profileOutputFO.CaseInit(nodeRadii, currConstantNodeZ, currThicknessToPressure);

    return true;
}


void WorkingCaptureData2DUnconfined::ApplySpec(const double&            testTime,
                                   const SC_DoubleArray&    uncThickness,
                                   const SC_DoubleMatrix&   node2DPressure,
                                   const SeqBoundVals&      tsData)
{
    double deltaT = GetDeltaT(testTime);;
    for (int i = 0; i < capturedSpec.Size(); i++)
    {
        // get ref
        SC_DoubleArray& currVec = (*currCapturedData)[i+1];

        double lastVal = 0.0;   // no production at start
        if (!currVec.IsEmpty())
            lastVal = currVec.LastIndex();

        capturedVals[i] = capturedSpec[i].ApplySpec(uncThickness, node2DPressure, lastVal, deltaT, tsData);
    }

    WorkingCaptureDataBase::ApplySpec(testTime);


    // slight kluge -- update profiles
    SC_DoubleArray dummy;
    profileOutputFO.CaseAddTimeStep(testTime, dummy, uncThickness, node2DPressure);
}


bool WorkingCaptureData2DConfined::WorkingCaptureSetup(const SC_DoubleArray&  nodeRadii,
                                             const double&          currStatic,
                                             const SC_DoubleArray&  currConstantNodeZ)
{
    dcapStatic = currStatic;

    int ncap = dataCaptureData.Size();
    capturedSpec.AllocAndSetSize(ncap);
    capturedVals.AllocAndSetSize(ncap);
    productionCapture.Alloc(ncap);

    // set capture spec
    for (int i = 0; i < ncap; i++)
    {
        capturedSpec[i].SetupSpec(*(dataCaptureData[i]), currStatic, nodeRadii, currConstantNodeZ);
        if (capturedSpec[i].captureType == DataCaptureSpec::ctProduction)
            productionCapture += i;
    }

    WorkingCaptureDataBase::WorkingCaptureSetup();

    // slight kluge -- initialize profiles
    profileOutputFO.CaseInit(nodeRadii, currConstantNodeZ, 0.0);

    return true;
}


void WorkingCaptureData2DConfined::ApplySpec(const double&          testTime,
                                   const SC_DoubleMatrix&   node2DPressure,
                                   const SeqBoundVals&      tsData)
{
    double deltaT = GetDeltaT(testTime);;
    for (int i = 0; i < capturedSpec.Size(); i++)
    {
        // get ref
        SC_DoubleArray& currVec = (*currCapturedData)[i+1];

        double lastVal = 0.0;   // no production at start
        if (!currVec.IsEmpty())
            lastVal = currVec.LastIndex();

        capturedVals[i] = capturedSpec[i].ApplySpec(node2DPressure, lastVal, deltaT, tsData);
    }

    WorkingCaptureDataBase::ApplySpec(testTime);


    // slight kluge -- update profiles
    SC_DoubleArray dummy;
    profileOutputFO.CaseAddTimeStep(testTime, dummy, dummy, node2DPressure);
}


