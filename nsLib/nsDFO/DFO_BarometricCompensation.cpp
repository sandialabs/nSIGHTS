///////////////////////////////////////////////////////////////////////////////////
//
// DFO_BarometricCompensation.cpp
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

#ifdef LAPACK
#include <genClass/U_Lapack.h>
#endif //LAPACK

#include <objClass/DC_Curve.h>

#include <nsDFO/DFO_BarometricCompensation.h>

DFO_BarometricCompensation :: DFO_BarometricCompensation() :
    FuncObjC("BarometricCompensation"),
    barometricEfficiencyMS("barometric efficiency value")
{
    correctionMethod = cmEfficiency;
    barometricEfficiency = 0.6;
    barometricEfficiencyMS.enteredRealValue = 0.6;

    regressLag = 1;
    regressM = 1;
    evenTimeSpacing = false;
    timeSpacing = 1.0;

    InitCommon();
}

DFO_BarometricCompensation::DFO_BarometricCompensation(const DFO_BarometricCompensation& a) :
    FuncObjC(a),
    barometricEfficiencyMS(a.barometricEfficiencyMS)
{
    LocalCopy(a);
    InitCommon();
}

DFO_BarometricCompensation :: ~DFO_BarometricCompensation ()
{
}

void DFO_BarometricCompensation::InitCommon()
{
    AddInPort(pressureInputObjRef, typeid(DO_XYData));
    AddInPort(barometricInputObjRef, typeid(DO_XYData));

    AddOutPort(outputPressureDO);
    outputPressureDO.xyData = &outputPressureDC;

    AddOutPort(outputResponseFuncDO);
    outputResponseFuncDO.xyData = &outputResponseFuncDC;

    barometricEfficiencyMS.AddPort(*this);
    barometricEfficiencyCheckRef = GetLastInPortCheckRef();


    DoStatusChk();
}

DFO_BarometricCompensation& DFO_BarometricCompensation::operator= (const DFO_BarometricCompensation& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        barometricEfficiencyMS = a.barometricEfficiencyMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_BarometricCompensation::LocalCopy(const DFO_BarometricCompensation& a)
{
    pressureInputObjRef = a.pressureInputObjRef;
    barometricInputObjRef = a.barometricInputObjRef;
    correctionMethod = a.correctionMethod;
    regressLag = a.regressLag;
    regressM = a.regressM;
    evenTimeSpacing = a.evenTimeSpacing;
    timeSpacing = a.timeSpacing;
}

void  DFO_BarometricCompensation:: DoStatusChk()
{
    *barometricEfficiencyCheckRef  = (correctionMethod == cmEfficiency);
    FuncObjC::DoStatusChk();

    inputPressureDC = 0;
    if (CheckInputFO(pressureInputObjRef, "test pressure input source"))
        inputPressureDC = static_cast<DO_XYData*>(GetInPortData(pressureInputObjRef))->xyData;

    inputBarometricDC = 0;
    if (CheckInputFO(barometricInputObjRef, "barometric pressure input source"))
        inputBarometricDC = static_cast<DO_XYData*>(GetInPortData(barometricInputObjRef))->xyData;

    if (StatusNotOK())
        return;

    double tStart = inputPressureDC->xData[0];
    if (tStart < inputBarometricDC->xData[0])
        SetObjErrMsg("test pressure start before barometric pressure start");
    double tEnd = inputPressureDC->xData.LastIndex();
    if (tEnd > inputBarometricDC->xData.LastIndex())
        SetObjErrMsg("test pressure end after barometric pressure end");

    if (correctionMethod == cmEfficiency)
    {
        barometricEfficiencyMS.DoCheck("Barometric efficiency value source", *this);
        barometricEfficiency = barometricEfficiencyMS.GetCurrentValue();
        if ((barometricEfficiency < 0.01) || (barometricEfficiency > 1.0))
            SetObjErrMsg("barometric efficiency must be between 0.01 and 1");
    }
    else
    {
        if (evenTimeSpacing)
        {
            if (timeSpacing < stdEps)
                SetObjErrMsg("time spacing must be +ve");
            else
            {
                double ntime  = (tEnd - tStart) / timeSpacing;
                if ((ntime < 10.0) || (ntime > 1.0E5))
                    SetObjErrMsg("less than 10 or more than 100000 even points");
                else
                {
                    npressure = int(ntime);
                }
            }
        }
        else
        {
            npressure = inputPressureDC->Size();
            if (npressure < 10)
                SetObjErrMsg("expecting at least 10 points");
        }

        if (StatusOK())
        {
            if ((npressure -  regressLag) < 10)
                SetObjErrMsg("insufficient points for lag");

            if ((npressure -  regressM) < 10)
                SetObjErrMsg("insufficient points for memory");
        }
    }

}


void DFO_BarometricCompensation:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    outputPressureDC.DeAlloc();

    SC_SetupErr curveErr;
    if (!baroCurve.CreateCurve(*inputBarometricDC, curveErr))
    {
        SetObjErrMsg("error converting barometric pressure to curve");
        return;
    }

    if (correctionMethod == cmEfficiency)
    {
        CalcEfficiency();
        outputResponseFuncDC.DeAlloc();
        outputResponseFuncDC.SetID("regress n/a");
    }
    else
        CalcRegress();


    outputPressureDC.SetID(GetID());
}


