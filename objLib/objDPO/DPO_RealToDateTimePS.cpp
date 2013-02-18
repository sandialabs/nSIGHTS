///////////////////////////////////////////////////////////////////////////////////
//
// DPO_RealToDateTimePS.cpp
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

#include <genClass/U_Str.h>

#include "DPO_RealToDateTime.h"
#include "DPO_RealToDateTimePS.h"



/////////////////////////////////////////////////////////////////
// DPO_RealToDateTimePS

DPO_RealToDateTimePS::DPO_RealToDateTimePS()
: inputRealObjRefUI(typeid(DO_Real)), timeSpecMasterObjRefUI(typeid(DO_DateTimeSpec),false)
{
}


void DPO_RealToDateTimePS::SetAllSensitive()
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
    int labelType = labelTypeUI.GetChoice();
    dateFormatUI.SetSensitive(labelType != 1);
    timeFormatUI.SetSensitive(labelType != 0);

    dateTimeDelimStrUI.SetSensitive(labelTypeUI.GetChoice() > 1);

    labelOverrideUI.SetSensitive(true);
}

void DPO_RealToDateTimePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_RealToDateTimePS*>(inObj)->SetAllSensitive();
}


void DPO_RealToDateTimePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    inputRealObjRefUI.InitFramedUI("Real to Convert", URform.GetWidget(), args);

    args.StdFormAttach();
    timeSpecMasterObjRefUI.horizontal = true;
    timeSpecMasterObjRefUI.InitFramedUI("Time Specification", PPform.GetWidget(), args);
    timeSpecMasterObjRefUI.isMaster.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(timeSpecMasterObjRefUI);
    timeSpecTopRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    static const char* taStrs[] = {"Arbitrary", "Calendar Year", "Excel Days"};
    timeSpecTypeUI.PreInitSetUp(taStrs, 3);
    timeSpecTypeUI.InitFramedUI("Conversion Type", timeSpecTopRow.GetWidget(), args);
    timeSpecTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    static const char* tuStrs[] = {"Years", "Days", "Hours", "Minutes", "Seconds"};
    timeSpecUnitsUI.PreInitSetUp(tuStrs, 5);
    timeSpecUnitsUI.InitFramedUI("Conversion Time Units", timeSpecTopRow.GetWidget(), args);

    args.StdTopObjectAttach(timeSpecTopRow.GetWidget());
    timeSpecSecondRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    arbitraryDateStrUI.InitFramedUI("Zero Date (mm/dd/yy)", timeSpecSecondRow.GetWidget(), args);

    args.StdSplitMiddle(34, 66);
    arbitraryTimeStrUI.InitFramedUI("Zero Time (hh:mm:ss)", timeSpecSecondRow.GetWidget(), args);

    args.StdSplitFinish(33);
    calendarYearStartUI.InitFramedUI("Zero Year", timeSpecSecondRow.GetWidget(), args);

    args.StdTopObjectAttach(timeSpecSecondRow.GetWidget());

    formatRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(33);
    static const char* ltStrs[] = {"Date", "Time", "Time/Date", "Date/Time" };
    labelTypeUI.PreInitSetUp(ltStrs, 4);
    labelTypeUI.InitFramedUI("Label Components", formatRow.GetWidget(), args);
    labelTypeUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(33, 66);
    timeFormatUI.InitFramedUI("Time Format", formatRow.GetWidget(), args);

    args.StdSplitFinish(33);
    dateTimeDelimStrUI.InitFramedUI("Time/Date Delimiter", formatRow.GetWidget(), args);

    args.StdTopObjectAttach(formatRow.GetWidget());
    dateFormatUI.InitFramedUI("Date Format", PPform.GetWidget(), args);

    args.StdTopObjectAttach(dateFormatUI);
    labelOverrideUI.InitFramedUI("Label Description", PPform.GetWidget(), args);

    args.StdTopObjectAttach(labelOverrideUI);
    statusRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    inputRealStatus.InitFramedUI("Input Value To Convert", statusRow.GetWidget(), args);

    args.StdSplitFinish();
    outputLabelStatus.InitFramedUI("Output Label", statusRow.GetWidget(), args);
}


