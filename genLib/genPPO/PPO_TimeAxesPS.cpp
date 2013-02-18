///////////////////////////////////////////////////////////////////////////////////
//
// PPO_TimeAxesPS.cpp
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
//      File I/O and user interface for all genPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "PPO_TimeAxesPS.h"

// ****************************************

PPO_TimeAxesPS::PPO_TimeAxesPS()
: timeSpecMasterObjRefUI(typeid(DO_DateTimeSpec),false)
{
}

void PPO_TimeAxesPS::SetAllSensitive()
{
    bool isMaster = timeSpecMasterObjRefUI.isMaster.GetValue();
    timeSpecTopRow.SetSensitive(isMaster);
    timeSpecSecondRow.SetSensitive(isMaster);
    if (isMaster)
    {
        int timeSpecType = timeSpecTypeUI.GetChoice();
        timeSpecUnitsUI.SetSensitive(timeSpecType != 2);
        calendarYearStartUI.SetSensitive(timeSpecType == 1);
        arbitraryTimeStrUI.SetSensitive(timeSpecType == 0);
        arbitraryDateStrUI.SetSensitive(timeSpecType == 0);
    }


    int incrementLabelType = incrementLabelTypeUI.GetChoice();
    dateFormatUI.SetSensitive(incrementLabelType != 1);
    timeFormatUI.SetSensitive(incrementLabelType != 0);

    bool isManual = !autoIncrementsUI.GetValue();

    minorIncrementUnitsUI.SetSensitive(isManual);
    minorIncrementCountUI.SetSensitive(isManual);
    majorIncrementUnitsUI.SetSensitive(isManual);
    majorIncrementCountUI.SetSensitive(isManual);

    dateTimeDelimStrUI.SetSensitive(incrementLabelTypeUI.GetChoice() > 1);
}

void PPO_TimeAxesPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_TimeAxesPS*>(inObj)->SetAllSensitive();
}


void PPO_TimeAxesPS::InitMenu(const PPO_TimeAxes& inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    timeSpecMasterObjRefUI.horizontal = true;
    timeSpecMasterObjRefUI.InitFramedUI("Axes Time Specification", PPform.GetWidget(), args);
    timeSpecMasterObjRefUI.isMaster.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(timeSpecMasterObjRefUI);
    timeSpecTopRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    static const char* taStrs[] = {"Arbitrary", "Calendar Year", "Excel Days"};
    timeSpecTypeUI.PreInitSetUp(taStrs, 3);
    timeSpecTypeUI.InitFramedUI("Axes Time Mapping", timeSpecTopRow.GetWidget(), args);
    timeSpecTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    static const char* tuStrs[] = {"Years", "Days", "Hours", "Minutes", "Seconds"};
    timeSpecUnitsUI.PreInitSetUp(tuStrs, 5);
    timeSpecUnitsUI.InitFramedUI("Axes Time Units", timeSpecTopRow.GetWidget(), args);

    args.StdTopObjectAttach(timeSpecTopRow.GetWidget());
    timeSpecSecondRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    arbitraryDateStrUI.InitFramedUI("Zero Date (mm/dd/yy)", timeSpecSecondRow.GetWidget(), args);

    args.StdSplitMiddle(34, 66);
    arbitraryTimeStrUI.InitFramedUI("Zero Time (hh:mm:ss)", timeSpecSecondRow.GetWidget(), args);

    args.StdSplitFinish(33);
    calendarYearStartUI.InitFramedUI("Zero Year", timeSpecSecondRow.GetWidget(), args);

    args.StdTopObjectAttach(timeSpecSecondRow.GetWidget());
    incrementRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(10);
    autoIncrementsUI.PreInitSetUp("Auto");
    autoIncrementsUI.InitFramedUI("Type", incrementRow.GetWidget(), args);
    autoIncrementsUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    static const char* atuStrs[] = {"Years", "Quarters", "Months", "Days", "Hours", "Minutes", "Seconds"};
    args.StdSplitMiddle(10, 35);
    majorIncrementUnitsUI.PreInitSetUp(atuStrs, 7);
    majorIncrementUnitsUI.InitFramedUI("Major Inc.", incrementRow.GetWidget(), args);
    args.StdSplitMiddle(35, 55);
    majorIncrementCountUI.SetFieldLen(5);
    majorIncrementCountUI.InitFramedUI("# per Inc", incrementRow.GetWidget(), args);

    args.StdSplitMiddle(55, 80);
    minorIncrementUnitsUI.PreInitSetUp(atuStrs, 7);
    minorIncrementUnitsUI.InitFramedUI("Minor Inc.", incrementRow.GetWidget(), args);

    args.StdSplitFinish(20);
    minorIncrementCountUI.SetFieldLen(5);
    minorIncrementCountUI.InitFramedUI("# per Inc", incrementRow.GetWidget(), args);

    args.StdTopObjectAttach(incrementRow.GetWidget());

    formatRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    static const char* iltStrs[] = {"Date", "Time", "Time/Date", "Date/Time" };
    incrementLabelTypeUI.PreInitSetUp(iltStrs, 4);
    incrementLabelTypeUI.InitFramedUI("Increment Label", formatRow.GetWidget(), args);
    incrementLabelTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(33, 66);
    timeFormatUI.InitFramedUI("Time Format", formatRow.GetWidget(), args);

    args.StdSplitFinish(33);
    dateTimeDelimStrUI.InitFramedUI("Time/Date Delimiter", formatRow.GetWidget(), args);

    args.StdTopObjectAttach(formatRow.GetWidget());
    dateFormatUI.InitFramedUI("Date Format", PPform.GetWidget(), args);

    args.StdTopObjectAttach(dateFormatUI);
    axesLabelUI.SetFieldLen(0);
    axesLabelUI.InitFramedUI("Axes Label", PPform.GetWidget(), args);
}


