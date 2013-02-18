///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ResidualDiagnostic.cpp
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
//      quantile-normal and standard residual diagnostic calculations.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <osClass/U_Resid.h>

#include <osDFO/DFO_ResidualDiagnostic.h>

DFO_ResidualDiagnostic :: DFO_ResidualDiagnostic() : FuncObjC("ResidualDiagnostic")
{
    diagnosticType = dtQuantileNormal;
    nnormalPoints = 300;
    InitCommon();
}

DFO_ResidualDiagnostic::DFO_ResidualDiagnostic(const DFO_ResidualDiagnostic& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_ResidualDiagnostic :: ~DFO_ResidualDiagnostic ()
{
}

void DFO_ResidualDiagnostic::InitCommon()
{
    AddInPort(residInputObjRef, typeid(DO_ResidData));

    outputXYDO.xyData = &outputXY;
    AddOutPort(outputXYDO);

    outputDistributionDO.xyData = &outputDistributionDC;
    AddOutPort(outputDistributionDO);

    DoStatusChk();
}

DFO_ResidualDiagnostic& DFO_ResidualDiagnostic::operator= (const DFO_ResidualDiagnostic& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ResidualDiagnostic::LocalCopy(const DFO_ResidualDiagnostic& a)
{
    diagnosticType = a.diagnosticType;
    nnormalPoints = a.nnormalPoints;
}


void  DFO_ResidualDiagnostic:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    if (CheckInputFO(residInputObjRef, "input data source"))
    {
        DO_ResidData* tmpDO = static_cast<DO_ResidData*>(GetInPortData(residInputObjRef));
        inputXY = tmpDO->xyData;
        noptPar = tmpDO->noptPararameters;
    }
}


void DFO_ResidualDiagnostic:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;


    outputXY.CreateFrom(inputXY->yData, inputXY->yData);  // ignore X input

    if (diagnosticType == dtStdDiagnostic)
    {
        SetResidIndexes(outputXY.yData);
        StandardizeResiduals(outputXY.xData);
        outputXY.xData.Sort(true);
        StandardNormalDistribution(outputDistributionDC, nnormalPoints, true, -5.0, 5.0);
        outputXY.SetID("StdResidDiag");
        outputDistributionDC.SetID("CumNormalDist");
    }
    else
    {
        int npts = outputXY.Size();
        if (npts <= noptPar)
        {
            SetObjErrMsg("# of residuals <= # of optimized parameters");
            outputXY.SetEmpty();
            outputDistributionDC.SetEmpty();
            return;
        }

        SetResidIndexes(outputXY.xData);
        outputXY.yData.Sort(true);

        SC_DoubleArray& resData = outputXY.yData;
        SC_DoubleArray& qiData  = outputXY.xData;

        double sse = 0.0;
        int i;
        for (i = 0; i < npts; i++)
            sse += Sqr(resData[i]);
        double mse = sse / double(npts - noptPar);
        double sig = sqrt(mse);

        for (i = 0; i < npts; i++)
            qiData[i] = sig * NormInv(qiData[i]);

        outputXY.SetID("Quantile-Normal");

        outputDistributionDC.Alloc(2);
        outputDistributionDC.xData += qiData[0];
        outputDistributionDC.xData += qiData.LastIndex();
        outputDistributionDC.yData = outputDistributionDC.xData;
        outputDistributionDC.SetID("QuantileLine");
    }

}


void  DFO_ResidualDiagnostic::SetResidIndexes(SC_DoubleArray& indexData)
{
    int nRes = indexData.Size();
    for (int i = 0; i < nRes; i++)
        indexData[i] = (double(i + 1) - 0.5) / double(nRes);
}


