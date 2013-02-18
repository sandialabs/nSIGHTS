///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableCalibrationStatistics.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <objDFO/DFO_TableCalibrationStatistics.h>

DFO_TableCalibrationStatistics :: DFO_TableCalibrationStatistics() :
    FuncObjC("Table Calibration Statistics")
{
    selectedYCol = 1;
    InitCommon();
}

DFO_TableCalibrationStatistics::DFO_TableCalibrationStatistics(const DFO_TableCalibrationStatistics& a) :
    FuncObjC(a),
    DSC_TableColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableCalibrationStatistics :: ~DFO_TableCalibrationStatistics ()
{
}

void DFO_TableCalibrationStatistics::InitCommon()
{
    DSC_TableColSel::AddPort(*this);

    AddOutPort(maxOverPredictionDO);
    AddOutPort(maxUnderPredictionDO);
    AddOutPort(averageResidualDO);
    AddOutPort(averageAbsResidualDO);
    AddOutPort(numMeasurementsDO);
    AddOutPort(residualRMSDO);
    AddOutPort(normalizedResidualRMSDO);
    AddOutPort(correlationCoefficientDO);

    maxOverPredictionDO.Reset("Maximum over prediction");
    maxUnderPredictionDO.Reset("Maximum under prediction");
    averageResidualDO.Reset("Average residual");
    averageAbsResidualDO.Reset("Average absolute residual");
    numMeasurementsDO.Reset("Number of residuals");
    residualRMSDO.Reset("Root mean square of residuals");
    normalizedResidualRMSDO.Reset("Normalized RMS of residuals (%)");
    correlationCoefficientDO.Reset("Correlation coefficient");

    DoStatusChk();
}

DFO_TableCalibrationStatistics& DFO_TableCalibrationStatistics::operator= (const DFO_TableCalibrationStatistics& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        DSC_TableColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableCalibrationStatistics::LocalCopy(const DFO_TableCalibrationStatistics& a)
{
    selectedYCol   = a.selectedYCol;
}


void  DFO_TableCalibrationStatistics:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    DSC_TableColSel::DoCheck(*this);
    if (StatusOK())
    {
        if ((inputData->dataTable.Size() <= selectedYCol) || (selectedYCol < 0))
            SetObjErrMsg("Y column index out of range");
    }
}

void DFO_TableCalibrationStatistics:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (!StatusOK())
        return;

    const SC_DoubleArray& xIn = *selectedColData;
    const SC_DoubleArray& yIn = inputData->dataTable[selectedYCol];

    double sumRes = 0.0;
    double absRes = 0.0;
    double sumSqr = 0.0;
    double maxRes, minRes, maxY, minY;
    int numRes = 0;
    for (int i = 0; i < xIn.Size(); i++)
    {
        double nextX = xIn[i];
        double nextY = yIn[i];

        if (RealIsNull(nextX) || RealIsNull(nextY))
            continue;

        double currRes = nextX - nextY;

        if (numRes == 0)
        {
            maxRes = currRes;
            minRes = currRes;
            maxY = nextY;
            minY = nextY;
        }
        else
        {
            maxRes = DMax(maxRes, currRes);
            minRes = DMin(minRes, currRes);
            maxY = DMax(maxY, nextY);
            minY = DMin(minY, nextY);
        }
        numRes++;
        absRes += fabs(currRes);
        sumRes += currRes;
        sumSqr += Sqr(currRes);
    }

    if (numRes < 2)
    {
        SetObjErrMsg("Less than two points to calculate statistics for");
        return;
    }

    maxOverPredictionDO.InitLabelAndValue(maxRes);
    maxUnderPredictionDO.InitLabelAndValue(-minRes);
    averageResidualDO.InitLabelAndValue(sumRes/double(numRes));
    averageAbsResidualDO.InitLabelAndValue(absRes/double(numRes));
    numMeasurementsDO.InitLabelAndValue(double(numRes));
    normalizedResidualRMSDO.InitLabelAndValue(0.0);
    if (sumSqr > 0.0)
    {
        double rms = sqrt(sumSqr/double(numRes));
        residualRMSDO.InitLabelAndValue(rms);
        double dy = maxY - minY; 
        if (dy > 0.0)
            normalizedResidualRMSDO.InitLabelAndValue(rms / dy * 100.0);
    }
    else
    {
        residualRMSDO.InitLabelAndValue(0.0);
    }
    correlationCoefficientDO.InitLabelAndValue(PearsonR(xIn, yIn));
}

