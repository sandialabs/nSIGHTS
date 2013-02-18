///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectOptCovar.cpp
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
//      selects one or more covariance results from optimizer results.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/SC_DoubleArray.h>

#include <osDFO/DFO_SelectOptCovar.h>


DFO_SelectOptCovar :: DFO_SelectOptCovar() : DFO_SelectOpt("SelectOptCovar", true, true)
{
    selectActualCovar   = false;
    includeSubFits      = false;
    selectOKonly        = true;
    InitCommon();
}

DFO_SelectOptCovar::DFO_SelectOptCovar(const DFO_SelectOptCovar& a) : DFO_SelectOpt(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SelectOptCovar::InitCommon()
{
    covarDO.covarArrayData = &covarDataArray;
    covarDO.covarHeader = &covarHeader;
    AddOutPort(covarDO);

    AddOutPort(shortParNames);
    AddOutPort(longParNames);
    shortParNames.Reset("Short parameter names");
    longParNames.Reset("Full parameter names");

    DoStatusChk();
}

DFO_SelectOptCovar& DFO_SelectOptCovar::operator= (const DFO_SelectOptCovar& a)
{
    if (&a != this)
    {
        DFO_SelectOpt::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectOptCovar::LocalCopy(const DFO_SelectOptCovar& a)
{
    selectActualCovar = a.selectActualCovar;
}

void  DFO_SelectOptCovar:: DoStatusChk()
{
    DFO_SelectOpt::DoStatusChk();
    indexMS.indexDO.SetTypeLabel("Selected Output");

    // make sure that file contains covariance
    if (StatusOK() && (!optSimResultsDC->containsCovariance))
        SetObjErrMsg("no covariance in input");
}

void DFO_SelectOptCovar:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    covarDataArray.AllocAndSetSize(runIndex.Size());

    // set header from first run
    int nopt = optSimResultsDC->noptVar;
    OptVarDescriptionArray&  varDesc = (*optSimResultsDC)[runIndex[0]].varDescriptions;

    covarHeader.AllocAndSetSize(nopt);
    shortParNames.Clear();
    longParNames.Clear();

    for (int i = 0; i < nopt; i++)
    {
        covarHeader[i] = varDesc[i];

        shortParNames.dataCoords += Coord3D(double(i));
        shortParNames.dataLabels += varDesc[i].shortVarDesc;

        longParNames.dataCoords += Coord3D(double(i));
        longParNames.dataLabels += varDesc[i].varDesc;
    }


    OptSimResultsHeader& runHeader = (*optSimResultsDC);

    //   add each covar
    for (int i = 0; i < runIndex.Size(); i++)
    {
        OptSimRunResults& currRun = (*optSimResultsDC)[runIndex[i]];
        OptCaseResults& currCase = currRun.runResults[caseIndex[i]];
        OptSingleFit& currFit = currCase.caseResults[fitIndex[i]];
        DC_Covar&  currCovar =  covarDataArray[i];

        currCovar.covarFitVals = currFit.finalNormParData;
        currCovar.sumSquaredError = currFit.sumSquaredError;

        char caseID[80];
        if (runHeader.IsSingle())
            CopyString(caseID, "Only case", 80);
        else
        {
            if (runHeader.IsPerturb())
                CopyString(caseID, "Pert#", 80);
            else if (runHeader.IsSample())
                CopyString(caseID, "Samp#", 80);
            else
                CopyString(caseID, "Case#", 80);

            ConcatInt(caseIndex[i], caseID, 3, 80);
        }
        const char* fitID = currRun.GetFitID(fitIndex[i]);


        if (includeSubFits)
        {
            OptFitComponent& currSubFit = currFit.fitComponents[subfitIndex[i]];
            currCovar.covarMatrix = currSubFit.fitCovariance;
            const char* subFitID = currRun.GetSubFitID(fitIndex[i], subfitIndex[i]);
            currCovar.SetSubFitID(currRun.runID, caseID, fitID, subFitID);
        }
        else
            if (selectActualCovar)
            {
                currCovar.covarMatrix = currFit.actualCovariance;
                currCovar.SetActID(currRun.runID, caseID, fitID);
            }
            else
            {
                currCovar.covarMatrix = currFit.estimatedCovariance;
                currCovar.SetEstID(currRun.runID, caseID, fitID);
            }
    }

}

