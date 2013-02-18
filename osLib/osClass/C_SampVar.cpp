///////////////////////////////////////////////////////////////////////////////////
//
// C_SampVar.cpp
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
//      variable to be sampled.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Real.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/C_Common.h>

#include <genClass/DO_XYCDF.h>

#include <genListClass/C_ListObj.h>

#include <osClass/C_SampVar.h>

SampleControlStatics::UncertType    SampleControlStatics::uncertType = SampleControlStatics::utMonteCarlo;
bool    SampleControlStatics::forceCorrelationsToZero   = false;

//Monte Carlo
int     SampleControlStatics::numberOfTrials            = 100;
int     SampleControlStatics::randSeed                  = 34969827;
int     SampleControlStatics::singleRunRealization      = 0;

bool    SampleControlStatics::samplingIsLHS             = true;
bool    SampleControlStatics::specifyCorrelations       = true;

int     SampleControlStatics::currentTrialIndex = -1;

SC_Random  SampleControlStatics::sampleRandGen;

//FOSM
bool    SampleControlStatics::calcDerivSpan = false;
double  SampleControlStatics::derivSpan = 1.0E-2;

void SampleControlStatics::SetTrialIndex(int indx)
{
    if (indx >= numberOfTrials)
        indx = numberOfTrials - 1;
    currentTrialIndex = indx;
}

bool SampleControlStatics::TrialIndexOK()
{
    return (numberOfTrials > 0) && (currentTrialIndex >= 0) && (currentTrialIndex < numberOfTrials);
}

SVCorrelation& SVCorrelation::operator=(const  SVCorrelation& a)
{
    if (&a != this)
    {
        correlatedVar   =  a.correlatedVar;
        correlationValue =  a.correlationValue;
    }
    return *this;
}

void SVCArray::IsBeingDeleted(SampVar*    testVar)
{
    for (int i = 0; i < Size(); i++)
        if ((*this)[i].correlatedVar ==  testVar)
        {
            (*this)[i] = SVCorrelation();  // set to 0 -- worry about cleanup later
            //  should be only one instance
            break;
        }
}

int SVCArray::CorrelatedIndex(SampVar*    testVar)
{
    for (int i = 0; i < Size(); i++)
        if ((*this)[i].correlatedVar ==  testVar)
            return i;
    return -1;
}

bool SVCArray::IsCorrelated(SampVar*    testVar)
{
    return CorrelatedIndex(testVar) >= 0;
}

void SVCArray::Cleanup()
{
    int nOK = 0;
    for (int i = 0; i < Size(); i++)
        if ((*this)[i].correlatedVar !=  0)
            (*this)[nOK++] = (*this)[i];
    SetSize(nOK);
}


VarBase* SampVar::listHead = 0;


SC_StringArray SampVar::corrGroupIDs;


SampVar::SampVar(bool& isActiveInit, int corrGroupInit, const char* varIDInit, bool defIsLinear)
        : SC_SampVar(defIsLinear), VarBase(isActiveInit, varIDInit, listHead), corrGroup(corrGroupInit)
{
    InitCommon();
}

SampVar::SampVar(bool& isActiveInit, int corrGroupInit, const char* varShortIDInit, const char* varLongIDInit, bool defIsLinear)
        : SC_SampVar(defIsLinear), VarBase(isActiveInit, varShortIDInit, varLongIDInit, listHead), corrGroup(corrGroupInit)
{
    InitCommon();
}

void SampVar::InitCommon()
{
    uncertVal  = nullReal;
    correlations.SetResizable();
    sampledValues = 0;
}

SampVar::~SampVar()
{
    ClearCorrelations();

    RemoveVarBase(listHead);
}

SampVar& SampVar::operator= (const SampVar& a)
{
    if (&a != this)
    {
        SC_SampVar::operator =(a);
        LocalCopy(a);
    }
    return *this;
}


bool SampVar::SampleSetupOK(SC_SetupErr& errData)
{
    userCDFDC = 0;
    // don't check if not set for sampling
    if (!isActive)
        return true;

    return SC_SampVar::SampleSetupOK(errData, varShortID);
}


bool SampVar::UsesAsInput(const FuncObjC* inputObj) const
{
    return isActive && SC_SampVar::UsesAsInput(inputObj);
}

bool SampVar::SampVarOK()
{
    return isActive && TrialIndexOK() &&
           (sampledValues != 0) && (numberOfTrials == sampledValues->Size());
}

