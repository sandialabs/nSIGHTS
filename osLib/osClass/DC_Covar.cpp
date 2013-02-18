///////////////////////////////////////////////////////////////////////////////////
//
// DC_Covar.cpp
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
//      OsLib supports sampling and processing/plotting of optimization output
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/U_Matrix.h>
#include <genClass/C_Common.h>

#ifdef LAPACK
#include <genClass/U_Lapack.h>
#endif //LAPACK

#include <osClass/DC_Covar.h>


double DC_Covar::snglParCnfLimits[]   = {6.63, 4.00, 2.71, 1.00};
double DC_Covar::dualParCnfLimits[]   = {9.21, 6.17, 4.61, 2.30};
double DC_Covar::tripleParCnfLimits[] = {11.3, 8.02, 6.25, 3.53};

DC_Covar::DC_Covar()
{
    SetToNull(covarID);
}


DC_Covar::DC_Covar(const DC_Covar& a)
{
    LocalCopy(a);
}


DC_Covar::~DC_Covar()
{
    // empty
}

DC_Covar& DC_Covar::operator= (const DC_Covar& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }

    return *this;
}


void  DC_Covar::LocalCopy(const DC_Covar& a)
{
    CopyString(covarID, a.covarID, idLen);
    covarFitVals = a.covarFitVals;
    covarMatrix  = a.covarMatrix;
    sumSquaredError = a.sumSquaredError;
}


void DC_Covar::SetActID(const char* runID, const char* caseID, const char* fitID)
{
    MakeString(covarID, "Act:", runID, ":", caseID, ":", fitID, idLen);
}

void DC_Covar::SetEstID(const char* runID, const char* caseID, const char* fitID)
{
    MakeString(covarID, "Est:", runID, ":", caseID, ":", fitID,  idLen);
}

void DC_Covar::SetSubFitID(const char* runID, const char* caseID, const char* fitID, const char* subFitID)
{
    MakeString(covarID, "Sub:", runID, ":", caseID, ":", fitID, ":", subFitID, idLen);
}

void DC_Covar::SetID(const char* runID, const char* caseID, const char* fitID, const char* subFitID, int index)
{
    if (index < 0)
        SetActID(runID, caseID, fitID);
    else if (index == 0)
        SetEstID(runID, caseID, fitID);
    else
        SetSubFitID(runID, caseID, fitID, subFitID);
}



void DC_Covar::CalcSingleLimits(ConfidenceLimits   cl,         // single cnf
                                int                indx,
                                double&            minVal,
                                double&            maxVal) const
{
    if (!CovarOK(indx))
        GenAppInternalError("DC_Covar::CalcSingleLimits");

    double cScale = sqrt(snglParCnfLimits[cl] * covarMatrix[indx][indx]);
    minVal = covarFitVals[indx] - cScale;
    maxVal = covarFitVals[indx] + cScale;
}
void DC_Covar::CalcSingleLimits(ConfidenceLimits   cl,
                                int                xIndx,
                                int                yIndx,
                                SC_PointArray&     stPoints,
                                SC_PointArray&     endPoints)  const
{
    stPoints.AllocAndFill(2, Point2D(covarFitVals[xIndx], covarFitVals[yIndx]));
    endPoints = stPoints;
    CalcSingleLimits(cl, xIndx, stPoints[0].pX, endPoints[0].pX);
    CalcSingleLimits(cl, yIndx, stPoints[1].pY, endPoints[1].pY);
}

void DC_Covar::CalcSingleLimits (ConfidenceLimits   cl,
                                 int                xIndx,
                                 int                yIndx,
                                 int                zIndx,
                                 SC_CoordArray&     stCoords,
                                 SC_CoordArray&     endCoords)  const
{
    stCoords.AllocAndFill(3, Coord3D(covarFitVals[xIndx], covarFitVals[yIndx], covarFitVals[zIndx]));
    endCoords = stCoords;
    CalcSingleLimits(cl, xIndx, stCoords[0].cX, endCoords[0].cX);
    CalcSingleLimits(cl, yIndx, stCoords[1].cY, endCoords[1].cY);
    CalcSingleLimits(cl, zIndx, stCoords[2].cZ, endCoords[2].cZ);
}

