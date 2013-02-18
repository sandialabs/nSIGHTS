///////////////////////////////////////////////////////////////////////////////////
//
// DC_DataLimit.cpp
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
// DESCRIPTION: Platform independent and application independent base classes and
//              support classes.
//              Data Classes (DC) that define basic types of data used within the
//              framework: Specification for mapping real values to integer intervals
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>
#include <genClass/U_Real.h>

#include <genClass/SC_IntArray.h>
#include <genClass/SC_DoubleArray.h>

#include <genClass/DC_DataLimit.h>


DC_DataLimit::DC_DataLimit(const DC_DataLimit& a)
{
    LocalCopy(a);
}

DC_DataLimit& DC_DataLimit::operator=(const DC_DataLimit& a)
{
    if ( this != &a)
        LocalCopy(a);
    return *this;
}

void DC_DataLimit::LocalCopy(const DC_DataLimit& a)
{
    logRange = a.logRange;
    minLimit = a.minLimit;
    maxLimit = a.maxLimit;
}

void DC_DataLimit::ResetLogLimit()
{
    if ((logRange) && (minLimit < stdEps))
        minLimit = pow(10.0, log10(maxLimit) - 6.0);
}

void DC_DataLimit::InitLimit(const double& inVal)
{
    minLimit = inVal;
    maxLimit = inVal;
}

void DC_DataLimit::AddToLimit(const double& inVal)
{
    if (RealIsNull(inVal))
        return;

    if (RealIsNull(minLimit))
        {
            minLimit = inVal;
            maxLimit = inVal;
            return;
        }

    if (inVal < minLimit)
        minLimit = inVal;
    else if (inVal > maxLimit)
        maxLimit = inVal;
}

void DC_DataLimit::InitLimit(const SC_DoubleArray& inData)
{
    InitLimit(inData[0]);
    AddToLimit(inData);
}


void DC_DataLimit::AddToLimit(const SC_DoubleArray& inData)
{
    for ( int i = 0; i < inData.Size(); i++)
        AddToLimit(inData[i]);
}


bool DC_DataLimit::MapDataToRange(const SC_DoubleArray& inData,
                                  SC_IntArray&   outMap,
                                  bool           clipToRange,
                                  int            maxMap) const
{
    outMap.AllocAndSetSize(inData.Size());
    actMin = minLimit;
    actMax = maxLimit;
    if (logRange)
        {
            actMin = log10(actMin);
            actMax = log10(actMax);
        }


    bool zeroLimit = fabs(actMax - actMin) < stdEps;

    dAct = (actMax - actMin) / double(maxMap);

    bool oneOK = false;

    maxMap --;

    for (int i = 0; i < inData.Size(); i++)
        {
            int nodeMap = -1;
            double dataVal = inData[i];
            if (!TransformValueForRange(dataVal, clipToRange))
                {
                    outMap[i] = -1;
                    continue;
                }

            if (dataVal < actMin)
                {
                    if (clipToRange)
                        nodeMap  = -1;
                    else
                        nodeMap  = 0;
                }
            else if (dataVal > actMax)
                {
                    if (clipToRange)
                        nodeMap  = -1;
                    else
                        nodeMap  = maxMap;
                }
            else if (zeroLimit)   // 0 range
                {
                    nodeMap = maxMap / 2;
                    oneOK = true;

                }
            else
                {
                    double dnodeMap  = (dataVal - actMin) /  dAct;
                    if (dnodeMap > double(maxMap))
                        nodeMap  = maxMap;
                    else
                        nodeMap  = int(dnodeMap);
                    oneOK = true;
                }
            outMap[i] = nodeMap;
        }

    return oneOK;
}

bool DC_DataLimit::MapDataToRange(const SC_DoubleArray& inData,
                                  SC_IntArray&   outMap,
                                  bool           clipToRange) const
{
    outMap.Alloc(inData.Size());
    actMin = minLimit;
    actMax = maxLimit;
    if (clipToRange && logRange)
        {
            actMin = log10(actMin);
            actMax = log10(actMax);
        }

    bool oneOK = false;
    for (int i = 0; i < inData.Size(); i++)
        {
            double dataVal = inData[i];
            if (!TransformValueForRange(dataVal, clipToRange))
                {
                    outMap[i] = -1;
                    continue;
                }

            int nodeMap = 0;
            if (clipToRange && ((dataVal < actMin) || (dataVal > actMax)))
                nodeMap = -1;

            oneOK = oneOK || (nodeMap == 0);
            outMap[i] = nodeMap;
        }

    return oneOK;
}

bool DC_DataLimit::TransformValueForRange(double& inVal,
                                          bool    clipToRange) const
{
    if (RealIsNull(inVal))
        return false;

    if (logRange)
        {
            if (inVal < stdEps)
                {
                    if (clipToRange)
                        return false;
                    inVal =  minLimit;
                }
            inVal = log10(inVal);
        }
    return true;
}