bool SampVar::UncertVarOK()
{
    return isActive && (uncertVal != nullReal);
}

double  SampVar::GetCurrentValue(int sampleNum) const
{
    //  sanity check 1 -- must be active
    if (!isActive)
        GenAppInternalError("SampVar::GetSampleVal-1");

    if (sampledValues == 0)
        GenAppInternalError("SampVar::GetSampleVal-2");

    //  sizes and expected size must match
    if (numberOfTrials != sampledValues->Size())
        GenAppInternalError("SampVar::GetSampleVal-3");

    return (*sampledValues)[sampleNum];
}

// base class does nothing
double SampVar::GetFinalValue(int sampleNum) const
{
    return GetCurrentValue(sampleNum);
}

bool SampVar::CheckLimits(int sampleNum) const
{
    return true;
}

double SampVar::GetCurrentValue(const double& currValue) const
{
    return currValue;
}

void SampVar::SetCorrGroup(int inGrp)
{
    if (corrGroup == inGrp)
        return;

    ClearCorrelations();

    corrGroup = inGrp;
}

void SampVar::ClearCorrelations()
{
    // cleanup any correlations to this
    for (SampVar* o = GetFirst(); o; o = o->GetNext())
        if ((o != this) && (o->GroupMatches(corrGroup)))
            o->correlations.IsBeingDeleted(this);
}


double SampVar::GetCorrelation(SampVar* otherVar)  // 1.0 if otherVar == this
                                                   // 0.0 if ! in correlations
                                                   // value otherwise
{
    if (otherVar == this)
        return 1.0;

    for (int i = 0; i < correlations.Size(); i++)
        if (otherVar == correlations[i].correlatedVar)
            return correlations[i].correlationValue;

    return nullReal;
}

double SampVar::GetUncertainCorrelation(SampVar* otherVar)
{
    double cVal = GetCorrelation(otherVar);

    // if no correlation set to 0 if forced
    if (RealIsNull(cVal) && forceCorrelationsToZero)
        return 0.0;

    return cVal;
}

void  SampVar::LocalCopy(const SampVar& a)
{
}

double SampVar::GetVarValue() const
{
    if (IsMonteCarlo())
        return GetSampleVal();
    else
        return uncertVal;
}

double SampVar::GetMinVarValue() const
{
    return GetDistMin();
}
double SampVar::GetMaxVarValue() const
{
    return GetDistMax();
}

int SampVar::GetnActive()
{
    return VarBase::GetnActive(listHead);
}

int SampVar::GetnCorrGroup(int groupToMatch)
{
    int nFound = 0;
    for (SampVar* o = GetFirst(); o; o = o->GetNext())
        if ((o->isActive) && (o->GroupMatches(groupToMatch)))
            nFound ++;

    return nFound;
}

bool SampVar::CorrGroupOK(int groupToMatch)
{
    return GetnCorrGroup(groupToMatch) > 1;
}

void  SampVar::GetCorrGroup(int            groupToMatch,
                            SampVarArray&  corrGroup)
{
    SampVarArray activeArray;  // filled by ctor
    corrGroup.Alloc(activeArray.Size());
    for (int i = 0; i < activeArray.Size(); i++)
        if (activeArray[i]->GroupMatches(groupToMatch))
            corrGroup += activeArray[i];
}

bool SampVar::GetCorrelationMatrix(int              matrixCorrGroup,    // only samples with same corr Group
                                   SC_DoubleMatrix& corrMatrix,         // returned as fully popuated nxn w
                                   SC_StringArray&  corrIDs)
{
    SampVarArray corrData(false);
    GetCorrGroup(matrixCorrGroup, corrData);
    corrIDs.Alloc(corrData.Size());
    for (int i = 0; i < corrData.Size(); i++)
        corrIDs += corrData[i]->varShortID;

    return GetCorrelationMatrix(matrixCorrGroup,  corrMatrix);
}

bool SampVar::GetCorrelationMatrix(int              matrixCorrGroup,    // only samples with same corr Group
                                   SC_DoubleMatrix& corrMatrix)        // returned as fully popuated nxn w

