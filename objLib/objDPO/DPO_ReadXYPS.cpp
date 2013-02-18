///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadXYPS.cpp
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

#include <genClassPS/PS_UIBase.h>

#include "DPO_ReadXYPS.h"


void DPO_ReadXYPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_ReadXYPS*>(inObj)->SetAllSensitive();
}

void DPO_ReadXYPS::SetAllSensitive()
{
    bool isTable = (xyFileFormatUI.GetChoice() == 1);
    tableSpecRow.SetSensitive(isTable);
    tableXcolUI.SetSensitive(isTable);
    tableYcolUI.SetSensitive(isTable);

    bool isDate = (xyFileFormatUI.GetChoice() == 2);
    timeSpecRow.SetSensitive(isDate);
    timeSpecTypeUI.SetSensitive(isDate);
    timeSpecUnitsUI.SetSensitive(isDate);
    calendarYearStartUI.SetSensitive(isDate);
    if (isDate)
    {
        calendarYearStartUI.SetSensitive(timeSpecTypeUI.GetChoice() == 1);
        timeSpecUnitsUI.SetSensitive(timeSpecTypeUI.GetChoice() < 2);
    }

    dataIDUI.SetSensitive(isDate || (!readColumnIDUI.GetValue()));
}

void DPO_ReadXYPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.StdFormAttach();
    xyFnameUI.InitFramedUI("XY Data File ", PPform.GetWidget(), args);
    xyFnameUI.SetPromptAndMask("XY data file to read",
        "All Files (*.*)|*.*|Text Files (*.txt)|*.txt|CSV files (*.csv)|*.csv|||");
        //"Data Files (*.dat)|*.dat|All Files (*.*)|*.*|||");

    args.StdTopObjectAttach(xyFnameUI);
    nextRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    static const char* formatLabels[] = {"Basic", "Table", "Hydrograph", "Other"};
    xyFileFormatUI.PreInitSetUp(formatLabels, 4);
    xyFileFormatUI.InitFramedUI("File Format", nextRow.GetWidget(), args);
    xyFileFormatUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    mpiFilesUI.nodeFileID += "XY file";
    mpiFilesUI.StdInit(nextRow.GetWidget(), args);

    args.StdTopObjectAttach(nextRow.GetWidget());
    optionsFrame.InitFramedUI("ID Source", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    readColumnIDUI.StdToggleInit("ID from Column Header", optionsFrame.GetForm(), args);
    readColumnIDUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    dataIDUI.SetFieldLen(10);
    dataIDUI.AttachLabelToObject();
    dataIDUI.InitLabeledUI("ID: ", optionsFrame.GetForm(), args);

    args.StdTopObjectAttach(optionsFrame);
    tableSpecRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    tableXcolUI.SetFieldLen(4);
    tableXcolUI.InitFramedUI("X Column Index", tableSpecRow.GetWidget(), args);

    args.StdSplitFinish();
    tableYcolUI.SetFieldLen(4);
    tableYcolUI.InitFramedUI("Y Column Index", tableSpecRow.GetWidget(), args);

    args.StdTopObjectAttach(tableSpecRow.GetWidget());
    timeSpecRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    static const char*  ettLabels[] = {"Data Start", "Year Start", "Excel Days"};
    timeSpecTypeUI.PreInitSetUp(ettLabels, 3);
    timeSpecTypeUI.InitFramedUI("Elapsed Time Type", timeSpecRow.GetWidget(), args);
    timeSpecTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(33, 66);
    static const char*  unitsLabels[] = {"Years", "Days", "Hours", "Minutes", "Seconds"};
    timeSpecUnitsUI.PreInitSetUp(unitsLabels, 5);
    timeSpecUnitsUI.InitFramedUI("Output Time Units", timeSpecRow.GetWidget(), args);

    args.StdSplitFinish(33);
    calendarYearStartUI.InitFramedUI("Start Year", timeSpecRow.GetWidget(), args);

    args.StdTopObjectAttach(timeSpecRow.GetWidget());
    statusFrame.InitFramedUI("Data Status", PPform.GetWidget(), args);

    args.StdFormAttach(5);
    args.LeftOffset(10);
    args.RightOffset(10);
    nrows.InitLabeledUI("# of XY points", statusFrame.GetForm(), args);
}