void DFO_BarometricCompensation::CalcEfficiency()
{
    if (!outputPressureDC.CreateFrom(inputPressureDC->xData, inputPressureDC->yData))
    {
        SetObjErrMsg("memory allocation ??");
        return;
    }

    double baro0 = baroCurve.GetCurveY(outputPressureDC.xData[0]);

    for (int i = 1; i < outputPressureDC.Size(); i++)
    {
        double baroP = baroCurve.GetCurveY(outputPressureDC.xData[i]);

        double deltaP = (baroP - baro0) * barometricEfficiency;

        outputPressureDC.yData[i] += deltaP;
    }
}


void DFO_BarometricCompensation::CalcRegress()
{
    if (evenTimeSpacing)
    {
        DC_Curve  pressureCurve;
        SC_SetupErr curveErr;
        if (!pressureCurve.CreateCurve(*inputPressureDC, curveErr))
        {
            SetObjErrMsg("error converting input pressure to curve");
            return;
        }

        DC_XYData evenSpaceInput;
        evenSpaceInput.Alloc(npressure + 2);
        double time = inputPressureDC->xData[0];
        double endTime = inputPressureDC->xData.LastIndex();
        while (time <= endTime) {
            evenSpaceInput.xData += time;
            evenSpaceInput.yData += pressureCurve.GetCurveY(time);
            time += timeSpacing;
        }

        DC_XYData evenSpaceOutput;
        DoRegression(evenSpaceInput, evenSpaceOutput);

        if (!pressureCurve.CreateCurve(evenSpaceOutput, curveErr))
        {
            SetObjErrMsg("error converting output pressure to curve");
            return;
        }

        if (!outputPressureDC.CreateFrom(inputPressureDC->xData, inputPressureDC->yData))
        {
            SetObjErrMsg("memory allocation ??");
            return;
        }

        for (int i = 0; i < outputPressureDC.Size(); i++)
            outputPressureDC.yData[i] = pressureCurve.GetCurveY(outputPressureDC.xData[i]);

    }
    else
    {
        DoRegression(*inputPressureDC, outputPressureDC);
    }



}

