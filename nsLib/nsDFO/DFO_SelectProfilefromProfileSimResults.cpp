///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectProfilefromProfileSimResults.cpp
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
//      Select a single profile from profile results
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <nsDFO/DFO_SelectProfilefromProfileSimResults.h>


DFO_SelectProfilefromProfileSimResults :: DFO_SelectProfilefromProfileSimResults() :
    DFO_SelectBase("SelectProfile", baseMS)
{
    InitCommon();
}

DFO_SelectProfilefromProfileSimResults::DFO_SelectProfilefromProfileSimResults(const DFO_SelectProfilefromProfileSimResults& a) :
    DFO_SelectBase(a, baseMS)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SelectProfilefromProfileSimResults::InitCommon()
{
    InitMSPorts();

    gridDataDO.gridData = &gridDataDC;
    AddOutPort(gridDataDO);
    AddOutPort(extendedProfileCaseDO);

    sequenceTimesDO.seqTimes = &sequenceTimesDC;
    AddOutPort(sequenceTimesDO);

    AddOutPort(staticPressureDO);

    for (int i = 0; i < maxCasePar; i++)
        AddOutPort(caseParameterDO[i]);

    AddInPort(profileSimResultsObjRef, typeid(DO_ProfileSimResults));

    gridDataDC.SetID("Profile");

    DoStatusChk();
}

DFO_SelectProfilefromProfileSimResults& DFO_SelectProfilefromProfileSimResults::operator= (const DFO_SelectProfilefromProfileSimResults& a)
{
    if (&a != this)
    {
        DFO_SelectBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectProfilefromProfileSimResults::LocalCopy(const DFO_SelectProfilefromProfileSimResults& a)
{
    profileSimResultsObjRef  = a.profileSimResultsObjRef;
}

void  DFO_SelectProfilefromProfileSimResults:: DoStatusChk()
{
    DFO_SelectBase::DoStatusChk();
    indexMS.SetTypeLabel("Selected Results");
    staticPressureDO.Reset("Static pressure");
    for (int i = 0; i < maxCasePar; i++)
        caseParameterDO[i].Reset("Case param");

    //  check input objects
    if (!CheckInputFO(profileSimResultsObjRef, "Data source"))
        return;

    profileSimResultsDC = static_cast<DO_ProfileSimResults*>(GetInPortData(profileSimResultsObjRef))->profileResults;

    if (IndexRangeOK(profileSimResultsDC->GetnCases()))
    {
        indexMS.indexDO.SetValueLabel("??");
    }
}

void DFO_SelectProfilefromProfileSimResults:: CalcOutput(FOcalcType  calcType)
{
    extendedProfileCaseDO.profileCaseResults = 0;
    gridDataDO.gridData = 0;

    DoStatusChk();
    if (StatusNotOK())
        return;

    bool found = false;
    int tmpIndex = indexMS.GetCurrentIndex();
    for (int i = 0; (!found) && i < profileSimResultsDC->Size(); i++)
    {
        ProfileSimRunResults& currRun = (*profileSimResultsDC)[i];
        for (int j = 0; j < currRun.Size(); j++)
        {
            ProfileSimCaseResults& currCase = currRun[j];
            if (tmpIndex == 0)
            {
                sequenceTimesDC = currRun.caseSeqData;

                int nPar = currCase.caseValues.Size();
                if (nPar > maxCasePar)
                    nPar = maxCasePar;

                for (int k = 0; k < nPar; k++)
                {
                    caseParameterDO[k].Reset(currRun.caseValDesc[k]);
                    caseParameterDO[k].realVal = currCase.caseValues[k];
                    caseParameterDO[k].SetValueLabel(currCase.caseValues[k]);
                }

                staticPressureDO.realVal = currCase.staticPressure;
                staticPressureDO.SetValueLabel(currCase.staticPressure);

                if (currCase.caseDataIsExtended)
                {
                    extendedProfileCaseDO.profileCaseResults = &currCase.extendedCaseData;
                }
                else
                {
                    if (!gridDataDC.CreateFrom(currCase.caseData))
                    {
                        SetObjErrMsg("grid creation ??");
                        return;
                    }
                    gridDataDO.gridData = &gridDataDC;
                }
                char tempStr[80];
                MakeString(tempStr, currRun.runID, "/", currCase.caseID, 80);
                indexMS.indexDO.SetValueLabel(tempStr);

                found = true;
                break;
            }

            tmpIndex--;
        }
    }

    if (found)
        if (extendedProfileCaseDO.DataOK())
            SetDefaultID("Extended Profile");
        else
            SetDefaultID(gridDataDC.GetID());
}

