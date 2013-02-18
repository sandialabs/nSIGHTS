///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ExtractSequenceInterval.cpp
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
//      Extracts interval corresponding to single sequence from XY data
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/SC_DoubleArray.h>

#include <genClass/DO_XYData.h>
#include <genClass/DO_Indx.h>
#include <nsClass/DO_SequenceTimes.h>

#include <nsDFO/DFO_ExtractSequenceInterval.h>

DFO_ExtractSequenceInterval::DFO_ExtractSequenceInterval() :
    FuncObjC("ExtractSequence")
{
    adjustTimeToStart        = false;
    startTimeOffsetValue     = 0.0;

    removeDuplicates        = true; // added v4

    adjustPressureToStart    = false;
    fixedPressureOffset      = false;
    startPressureOffsetValue = 0.0;
    absValuePressure         = false;          // added v1
    log10Pressure            = false;          // added v3
    setStartAndEnd           = false;
    setFinalFlowRate         = false;
    finalFlowRate            = 0.0;

    InitCommon();
}

DFO_ExtractSequenceInterval::DFO_ExtractSequenceInterval(const DFO_ExtractSequenceInterval& a) :
    FuncObjC(a), startSeqMS(a.startSeqMS), endSeqMS(a.endSeqMS)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ExtractSequenceInterval::InitCommon()
{
    startSeqMS.AddPort(*this);
    endSeqMS.AddPort(*this);
    endCheckRef = GetLastInPortCheckRef();

    AddOutPort(xyOutputDataDO);
    xyOutputDataDO.xyData = &xyOutputData;

    AddInPort(xyInputDataObjRef, typeid(DO_XYData));
    AddInPort(seqTimesObjRef, typeid(DO_SequenceTimes));

    DoStatusChk();
}

DFO_ExtractSequenceInterval& DFO_ExtractSequenceInterval::operator= (const DFO_ExtractSequenceInterval& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        startSeqMS = a.startSeqMS;
        endSeqMS   = a.endSeqMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ExtractSequenceInterval::LocalCopy(const DFO_ExtractSequenceInterval& a)
{
    xyInputDataObjRef   = a.xyInputDataObjRef;
    seqTimesObjRef      = a.seqTimesObjRef;

    removeDuplicates        = a.removeDuplicates;

    adjustTimeToStart        = a.adjustTimeToStart;
    startTimeOffsetValue     = a.startTimeOffsetValue;
    adjustPressureToStart    = a.adjustPressureToStart;
    fixedPressureOffset      = a.fixedPressureOffset;
    startPressureOffsetValue = a.startPressureOffsetValue;
    absValuePressure         = a.absValuePressure;
    log10Pressure            = a.log10Pressure;
    setStartAndEnd           = a.setStartAndEnd;
    setFinalFlowRate         = a.setFinalFlowRate;
    finalFlowRate            = a.finalFlowRate;
}

void  DFO_ExtractSequenceInterval:: DoStatusChk()
{
    *endCheckRef = setStartAndEnd;

    FuncObjC::DoStatusChk();
    startSeqMS.SetTypeLabel("Start sequence");
    endSeqMS.SetTypeLabel("End sequence");
    xyOutputData.SetID("sequence");

    //  check input objects
    CheckInputFO(xyInputDataObjRef, "XY data source");
    CheckInputFO(seqTimesObjRef, "Sequence time data source");

    startSeqMS.DoCheck("Start sequence", *this);
    if (setStartAndEnd)
        endSeqMS.DoCheck("End sequence", *this);

    if (StatusNotOK())
        return;

    xyInputDataDC = static_cast<DO_XYData*>(GetInPortData(xyInputDataObjRef))->xyData;
    sequenceTimesDC = static_cast<DO_SequenceTimes*>(GetInPortData(seqTimesObjRef))->seqTimes;

    startSeq = startSeqMS.GetCurrentIndex();
    if (setStartAndEnd)
    {
        endSeq = endSeqMS.GetCurrentIndex();
        if (endSeq < startSeq)
        {
            int tmp = startSeq;
            startSeq = endSeq;
            endSeq = tmp;
        }
    }
    else
        endSeq = startSeq;

    int maxSeq = sequenceTimesDC->Size();
    if ((startSeq < 0) || (endSeq >= sequenceTimesDC->Size()))
        SetObjErrMsg("Sequence out of range");

}

void DFO_ExtractSequenceInterval:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    if (!xyOutputData.CreateFrom(xyInputDataDC->xData, xyInputDataDC->yData))
    {
        SetObjErrMsg("memory allocation ??");
        return;
    }

    SC_DoubleArray& xData = xyOutputData.xData;
    SC_DoubleArray& yData = xyOutputData.yData;

    double minTime = sequenceTimesDC->sequenceStartTimes[startSeq];
    double maxTime = sequenceTimesDC->sequenceStartTimes[endSeq + 1];
    bool pressureFound = false;
    double startPressure;
    int nOK = 0;


    for (int i = 0; i < xyOutputData.Size(); i++)
    {
        double currTime = xData[i];
        if ((currTime >= minTime) && (currTime <= maxTime))
        {
            double currPress = yData[i];
            if (!pressureFound)
            {
                startPressure = currPress;
                pressureFound = true;
            }

            if (adjustTimeToStart)
                currTime = currTime - minTime + startTimeOffsetValue;

            if (adjustPressureToStart)
                if (fixedPressureOffset)
                    currPress = currPress - startPressureOffsetValue;
                else
                    currPress = currPress - startPressure + startPressureOffsetValue;

            if (absValuePressure)
                currPress = fabs(currPress);

            if (log10Pressure)
            {
                if (!IsPositive(currPress))
                    continue;
                currPress = log10(currPress);
            }

            xData[nOK] = currTime;
            yData[nOK] = currPress;
            nOK++;
        }
    }
    xyOutputData.SetSize(nOK);

    // remove duplicates after all other processing
    if (removeDuplicates && (nOK > 2))
    {
        const double timeEps = 1.0E-07;
        xyOutputData.RemoveXDuplicates(timeEps);
        nOK = xyOutputData.Size();
    }


    if (setFinalFlowRate && (nOK > 0))
        yData.LastIndex() = finalFlowRate;

    if (nOK == 0)
    {
        SetObjErrMsg("no input data in interval");
        return;
    }

    xyOutputData.SetID(sequenceTimesDC->sequenceIDs[startSeq]);
    SetDefaultID(xyOutputData.dataID);
}