void  DPO_ReadXYPS::MenuClose()
{
    xyFnameUI.CloseDialog();
    MenuBaseC::MenuClose();
}

void DPO_ReadXYPS::SetMenuData(DPO_ReadXY&  inData)
{
    MenuBaseC::SetMenuData(inData);

    xyFnameUI.SetFieldString(inData.xyFname);
    xyFileFormatUI.SetChoice(int(inData.xyFileFormat));
    readColumnIDUI.SetValue(inData.readColumnID);
    mpiFilesUI.SetNodeFileArray(inData.mpiFiles);

    dataIDUI.SetFieldString(inData.dataID);
    tableXcolUI.SetFieldValue(inData.tableXcol);
    tableYcolUI.SetFieldValue(inData.tableYcol);

    timeSpecTypeUI.SetChoice(int(inData.dateSpec.timeSpecType));
    timeSpecUnitsUI.SetChoice(int(inData.dateSpec.timeSpecUnits));
    calendarYearStartUI.SetFieldValue(inData.dateSpec.calendarYearStart);

    bool isOK = (inData.GetStatus() == os_OK);
    if (isOK)
    {
        nrows.SetFieldValue(inData.xyData.Size());
    }
    else
    {
        nrows.SetStatusNA();
    }
    statusFrame.SetSensitive(isOK);

    SetAllSensitive();
}

bool DPO_ReadXYPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    xyFnameUI.FieldFileOK(allOK, "XY file", true, false);
    if (xyFileFormatUI.GetChoice() == 1)
    {
        tableXcolUI.FieldValueOK(allOK, "X column", 1, 100);
        tableYcolUI.FieldValueOK(allOK, "Y column", 1, 100);
    }
    if (xyFileFormatUI.GetChoice() == 2)
    {
        calendarYearStartUI.FieldValueOK(allOK, "start year", 1900, 2100);
    }
    return allOK;
}

void DPO_ReadXYPS::GetMenuData(DPO_ReadXY&  outData)
{
    MenuBaseC::GetMenuData(outData);
    xyFnameUI.GetFieldFile(outData.xyFname);
    outData.xyFileFormat = DSC_XYInputSpec::XYFileFormat(xyFileFormatUI.GetChoice());
    mpiFilesUI.GetNodeFileArray(outData.mpiFiles);

    outData.readColumnID = readColumnIDUI.GetValue();
    if((outData.xyFileFormat == DSC_XYInputSpec::xyffDate) || (!outData.readColumnID))
        dataIDUI.GetFieldString(outData.dataID, DC_XYData::dataIDLen);

    if(outData.xyFileFormat == DSC_XYInputSpec::xyffDate)
    {
        outData.dateSpec.timeSpecType = DC_DateTimeSpec::TimeSpecType(timeSpecTypeUI.GetChoice());
        if (outData.dateSpec.timeSpecType != DC_DateTimeSpec::tstExcelDays)
            outData.dateSpec.timeSpecUnits = DC_DateTime::TimeUnits(timeSpecUnitsUI.GetChoice());
        if (outData.dateSpec.timeSpecType == DC_DateTimeSpec::tstCalendarYear)
            outData.dateSpec.calendarYearStart = calendarYearStartUI.GetFieldValue();
    }

    if (outData.xyFileFormat == DSC_XYInputSpec::xyffTable)
    {
        outData.tableXcol = tableXcolUI.GetFieldValue();
        outData.tableYcol = tableYcolUI.GetFieldValue();
    }
}

void DPO_ReadXYPS::MenuClear()
{
    xyFnameUI.SetFieldString(NULL);
}

void DPO_ReadXYPS::MenuDefault()
{
    MenuBaseC::MenuDefault();

    MenuClear();
    xyFileFormatUI.SetChoice(0);
    readColumnIDUI.SetValue(true);
}