void DC_Covar::CalcDualLimits  (ConfidenceLimits   cl,
                                int                xIndx,
                                int                yIndx,
                                int                npts,
                                SC_PointArray&     ellipsePoints)  const
{
    double dumA, dumB;
    CalcDualLimits(cl, xIndx, yIndx, npts, ellipsePoints, dumA, dumB);
}

void DC_Covar::CalcDualLimits  (ConfidenceLimits   cl,
                                int                xIndx,
                                int                yIndx,
                                int                npts,
                                SC_PointArray&     ellipsePoints,
                                double&            a,
                                double&            b)  const
{
    SC_DoubleMatrix covar(2, 2);
    covar[0][0] = covarMatrix[xIndx][xIndx];
    covar[0][1] = covarMatrix[xIndx][yIndx];
    covar[1][0] = covarMatrix[xIndx][yIndx];
    covar[1][1] = covarMatrix[yIndx][yIndx];

#ifdef LAPACK
    LAPACKInvert(covar, 2);
#else //LAPACK
    Invert(covar, 2);
#endif //LAPACK

    SC_DoubleMatrix c1(covar);
    SC_DoubleArray dVec(2);
    SC_DoubleMatrix u(2, 2);

#ifdef LAPACK

    char jobz = 'V';                //compute both eigenvalues and eigenvectors ('N' for just eigenvalues)
    char uplo = 'U';                //upper triangle of A is stored (as opposed to 'L' for lower)
    int n = 2;                      //the size of the problem
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
            A[i*n+j]=covar[j][i];
        }
    }

    dsyev_(&jobz, &uplo, &n, A, &lda, w, work, &lwork, &info);

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            u[i][j] = A[i*n+j];
        }
        dVec[i] = w[i];
    }

    delete [] A;
    delete [] w;
    delete [] work;

#else //LAPACK
    int nrot;
    Jacobi(c1, 2, dVec, u, nrot);
#endif //LAPACK

    if ((dVec[0] <= 0.0) || (dVec[1] <= 0.0))
        throw MathError("-ve Eigenvalues - cannot calculate ellipse");

    double cScale = dualParCnfLimits[cl];
    a = sqrt(cScale / dVec[0]);
    b = sqrt(cScale / dVec[1]);

    int nhalf = npts / 2;
    npts = nhalf + nhalf;
    ellipsePoints.AllocAndSetSize(npts);

    double jInc = 2.0 * a / double(nhalf - 1);
    int j;
    for (j = 0; j < nhalf; j++)
    {
        double x = -a + double(j) * jInc;
        double y;
        if ((j == 0) || (j == (nhalf - 1)))
            y = 0.0;
        else
            y = b / a * sqrt(-Sqr(x) + Sqr(a));

        ellipsePoints[j].pX = x;
        ellipsePoints[j].pY = y;

        ellipsePoints[npts - j - 1].pX = x;
        ellipsePoints[npts - j - 1].pY = -y;

    }

    for (j = 0; j < npts; j++)
    {
        double xp = ellipsePoints[j].pX;
        double yp = ellipsePoints[j].pY;

        ellipsePoints[j].pX = u[0][0] * xp + u[0][1] * yp + covarFitVals[xIndx];
        ellipsePoints[j].pY = u[1][0] * xp + u[1][1] * yp + covarFitVals[yIndx];
    }
}

void DC_Covar::CalcDualLimits(ConfidenceLimits   cl,
                              int                xIndx,
                              int                yIndx,
                              int                zIndx,
                              int                npts,
                              SC_CoordArray&     ellipseCoords)  const
{
    SC_PointArray  ellPts(npts);
    CalcDualLimits(cl, xIndx, yIndx, npts, ellPts);
    ellipseCoords.ConvertPoints(ellPts, covarFitVals[zIndx]);
}



