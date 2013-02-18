///////////////////////////////////////////////////////////////////////////////////
//
// UncertPEM.cpp
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
//      Main optimizer/sampler code and global variables and other support classes used by optimizer and application.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Matrix.h>  //for Jacobi eigenvalue/vector calc
#include <genClass/C_TxtFile.h>

#ifdef LAPACK
#include <genClass/U_Lapack.h>
#endif //LAPACK

#include <osClass/C_SimErr.h>
#include <osMain/G_Sample.h>

#include "UncertPEM.h"


PEM::PEM()
{
}
PEM::~PEM()
{
}

//static TxtFileC debugF("C:/WorkingFiles/Temp/test.out", false);

bool PEM::Initialize(RunnableC&  simulator)
{
    if (!UncertaintyPropagation::Initialize(simulator))
        return false;
    try
    {
       //allocate arrays
        eigenvalues.Alloc(nUncertVars);
        eigenvectors.MatrixAlloc(nUncertVars, nUncertVars);
        secondMoment.Alloc(numM);
        functionPlus.Alloc(numM);
        functionMinus.Alloc(numM);
        functionValues.Alloc(numM);

        //calculate eigenvalues and eigenvectors
        int nrot;  //dummy value for now
        int corrGroup = uncertainVars[0]->GetCorrGroup();  //assumes all variables have same correlation group
        SC_DoubleMatrix corrMat;
        SampVar::GetUncertCorrelationMatrix(corrGroup, corrMat);
    #ifdef LAPACK

        char jobz = 'V';                //compute both eigenvalues and eigenvectors ('N' for just eigenvalues)
        char uplo = 'U';                //upper triangle of A is stored (as opposed to 'L' for lower)
        int n = nUncertVars;            //the size of the problem
        int lda = n;                    //size of the leading dimension of A
        double *A = new double[n*lda];  //symmetric matrix.  On exit, if jobz = 'V', then A contains the orthonormal vectors of A
        double *w = new double[n];      //if info==0 this will hold the eigenvalues in ascending order
        int lwork = 3*n-1;              //size of work area.  if lwork is set to 1 then the optimal value will be the first element in work
        double *work = new double[lwork];//work area.
        int info;                       //return status variable

        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j < n; j++)
            {
                A[i*n+j]=corrMat[j][i];
            }
        }

        dsyev_(&jobz, &uplo, &n, A, &lda, w, work, &lwork, &info);

        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j < n; j++)
            {
                eigenvectors[i][j] = A[i*n+j];
            }
            eigenvalues[i] = w[i];
        }

        delete [] A;
        delete [] w;
        delete [] work;

    #else // LAPACK
        Jacobi(corrMat, nUncertVars, eigenvalues, eigenvectors, nrot);
    #endif // LAPACK
        //NOTE:  obtained different eigenvalues and eigenvectors (less accurate) with this
        //  method than calculated by hand in verification problem (PEM_Example, also spreadsheet
        //  EigenValueCalc.xls) - however, end result is good

        //calculate perturbation steps
        xPlusStep.MatrixAlloc(nUncertVars, nUncertVars);
        xMinusStep.MatrixAlloc(nUncertVars, nUncertVars);

//std::fstream& outF = *debugF.GetStream();
        for (int i = 0; i < nUncertVars; i++)  //eigenvalue loop
        {
            for (int j = 0; j < nUncertVars; j++)   //parameter loop
            {
//              outF  << i << " " << j << std::endl;
                double step = sqrt(double(nUncertVars)) * eigenvectors[j][i] * stdDevVarValues[j];
//outF  << i << " " << j << " " << step << " " << eigenvectors[j][i] << " " << stdDevVarValues[j] << std::endl;
                double plus = meanVarValues[j] + step;
                double minus = meanVarValues[j] - step;
//outF  << xPlusStep[i][j] << " " << xMinusStep[i][j] << std::endl;

                if (isLogVar[j])
                {
                    xPlusStep[i][j] = exp(plus);
                    xMinusStep[i][j] = exp(minus);
                }
                else
                {
                    xPlusStep[i][j] = plus;
                    xMinusStep[i][j] = minus;
                }
            }
        }

    }
    catch (SC_Array::AllocationError)
    {
        return false;
    }

    return true;
}

void  PEM::CalcUncert()
{
    try
    {
        //double meanMetricValue = 0.0;
        //double secondMoment = 0.0;
        meanMetricValues.FillToAlloc(0.0);
        secondMoment.FillToAlloc(0.0);
        meanComponentValues.MatrixSetSize(nUncertVars, numM);
        meanComponentValues.MatrixFill(0.0);
        stdDevComponentValues.MatrixSetSize(nUncertVars, numM);
        stdDevComponentValues.MatrixFill(0.0);
//std::fstream& outF = *debugF.GetStream();
        for (int i = 0; i < nUncertVars; i++)  //eigenvalue loop
        {
            //calc functionPlus
            functionPlus = EvalFunction(xPlusStep[i]);
            functionMinus = EvalFunction(xMinusStep[i]);

            for (int j = 0; j < numM; j++)
            {
//outF << i << " " << j << " " << functionPlus[j] << " " <<  functionMinus[j] << " " << eigenvalues[i] << std::endl;
                double meanComponent = (functionPlus[j] + functionMinus[j]) * eigenvalues[i] / (2.0 * nUncertVars);
                meanMetricValues[j] += meanComponent;
                meanComponentValues[i][j] = meanComponent;

                double stdDevComponent = (Sqr(functionPlus[j]) + Sqr(functionMinus[j])) * eigenvalues[i] / (2.0 * nUncertVars);
                secondMoment[j] += stdDevComponent;
                stdDevComponentValues[i][j] = stdDevComponent;
//outF << i << " " << meanMetricValues[j] << " " <<  secondMoment[j] << std::endl;
            }
        }
        for (int j = 0; j < numM; j++)
        {
            double diff = secondMoment[j] - Sqr(meanMetricValues[j]);
            if (diff < 0.0)  //should always be positive, unless loss of precision
                diff = 0.0;
            stdDevMetricValues[j] = sqrt(double(diff));
        }
    }
    catch (SimError& err)
    {
        throw err;  ///TODO check
    }
}

SC_DoubleArray&  PEM::EvalFunction(const SC_DoubleArray& xStep)
{
//std::fstream& outF = *debugF.GetStream();
//outF << "Eval Func " << std:: endl;

//  for (int j = 0; j < nUncertVars; j++)  //parameter loop
//outF << j << " " << xStep[j] << std::endl;

    CalcFitValue(xStep, functionValues);

//for (int i = 0; i < numM; i ++)
//outF << i << " " << functionValues[i] << std::endl;

    return functionValues;
}

