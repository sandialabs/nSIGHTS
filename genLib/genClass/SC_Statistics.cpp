///////////////////////////////////////////////////////////////////////////////////
//
// SC_Statistics.cpp
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
// DESCRIPTION:Generic support classes: performs univariate statistics calculations
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>
#include <genClass/SC_Random.h>

#include <genClass/U_Real.h>
#include <genClass/U_Msg.h>

#include <genClass/SC_Statistics.h>


const char* SC_Statistics::opIDs[maxStatisticType] = {
    "Sum",          "Minimum",      "Maximum",      "Mean",         "Variance",
    "Std. dev.",    "Range",        "N",            "N null",       "N non +ve",
    "% non null",
    "Median", "Mode",
    "Min Abs",      "Min +ve",
    "Min Index ",   "Max Index",    "Min Abs index", "Min +ve index",
    "Geometric Mean", "Harmonic Mean",
};



SC_Statistics::SC_Statistics()
{
    Init();
}

SC_Statistics::SC_Statistics(const SC_Statistics& a)
{
    LocalCopy(a);
}

SC_Statistics& SC_Statistics::operator= (const SC_Statistics& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void SC_Statistics::LocalCopy(const SC_Statistics& a)
{
    for (int i = 0; i < maxStatisticType; i++)
        {
            intResults[i] = a.intResults[i];
            realResults[i] = a.realResults[i];
        }

    resultsOK = a.resultsOK;
}

void SC_Statistics::Init()
{
    for (int i = 0; i < maxStatisticType; i++)
        {
            realResults[i] = nullReal;
            intResults[i] = -1;
        }

    realResults[soSum]      = 0.0;
    realResults[soN]        = 0.0;
    realResults[soNnull]    = 0.0;
    realResults[soNnegZero] = 0.0;
    realResults[soRange]    = 0.0;

    intResults[soN]         = 0;
    intResults[soNnull]     = 0;
    intResults[soNnegZero]  = 0;

    realResults[soMinIndx]    = -1.0;
    realResults[soMaxIndx]    = -1.0;
    realResults[soMinAbsIndx] = -1.0;
    realResults[soMinPosIndx] = -1.0;

    resultsOK = false;
}


bool SC_Statistics::IsInteger(int indx)
{
    if ((indx < 0) || (indx >= maxStatisticType))
        GenAppInternalError("SC_Statistics::IsInteger");

    return  (indx == soN) ||
        (indx == soNnull) ||
        (indx == soNnegZero) ||
        ((indx >= soMinIndx) && (indx <= soMinPosIndx));
}


void SC_Statistics::CalcStatistics(const SC_DoubleArray& data,
                                   bool logData, bool baseTen)
{
    Init();
    if (data.IsEmpty())
        return;

    int nData = data.Size();

    double sum = 0.0;
    double geoSum = 0.0, harmSum = 0.0; //for geometric and harmonic means
    double minV, maxV, minAV;
    int minIndx, maxIndx, minAbsIndx;

    double minPV = nullReal;
    int minPosIndx = -1;

    nOK = 0;
    nPositive = 0;

    int nnegZero = 0;
    for (int i = 0; i < nData; i++)
        {
            double dVal = data[i];
            if (dVal < 1.0E-99)
                nnegZero++;

            if (RealIsNull(dVal) || (logData && (dVal < 1.0E-99)))
                continue;

            // this used to be calculated after min/max
            // changed for SyView 1.2 - Mar 06
            // minVal, maxVal, minAV, and minIndxs now based/reported on logged data
            if (logData)
                {
                    if (baseTen)
                        {
                            dVal = log10(dVal);
                        }
                    else
                        {
                            dVal = log(dVal);
                        }
                }

            double absVal = fabs(dVal);

            if (nOK == 0)
                {
                    minV = dVal;
                    maxV = dVal;
                    minAV = absVal;
                    minIndx = i;
                    minAbsIndx = i;
                    maxIndx = i;
                }

            if (dVal < minV)
                {
                    minV = dVal;
                    minIndx = i;
                }
            else if (dVal > maxV)
                {
                    maxV = dVal;
                    maxIndx = i;
                }

            if (absVal < minAV)
                {
                    minAV = absVal;
                    minAbsIndx = i;
                }

            if (dVal > 0.0)
                {
                    if (minPosIndx < 0)
                        {
                            minPV = dVal;
                            minPosIndx = i;
                        }
                    else if (dVal < minPV)
                        {
                            minPV = dVal;
                            minPosIndx = i;
                        }
                }

            sum += dVal;
            if(dVal > 0)
                {
                    geoSum += log(dVal);
                    harmSum += 1/dVal;
                    nPositive++;
                }
            nOK++;
        }

    double  dOK = double(nOK);
    double  dPositive = double(nPositive);

    realResults[soN]     = dOK;
    realResults[soNnull] = double(nData - nOK);
    realResults[soNnegZero] = double(nnegZero);

    intResults[soN] = nOK;
    intResults[soNnull] = nData - nOK;
    intResults[soNnegZero] = nnegZero;

    if (nData > 0)
        realResults[soPercentNonNull] = dOK / double(nData) * 100.0;

    if (nOK == 0)
        return;

    resultsOK = true;

    realResults[soSum]      = sum;
    realResults[soMin]      = minV;
    realResults[soMax]      = maxV;
    realResults[soMinAbs]   = minAV;
    realResults[soMinPos]   = minPV;

    intResults[soMinIndx] = minIndx;
    intResults[soMaxIndx] = maxIndx;
    intResults[soMinAbsIndx] = minAbsIndx;
    intResults[soMinPosIndx] = minPosIndx;

    realResults[soMinIndx] = double(minIndx);
    realResults[soMaxIndx] = double(maxIndx);
    realResults[soMinAbsIndx] = double(minAbsIndx);
    realResults[soMinPosIndx] = double(minPosIndx);

    if (nOK > 0)
        {
            // pathological
            realResults[soRange]   = maxV - minV;
            if (realResults[soRange] < stdEps)
                {
                    realResults[soMean] = (minV + maxV) / 2.0;
                    realResults[soMeanGeo] = (minV + maxV) / 2.0;
                    realResults[soMeanHarm] = (minV + maxV) / 2.0;
                    realResults[soVar]  = 0.0;
                    realResults[soStdDev]  = 0.0;
                }
            else
                {
                    realResults[soMean]            = sum / dOK;
                    realResults[soMeanGeo]             = exp(geoSum / dPositive);
                    realResults[soMeanHarm]        = dPositive / harmSum;

                    // variance and std dev
                    if (nOK > 1)
                        {
                            double var = 0.0;
                            double mean = realResults[soMean];
                            for (int i = 0; i < data.Size(); i++)
                                {
                                    double dVal = data[i];
                                    if (RealIsNull(dVal) || (logData && (dVal < 1.0E-99)))
                                        continue;

                                    if (logData)  {
                                        if (baseTen)  {
                                            dVal = log10(dVal);
                                        }
                                        else {
                                            dVal = log(dVal);
                                        }
                                    }

                                    double dx = dVal - mean;
                                    var += dx * dx;
                                }

                            var /= double(nOK - 1);

                            realResults[soVar]  = var;
                            realResults[soStdDev]  = sqrt(var);
                        }
                }
        }

    /*
      mean results now log data
      if (logData) {
      if (baseTen) {
      realResults[soMean] = InvLgt(realResults[soMean]);
      }
      else {
      realResults[soMean] = exp(realResults[soMean]);
      }
      } */

}

void SC_Statistics::CalcMedian(SC_DoubleArray& data)
{
    if (nOK == 0)
        return;

    if (nOK != data.Size())
        {
            data.Cleanup();
            nOK = data.Size();
        }

    // mix before sort to help sort routine
    SC_Random mix;
    mix.RandomMix(data);
    if (!data.Sort(true))
        {
            realResults[soMedian] = nullReal;

        }
    else
        {
            int midPt = data.Size() / 2;
            if ((data.Size() % 2) == 1)
                {
                    // odd entries -- pick middle
                    realResults[soMedian] = data[midPt];
                }
            else
                {
                    // average possibilities
                    realResults[soMedian] = data[midPt];
                    realResults[soMedian] += data[midPt - 1];
                    realResults[soMedian] /= 2.0;
                }
        }
}

void SC_Statistics::CalcMode(const SC_DoubleArray& data)
{
    if (nOK == 0)
        return;

    // min == max
    if (realResults[soVar] < stdEps)
        {
            realResults[soMode] = realResults[soMean];
            return;
        }

    // Scott, 1979 algorithm for bin width
    // W = (3.49)(std.dev.)[(#samples)^(-1/3)]
    double width = 3.49 * realResults[soStdDev] * (pow(realResults[soN], -0.3333));
    if (width < stdEps)
        {
            realResults[soMode] = realResults[soMean];
            return;
        }

    double maxNBins = (realResults[soMax] - realResults[soMin]) / width;
    if (maxNBins > 10000.0)
        return;

    int nbins = int(ceil(maxNBins));

    // note just linear bins
    width = (realResults[soMax] - realResults[soMin]) / double(nbins);

    SC_IntArray binCount(nbins, 0);
    for (int i = 0; i < data.Size(); i++)
        {
            double dVal = data[i];
            if (RealIsNull(dVal) || (dVal < realResults[soMin])) // can happen if stats calc was log
                continue;

            int binIndex = int(floor((dVal - realResults[soMin]) / width));
            // pathological case == maxVal
            if (binIndex == nbins)
                binIndex--;

            binCount[binIndex]++;
        }

    int maxBin = 0;
    int maxCount = binCount[0];
    for (int i = 1; i < nbins; i++)
        if (binCount[i] > maxCount)
            {
                maxBin = i;
                maxCount = binCount[i];
            }

    realResults[soMode]  = realResults[soMin] + (double(maxBin) + 0.5) * width;
}