void DC_Covar::CalcTripleLimits (ConfidenceLimits   cl,
                                 int                xIndx,
                                 int                yIndx,
                                 int                zIndx,
                                 int                nmeridian,
                                 int                nslice,
                                 SC_CoordMatrix&    ellipseCoords)  const
{
    SC_DoubleMatrix covar(3, 3);
    covar[0][0] = covarMatrix[xIndx][xIndx];
    covar[0][1] = covarMatrix[xIndx][yIndx];
    covar[0][2] = covarMatrix[xIndx][zIndx];
    covar[1][0] = covarMatrix[xIndx][yIndx];
    covar[1][1] = covarMatrix[yIndx][yIndx];
    covar[1][2] = covarMatrix[yIndx][zIndx];
    covar[2][0] = covarMatrix[xIndx][zIndx];
    covar[2][1] = covarMatrix[yIndx][zIndx];
    covar[2][2] = covarMatrix[zIndx][zIndx];

#ifdef LAPACK
    LAPACKInvert(covar, 3);
#else //LAPACK
    Invert(covar, 3);
#endif //LAPACK

    SC_DoubleMatrix c1(covar);
    SC_DoubleArray dVec(3);
    SC_DoubleMatrix u(3, 3);

#ifdef LAPACK

    char jobz = 'V';                //compute both eigenvalues and eigenvectors ('N' for just eigenvalues)
    char uplo = 'U';                //upper triangle of A is stored (as opposed to 'L' for lower)
    int n = 3;                      //the size of the problem
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
            A[i*n+j]=covar[j][i];
        }
    }

    dsyev_(&jobz, &uplo, &n, A, &lda, w, work, &lwork, &info);

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            u[i][j] = A[i*n+j];
        }
        dVec[i] = w[i];
    }

    delete [] A;
    delete [] w;
    delete [] work;

#else //LAPACK
    int nrot;
    Jacobi(c1, 3, dVec, u, nrot);
#endif //LAPACK

    if ((dVec[0] <= 0.0) || (dVec[1] <= 0.0) || (dVec[2] <= 0.0))
        throw MathError("-ve Eigenvalues - cannot calculate ellipsoid");

    double cScale = tripleParCnfLimits[cl];
    double a = sqrt(cScale / dVec[0]);
    double b = sqrt(cScale / dVec[1]);
    double c = sqrt(cScale / dVec[2]);


    double iInc = 2.0 * pi / double(nmeridian);
    double jInc = 2.0 * c / double(nslice);

    ellipseCoords.MatrixAllocAndSetSize(nmeridian, nslice);

    for (int i = 0; i < nmeridian; i++)
    {
        SC_CoordArray& jVec = ellipseCoords[i];
        double ti = double(i + 1) * iInc;
        for (int j = 0; j < nslice; j++)
        {
            double zij = -c + double(j) * jInc;
            double zterm = 0.0;
            if ((j > 0) && (j < (nslice - 1)))
               zterm = sqrt(1.0 - Sqr(zij) / Sqr(c));

            double xij = a * zterm * cos(ti);
            double yij = b * zterm * sin(ti);

            jVec[j].cX = u[0][0] * xij + u[0][1] * yij + u[0][2] * zij + covarFitVals[xIndx];
            jVec[j].cY = u[1][0] * xij + u[1][1] * yij + u[1][2] * zij + covarFitVals[yIndx];
            jVec[j].cZ = u[2][0] * xij + u[2][1] * yij + u[2][2] * zij + covarFitVals[zIndx];
        }
    }
}



CovarVarDesc::CovarVarDesc()
{
}

CovarVarDesc::CovarVarDesc(const CovarVarDesc& a) : OptVarDescription(a)
{
    LocalCopy(a);
}


CovarVarDesc::~CovarVarDesc()
{
    // empty
}

