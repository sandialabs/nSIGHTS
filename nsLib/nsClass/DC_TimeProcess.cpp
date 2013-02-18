///////////////////////////////////////////////////////////////////////////////////
//
// DC_TimeProcess.cpp
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
//      Derived from C_TimeProcess and adds calculation
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>


#include <nsClass/DC_TimeProcess.h>


DC_TimeProcess::DC_TimeProcess() :
    timeProcessIsCreated(false)
{
}

DC_TimeProcess::DC_TimeProcess(const DC_TimeProcess& a) :
    TimeProcessDescription(a)
{
    LocalCopy(a);
}

DC_TimeProcess::DC_TimeProcess(const TimeProcessDescription& a) :
    TimeProcessDescription(a),
    timeProcessIsCreated(false)
{
}

DC_TimeProcess::~DC_TimeProcess()
{
    // empty
}

DC_TimeProcess& DC_TimeProcess::operator= (const DC_TimeProcess& a)
{
    if (&a != this)
    {
        TimeProcessDescription::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

DC_TimeProcess& DC_TimeProcess::operator= (const TimeProcessDescription& a)
{
    if (&a != this)
        TimeProcessDescription::operator=(a);
    return *this;
}

void  DC_TimeProcess::LocalCopy(const DC_TimeProcess& a)
{
    timeProcessIsCreated   = a.timeProcessIsCreated;
    tqi = a.tqi;
}


bool DC_TimeProcess::CreateTimeProcess(SC_SetupErr& errData)
{
    timeProcessIsCreated = false;
    if (!TimeProcessSetupOK(errData))
        return false;

    if ((timeProcessType == TimeProcess::tpHorner) || (timeProcessType == TimeProcess::tpArgawal))
        timeProcessIsCreated = true;

    return timeProcessIsCreated;
}

bool DC_TimeProcess::CreateTimeProcess(const DC_XYData&   tqInput,  // for all
                                            SC_SetupErr& errData)
{
    timeProcessIsCreated = false;
    tqi.SetID("not used");
    if (!TimeProcessSetupOK(errData))
        return false;

    if (CreateTimeProcess(errData))
        return true;

    tqi.SetID("ti/qi");
    if (!tqi.CreateFrom(tqInput.xData, tqInput.yData))
    {
        errData.SetConstantError("memory full ??");
        return false;
    }

    if ((tqi.xData.Size() > 1) && (!tqi.xData.IsAscending()))
    {
        errData.SetConstantError("time must be ascending");
        return false;
    }

    // new Bourdet processing
    if (timeProcessType == TimeProcess::tpBourdetSuper)
    {
        tqi.SetEmpty();
        SC_DoubleArray& ti = tqi.xData;
        SC_DoubleArray& qi = tqi.yData;

        const SC_DoubleArray& inT = tqInput.xData;
        const SC_DoubleArray& inQ = tqInput.yData;

        for (int i = 0; i < tqInput.Size(); i++)
        {
            double nextT = inT[i];
            if (!IsPositive(nextT))
                continue;

            double nextQ = inQ[i];

            if ((qi.IsNotEmpty()) &&  (nextQ == qi.LastIndex()))
            {
                ti.LastIndex() = nextT;
            }
            else
            {
                ti += nextT;
                qi += nextQ;
            }
        }

        if (addReplaceLastQ)
        {
            ti += ti.LastIndex() * 2.0;  // synthetic time -- not used
            qi += newLastQ;
        }
    }

    if (tqi.Size() < 2)
    {
        errData.SetConstantError("at least 2 ti/qi data are required");
    }
    else
    {
        SC_DoubleArray& qi = tqi.yData;

        if (addReplaceLastQ && (timeProcessType == TimeProcess::tpHornerSuper))
            qi.LastIndex() = newLastQ;

        double qChk = qi.LastIndex();
        if (timeProcessType == TimeProcess::tpBourdetSuper)
            qChk -= qi.LastIndex(1);

        if (fabs(qChk) < stdEps)
            if (timeProcessType == TimeProcess::tpBourdetSuper)
                errData.SetConstantError("final q = penultimate q");
            else
                errData.SetConstantError("final Q cannot be 0.0");
    }

    timeProcessIsCreated = errData.NoErrors();
    return errData.NoErrors();
}


bool DC_TimeProcess::GetProcessedTime(const double& rawTime,
                                            double& processedTime) const
{
    if (!timeProcessIsCreated)
        GenAppInternalError("DC_TimeProcess::GetTimeProcessY");

    if (RealIsNull(rawTime) || (rawTime < stdEps))
        return false;

    switch(timeProcessType)
    {
        case TimeProcess::tpHorner : {
            processedTime = (hornerTval + rawTime) / rawTime;
            break;
        }
        case TimeProcess::tpArgawal : {
            processedTime = (argawalTpVal * rawTime) / (rawTime + argawalTpVal);
            break;
        }
        case TimeProcess::tpHornerSuper : {
            const SC_DoubleArray& ti = tqi.xData;
            const SC_DoubleArray& qi = tqi.yData;
            double tSum = 0;
            double ti1 = 0.0;
            double tn = ti.LastIndex();
            int nloop = ti.Size();
            for (int i = 0; i < nloop; i++ )
            {
                double t = ti[i];
                tSum += qi[i] * log10((tn - ti1 + rawTime) / (tn - t + rawTime));
                ti1 = t;
            }
            processedTime = tSum / qi.LastIndex();
            break;
        }
        case TimeProcess::tpBourdetSuper : {
            double qSum = 0.0;
            const SC_DoubleArray& ti = tqi.xData;
            const SC_DoubleArray& qi = tqi.yData;
            int nloop = ti.UpperBound();

//          rawTime is deltaT in Bourdet eqn,
//          ti[j] is delta T sub j 

            double endFlowTime = ti.LastIndex(1);
            double startFlowTime = 0.0;
            double startFlow = 0.0;
            for (int i = 0; i < nloop; i++ )
            {
                double deltaFlowTime = endFlowTime - startFlowTime;
                double deltaFlow = qi[i] - startFlow;


                qSum += deltaFlow  * log(deltaFlowTime + rawTime);

                startFlow = qi[i];

                startFlowTime = ti[i];
            }

            qSum /= (qi.LastIndex() - startFlow);

            processedTime = qSum + log(rawTime);
            break;
        }

    }
    return true;
}

bool DC_TimeProcess::GetProcessedTime(const DC_XYData&      inputXY,
                                            DC_XYData&      outputXY) const
{
    outputXY.Alloc(inputXY.Size());
    for (int i = 0; i < inputXY.Size(); i++)
    {
        double tVal;
        if ((!RealIsNull(inputXY.yData[i])) && GetProcessedTime(inputXY.xData[i], tVal))
        {
            outputXY.xData += tVal;
            outputXY.yData += inputXY.yData[i];
        }
    }

    // to keep time ascending ...
    if ((timeProcessType == TimeProcess::tpHornerSuper) ||
        (timeProcessType == TimeProcess::tpHorner))
    {
        outputXY.xData.Reverse();
        outputXY.yData.Reverse();
    }

    return !outputXY.IsEmpty();
}

