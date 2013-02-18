///////////////////////////////////////////////////////////////////////////////////
//
// DSC_TimeLimits.cpp
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

#include <genClass/U_Real.h>

#include <nsClass/DO_SequenceTimes.h>

#include <nsDataClass/DSC_TimeLimits.h>

DSC_TimeLimits::DSC_TimeLimits()
{
    timeLimitSelection = tlsAll;
    startSeq    = 0;
    endSeq      = 0;
    startVal    = 0.0;
    endVal      = 1.0E+06;
}

DSC_TimeLimits::DSC_TimeLimits(const DSC_TimeLimits& a)
{

    LocalCopy(a);
}


DSC_TimeLimits& DSC_TimeLimits::operator= (const DSC_TimeLimits& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void DSC_TimeLimits::LocalCopy(const DSC_TimeLimits& a)
{
    timeLimitSelection    = a.timeLimitSelection;
    seqTimesObjRef        = a.seqTimesObjRef;
    startSeq              = a.startSeq    ;
    endSeq                = a.endSeq      ;
    startVal              = a.startVal    ;
    endVal                = a.endVal      ;
}

void DSC_TimeLimits::DoPreCheck()
{
    *seqTimesCheckRef = (timeLimitSelection == tlsSequence);
}

void  DSC_TimeLimits::DoCheck(FuncObjC&  specFO)
{
    switch (timeLimitSelection)
    {
        case tlsAll : {
            break;
        }
        case tlsSequence : {
                if (specFO.CheckInputFO(seqTimesObjRef, "Sequence time data source"))
                {
                    sequenceTimesDC = static_cast<DO_SequenceTimes*>(specFO.GetInPortData(seqTimesObjRef))->seqTimes;
                    if (endSeq < startSeq)
                    {
                        int tmp = startSeq;
                        startSeq = endSeq;
                        endSeq = tmp;
                    }

                    int maxSeq = sequenceTimesDC->Size();
                    if ((startSeq < 0) || (endSeq >= sequenceTimesDC->Size()))
                        specFO.SetObjErrMsg("Sequence out of range");
                    else
                    {
                        timeMin = sequenceTimesDC->sequenceStartTimes[startSeq];
                        timeMax = sequenceTimesDC->sequenceStartTimes[endSeq + 1];
                    }
                }
                break;
        };
        case tlsUserLimits : {
                if (endVal < startVal)
                {
                    double temp = startVal;
                    startVal = endVal;
                    endVal = temp;
                }
                timeMin = startVal;
                timeMax = endVal;

                if (RealsAreSame(startVal, endVal))
                    specFO.SetObjErrMsg("Limits must be different");
                break;
        };
    };
}

void DSC_TimeLimits::AddPort(FuncObjC&   specFO)
{
    specFO.AddInPort(seqTimesObjRef, typeid(DO_SequenceTimes));
    seqTimesCheckRef = specFO.GetLastInPortCheckRef();
}


void DSC_TimeLimits::ExtractLimits(DC_XYData& inData)
{
    if (!AllData())
    {
        int nOK = 0;
        for ( int i = 0; i < inData.Size(); i++)
        {
            double xVal = inData.xData[i];
            if ((xVal >= timeMin) && (xVal <= timeMax))
            {
                inData.xData[nOK] = xVal;
                inData.yData[nOK++] = inData.yData[i];
            }
        }
        inData.SetSize(nOK);
    }
}

