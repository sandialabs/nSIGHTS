///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CreateBEETResponseFunction.cpp
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
//      Calculates a response function based on baseline pressure data, barometric
//  pressure data and earth tide data
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/U_Matrix.h>
#ifdef LAPACK
#include <genClass/U_Lapack.h>
#endif // LAPACK

#include <objClass/DC_Curve.h>

#include <nsDFO/DFO_CreateBEETResponseFunction.h>

DFO_CreateBEETResponseFunction :: DFO_CreateBEETResponseFunction() :
    FuncObjC("Create BP/ET Response Function"),
    maxTimeDelayMS("Maximum Time Delay"),
    specTimeSpacingMS("Specified Time Spacing")
{
    calcBE = true;
    calcET = true;

    // regression parameter
    maxTimeDelayMS.enteredRealValue = 12.0;
    timeBase = tbSpecific;
    specTimeSpacingMS.enteredRealValue = 1.0;

    //options
    convertType = 0;

    InitCommon();
}

DFO_CreateBEETResponseFunction::DFO_CreateBEETResponseFunction(const DFO_CreateBEETResponseFunction& a) :
    FuncObjC(a),
    maxTimeDelayMS("Maximum Time Delay"),
    specTimeSpacingMS("Specified Time Spacing")
{
    LocalCopy(a);
    InitCommon();
}

DFO_CreateBEETResponseFunction :: ~DFO_CreateBEETResponseFunction ()
{
}

void DFO_CreateBEETResponseFunction::InitCommon()
{
    AddInPort(pressureInputObjRef, typeid(DO_XYData));
    AddUseFlagInPort(barometricInputObjRef, typeid(DO_XYData), calcBE);
    AddUseFlagInPort(earthTideInputObjRef, typeid(DO_XYData), calcET);

    AddOutPort(outputResponseFuncDO);
    outputResponseFuncDO.xyData = &outputResponseFuncDC;

    /*AddOutPort(baroResponseFuncDO);
    baroResponseFuncDO.xyData = &baroResponseFuncDC;

    AddOutPort(etResponseFuncDO);
    etResponseFuncDO.xyData = &etResponseFuncDC;*/

    maxTimeDelayMS.AddPort(*this);
    specTimeSpacingMS.AddPort(*this);

    DoStatusChk();
}

