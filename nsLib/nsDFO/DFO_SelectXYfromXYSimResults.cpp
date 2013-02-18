///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectXYfromXYSimResults.cpp
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
//      Selects one or more XY results from simulator results.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <nsDFO/DFO_SelectXYfromXYSimResults.h>


DFO_SelectXYfromXYSimResults :: DFO_SelectXYfromXYSimResults() :
    DFO_SelectBase("SelectXYfromXYSimResults", baseMS), baseMS(true, true)
{
    xySelIndex = 0;
    InitCommon();
}

DFO_SelectXYfromXYSimResults::DFO_SelectXYfromXYSimResults(const DFO_SelectXYfromXYSimResults& a) :
    DFO_SelectBase(a, baseMS), baseMS(a.baseMS)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SelectXYfromXYSimResults::InitCommon()
{
    InitMSPorts();

    //  single output
    xyDataDO.xyData = &xyDataDC;
    AddOutPort(xyDataDO);
    sequenceTimesDO.seqTimes = &sequenceTimesDC;
    AddOutPort(sequenceTimesDO);
    AddOutPort(staticPressureDO);
    for (int i = 0; i < maxCasePar; i++)
        AddOutPort(caseParameterDO[i]);

    // multiple output
    xyDataArrayDO.xyDataArray = &xyDataArrayDC; 
    AddOutPort(xyDataArrayDO);

    AddInPort(xySimResultsObjRef, typeid(DO_XYSimResults));

    DoStatusChk();
}

DFO_SelectXYfromXYSimResults& DFO_SelectXYfromXYSimResults::operator= (const DFO_SelectXYfromXYSimResults& a)
{
    if (&a != this)
    {
        DFO_SelectBase::operator=(a);
        baseMS = a.baseMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectXYfromXYSimResults::LocalCopy(const DFO_SelectXYfromXYSimResults& a)
{
    xySimResultsObjRef  = a.xySimResultsObjRef;
    xySelIndex = a.xySelIndex;
}

void  DFO_SelectXYfromXYSimResults:: DoStatusChk()
{
    DFO_SelectBase::DoStatusChk();

    indexMS.SetTypeLabel("Selected Results");
    staticPressureDO.Reset("Static pressure");
    for (int i = 0; i < maxCasePar; i++)
        caseParameterDO[i].Reset("Case param");

    //  check input objects
    if (!CheckInputFO(xySimResultsObjRef, "Data source"))
        return;

    xySimResultsDC = static_cast<DO_XYSimResults*>(GetInPortData(xySimResultsObjRef))->xyResults;

    if (IndexRangeOK(xySimResultsDC->GetnCases()))
    {
        indexMS.indexDO.SetValueLabel("??");
    }

    if (xySelIndex >= xySimResultsDC->GetnXY())
        xySelIndex = 0;

}

void DFO_SelectXYfromXYSimResults:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    xyDataDC.DeAlloc();
    xyDataArrayDC.DeAlloc();
    if (baseMS.CurrentIndexIsSingle())
    {
        AddIndex(indexMS.GetCurrentIndex(), xyDataDC);
        if (!xyDataDC.IsEmpty())
            SetDefaultID(xyDataDC.dataID);
    }
    else
    {
        const SC_IntArray& tmp = baseMS.GetCurrentIndexArray();
        int nSel = tmp.Size();
        xyDataArrayDC.AllocAndSetSize(nSel);
        for (int i = 0; i < nSel; i++)
            AddIndex(tmp[i], xyDataArrayDC[i]);
    }

}



void  DFO_SelectXYfromXYSimResults::AddIndex(int        indx,
                                             DC_XYData& nextXY)
{
    bool found = false;
    for (int i = 0; (!found) && i < xySimResultsDC->Size(); i++)
    {
        XYSimRunResults& currRun = (*xySimResultsDC)[i];

        int nCase = currRun.Size();
        if (indx < nCase)
        {
            XYSimCaseResults& currCase = currRun[indx];
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

            nextXY = currCase.caseData[xySelIndex];

            char tempStr[80];
            MakeString(tempStr, currRun.runID, "/C", currCase.caseID, 80);
            if (!IsNull(nextXY.GetID()))
            {
                ConcatString(tempStr, "/", 80);
                ConcatString(tempStr, nextXY.GetID(), 80);
            }
            nextXY.SetID(tempStr);
            indexMS.indexDO.SetValueLabel(tempStr);

            found = true;
            break;
        }
        indx -= nCase;
    }
}

