///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableRowStatisticsPS.cpp
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

#include <genClass/DC_XYCDF.h>
#include <genClass/DO_TableData.h>

#include "DPO_TableRowStatisticsPS.h"
#include "DPO_TableRowStatistics.h"

DPO_TableRowStatisticsPS::DPO_TableRowStatisticsPS() :
    inputTableObjRefUI(typeid(DO_TableData))
{
}


void DPO_TableRowStatisticsPS::SetAllSensitive()
{
    xcolumnIndexUI.SetSensitive(listDataOK && skipXColumnUI.GetValue());

    bool doCDF= outputCDF99UI.GetValue() || outputCDF95UI.GetValue() || outputCDF90UI.GetValue();

    quantileTypeUI.SetSensitive(doCDF);
    genericQuantileValueUI.SetSensitive(doCDF && quantileTypeUI.GetChoice() == 3);
}


void DPO_TableRowStatisticsPS::ResetColumnList()
{
    listDataOK = inputTableObjRefUI.CheckDataObj();

    if (listDataOK)
    {
        const SC_StringArray& colDesc = static_cast<DO_TableData*>(inputTableObjRefUI.GetDataObj())->tableData->columnDesc;;
        xcolumnIndexUI.SetListText(colDesc);
    }
    else
    {
        xcolumnIndexUI.SetListNA();
    }

    SetAllSensitive();
}

void DPO_TableRowStatisticsPS::ResetColumnListCB(void* inObj)
{
    static_cast<DPO_TableRowStatisticsPS*>(inObj)->ResetColumnList();
}


void DPO_TableRowStatisticsPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_TableRowStatisticsPS*>(inObj)->SetAllSensitive();
}



void DPO_TableRowStatisticsPS::InitMenu()
{
    MenuBaseC::InitMenu();
    ArgListC args;

    args.AllFormAttach();
    inputTableObjRefUI.InitFramedUI("Table to Summarize", URform.GetWidget(), args);
    inputTableObjRefUI.AddOtherObjectCallback(ResetColumnListCB, this);

    //  form for row with row/col selections
    args.StdFormAttach();
    skipXFrame.InitFramedUI("X Column Processing", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    skipXColumnUI.StdToggleInit("skip X column", skipXFrame.GetForm(), args);
    skipXColumnUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(60);
    xcolumnIndexUI.InitWidget(skipXFrame.GetForm(), args);

    args.StdTopObjectAttach(skipXFrame);
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdFormAttach();
    topRow.InitWidget(optionsFrame.GetForm(), args);

    args.StdToggleSetup(5);
    outputAllStatsUI.StdToggleInit("all statistics", topRow.GetWidget(), args);

    args.StdToggleSetup(35);
    outputConfidenceLimitsUI.StdToggleInit("normal 95 limits", topRow.GetWidget(), args);

    args.StdToggleSetup(70);
    outputMedianUI.StdToggleInit("median && mode", topRow.GetWidget(), args);

    args.StdTopObjectAttach(topRow.GetWidget());
    nextRow.InitWidget(optionsFrame.GetForm(), args);

    args.StdToggleSetup(5);
    outputCDF99UI.StdToggleInit("CDF 99/01", nextRow.GetWidget(), args);
    outputCDF99UI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(35);
    outputCDF95UI.StdToggleInit("CDF 95/05", nextRow.GetWidget(), args);
    outputCDF95UI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(70);
    outputCDF90UI.StdToggleInit("CDF 90/10", nextRow.GetWidget(), args);
    outputCDF90UI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(optionsFrame);
    quantileForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(65);
    quantileTypeUI.PreInitSetUp(DC_XYCDF::quantileStrs, DC_XYCDF::nquantileType);
    quantileTypeUI.InitFramedUI("Quantile Type", quantileForm.GetWidget(), args);
    quantileTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(35);
    genericQuantileValueUI.InitFramedUI("Generic Value", quantileForm.GetWidget(), args);

}

void DPO_TableRowStatisticsPS::MenuOpen(DPO_TableRowStatistics& inData)
{
    MenuBaseC::MenuOpen(false, false);
    inputTableObjRefUI.StdMenuInit(inData.inputTableObjRef);
}


void DPO_TableRowStatisticsPS::SetMenuData(DPO_TableRowStatistics&  inData)
{
    inputTableObjRefUI.SetFuncObjRef(inData.inputTableObjRef);

    skipXColumnUI.SetValue(inData.skipXColumn);
    xcolumnIndexUI.SetSelection(inData.xcolumnIndex);

    outputAllStatsUI.SetValue(inData.outputAllStats);
    outputConfidenceLimitsUI.SetValue(inData.outputConfidenceLimits);
    outputMedianUI.SetValue(inData.outputMedian);

    outputCDF99UI.SetValue(inData.outputCDF99);
    outputCDF95UI.SetValue(inData.outputCDF95);
    outputCDF90UI.SetValue(inData.outputCDF90);

    quantileTypeUI.SetChoice(inData.quantileType);
    genericQuantileValueUI.SetFieldValue(inData.genericQuantileValue);

    ResetColumnList();

    MenuBaseC::SetMenuData(inData);

}

bool DPO_TableRowStatisticsPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputTableObjRefUI.FuncObjRefOK(allOK, "Table source");

    bool doCDF= outputCDF99UI.GetValue() || outputCDF95UI.GetValue() || outputCDF90UI.GetValue();
    if (doCDF && (quantileTypeUI.GetChoice() == 3))
        genericQuantileValueUI.FieldValueOK(allOK, "quantile value", 0.0, 1.0);

    return allOK;
}

void DPO_TableRowStatisticsPS::GetMenuData(DPO_TableRowStatistics&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputTableObjRef = inputTableObjRefUI.GetFuncObjRef();

    outData.skipXColumn = skipXColumnUI.GetValue();
    outData.xcolumnIndex = xcolumnIndexUI.GetSelection();

    outData.outputAllStats = outputAllStatsUI.GetValue();
    outData.outputConfidenceLimits = outputConfidenceLimitsUI.GetValue();
    outData.outputMedian = outputMedianUI.GetValue();

    outData.outputCDF99 = outputCDF99UI.GetValue();
    outData.outputCDF95 = outputCDF95UI.GetValue();
    outData.outputCDF90 = outputCDF90UI.GetValue();

    if (outData.outputCDF99 || outData.outputCDF95 ||outData.outputCDF90)
    {
        outData.quantileType = DC_XYCDF::QuantileType(quantileTypeUI.GetChoice());

        if (outData.quantileType == DC_XYCDF::qtGeneric)
            outData.genericQuantileValue = genericQuantileValueUI.GetFieldValue();
    }

}