void DPO_RealToDateTimePS::MenuOpen(DPO_RealToDateTime&   inData)
{
    MenuBaseC::MenuOpen(false, true);
    timeSpecMasterObjRefUI.masterObjectList.StdMenuInit(inData.timeSpecMasterObjRef);
    inputRealObjRefUI.StdMenuInit(inData.inputRealObjRef);
}


void DPO_RealToDateTimePS::SetMenuData(DPO_RealToDateTime&    inData)
{
    MenuBaseC::SetMenuData(inData);

    inputRealObjRefUI.SetFuncObjRef(inData.inputRealObjRef);

    timeSpecMasterObjRefUI.SetMasterSlaveSettings(inData.timeSpecIsMaster, inData.timeSpecMasterObjRef);

    timeSpecTypeUI.SetChoice(int(inData.dateTimeSpec.timeSpecType));
    timeSpecUnitsUI.SetChoice(int(inData.dateTimeSpec.timeSpecUnits));

    calendarYearStartUI.SetFieldValue(inData.dateTimeSpec.calendarYearStart);
    arbitraryTimeStrUI.SetFieldString(inData.arbitraryTimeStr);
    arbitraryDateStrUI.SetFieldString(inData.arbitraryDateStr);

    labelTypeUI.SetChoice(int(inData.labelType));

    dateFormatUI.SetDateFormat(inData.dateFormat);
    timeFormatUI.SetChoice(int(inData.timeFormat));

    dateTimeDelimStrUI.SetFieldString(inData.dateTimeDelimStr);

    labelOverrideUI.SetLegendOverride(inData.labelOverride);

    if (inData.StatusOK())
    {
        inputRealStatus.SetFieldValue(inData.currRealVal);
        outputLabelStatus.SetFieldString(inData.outputLabelDO.GetValueLabel());
    }
    else
    {
        inputRealStatus.SetStatusNA();
        outputLabelStatus.SetStatusNA();
    }

    SetAllSensitive();
}

bool DPO_RealToDateTimePS::UIdataOK()
{
    MenuBaseC::UIdataOK();

    bool isMaster = timeSpecMasterObjRefUI.isMaster.GetValue();
    if (isMaster && (timeSpecTypeUI.GetChoice() == 1))
        calendarYearStartUI.FieldValueOK(allOK, "Calendar year", 1900, 2100);

    return allOK;
}

void DPO_RealToDateTimePS::GetMenuData(DPO_RealToDateTime&    outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.inputRealObjRef = inputRealObjRefUI.GetFuncObjRef();

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
            arbitraryTimeStrUI.GetFieldString(outData.arbitraryTimeStr, DFO_RealToDateTime::dateTimeStrLen);
            arbitraryDateStrUI.GetFieldString(outData.arbitraryDateStr, DFO_RealToDateTime::dateTimeStrLen);
        }
    }

    outData.labelType = DFO_RealToDateTime::LabelType(labelTypeUI.GetChoice());

    if (outData.labelType != DFO_RealToDateTime::ltTime)
        dateFormatUI.GetDateFormat(outData.dateFormat);
    if (outData.labelType != DFO_RealToDateTime::ltDate)
        outData.timeFormat = DC_DateTime::TimeFormat(timeFormatUI.GetChoice());

    if ((outData.labelType == DFO_RealToDateTime::ltDateTime) ||
        (outData.labelType == DFO_RealToDateTime::ltTimeDate))
        dateTimeDelimStrUI.GetFieldString(outData.dateTimeDelimStr, DFO_RealToDateTime::dateTimeStrLen);

    labelOverrideUI.GetLegendOverride(outData.labelOverride);

}