DFO_CreateBEETResponseFunction& DFO_CreateBEETResponseFunction::operator= (const DFO_CreateBEETResponseFunction& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        maxTimeDelayMS = a.maxTimeDelayMS;
        specTimeSpacingMS = a.specTimeSpacingMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_CreateBEETResponseFunction::LocalCopy(const DFO_CreateBEETResponseFunction& a)
{
    pressureInputObjRef = a.pressureInputObjRef;
    calcBE = a.calcBE;
    barometricInputObjRef = a.barometricInputObjRef;
    calcET = a.calcET;
    earthTideInputObjRef = a.earthTideInputObjRef;

    timeBase = a.timeBase;
    convertType = a.convertType;

}

void  DFO_CreateBEETResponseFunction:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    inputPressureDC = 0;
    CheckInputFO(pressureInputObjRef, "test pressure input source");
    if (StatusNotOK())
        return;

    inputPressureDC = static_cast<DO_XYData*>(GetInPortData(pressureInputObjRef))->xyData;
    double tStart = inputPressureDC->xData[0];
    double tEnd = inputPressureDC->xData.LastIndex();

    inputBarometricDC = 0;
    if (calcBE)
    {
        if (!CheckInputFO(barometricInputObjRef, "barometric pressure input source"))
            return;

        inputBarometricDC = static_cast<DO_XYData*>(GetInPortData(barometricInputObjRef))->xyData;
        if (tStart < inputBarometricDC->xData[0])
            SetObjErrMsg("test pressure start before barometric pressure start");
        if (tEnd > inputBarometricDC->xData.LastIndex())
            SetObjErrMsg("test pressure end after barometric pressure end");
    }

    inputEarthTideDC = 0;
    if (calcET)
    {
        if (!CheckInputFO(earthTideInputObjRef, "earth tide signal input source"))
            return;

        inputEarthTideDC = static_cast<DO_XYData*>(GetInPortData(earthTideInputObjRef))->xyData;
        if (tStart < inputEarthTideDC->xData[0])
            SetObjErrMsg("test pressure start before earth tide signal start");
        if (tEnd > inputEarthTideDC->xData.LastIndex())
            SetObjErrMsg("test pressure end after earth tide signal end");
    }

    if (!calcBE && !calcET)
        SetObjErrMsg("must have barometric or earth tide data");
    if (convertType > 0 && !calcBE)
        SetObjErrMsg("cannot convert pressures without barometric data");

    if (StatusNotOK())
        return;

    maxTimeDelayMS.DoCheck("maximum time delay value source", *this);
    double maxTimeDelay = maxTimeDelayMS.GetCurrentValue();
    if (maxTimeDelay < stdEps)
        SetObjErrMsg("Maximum Time Delay must be greater than 0");

    switch (timeBase){
        case tbSpecific :
        {
            specTimeSpacingMS.DoCheck("specific time spacing value source", *this);
            timeSpacing = specTimeSpacingMS.GetCurrentValue();
            break;
        }
        case tbAvgPressure :
        {
            CalcAvgTimeSpacing(inputPressureDC);
            break;
        }
        case tbAvgBE :
        {
            if (!calcBE)
                SetObjErrMsg("Barometric Data Not Included - Cannot Average Time Spacing");
            else
                CalcAvgTimeSpacing(inputBarometricDC);
            break;
        }
        case tbAvgET :
        {
            if (!calcET)
                SetObjErrMsg("Earth Tide Data Not Included - Cannot Average Time Spacing");
            else
                CalcAvgTimeSpacing(inputEarthTideDC);
            break;
        }
    }

    if (StatusNotOK())
        return;

    if (timeSpacing < stdEps)
        SetObjErrMsg("time spacing must be greater than 0");

    //convert maximum time delay to an integer which describes the time interval lag
    timeIntervalLag = int(maxTimeDelay / timeSpacing);
    if (timeIntervalLag < 0)
        SetObjErrMsg("time delay must be greater than 0");

    double ntime  = (tEnd - tStart) / timeSpacing;
    if ((ntime < 10.0) || (ntime > 1.0E5))
        SetObjErrMsg("less than 10 or more than 100000 pressure points");
    else
        npts = int(ntime);


    if (StatusOK())
    {
        if ((npts - timeIntervalLag) < 10)
            SetObjErrMsg("insufficient points for memory");
    }

}

void DFO_CreateBEETResponseFunction::CalcAvgTimeSpacing(const DC_XYData* inputData)
{
    int numPts = inputData->Size();
    double deltaT;
    double sumDeltaT = 0.0;
    for (int i = 1; i < numPts; i++)
    {
        deltaT = inputData->xData[i] - inputData->xData[i - 1];
        sumDeltaT += deltaT;
    }

    timeSpacing = sumDeltaT / (numPts - 1);
}

