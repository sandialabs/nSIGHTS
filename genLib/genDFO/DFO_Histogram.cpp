///////////////////////////////////////////////////////////////////////////////////
//
// DFO_Histogram.cpp
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
// DESCRIPTION: Basic functional objects (all derived from FuncObjC)
//              used by all applications, primarily concerned with
//              defining/operating on pen sets and colour maps: Also
//              includes base classes for common operations with
//              different application specific implementations:
//              Calculates a histogram for an input vector.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>
#include <genClass/SC_Statistics.h>

#include <genDFO/DFO_Histogram.h>


DFO_Histogram :: DFO_Histogram(const char* inID) :
    FuncObjC(inID),
    minValueMS("Minimum value"),
    maxValueMS("Maximum value")
{
    autoBinWidth    = false;
    intBinWidth     = false;
    autoLimits      = true;
    logHistogram    = false;
    numberOfBins    = 50;
    cumulative      = false;
    outputXisIndex  = false;
    normalizeY      = false;

    InitCommon();

}

DFO_Histogram::DFO_Histogram(const DFO_Histogram& a) :
    FuncObjC(a),
    minValueMS(a.minValueMS),
    maxValueMS(a.maxValueMS)

{
    LocalCopy(a);
    InitCommon();
}

DFO_Histogram :: ~DFO_Histogram ()
{
}

void DFO_Histogram::InitCommon()
{
    minValueMS.AddPort(*this);
    minValCheckRef = GetLastInPortCheckRef();

    maxValueMS.AddPort(*this);
    maxValCheckRef = GetLastInPortCheckRef();

    AddOutPort(outputXYDO);
    outputXYDO.xyData = &outputXY;

    binWidth = 0.0;
}

DFO_Histogram& DFO_Histogram::operator= (const DFO_Histogram& a)
{
    if (&a != this)
        {
            FuncObjC::operator=(a);
            minValueMS = a.minValueMS;
            maxValueMS = a.maxValueMS;
            LocalCopy(a);
        }
    return *this;
}


void  DFO_Histogram::LocalCopy(const DFO_Histogram& a)
{
    autoBinWidth    = a.autoBinWidth;
    intBinWidth     = a.intBinWidth;
    autoLimits      = a.autoLimits;
    logHistogram    = a.logHistogram;
    numberOfBins    = a.numberOfBins;
    cumulative      = a.cumulative;
    outputXisIndex  = a.outputXisIndex;
    normalizeY      = a.normalizeY;
}

void  DFO_Histogram:: DoStatusChk()
{
    // only check MS ref if not auto
    *minValCheckRef  = ! autoLimits;
    *maxValCheckRef  = ! autoLimits;

    FuncObjC::DoStatusChk();

    outputXY.SetID("histogram");

    if (!autoLimits)
        {
            minValueMS.DoCheck("min value master", *this);
            minValue = minValueMS.GetCurrentValue();
            maxValueMS.DoCheck("max value master", *this);
            maxValue = maxValueMS.GetCurrentValue();
            CheckMinMax();
        }
}

void DFO_Histogram::CheckMinMax()
{
    if (logHistogram && ((maxValue < stdEps) || (minValue < stdEps)))
        SetObjErrMsg("non +ve log limits");

    if (maxValue <= minValue)
        SetObjErrMsg("no span in limits");

    // set output values anyway
    minValueMS.InitLabelAndValue(minValue);
    maxValueMS.InitLabelAndValue(maxValue);
}

void DFO_Histogram:: DoHistogram(const SC_DoubleArray& data)
{
    SC_DoubleArray dummy;
    DoHistogram(data, dummy);
}