void PPO_TimeAxesPS::MenuOpen(PPO_TimeAxes&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    timeSpecMasterObjRefUI.masterObjectList.StdMenuInit(inData.timeSpecMasterObjRef);

}

void PPO_TimeAxesPS::SetMenuData(PPO_TimeAxes&    inData)
{
    PPO_BasePS::SetMenuData(inData);
    timeSpecMasterObjRefUI.SetMasterSlaveSettings(inData.timeSpecIsMaster, inData.timeSpecMasterObjRef);

    timeSpecTypeUI.SetChoice(int(inData.dateTimeSpec.timeSpecType));
    timeSpecUnitsUI.SetChoice(int(inData.dateTimeSpec.timeSpecUnits));

    calendarYearStartUI.SetFieldValue(inData.dateTimeSpec.calendarYearStart);
    arbitraryTimeStrUI.SetFieldString(inData.arbitraryTimeStr);
    arbitraryDateStrUI.SetFieldString(inData.arbitraryDateStr);

    autoIncrementsUI.SetValue(inData.autoIncrements);
    minorIncrementUnitsUI.SetChoice(int(inData.minorIncrementUnits));
    minorIncrementCountUI.SetFieldValue(inData.minorIncrementCount);

    majorIncrementUnitsUI.SetChoice(int(inData.majorIncrementUnits));
    majorIncrementCountUI.SetFieldValue(inData.majorIncrementCount);

    incrementLabelTypeUI.SetChoice(int(inData.incrementLabelType));

    dateFormatUI.SetDateFormat(inData.dateFormat);
    timeFormatUI.SetChoice(int(inData.timeFormat));

    dateTimeDelimStrUI.SetFieldString(inData.dateTimeDelimStr);

    axesLabelUI.SetFieldString(inData.axesLabel);

    SetAllSensitive();
}

bool PPO_TimeAxesPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();

    bool isMaster = timeSpecMasterObjRefUI.isMaster.GetValue();
    if (isMaster && (timeSpecTypeUI.GetChoice() == 1))
        calendarYearStartUI.FieldValueOK(allOK, "Calendar year", 1900, 2100);

    if (!autoIncrementsUI.GetValue())
    {
        minorIncrementCountUI.FieldValueOK(allOK, "minor increment", 1, 100);
        majorIncrementCountUI.FieldValueOK(allOK, "major increment", 1, 100);
    }
    return allOK;
}

void PPO_TimeAxesPS::GetMenuData(PPO_TimeAxes&  outData)
{
    PPO_BasePS::GetMenuData(outData);

    timeSpecMasterObjRefUI.GetMasterSlaveSettings(outData.timeSpecIsMaster, outData.timeSpecMasterObjRef);

    if (outData.timeSpecIsMaster)
    {
        outData.dateTimeSpec.timeSpecType = DC_DateTimeSpec::TimeSpecType(timeSpecTypeUI.GetChoice());
        if (outData.dateTimeSpec.timeSpecType != DC_DateTimeSpec::tstExcelDays)
            outData.dateTimeSpec.timeSpecUnits = DC_DateTime::TimeUnits(timeSpecUnitsUI.GetChoice());

        if (outData.dateTimeSpec.timeSpecType == DC_DateTimeSpec::tstCalendarYear)
            outData.dateTimeSpec.calendarYearStart = calendarYearStartUI.GetFieldValue();

        if (outData.dateTimeSpec.timeSpecType == DC_DateTimeSpec::tstArbitraryZero)
        {
            arbitraryTimeStrUI.GetFieldString(outData.arbitraryTimeStr, PFO_TimeAxes::dateTimeStrLen);
            arbitraryDateStrUI.GetFieldString(outData.arbitraryDateStr, PFO_TimeAxes::dateTimeStrLen);
        }
    }

    outData.autoIncrements = autoIncrementsUI.GetValue();
    if (!outData.autoIncrements)
    {
        outData.minorIncrementUnits = PFO_TimeAxes::AxesTimeUnits(minorIncrementUnitsUI.GetChoice());
        outData.minorIncrementCount = minorIncrementCountUI.GetFieldValue();

        outData.majorIncrementUnits = PFO_TimeAxes::AxesTimeUnits(majorIncrementUnitsUI.GetChoice());
        outData.majorIncrementCount = majorIncrementCountUI.GetFieldValue();
    }
    outData.incrementLabelType = PFO_TimeAxes::IncrementLabelType(incrementLabelTypeUI.GetChoice());

    if (outData.incrementLabelType != PFO_TimeAxes::iltTime)
        dateFormatUI.GetDateFormat(outData.dateFormat);
    if (outData.incrementLabelType != PFO_TimeAxes::iltDate)
        outData.timeFormat = DC_DateTime::TimeFormat(timeFormatUI.GetChoice());

    if ((outData.incrementLabelType == PFO_TimeAxes::iltDateTime) ||
        (outData.incrementLabelType == PFO_TimeAxes::iltTimeDate))
        dateTimeDelimStrUI.GetFieldString(outData.dateTimeDelimStr, PFO_TimeAxes::dateTimeStrLen);

    axesLabelUI.GetFieldString(outData.axesLabel, PFO_TimeAxes::maxLabelLen);
}