void DFO_CreateBEETResponseFunction:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    SC_SetupErr curveErr;
    DC_Curve pressureCurve;
    DC_Curve baroCurve;
    DC_Curve etCurve;

    if (!pressureCurve.CreateCurve(*inputPressureDC, curveErr))
    {
        SetObjErrMsg("error converting input pressure to curve");
        return;
    }
    if (calcBE)
    {
        if (!baroCurve.CreateCurve(*inputBarometricDC, curveErr))
        {
            SetObjErrMsg("error converting barometric pressure to curve");
            return;
        }
    }
    if (calcET)
    {
        if (!etCurve.CreateCurve(*inputEarthTideDC, curveErr))
        {
            SetObjErrMsg("error converting earth tide signal to curve");
            return;
        }
    }

    // based on/extracted from BETCO code (ERMS#540534) from Nate Toll
    // equation numbers based on design doc/user manual for BETCO version 1
    // modified to use GenLib arrays and changed to all 0 based

    int n = npts - 1;
    SC_DoubleArray dWL(n);
    SC_DoubleArray dBP, dET;
    if (calcBE)
        dBP.Alloc(n);
    if (calcET)
        dET.Alloc(n);

    // differenced data (Eqn 4, 5 & 15)
    double time = inputPressureDC->xData[0];
    for(int i = 0; i < n; i++)
    {
        dWL[i] = pressureCurve.GetCurveY(time + timeSpacing) - pressureCurve.GetCurveY(time);
        if (calcBE)
            dBP[i] = baroCurve.GetCurveY(time + timeSpacing) - baroCurve.GetCurveY(time);
        if (calcET)
            dET[i] = etCurve.GetCurveY(time + timeSpacing) - etCurve.GetCurveY(time);

        //correct water levels if required
        if (convertType == 1)
        {
            dWL[i] -= dBP[i];
        }
        else if(convertType == 2)
        {
            dWL[i] += dBP[i];
        }

        time += timeSpacing;
    }


    // change to BETCO local vars for comparison clarity
    int m = timeIntervalLag;            // m is "memory"

    //matrix sizing dependent on whether just baro, earth tides or both are included
    int matrixSize = m + 1;
    int offset = 0;
    if (calcBE && calcET)
    {
        matrixSize += m + 1;
        offset = m + 1; //offset ET data if BP data also included in matrix
    }

    //  BETCO classes (1 based)
    //  Matrix y((n-m),1);   //Create vector Uses Matrix Class (indexing starts at 1
    //  Matrix x(n-m,m+1);   //Create Matrix Uses Matrix Class (indexing starts at 1

    // GenLib equivalents (0 based)
    SC_DoubleArray y(n - m);
    SC_DoubleMatrix x;
    x.FixedMatrixAllocAndSetSize(n - m, matrixSize);

    //populate vector (Eqn 6 or 16)
    for (int i = 0; i < (n - m); i++)
    {
        y[i]= dWL[m + i];
    }

    //populate matrix (Eqn 7 or 17)
    for (int i = 0; i < (n - m); i++)
    {
        for(int k = 0; k < (m + 1); k++)
        {
            if (calcBE)
                x[i][k]= dBP[m + i - k];
            if (calcET)
                x[i][k + offset]= dET[m + i - k];
        }
    }


    outputResponseFuncDC.u.AllocAndSetSize(matrixSize);
    SC_DoubleArray& localU = outputResponseFuncDC.u;

#ifdef LAPACK

    //With LAPACK there is no need to first square the matrix since
    //LAPACK can just perform a QR decomposition of the asymmetric matrix.

    //LAPACK solves Ax=b though by the above variable definitions it looks
    //like we are solving Xu=y
    char trans = 'T';                   //Not working with the transpose
    int M = matrixSize;                 //number of rows
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
        for(int i = 0; i < matrixSize; i++)
        {
            localU[i] = y[i];
        }
    }
    else //info != 0
    {
        SetObjErrMsg("DFO_CreateBEETResponseFunction::CalcOutput = matrix solver failed");
        return;
    }

    delete [] work;

#else //LAPACK

    SC_DoubleArray b(matrixSize);
    SC_DoubleMatrix A;
    A.FixedMatrixAllocAndSetSize(matrixSize, matrixSize);

    // set up for Eqn 18 (8) and 19 (9)
    for (int i = 0; i < matrixSize; i++)
    {
        b[i] = 0.0;
        for (int z = 0; z < (n - m); z++)
             b[i] += x[z][i]* y[z]; // makes yx'

        for(int j = 0; j < matrixSize; j++)
        {
            A[i][j] = 0.0;
            for(int k = 0; k < (n-m); k++)
                 A[i][j] += x[k][i] * x[k][j]; // makes xx'
        }
    }

    // solve for u
    //GaussElim(A,b,localU);    //function from GE.cpp for GE returns u vector (step 5)

    //method in OLDBETCO is to solve A u = b where A = X^T X and b = X^T y to make the system square
    LUsolve(A, b, localU);

#endif //LAPACK
    //response function output
    outputResponseFuncDC.Alloc(m + 1);
    outputResponseFuncDC.SetSize(m + 1);
    outputResponseFuncDC.SetID("Response function");

    //initialize data
    outputResponseFuncDC.memory = m;
    outputResponseFuncDC.timeStep = timeSpacing;
    outputResponseFuncDC.useBE = calcBE;
    outputResponseFuncDC.useET = calcET;
    outputResponseFuncDC.convertType = convertType;

    SC_DoubleArray& timeLag = outputResponseFuncDC.xData;
    SC_DoubleArray& uout = outputResponseFuncDC.yData;

    // create cumulative response function (Eqn 20)
    timeLag[0] = 0.0;
    uout[0] = -localU[0];
    for(int i = 1; i <= m; i++)
    {
        timeLag[i] = double(i) * timeSpacing;
        uout[i] = uout[i-1] - localU[i];
    }
}