CovarVarDesc& CovarVarDesc::operator= (const CovarVarDesc& a)
{
    if (&a != this)
    {
        OptVarDescription::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

CovarVarDesc& CovarVarDesc::operator= (const OptVarDescription& a)
{
    if (&a != this)
    {
        OptVarDescription::operator=(a);
    }
    return *this;
}


void  CovarVarDesc::LocalCopy(const CovarVarDesc& a)
{
}


double CovarVarDesc::Normalize(const double& userVal)
{
    if (parIsLinear)
        return (userVal - userMinLim) / (userMaxLim - userMinLim);

    double logMin = log10(userMinLim);

    return  (log10(userVal) - logMin) / (log10(userMaxLim) - logMin);
}


double CovarVarDesc::DeNormalize(const double& normVal)
{
//    if ((normVal < 0.0) || (normVal > 1.0))
//        GenAppInternalError("CovarVarDesc::DeNormalize");

    if (parIsLinear)
        return userMinLim + normVal * (userMaxLim - userMinLim);

    double logMin = log10(userMinLim);
    return pow(10.0,  logMin + normVal * (log10(userMaxLim) - logMin));
}



void DC_CovarHeader::GetIDs(SC_StringArray& ids)
{
    ids.ForceMinStringLen(OptVarDescription::varDescLen);
    ids.Alloc(Size());
    for (int i = 0; i < Size(); i++)
        ids += (*this)[i].varDesc;
}

void DC_CovarHeader::DeNormalize(SC_PointArray&     points,
                                 int                xIndx,
                                 int                yIndx)
{
    CovarVarDesc& xVar = (*this)[xIndx];
    CovarVarDesc& yVar = (*this)[yIndx];
    for ( int i = 0; i < points.Size(); i++)
    {
        points[i].pX = xVar.DeNormalize(points[i].pX);
        points[i].pY = yVar.DeNormalize(points[i].pY);
    }
}

void DC_CovarHeader::DeNormalize(SC_CoordArray&     coords,
                                 int                xIndx,
                                 int                yIndx,
                                 int                zIndx)
{
    CovarVarDesc& xVar = (*this)[xIndx];
    CovarVarDesc& yVar = (*this)[yIndx];
    CovarVarDesc& zVar = (*this)[zIndx];
    for ( int i = 0; i < coords.Size(); i++)
    {
        coords[i].cX = xVar.DeNormalize(coords[i].cX);
        coords[i].cY = yVar.DeNormalize(coords[i].cY);
        coords[i].cZ = zVar.DeNormalize(coords[i].cZ);
    }

}

void DC_CovarHeader::DeNormalize(SC_DoubleMatrix&  covarMatrix)
{
    int n = covarMatrix.Size();

    if (n != Size())
        GenAppInternalError("DC_CovarHeader::DeNormalize(SC_DoubleMatrix&  covarMatrix)");


    SC_DoubleMatrix temp, tMat;
    tMat.MatrixAllocAndSetSize(n, n);
    tMat.MatrixFill(0.0);
    for (int i = 0; i < n; i++)
        tMat[i][i] = (*this)[i].userMaxLim - (*this)[i].userMinLim;

    temp.Multiply(tMat, covarMatrix);
    covarMatrix.Multiply(temp, tMat);
}



DC_FullCovar::DC_FullCovar()
{
}

DC_FullCovar::DC_FullCovar(const DC_CovarHeader& a)
    : DC_CovarHeader(a)
{
}


DC_FullCovar::DC_FullCovar(const DC_FullCovar& a) :
    DC_CovarHeader(a), DC_Covar(a)
{
}

DC_FullCovar& DC_FullCovar::operator=(const DC_FullCovar& a)
{
    if (&a != this)
    {
        DC_CovarHeader::operator=(a);
        DC_Covar::operator=(a);
    }
    return *this;
}


//  set up

void DC_FullCovar::SetHeader(const OptSimRunResults& currRun)
{
    const OptVarDescriptionArray&  varDesc = currRun.varDescriptions;
    AllocAndSetSize(varDesc.Size());
    for (int i = 0; i < varDesc.Size(); i++)
        (*this)[i]= varDesc[i];
}


void DC_FullCovar::SetActualCovar(const OptSimRunResults&   currRun,
                                  const OptSingleFit&       currFit)
{
    SetHeader(currRun);
    covarFitVals = currFit.finalNormParData;
    covarMatrix  = currFit.actualCovariance;
}

void DC_FullCovar::SetEstimatedCovar(const OptSimRunResults&    currRun,
                                  const OptSingleFit&       currFit)
{
    SetHeader(currRun);
    covarFitVals = currFit.finalNormParData;
    covarMatrix  = currFit.estimatedCovariance;
}

void DC_FullCovar::SetSubfitCovar(const OptSimRunResults&   currRun,
                                  const OptSingleFit&       currFit,
                                        int                 subfitIndex)
{
    SetHeader(currRun);

    covarFitVals = currFit.finalNormParData;
    covarMatrix  = currFit.fitComponents[subfitIndex].fitCovariance;
}


void DC_FullCovar::SetCovar(const OptSimRunResults& currRun,
                             const OptSingleFit&    currFit,
                                   int              index)
{
    if (index < 0)
        SetActualCovar(currRun, currFit);
    else
        if (index == 0)
            SetEstimatedCovar(currRun, currFit);
        else
            SetSubfitCovar(currRun, currFit, index - 1);
}



void  DC_FullCovar::DeNormalize()
{
    DC_CovarHeader::DeNormalize(covarMatrix);
}