{
    SampVarArray corrData(false);
    GetCorrGroup(matrixCorrGroup, corrData);

    int nGroup = corrData.Size();
    corrMatrix.MatrixAllocAndSetSize(nGroup, nGroup);

    for (int i = 0; i < nGroup; i++)
        for (int j = 0; j < nGroup; j++)
            corrMatrix[i][j] = corrData[i]->GetCorrelation(corrData[j]);

    return (nGroup > 1);
}

bool SampVar::GetUncertCorrelationMatrix(int matrixCorrGroup,    // only samples with same corr Group
                                   SC_DoubleMatrix& corrMatrix)        // returned as fully popuated nxn w

{
    bool retVal = GetCorrelationMatrix(matrixCorrGroup, corrMatrix);
    if (forceCorrelationsToZero)
    {
        for (int i = 0; i < corrMatrix.Size(); i++)
            for (int j = 0; j < corrMatrix[i].Size(); j++)
            {
                if (RealIsNull(corrMatrix[i][j]))
                    corrMatrix[i][j] = 0.0;
            }
    }
    return retVal;
}

void  SampVar::SetCorrelationMatrix(int                     matrixCorrGroup,
                                            const SC_DoubleMatrix&  corrMatrix)
{
    int nGroup = GetnCorrGroup(matrixCorrGroup);

    //  sanity check
    if (nGroup != corrMatrix.Size())
        GenAppInternalError("SampVar::SetCorrelationMatrix");

    SampVarArray corrData(false);
    GetCorrGroup(matrixCorrGroup, corrData);

    for (int i = 0; i < nGroup; i++)
    {
        SVCArray& currCorrs = corrData[i]->correlations;
        currCorrs.SetEmpty();
        for (int j = 0; j < nGroup; j++)
        {
            double rVal = corrMatrix[i][j];
            // only add non null corrs to other vars
            if ((j != i) && (!RealIsNull(rVal)))
                currCorrs +=  SVCorrelation(corrData[j], rVal);
        }
    }
}


SampVar* SampVar::GetNextActive() const
{
    return static_cast<SampVar*>(VarBase::GetNextActive());
}

SampVar* SampVar::GetFirstActive()
{
    return static_cast<SampVar*>(VarBase::GetFirstActive(listHead));
}

void SampVar::SetWasActive()
{
    VarBase::SetWasActive(listHead);
}


SampVar* SampVar::GetNext() const
{
    return static_cast<SampVar*>(nextVB);
}

SampVar* SampVar::GetFirst()
{
    return static_cast<SampVar*>(listHead);
}



void SampVar::GetActiveArray(SampVarArray& activeArray)
{
    // klugy approach
    VarBaseArray temp;
    temp.GetSortedArray(listHead);
    activeArray.CreateFrom(temp);
}

SampVarArray::SampVarArray()
{
    SampVar::GetActiveArray(*this);
}


void SampVarArray::CreateFrom(const VarBaseArray& a)
{
    AllocAndSetSize(a.Size());
    for (int i = 0; i < Size(); i++)
        (*this)[i] = static_cast<SampVar*>(a[i]);
}

void SampVar::AddToListing(ListObjC& listObj) const
{
    listObj.AddStdText("   Distribution", GetDistString());

    if (IsPoisson())
    {
        listObj.AddStdReal("   Mean", poissonLambda);
    }
    else if (IsWeibull())
    {
        listObj.AddStdReal("   Shape (alpha)", weibullShape);
        listObj.AddStdReal("   Scale (beta)", weibullScale);
    }
    else if (IsUserCDF())
    {
        listObj.AddFuncObjRef("   User CDF object", userCDFInputObjRef);
    }
    else if ((svDistribution == svdNormal) || (svDistribution == svdLogNormal))
    {
        listObj.AddStdReal("   Mean", distMean);
        listObj.AddStdReal("   Std deviation", distStdDev);
    }
    else
    {
        listObj.AddStdReal("   Minimum", distMin);
        if ((svDistribution == svdTriangular) || (svDistribution == svdLogTriangular))
            listObj.AddStdReal("   Peak", distPeak);
        listObj.AddStdReal("   Maximum", distMax);

        if (IsBeta())
        {
            listObj.AddStdReal("   P exponent (alpha)", betaP);
            listObj.AddStdReal("   Q exponent (beta)", betaQ);
        }
    }
}


// ********************************************************** sampling stuff


void SampVar::Sample(SC_DoubleArray&   x)
{
    SC_SampVar::Sample(sampleRandGen, numberOfTrials, samplingIsLHS, x);
}