void DFO_BarometricCompensation::DoRegression(const DC_XYData& inputPressure,
                                                   DC_XYData& outputPressure)
{
    // based on/extracted from BaroCorrect code from Nate Toll
    // modified to use GenLib arrays and changed to all 0 based

    npressure = inputPressure.Size();
    const SC_DoubleArray& dataTime = inputPressure.xData;
    const SC_DoubleArray& dataWL = inputPressure.yData;

    // assume equal time step, use average time step for dt based calcs
    double avgDeltaT = (dataTime.LastIndex() - dataTime[0]) / double(npressure - 1);

    int dnpressure = npressure - regressLag;

    SC_DoubleArray dBP(dnpressure), dWL(dnpressure);

    // sets array values to differenced data (Eqn 1 & 2)
    for(int i = 0; i < dnpressure; i++)
    {
        dBP[i] = baroCurve.GetCurveY(dataTime[i + regressLag]) - baroCurve.GetCurveY(dataTime[i]);
        dWL[i] = dataWL[i + regressLag] - dataWL[i];
    }

    // change to BaroCorrect local vars for comparison clarity
    int n = dnpressure;         // n is set to the number of differenced data values
    int m = regressM;           // m is "memory"

//  BaroCorrect classes (1 based)
//  Matrix y((n-m),1);   //Create vector Uses Matrix Class (indexing starts at 1
//  Matrix x(n-m,m+1);   //Create Matrix Uses Matrix Class (indexing starts at 1

    // GenLib equivalents (0 based)
    SC_DoubleArray y(n - m);
    SC_DoubleMatrix x;
    x.FixedMatrixAllocAndSetSize(n - m, m + 1);

    //populate vector (Eqn 3)
    for(int i = 0; i < (n-m); i++)
    {
        y[i]= dWL[m + i];
    }

    //populate matrix (Eqn 4)
    for(int i = 0; i < (n-m); i++)
    {
        for(int k = 0; k < (m+1); k++)
        {
            x[i][k]= dBP[m + i - k];
        }
    }

    //results go in here
    SC_DoubleArray u(m+1);

#ifdef LAPACK

    //With LAPACK there is no need to first square the matrix since
    //LAPACK can just perform a QR decomposition of the asymmetric matrix.

    //LAPACK solves Ax=b though by the above variable definitions it looks
    //like we are solving Xu=y
    char trans = 'T';                   //Not working with the transpose
    int M = m+1;                        //number of rows
    int N = n-m;                        //number of columns
    int nrhs = 1;                       //number of right hand sides
    int lda = M;                        //leading dimension of A
    int ldb = N;                        //leading dimension of b
    int lwork = 100*M;                  //size of the work area
    double *work = new double[lwork];   //work area
    int info;                           //success or failure

    dgels_(&trans, &M, &N, &nrhs, (double *) x.buffer, &lda, (double *) y.tListData, &ldb, work, &lwork, &info);

    if(info == 0)
    {
        //copy results over
        for(int i = 0; i < N; i++)
        {
            u[i] = y[i];
        }
    }
    else //info != 0
    {
        SetObjErrMsg("DFO_BarometricCompensation::DoRegression = matrix solver failed");
        return;
    }

    delete [] work;

#else //LAPACK

    SC_DoubleArray b(m + 1);
    SC_DoubleMatrix A;
    A.FixedMatrixAllocAndSetSize(m + 1, m + 1);

    // set up for Eqn 5
    for(int i = 0; i < (m+1); i++)
    {
        b[i] = 0.0;
        for(int z = 0; z < (n-m); z++)
             b[i] += x[z][i]* y[z]; // makes yx'

        for(int j = 0; j < m+1; j++)
        {
            A[i][j] = 0.0;
            for(int k = 0; k < (n-m); k++)
                 A[i][j] += x[k][i] * x[k][j]; // makes xx'
        }
    }


    // solve for u
    GaussElim(A,b,u);    //function from GE.cpp for GE returns u vector (step 5)
#endif //LAPACK

//      U= new double[m+1];
//      Lag= new double[m+1];

    outputResponseFuncDC.Alloc(m + 1);
    outputResponseFuncDC.SetSize(m + 1);
    outputResponseFuncDC.SetID("Response function");

    SC_DoubleArray& lag = outputResponseFuncDC.xData;
    SC_DoubleArray& uout = outputResponseFuncDC.yData;

    // create cumulative response function (Eqn 6)
    lag[0] = 0.0;
    uout[0] = -u[0];
    for(int i = 1; i <= m; i++)
    {
        lag[i] = double(i * regressLag) * avgDeltaT;
        uout[i]= uout[i-1] - u[i];
    }

    // calculate correction vector (ux in Eqn 7)
    SC_DoubleArray cor(n - m);
    double sum = 0.0;
    for (int k = 0; k < (m+1); k++)    //loops calculate the correction for
    {             //each ith WL
        sum += x[0][k] * u[k];
    }
    cor[0] = sum;

    for (int i = 1; i < n-m; i++)
    {
        sum = 0.0;
        for (int k = 0; k < (m+1); k++)
        {
            sum += x[i][k] * u[k];
        }
        cor[i] = cor[i-1] + sum;
    }


    outputPressure.Alloc(n);
    outputPressure.SetSize(n);
    SC_DoubleArray& corWL = outputPressure.yData;
    SC_DoubleArray& corT = outputPressure.xData;

    // set pre-lag
    int lagOffset = m + regressLag;
    for(int i = 0; i < lagOffset; i++)
    {
        corWL[i]= dataWL[0];
        corT[i]= dataTime[i];
    }

    SC_DoubleArray sumdWL(n);
    sumdWL[0] = 0.0; //dWL[0]  this will add first offset twice, set to zero instead;
    for(int i = 1; i < n; i++)
        sumdWL[i] = sumdWL[i-1] + dWL[i - 1];

    // apply correction
    for(int i = lagOffset; i < n; i++)
    {
        corWL[i] = (sumdWL[i] - cor[i - lagOffset]) / double(regressLag) + dataWL[0];
        //note newer version below provides identical results
        //corWL[i] = (dataWL[i] - cor[i - lagOffset]) / double(regressLag);
        corT[i]= dataTime[i];
    }

}

