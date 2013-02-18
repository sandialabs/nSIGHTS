///////////////////////////////////////////////////////////////////////////////////
//
// DFO_HeadCalculation.cpp
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

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/U_Matrix.h>

#include <objClass/DC_Curve.h>

#include <nsDFO/DFO_HeadCalculation.h>

DFO_HeadCalculation :: DFO_HeadCalculation() :
    FuncObjC("Head Calculation"),
    groundSurfaceElevationMS("Ground surface elevation [mASL]")
{
    headCalculation = hcFreshwater;
    groundSurfaceElevation = 0.0;
    groundSurfaceElevationMS.enteredRealValue = 0.0;
    outputIsElevation = false;
    InitCommon();
}

DFO_HeadCalculation::DFO_HeadCalculation(const DFO_HeadCalculation& a) :
    FuncObjC(a),
    groundSurfaceElevationMS(a.groundSurfaceElevationMS)
{
    LocalCopy(a);
    InitCommon();
}

DFO_HeadCalculation :: ~DFO_HeadCalculation ()
{
}

void DFO_HeadCalculation::InitCommon()
{
    AddInPort(inputPressureObjRef, typeid(DO_XYData));
    AddInPort(inputDensityObjRef, typeid(DO_XYData));
    inputDensityCheckRef = GetLastInPortCheckRef();

    AddOutPort(outputHeadDO);
    outputHeadDO.xyData = &outputHeadDC;

    AddOutPort(outputHydrostaticPressureDO);
    outputHydrostaticPressureDO.xyData = &outputHydrostaticPressureDC;

    groundSurfaceElevationMS.AddPort(*this);


    DoStatusChk();
}

DFO_HeadCalculation& DFO_HeadCalculation::operator= (const DFO_HeadCalculation& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        groundSurfaceElevationMS = a.groundSurfaceElevationMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_HeadCalculation::LocalCopy(const DFO_HeadCalculation& a)
{
    inputPressureObjRef = a.inputPressureObjRef;
    inputDensityObjRef = a.inputDensityObjRef;
    headCalculation = a.headCalculation;
    outputIsElevation = a.outputIsElevation;
}

void  DFO_HeadCalculation:: DoStatusChk()
{
    *inputDensityCheckRef = (headCalculation != hcFreshwater);
    FuncObjC::DoStatusChk();

    inputPressureDC = 0;
    if (CheckInputFO(inputPressureObjRef, "pressure input source"))
        inputPressureDC = static_cast<DO_XYData*>(GetInPortData(inputPressureObjRef))->xyData;

    inputDensityDC = 0;
    if ((headCalculation != hcFreshwater) && CheckInputFO(inputDensityObjRef, "density input source"))
    {   
        inputDensityDC = static_cast<DO_XYData*>(GetInPortData(inputDensityObjRef))->xyData;
        if (!RealsAreSame(inputDensityDC->xData[0], 0.0))
            SetObjErrMsg("Density profile must start at 0.0 depth");
    }

    if (StatusNotOK())
        return;

    groundSurfaceElevationMS.DoCheck("Ground surface elevation", *this);
    groundSurfaceElevation = groundSurfaceElevationMS.GetCurrentValue();
}


void DFO_HeadCalculation:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    const double& gravConst = 9.8065;
    const double& freshwaterDensity  = 1000.0;

    DC_XYData freshwaterDensityDC;
    if (headCalculation == hcFreshwater)
    {
        freshwaterDensityDC.Alloc(2);
        freshwaterDensityDC.xData += 0.0;   
        double minDepth, maxDepth;
        inputPressureDC->xData.CalcMinMax(minDepth, maxDepth);
        freshwaterDensityDC.xData += maxDepth;  
        freshwaterDensityDC.yData += freshwaterDensity; 
        freshwaterDensityDC.yData += freshwaterDensity; 
        inputDensityDC = &freshwaterDensityDC;
    }

    DC_XYData integratedDensity;
    if (!inputDensityDC->Integrate(integratedDensity))
    {
        SetObjErrMsg("error integrating density");
        return;
    }

    outputHydrostaticPressureDC = integratedDensity;
    SC_DoubleArray& outHydro = outputHydrostaticPressureDC.yData;
    for (int i = 0; i < outputHydrostaticPressureDC.Size(); i++)
    {
        outHydro[i] *= gravConst / 1000.0; // kPa conversion
    }

    outputHeadDC  = *inputPressureDC;
    SC_DoubleArray& outHead = outputHeadDC.yData;
    SC_DoubleArray& outDepth = outputHeadDC.xData;

    if (headCalculation != hcDensityNeutral)
    {
        for (int i = 0; i < outHead.Size(); i++)
            outHead[i] = (groundSurfaceElevation - outDepth[i]) + outHead[i] * 1000.0 / (gravConst *  freshwaterDensity);
    }

    if (headCalculation != hcFreshwater)
    {
        if (headCalculation == hcEnvironmental)
        {
            DC_Curve integratedDensityCurve;
            SC_SetupErr curveErr;
            if (!integratedDensityCurve.CreateCurve(integratedDensity, curveErr))
            {
                SetObjErrMsg("error creating integrated density curve");
                return;
            }
            for (int i = 0; i < outHead.Size(); i++)
            {
                const double& Zr = outDepth[i];
                if (Zr > 0.0)
                {
                    double averageDensity = integratedDensityCurve.GetCurveY(Zr) / Zr ; 
                    outHead[i] += (freshwaterDensity - averageDensity) / freshwaterDensity * Zr;
                }
            }
        }
        else
        {
            DC_Curve densityCurve;
            SC_SetupErr curveErr;
            if (!densityCurve.CreateCurve(*inputDensityDC, curveErr))
            {
                SetObjErrMsg("error creating density curve");
                return;
            }
            DC_Curve isopressureCurve;
            if (!isopressureCurve.CreateCurve(outputHydrostaticPressureDC, curveErr))
            {
                SetObjErrMsg("error creating hydrostatic pressure curve");
                return;
            }
            for (int i = 0; i < outHead.Size(); i++)
            {
                double Zr = outDepth[i];
                double densityAtDepth = densityCurve.GetCurveY(Zr);
                double isoPressure = isopressureCurve.GetCurveY(Zr);
                outHead[i] = groundSurfaceElevation + (outHead[i] - isoPressure) * 1000.0 / (gravConst * densityAtDepth);
            }
        }
    }

    if (outputIsElevation)
    {
        for (int i = 0; i < outputHydrostaticPressureDC.Size(); i++)
            outputHydrostaticPressureDC.xData[i] = groundSurfaceElevation - outputHydrostaticPressureDC.xData[i];
        for (int i = 0; i < outDepth.Size(); i++)
            outDepth[i] = groundSurfaceElevation - outDepth[i];
    }

    outputHydrostaticPressureDC.SetID("Density compenstated hydrostatic");
    switch (headCalculation) {
        case hcFreshwater : {
            outputHeadDC.SetID("Freshwater head");
            outputHydrostaticPressureDC.SetID("Freshwater hydrostatic");
            break;
        }
        case hcEnvironmental : {
            outputHeadDC.SetID("Environmental head");
            break;
        }
        case hcDensityNeutral : {
            outputHeadDC.SetID("Density neutral head");
            break;
        }
    }

}


