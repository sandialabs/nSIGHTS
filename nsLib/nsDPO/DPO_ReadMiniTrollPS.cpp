///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadMiniTrollPS.cpp
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

#include "nsLibAfx.h"

#include <genClassPS/PS_UIBase.h>

#include "DPO_ReadMiniTrollPS.h"


void DPO_ReadMiniTrollPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_ReadMiniTrollPS*>(inObj)->SetAllSensitive();
}

void DPO_ReadMiniTrollPS::SetAllSensitive()
{
    calendarYearStartUI.SetSensitive(timeSpecTypeUI.GetChoice() == 1);
    timeSpecUnitsUI.SetSensitive(timeSpecTypeUI.GetChoice() < 2);
    readAllColumnsUI.SetSensitive(fileTypeUI.GetChoice() < 2); 
    minimumDeltaTUI.SetSensitive(fileTypeUI.GetChoice() == 4); 

    bool isDAS = (fileTypeUI.GetChoice() == 2);
    dasOptions.SetSensitive(isDAS); 

    csvYColumnUI.SetSensitive(fileTypeUI.GetChoice() == 1 && !readAllColumnsUI.GetBoolChoice());
    mfYColumnUI.SetSensitive(fileTypeUI.GetChoice() == 0 && !readAllColumnsUI.GetBoolChoice());
}


void DPO_ReadMiniTrollPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.StdFormAttach();
    mtFnameUI.InitFramedUI("MiniTroll Data File ", PPform.GetWidget(), args);

    args.StdTopObjectAttach(mtFnameUI);
    formatRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(15);
    const char* fileTypeStrs[] = {"Mini Troll Raw", "Mini Troll CSV", "OPG DAS CSV", "Level Troll 07", "Westbay MOSDAQ", "Level Troll 08"};
    fileTypeUI.PreInitSetUp(fileTypeStrs, 6);
    fileTypeUI.InitFramedUI("File Format", formatRow.GetWidget(), args);
    fileTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(16, 40);
    readAllColumnsUI.PreInitSetUp("Single Col", "All Data");
    readAllColumnsUI.InitFramedUI("Column Selection", formatRow.GetWidget(), args);
    readAllColumnsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(41, 60);
    yColSelect.InitFramedUI("Y Data Col", formatRow.GetWidget(), args);

    args.StdSplitMiddle(61, 80);
    dasOptions.InitFramedUI("OPG DAS Options", formatRow.GetWidget(), args);

    args.StdToggleSetup(5);
    dasReadIDUI.StdToggleInit("ID", dasOptions.GetForm(), args);

    args.StdToggleSetup(50);
    dasReadKeyUI.StdToggleInit("Key", dasOptions.GetForm(), args);

    args.StdSplitFinish(20);
    minimumDeltaTUI.InitFramedUI("Minimum Delta T", formatRow.GetWidget(), args);

    args.StdSplitSetup();
    const char* mfColIDs[] = {"3", "4", "5", "6", "7", 
                            "8", "9", "10", "11", "12", 
                            "13", "12", "15", "16", "17"}; 
    mfYColumnUI.PreInitSetUp(mfColIDs, 15);
    mfYColumnUI.InitLabeledUI("", yColSelect.GetForm(), args);

    args.StdSplitFinish();
    const char* colIDs[] = {"B", "C", "D", "E", "F", 
                            "G", "H", "I", "J", "K", 
                            "L", "M", "N", "O", "P"}; 
    csvYColumnUI.PreInitSetUp(colIDs, 15);
    csvYColumnUI.InitLabeledUI("", yColSelect.GetForm(), args);

    args.StdTopObjectAttach(formatRow.GetWidget());
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
    statusFrame.InitFramedUI("Table Status", PPform.GetWidget(), args);
    args.StdFormAttach(5);
    args.LeftOffset(10);
    args.RightOffset(10);
    ncols.InitLabeledUI("# of columns", statusFrame.GetForm(), args);

    args.StdTopObjectAttach(ncols);
    args.LeftOffset(10);
    args.RightOffset(10);
    nrows.InitLabeledUI("# of rows", statusFrame.GetForm(), args);

}