void DFO_Histogram:: DoHistogram(const SC_DoubleArray& data,
                                 SC_DoubleArray& binIndex)
{
    bool doBinIndex = (data.Size() == binIndex.Size());
    if (doBinIndex)
        binIndex.FillToSize(-1.0);

    // if auto find min max
    if (autoLimits)
        {
            data.CalcMinMax(minValue, maxValue);
            CheckMinMax();
            if (StatusNotOK())
                return;
        }

    // set bins
    if (logHistogram)
        {
            minValue = log10(minValue);
            maxValue = log10(maxValue);
        }

    int nBins = numberOfBins;
    binWidth = 0.0;
    if (autoBinWidth)
        {

            SC_Statistics stdDevCalc;

            SC_DoubleArray temp(data);
            if (logHistogram)
                temp.Log10();

            int nOK = 0;
            for (int i = 0; i < temp.Size(); i++)
                {
                    double dVal = temp[i];
                    if ((dVal >= minValue) &&  (dVal <= maxValue))
                        temp[nOK++] = dVal;
                }
            temp.SetSize(nOK);


            stdDevCalc.CalcStatistics(temp);

            if ((stdDevCalc.realResults[SC_Statistics::soN] < 2.0) ||
                (stdDevCalc.realResults[SC_Statistics::soVar] < stdEps))
                {
                    SetObjErrMsg("stats bad -- cannot calc auto width");
                    return;
                }

            // Scott, 1979 algorithm for bin width
            // W = (3.49)(std.dev.)[(#samples)^(-1/3)]
            binWidth = 3.49 * stdDevCalc.realResults[SC_Statistics::soStdDev] *
                (pow(stdDevCalc.realResults[SC_Statistics::soN], -0.3333));

            if (binWidth < stdEps)
                {
                    SetObjErrMsg("stats bad -- calc width < 1E-20");
                    return;
                }

            // do we want to reset ???
            minValue = stdDevCalc.realResults[SC_Statistics::soMin];
            maxValue = stdDevCalc.realResults[SC_Statistics::soMax];
            double maxNBins = (maxValue - minValue) / binWidth;
            if (maxNBins > 10000.0)
                {
                    SetObjErrMsg("stats bad -- more than 10000 bins");
                    return;
                }
            nBins = int(ceil(maxNBins));
        }
    if (intBinWidth)
        {
            double maxNBins = maxValue - minValue + 1;
            if (maxNBins > 100000.0)
                {
                    SetObjErrMsg("int bins -- more than 100000 bins");
                    return;
                }
            nBins = int(maxNBins);
        }
    // alloc output
    if (!outputXY.Alloc(nBins))
        {
            SetObjErrMsg("output allocation ??");
            return;
        }

    binWidth = (maxValue - minValue) / double(nBins);
    if(intBinWidth) {
        binWidth = 1.0;
    }
    double currVal = minValue;
    for (int i = 0; i < nBins; i++)
        {
            outputXY.yData[i] = 0.0;
            if (outputXisIndex)
                outputXY.xData[i] = double(i + 1);
            else
                {
                    if(intBinWidth) {
                        outputXY.xData[i] = minValue + binWidth * i;
                    }
                    else {
                        outputXY.xData[i] = currVal + binWidth / 2.0;
                        if (logHistogram)
                            outputXY.xData[i] = InvLgt(outputXY.xData[i]);

                    }
                }

            currVal += binWidth;
        }

    int nvalidCount = 0;
    for (int i = 0; i < data.Size(); i++)
        {
            double dVal = data[i];
            if (RealIsNull(dVal))
                {
                    if (doBinIndex)
                        binIndex[i] = -2.0;
                    continue;
                }

            if (logHistogram)
                {
                    if (dVal < stdEps)
                        continue;
                    dVal = log10(dVal);
                }

            if ((dVal < minValue) || (dVal > maxValue))
                continue;

            int ibinIndex = int(floor((dVal - minValue) / binWidth));

            // pathological case == maxVal
            if (ibinIndex == nBins)
                ibinIndex--;

            outputXY.yData[ibinIndex] += 1.0;

            if (doBinIndex)
                binIndex[i] = double(ibinIndex);

            nvalidCount++;
        }

    if (cumulative)
        {
            for (int j = 1; j < nBins; j++)
                outputXY.yData[j] += outputXY.yData[j - 1];
        }

    if (normalizeY && (nvalidCount > 0))
        {
            double scaleFactor = 1.0 / double(nvalidCount);
            if (!cumulative)
                {
                    if (outputXisIndex)
                        binWidth = 1.0;
                    scaleFactor /= binWidth;
                }

            for (int j = 0; j < nBins; j++)
                outputXY.yData[j] *= scaleFactor;
        }

    outputXY.xData.SetSize(nBins);
    outputXY.yData.SetSize(nBins);

    outputXY.SetID(GetID());
}

