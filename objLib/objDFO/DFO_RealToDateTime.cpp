///////////////////////////////////////////////////////////////////////////////////
//
// DFO_RealToDateTime.cpp
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

#include <objDFO/DFO_RealToDateTime.h>


DFO_RealToDateTime :: DFO_RealToDateTime()
    : FuncObjC("Convert Real to Date/Time Label")
{
    timeSpecIsMaster = true;

    CopyString(arbitraryTimeStr, "00:00:00", dateTimeStrLen);
    CopyString(arbitraryDateStr, "01/01/2000", dateTimeStrLen);

    labelType = ltDate;

    timeFormat = DC_DateTime::tfHHMMSS24;
    CopyString(dateTimeDelimStr, " / ", dateTimeStrLen);

    InitCommon();
}

DFO_RealToDateTime::DFO_RealToDateTime(const DFO_RealToDateTime& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_RealToDateTime :: ~DFO_RealToDateTime ()
{
}

void DFO_RealToDateTime::InitCommon()
{
    AddInPort(inputRealObjRef, typeid(DO_Real));

    AddMSInPort(timeSpecMasterObjRef, typeid(DO_DateTimeSpec), timeSpecIsMaster);

    AddOutPort(outputLabelDO);

    DoStatusChk();
}

DFO_RealToDateTime& DFO_RealToDateTime::operator= (const DFO_RealToDateTime& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  DFO_RealToDateTime::LocalCopy(const DFO_RealToDateTime& a)
{
    timeSpecIsMaster        = a.timeSpecIsMaster;
    timeSpecMasterObjRef    = a.timeSpecMasterObjRef;

    dateTimeSpec = a.dateTimeSpec;

    CopyString(arbitraryTimeStr, a.arbitraryTimeStr, dateTimeStrLen);
    CopyString(arbitraryDateStr, a.arbitraryDateStr, dateTimeStrLen);

    labelType = a.labelType;

    dateFormat = a.dateFormat;
    timeFormat = a.timeFormat;
    CopyString(dateTimeDelimStr, a.dateTimeDelimStr, dateTimeStrLen);

    labelOverride = a.labelOverride;
}



void  DFO_RealToDateTime:: DoStatusChk()
{
    // base class check
    FuncObjC::DoStatusChk();

    if (!CheckInputFO(inputRealObjRef, "Real to convert"))
        return;

    const DO_Real* tmpDO = static_cast<const DO_Real*>(GetInPortData(inputRealObjRef));
    currRealVal = tmpDO->realVal;
    inputLabel = tmpDO->dataTypeLabel;

    if (RealIsNull(currRealVal))
        SetObjErrMsg("input real is null");

    if ((!timeSpecIsMaster) && (CheckInputFO(timeSpecMasterObjRef, "Time/Date master spec")))
    {
        DO_DateTimeSpec*  inputDO = static_cast<DO_DateTimeSpec*>(GetInPortData(timeSpecMasterObjRef));
        currDateTimeSpec = *(inputDO->dateTimeSpec);
    }

    if (StatusNotOK())
        return;

    if (timeSpecIsMaster) 
    {
        if (dateTimeSpec.timeSpecType == DC_DateTimeSpec::tstArbitraryZero)
        {
            DC_DateTimeConv temp;
            if (!temp.StringToDate(arbitraryDateStr))
                SetObjErrMsg("zero date is bad (must be MM/DD/YY)");
            if (!temp.StringToTime(arbitraryTimeStr))
                SetObjErrMsg("zero time is bad (must be HH:MM:SS)");
            dateTimeSpec.arbitraryZeroDT = temp;
        }
        currDateTimeSpec = dateTimeSpec;
    }

    if ((currDateTimeSpec.timeSpecType == DC_DateTimeSpec::tstExcelDays) &&
        ((currRealVal < 0.0) || (currRealVal > 1.0E6)))
        SetObjErrMsg("Excel Days should be > 0.0 and < 3.5+05");
}


void DFO_RealToDateTime:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    outputLabelDO.SetTypeLabel(labelOverride.GetLabel(inputLabel));

    DC_DateTime::TimeUnits  currUnits = currDateTimeSpec.timeSpecUnits;  
    double                  zeroTime = 0.0;

    switch (currDateTimeSpec.timeSpecType) {
        case DC_DateTimeSpec::tstArbitraryZero : { 
            zeroTime = currDateTimeSpec.arbitraryZeroDT.GetExcelTime(currUnits);
            break;
        }
        case DC_DateTimeSpec::tstCalendarYear : {
            DC_DateTime tempTime;
            tempTime.year = currDateTimeSpec.calendarYearStart;
            zeroTime = tempTime.GetExcelTime(currUnits);
            break;
        }
        case DC_DateTimeSpec::tstExcelDays : {
            currUnits = DC_DateTime::tuDays;
            break;
        }
    }

    DC_DateTime currTime;
    currTime.SetExcelTime(currRealVal + zeroTime, currUnits);
    if (!currTime.IsOK())
    {
        SetObjErrMsg("error in time/date conversion - probably out of range");
        outputLabelDO.SetValueLabel("bad T/D");
    }
    else
    {
        char labStr[120];
        switch (labelType) {
            case ltDate : {
                currTime.DateToString(labStr, 120, dateFormat);
                break;
            }
            case ltTime : {
                currTime.TimeToString(labStr, 120, timeFormat);
                break;
            }
            case ltTimeDate : {
                char timeStr[80]; 
                currTime.TimeToString(timeStr, 80, timeFormat);
                char dateStr[80];
                currTime.DateToString(dateStr, 80, dateFormat);
                MakeString(labStr, timeStr, dateTimeDelimStr, dateStr, 120);
                break;
            }
            case ltDateTime : {
                char timeStr[80]; 
                currTime.TimeToString(timeStr, 80, timeFormat);
                char dateStr[80];
                currTime.DateToString(dateStr, 80, dateFormat);
                MakeString(labStr, dateStr, dateTimeDelimStr, timeStr, 120);
                break;
            }
        }
        outputLabelDO.SetValueLabel(labStr);
    }
}