void  DPO_ReadMiniTrollPS::MenuClose()
{
    mtFnameUI.CloseDialog();
    MenuBaseC::MenuClose();
}

void DPO_ReadMiniTrollPS::SetMenuData(DPO_ReadMiniTroll&    inData)
{
    MenuBaseC::SetMenuData(inData);

    mtFnameUI.SetFieldString(inData.mtFname);

    timeSpecTypeUI.SetChoice(int(inData.timeSpecDC.timeSpecType));
    timeSpecUnitsUI.SetChoice(int(inData.timeSpecDC.timeSpecUnits));
    calendarYearStartUI.SetFieldValue(inData.timeSpecDC.calendarYearStart);

    fileTypeUI.SetChoice(int(inData.fileType));
    readAllColumnsUI.SetBoolChoice(inData.readAllColumns);
    csvYColumnUI.SetChoice(inData.yColumn - 1);
    mfYColumnUI.SetChoice(inData.yColumn);
    minimumDeltaTUI.SetFieldValue(inData.minimumDeltaT);

    dasReadIDUI.SetValue(inData.dasReadID);
    dasReadKeyUI.SetValue(inData.dasReadKey);

    if (inData.StatusOK())
    {
        ncols.SetFieldValue(inData.tableData.columnDesc.Size());
        nrows.SetFieldValue(inData.tableData.rowDesc.Size());
    }
    else
    {
        ncols.SetStatusNA();
        nrows.SetStatusNA();
    }
 
    statusFrame.SetSensitive(inData.StatusOK());

    SetAllSensitive();
}

bool DPO_ReadMiniTrollPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    mtFnameUI.FieldFileOK(allOK, "MiniTroll file", true, false);

    if (timeSpecTypeUI.GetChoice() == 1)
        calendarYearStartUI.FieldValueOK(allOK, "start year", 1900, 2100);

    if (fileTypeUI.GetChoice() == 4)
        minimumDeltaTUI.FieldValueOK(allOK, "delta T", 0.0, 1.0E+06);

    return allOK;
}

void DPO_ReadMiniTrollPS::GetMenuData(DPO_ReadMiniTroll&  outData)
{
    MenuBaseC::GetMenuData(outData);
    mtFnameUI.GetFieldFile(outData.mtFname);

    outData.timeSpecDC.timeSpecType = DC_DateTimeSpec::TimeSpecType(timeSpecTypeUI.GetChoice());
    if (outData.timeSpecDC.timeSpecType != DC_DateTimeSpec::tstExcelDays)
        outData.timeSpecDC.timeSpecUnits = DC_DateTime::TimeUnits(timeSpecUnitsUI.GetChoice());

    if (outData.timeSpecDC.timeSpecType == DC_DateTimeSpec::tstCalendarYear)
        outData.timeSpecDC.calendarYearStart = calendarYearStartUI.GetFieldValue();

    outData.fileType = DFO_ReadMiniTroll::FileType(fileTypeUI.GetChoice());
    outData.readAllColumns = readAllColumnsUI.GetBoolChoice();
    if (((outData.fileType == DFO_ReadMiniTroll::mtRawText) || (outData.fileType == DFO_ReadMiniTroll::mtCSV)) && (!outData.readAllColumns))
    {
        if (outData.fileType == DFO_ReadMiniTroll::mtCSV)
            outData.yColumn = csvYColumnUI.GetChoice() + 1;
        else
            outData.yColumn = mfYColumnUI.GetChoice();
    }
    if (outData.fileType == DFO_ReadMiniTroll::wbMOSDAQ)
    {
        outData.minimumDeltaT = minimumDeltaTUI.GetFieldValue();
    }
    if (outData.fileType == DFO_ReadMiniTroll::dasCSV)
    {
        outData.dasReadKey = dasReadKeyUI.GetValue(); 
        outData.dasReadID = dasReadIDUI.GetValue(); 
    }
}

void DPO_ReadMiniTrollPS::MenuClear()
{
    mtFnameUI.SetFieldString(NULL);
}

void DPO_ReadMiniTrollPS::MenuDefault()
{
    MenuBaseC::MenuDefault();

    MenuClear();
}

