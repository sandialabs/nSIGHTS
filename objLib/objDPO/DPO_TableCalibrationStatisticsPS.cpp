///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableCalibrationStatisticsPS.cpp
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>
#include <genClass/DO_TableData.h>

#include "DPO_TableCalibrationStatistics.h"
#include "DPO_TableCalibrationStatisticsPS.h"


DPO_TableCalibrationStatisticsPS::DPO_TableCalibrationStatisticsPS()
{
}

void DPO_TableCalibrationStatisticsPS::SetSelectedYColTableColumns()
{
    tableColSelUI.SetTableColumns(selectedYColUI);
    selectedYColUI.SetSensitive(tableColSelUI.tableDataOK);
}

void DPO_TableCalibrationStatisticsPS::SetSelectedYColTableColumnsCB(void* inObj)
{
    static_cast<DPO_TableCalibrationStatisticsPS*>(inObj)->SetSelectedYColTableColumns();
}


void DPO_TableCalibrationStatisticsPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    tableColSelUI.ObjRefInit(URform.GetWidget(), args);
    tableColSelUI.tableInputObjRefUI.AddOtherObjectCallback(SetSelectedYColTableColumnsCB, this);

    args.Clear();
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    tableColSelUI.selectedColUI.InitFramedUI("Simulated Values Column", topRowForm.GetWidget(), args);

    args.StdSplitFinish();
    selectedYColUI.InitFramedUI("Observed Values Column", topRowForm.GetWidget(), args);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    resultsFrame.InitFramedUI("Result Values", PPform.GetWidget(), args);

    args.StdFormAttach();
    args.LeftOffset(5);
    args.RightOffset(15);
    maxOverPredictionUI.InitLabeledUI("Maximum over prediction", resultsFrame.GetLeft(), args);

    args.StdTopObjectAttach(maxOverPredictionUI);
    args.LeftOffset(5);
    args.RightOffset(15);
    maxUnderPredictionUI.InitLabeledUI("Maximum under prediction", resultsFrame.GetLeft(), args);

    args.StdTopObjectAttach(maxUnderPredictionUI);
    args.LeftOffset(5);
    args.RightOffset(15);
    averageResidualUI.InitLabeledUI("Average residual", resultsFrame.GetLeft(), args);

    args.StdTopObjectAttach(averageResidualUI);
    args.LeftOffset(5);
    args.RightOffset(15);
    averageAbsResidualUI.InitLabeledUI("Average absolute residual", resultsFrame.GetLeft(), args);

    args.StdFormAttach();
    args.LeftOffset(5);
    args.RightOffset(15);
    numMeasurementsUI.InitLabeledUI("Number of residuals", resultsFrame.GetRight(), args);

    args.StdTopObjectAttach(numMeasurementsUI);
    args.LeftOffset(5);
    args.RightOffset(15);
    residualRMSUI.InitLabeledUI("Root mean square of residuals", resultsFrame.GetRight(), args);

    args.StdTopObjectAttach(residualRMSUI);
    args.LeftOffset(5);
    args.RightOffset(15);
    normalizedResidualRMSUI.InitLabeledUI("Normalized RMS of residuals (%)", resultsFrame.GetRight(), args);

    args.StdTopObjectAttach(normalizedResidualRMSUI);
    args.LeftOffset(5);
    args.RightOffset(15);
    correlationCoefficientUI.InitLabeledUI("Correlation coefficient", resultsFrame.GetRight(), args);

}

void DPO_TableCalibrationStatisticsPS::SetMenuData(DPO_TableCalibrationStatistics&  inData)
{
    tableColSelUI.SetTableColSel(inData);
    selectedYColUI.SetSelection(inData.selectedYCol);

    if (inData.StatusOK())
    {
        maxOverPredictionUI.SetFieldValue(inData.maxOverPredictionDO.realVal);
        maxUnderPredictionUI.SetFieldValue(inData.maxUnderPredictionDO.realVal);
        averageResidualUI.SetFieldValue(inData.averageResidualDO.realVal);
        averageAbsResidualUI.SetFieldValue(inData.averageAbsResidualDO.realVal);
        numMeasurementsUI.SetFieldValue(inData.numMeasurementsDO.realVal);
        residualRMSUI.SetFieldValue(inData.residualRMSDO.realVal);
        normalizedResidualRMSUI.SetFieldValue(inData.normalizedResidualRMSDO.realVal);
        correlationCoefficientUI.SetFieldValue(inData.correlationCoefficientDO.realVal);
    }
    else
    {
        maxOverPredictionUI.SetStatusNA();
        maxUnderPredictionUI.SetStatusNA();
        averageResidualUI.SetStatusNA();
        averageAbsResidualUI.SetStatusNA();
        numMeasurementsUI.SetStatusNA();
        residualRMSUI.SetStatusNA();
        normalizedResidualRMSUI.SetStatusNA();
        correlationCoefficientUI.SetStatusNA();
    }

    MenuBaseC::SetMenuData(inData);
    SetSelectedYColTableColumns();
}


void DPO_TableCalibrationStatisticsPS::MenuOpen(DPO_TableCalibrationStatistics& inData)
{
    MenuBaseC::MenuOpen(false, false);
    tableColSelUI.MenuOpen(inData);
}


bool DPO_TableCalibrationStatisticsPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    tableColSelUI.UIdataOK(allOK);
    return allOK;
}

void DPO_TableCalibrationStatisticsPS::GetMenuData(DPO_TableCalibrationStatistics&  outData)
{
    MenuBaseC::GetMenuData(outData);
    tableColSelUI.GetTableColSel(outData);
    outData.selectedYCol = selectedYColUI.GetSelection();
}

