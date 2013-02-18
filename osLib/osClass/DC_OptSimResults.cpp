///////////////////////////////////////////////////////////////////////////////////
//
// DC_OptSimResults.cpp
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
//      class to package up all optimizer results for file I/O.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <osClass/DC_OptSimResults.h>

OptFitComponent::OptFitComponent() :
    fitSumSquaredErrors(0.0),
    fitEstimatedVariance(0.0),
    nfitPoints(-1)
{
}

OptFitComponent::OptFitComponent(const OptFitComponent& a)
{
    LocalCopy(a);
}

OptFitComponent&  OptFitComponent::operator=(const OptFitComponent& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void OptFitComponent::LocalCopy(const OptFitComponent& a)
{
    fitSumSquaredErrors     = a.fitSumSquaredErrors  ;
    fitEstimatedVariance    = a.fitEstimatedVariance ;
    nfitPoints              = a.nfitPoints           ;
    fitCovariance           = a.fitCovariance        ;
    fitResiduals            = a.fitResiduals         ;
    fitJacobian             = a.fitJacobian          ;
}


OptSingleFit::OptSingleFit() :
    fitCompletedOK(true),
    nsimsRequired(0),
    niteration(0),
    fitValue(0.0),
    fitDelta(0.0),
    derivSpan(0.0),
    sumSquaredError(0.0),
    meanSquaredError(0.0),
    estimatedVariance(0.0)
{
}

OptSingleFit::OptSingleFit(const OptSingleFit& a)
{
    LocalCopy(a);
}

OptSingleFit&  OptSingleFit::operator=(const OptSingleFit& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void OptSingleFit::LocalCopy(const OptSingleFit& a)
{
    fitCompletedOK      = a.fitCompletedOK   ;
    nsimsRequired       = a.nsimsRequired    ;
    niteration          = a.niteration     ;
    fitValue            = a.fitValue         ;
    fitDelta            = a.fitDelta         ;
    derivSpan           = a.derivSpan        ;
    sumSquaredError     = a.sumSquaredError  ;
    meanSquaredError    = a.meanSquaredError ;
    estimatedVariance   = a.estimatedVariance;
    finalParData        = a.finalParData     ;
    finalNormParData    = a.finalNormParData ;
    startParData        = a.startParData     ;
    startNormParData    = a.startNormParData ;
    calcParData         = a.calcParData      ;
    fitComponents       = a.fitComponents    ;
}

int OptSingleFit::GetnFitPoints() const
{
    int npts = 0;
    for (int i = 0; i < fitComponents.Size(); i++)
        npts += fitComponents[i].nfitPoints;

    return npts;
}

OptCaseResults::OptCaseResults()
{
    caseIndex = -1;
    CopyString(caseDesc, "Case", caseDescLen);
}

OptCaseResults::OptCaseResults(const OptCaseResults& a)
{
    LocalCopy(a);
}

OptCaseResults&  OptCaseResults::operator=(const OptCaseResults& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void  OptCaseResults::Cleanup()
{
    caseParVals.DeAlloc();
    caseResults.DeAlloc();
}


void OptCaseResults::LocalCopy(const OptCaseResults& a)
{
    CopyString(caseDesc, a.caseDesc, caseDescLen);
    caseParVals = a.caseParVals;
    caseResults = a.caseResults;
    caseIndex  = a.caseIndex;
}

OptFitComponentDescription::OptFitComponentDescription() :
    nfitPoints(0),
    fitWasL1(false),
    fitWasNormed(false),
    fitChisqSigma(1.0)
{
    CopyString(fitID, "Fit", fitIDLen);
}

OptFitComponentDescription::OptFitComponentDescription(const OptFitComponentDescription& a)
{
    LocalCopy(a);
}

OptFitComponentDescription&  OptFitComponentDescription::operator=(const OptFitComponentDescription& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void OptFitComponentDescription::LocalCopy(const OptFitComponentDescription& a)
{
    CopyString(fitID, a.fitID, fitIDLen);
    nfitPoints      = a.nfitPoints   ;
    fitWasL1        = a.fitWasL1     ;
    fitWasNormed    = a.fitWasNormed ;
    fitChisqSigma   = a.fitChisqSigma;
    fitResidJacobX  = a.fitResidJacobX;
}


OptFitDescription::OptFitDescription()
{
    CopyString(fitID, "CompFit", fitIDLen);
}

OptFitDescription::OptFitDescription(const OptFitDescription& a)
{
    LocalCopy(a);
}

OptFitDescription&  OptFitDescription::operator=(const OptFitDescription& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void OptFitDescription::LocalCopy(const OptFitDescription& a)
{
    CopyString(fitID, a.fitID, fitIDLen);
    fitScaling              = a.fitScaling;
    fitComponentDescription = a.fitComponentDescription;
}

OptVarDescription::OptVarDescription() :
    userMinLim(0.0),  // in user units
    userMaxLim(1.0),
    parSigma(1.0),
    parIsLinear(true)

{
    CopyString(varDesc, "OptVar", varDescLen);
    CopyString(shortVarDesc, "OV", shortVarDescLen);
    CopyString(varUnits, "[]", varUnitsLen);
}

OptVarDescription::OptVarDescription(const OptVarDescription& a)
{
    LocalCopy(a);
}

OptVarDescription&  OptVarDescription::operator=(const OptVarDescription& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void OptVarDescription::LocalCopy(const OptVarDescription& a)
{
    CopyString(varDesc, a.varDesc, varDescLen);
    CopyString(shortVarDesc, a.shortVarDesc, shortVarDescLen);
    CopyString(varUnits, a.varUnits, varUnitsLen);
    userMinLim  = a.userMinLim ;
    userMaxLim  = a.userMaxLim ;
    parSigma    = a.parSigma   ;
    parIsLinear = a.parIsLinear;
}

CaseVarDescription::CaseVarDescription()
{
    CopyString(caseVarDesc, "CaseVar", caseVarDescLen);
    CopyString(caseVarShortDesc, "CV", caseVarShortDescLen);
    CopyString(caseVarUnits, "[]", caseVarUnitsLen);
    varIsLinear = true;
}

CaseVarDescription::CaseVarDescription(const CaseVarDescription& a)
{
    LocalCopy(a);
}

CaseVarDescription&  CaseVarDescription::operator=(const CaseVarDescription& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void CaseVarDescription::SetVars(const char* vDes, const char* vShort, const char* vUn, bool isLin)
{
    CopyString(caseVarDesc, vDes, caseVarDescLen);
    CopyString(caseVarShortDesc, vShort, caseVarShortDescLen);
    CopyString(caseVarUnits, vUn, caseVarUnitsLen);
    varIsLinear = isLin;
}


void CaseVarDescription::LocalCopy(const CaseVarDescription& a)
{
    CopyString(caseVarDesc, a.caseVarDesc, caseVarDescLen);
    CopyString(caseVarShortDesc, a.caseVarShortDesc, caseVarShortDescLen);
    CopyString(caseVarUnits, a.caseVarUnits, caseVarUnitsLen);
    varIsLinear = a.varIsLinear;
}


OptSimRunResults::OptSimRunResults()
{
    runID[0] = '\0';
    fullCovarianceCalc = false;
    CopyString(optDescription, "not set", optDescLen);
}

OptSimRunResults::OptSimRunResults(const OptSimRunResults& a)
{
    LocalCopy(a);
}


OptSimRunResults& OptSimRunResults::operator=(const OptSimRunResults& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void OptSimRunResults::LocalCopy(const OptSimRunResults& a)
{
    CopyString(runID, a.runID, runIDLen);
    CopyString(optDescription, a.optDescription, optDescLen);
    fullCovarianceCalc = a.fullCovarianceCalc;
    fitDescriptions  = a.fitDescriptions;
    varDescriptions  = a.varDescriptions;
    calcDescriptions = a.calcDescriptions;
    caseDescriptions = a.caseDescriptions;
    runResults       = a.runResults;
    nFits            = a.nFits;
    ncaseDesc        = a.ncaseDesc;

}

const char* OptSimRunResults::GetFitID(int fitIndex) const
{
    return fitDescriptions[fitIndex].fitID;
}

const char* OptSimRunResults::GetSubFitID(int fitIndex, int subfitIndex) const
{
    return fitDescriptions[fitIndex].fitComponentDescription[subfitIndex].fitID;
}



void OptSimRunResults::GetFitIDs(SC_StringArray&  fitIDs) const
{
    fitIDs.ForceMinStringLen(OptFitDescription::fitIDLen);
    fitIDs.Alloc(fitDescriptions.Size());
    for (int i = 0; i < fitDescriptions.Size(); i++)
        fitIDs += fitDescriptions[i].fitID;
};


void  OptSimRunResults::Cleanup()
{
    nFits = 0;
    ncaseDesc = 0;
    fitDescriptions.DeAlloc();
    varDescriptions.DeAlloc();
    caseDescriptions.DeAlloc();
    runResults.DeAlloc();
}


OptSimResultsHeader::OptSimResultsHeader()
{
    runType = rtSingle;
    containsCovariance = false;
    containsResiduals  = false;
    containsJacobian   = false;
    noptVar            = 0;
    ncalcVar           = 0;
}

OptSimResultsHeader::OptSimResultsHeader(const OptSimResultsHeader& a)
{
    LocalCopy(a);
}


OptSimResultsHeader& OptSimResultsHeader::operator=(const OptSimResultsHeader& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

bool OptSimResultsHeader::AreCompatible(const OptSimResultsHeader& a)
{
    return  (runType == a.runType)  &&
            (noptVar == a.noptVar) &&
            (ncalcVar == a.ncalcVar) &&
           ((containsCovariance && a.containsCovariance) || ((!containsCovariance) && (!a.containsCovariance))) &&
           ((containsResiduals && a.containsResiduals) || ((!containsResiduals) && (!a.containsResiduals))) &&
           ((containsJacobian && a.containsJacobian) || ((!containsJacobian) && (!a.containsJacobian)));
}


void OptSimResultsHeader::LocalCopy(const OptSimResultsHeader& a)
{
    runType            = a.runType;
    containsCovariance = a.containsCovariance;
    containsResiduals  = a.containsResiduals ;
    containsJacobian   = a.containsJacobian  ;
    noptVar            = a.noptVar;
    ncalcVar           = a.ncalcVar;
}


DC_OptSimResults::DC_OptSimResults() : T_SC_Array<OptSimRunResults>()
{
}

DC_OptSimResults::DC_OptSimResults(const DC_OptSimResults& a) :
    T_SC_Array<OptSimRunResults>(a), OptSimResultsHeader(a)
{
    LocalCopy(a);
}

DC_OptSimResults::~DC_OptSimResults()
{
}

DC_OptSimResults& DC_OptSimResults::operator=(const DC_OptSimResults& a)
{
    if (&a != this)
    {
        T_SC_Array<OptSimRunResults>::operator=(a);
        OptSimResultsHeader::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void DC_OptSimResults::LocalCopy(const DC_OptSimResults& a)
{
}

void  DC_OptSimResults::GetRunIDs(SC_StringArray&  runIDs) const
{
    runIDs.ForceMinStringLen(OptSimRunResults::runIDLen);
    runIDs.Alloc(Size());
    for (int i = 0; i < Size(); i++)
        runIDs += (*this)[i].runID;
};



int DC_OptSimResults::GetnCases() const
{
    int nCase = 0;
    for (int i = 0; i < Size(); i++)
        nCase += (*this)[i].GetnCases();
    return nCase;
}


int  DC_OptSimResults::GetnFits() const
{
    int nfits = 0;
    for (int i = 0; i < Size(); i++)
    {
        OptCaseResultsArray& currCases = (*this)[i].runResults;
        for (int j = 0; j < currCases.Size(); j++)
            nfits += currCases[j].GetnFits();
    }

    return nfits;
}

int  DC_OptSimResults::GetnFitComponents() const
{
    int nfitComp = 0;
    for (int i = 0; i < Size(); i++)
    {
        OptCaseResultsArray& currCases = (*this)[i].runResults;
        for (int j = 0; j < currCases.Size(); j++)
        {
            OptSingleFitArray&   currFits = currCases[j].caseResults;
            for (int k = 0; k < currFits.Size(); k++)
                nfitComp += currFits[k].GetnFitComponent();
        }
    }
    return nfitComp;
}


// for use in menus
void DC_OptSimResults::GetMenuStrings(SC_StringArray&  ids,
                                      bool             includeFitComponents,
                                      bool             includeOKonly) const
{

    static const int maxStrLen = OptSimRunResults::runIDLen + 2 +
                                 CaseVarDescription::caseVarDescLen + 2 +
                                 OptFitDescription::fitIDLen  + 2 +
                                 OptFitComponentDescription::fitIDLen;

    ids.ForceMinStringLen(maxStrLen);
    if (includeFitComponents)
    {
        ids.Alloc(GetnFitComponents());
    }
    else
    {
        ids.Alloc(GetnFits());
    }

    char runStr[maxStrLen];
    char caseStr[maxStrLen];
    char fitStr[maxStrLen];
    char fitCompStr[maxStrLen];
    char okStr[maxStrLen];

    for (int i = 0; i < Size(); i++)
    {
        OptCaseResultsArray& currCases = (*this)[i].runResults;

        CopyString(runStr, (*this)[i].runID, maxStrLen);

        for (int j = 0; j < currCases.Size(); j++)
        {
            CopyString(caseStr, currCases[j].caseDesc, maxStrLen);

            int nfit = currCases[j].caseResults.Size();

            OptSingleFitArray&   currFits = currCases[j].caseResults;

            for (int k = 0; k < currFits.Size(); k++)
            {
                if (includeOKonly && (!currFits[k].fitCompletedOK))
                    continue;

                if (currFits[k].fitCompletedOK)
                {
                    CopyString(okStr, " \t OK", maxStrLen);
                }
                else
                {
                    CopyString(okStr, " \t Bad", maxStrLen);
                }

                CopyString(fitStr, runStr, maxStrLen);
                ConcatString(fitStr, "\t", maxStrLen);
                ConcatString(fitStr, caseStr, maxStrLen);
                ConcatString(fitStr, "\t", maxStrLen);
                ConcatString(fitStr, (*this)[i].fitDescriptions[k].fitID, maxStrLen);
                if (!includeOKonly)
                    ConcatString(fitStr, okStr, maxStrLen);

                if (includeFitComponents)
                {
                    OptFitComponentArray&   currFitComp = currFits[k].fitComponents;

                    for (int l = 0; l < currFitComp.Size(); l++)
                    {
                        CopyString(fitCompStr, fitStr, maxStrLen);
                        ConcatString(fitCompStr, "\t", maxStrLen);
                        ConcatString(fitCompStr, (*this)[i].fitDescriptions[k].fitComponentDescription[l].fitID, maxStrLen);
                        ids += fitCompStr;
                    }
                }
                else
                {
                    ids += fitStr;
                }
            }
        }
    }
}

