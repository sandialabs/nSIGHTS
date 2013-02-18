///////////////////////////////////////////////////////////////////////////////////
//
// DC_DateTime.cpp
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
//              framework: Date and time data structures
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genClass/DC_DateTime.h>

DC_DateFormat DC_DateTime::defaultDF;

DC_DateTime::DC_DateTime()
{
    year   = 1900;
    month  = 1;
    day    = 1;

    hour   = 0;
    minute = 0;
    second = 0;
}

DC_DateTime::DC_DateTime(const DC_DateTime& a)
{
    LocalCopy(a);
}

DC_DateTime&  DC_DateTime::operator=(const DC_DateTime& a)
{
    if (this != &a)
        LocalCopy(a);
    return *this;
}


void DC_DateTime::LocalCopy(const DC_DateTime& a)
{
    year   = a.year;
    month  = a.month;
    day    = a.day;
    hour   = a.hour;
    minute = a.minute;
    second = a.second;
}
static double tconv[] = {31557600.0, 86400.0, 3600.0, 60.0, 1.0};

////////////////////////////////////////////////////////////////////////////
// Convert a Day, Month, Year tuple to an Excel datetime number
////////////////////////////////////////////////////////////////////////////
double DC_DateTime::GetExcelTime(TimeUnits tUnits)  const
{

    double ndays; //the number of Excel days

    // Oops: Excel/Lotus 123 think 2-29-1900 is a leap year, but it's not
    if(day == 29 && month == 2 && year==1900)
    {
        ndays = 60;
    }
    else
    {
        //convert to Julian calendar and then subtract 2415019 (ask Microsoft)
        ndays = (1461 * (year + 4800 + (month - 14)/12))/4 + (367 * (month - 2 - 12*((month - 14)/12)))/12 - (3 * (( year + 4900 + (month - 14)/12)/100))/4 + day - 2415019 - 32075;

        if ((int) ndays <= 60)
        {
            //get rid of the extra Microsoft day.
            ndays--;
        }
    }

    double nsecs = ndays * 86400.0 + double(hour) * 3600.0 + double(minute) * 60.0 + double(second);

    //convert to appropriate units
    return nsecs / tconv[int(tUnits)];

}


void DC_DateTime::SetExcelTime(const double& inExcel,TimeUnits tUnits)
{
    //convert to seconds from whatever units we are in.
    double nSecs = inExcel * tconv[int(tUnits)];
    //return -1's to indicate bad date.
    if ((nSecs < 0.0) || (nSecs > 3.1E10))
    {
        day  = -1;
        hour = -1;
        return;
    }

    //grab the number of days (since this is where the meat of the conversion lies)
    int nDays = int(floor(nSecs / 86400.0));
    int ExcelDays = nDays;
    if (nDays == 60)
    {
        // Oops: Excel/Lotus 123 think 29-02-1900 is a leap year, but it's not
        day = 29;
        month = 2;
        year = 1900;
    }
    else
    {
        if (nDays < 60)
        {
            // Oops: Excel/Lotus 123 think 29-02-1900 is a leap year, but it's not
            ExcelDays++;
        }

        // Convert from Julian (modified by 2415019 thanks to M$) to day, month, year tuple
        int l = ExcelDays + 68569 + 2415019;
        int n = int(( 4 * l ) / 146097);
        l = l - int(( 146097 * n + 3 ) / 4);
        int i = int(( 4000 * ( l + 1 ) ) / 1461001);
        l = l - int(( 1461 * i ) / 4) + 31;
        int j = int(( 80 * l ) / 2447);
        day = l - int(( 2447 * j ) / 80);
        l = int(j / 11);
        month = j + 2 - ( 12 * l );
        year = 100 * ( n - 49 ) + i + l;

    }

    // get residual time
    nSecs -= double(nDays) * 86400.0;

    hour = int(nSecs / 3600.0);

    nSecs -= double(hour) * 3600.0;
    minute = int(nSecs / 60.0);

    nSecs -= double(minute) * 60.0;

    second = int(nSecs);
}

bool DC_DateTime::DateOK() const
{
    return  (year >= 1900) && (year  < 10000) &&
        (month > 0) && (month < 13) &&
        (day > 0) && (day < 32); // not going to bother on checking for April 31 etc
}

void DC_DateTime::DateToString(char*    dateStr, int maxLen,
                               const DC_DateFormat& df) const
{
    df.DateToString(dateStr, maxLen, *this);
}


bool DC_DateTime::TimeOK() const
{
    return  (second >= 0) && (second < 60) &&
        (minute >= 0) && (minute < 60) &&
        (hour >= 0) && (hour < 24);
}

void DC_DateTime::TimeToString(char*    timeStr, int maxLen,
                               TimeFormat tf) const
{
    if (!TimeOK())
        {
            CopyString(timeStr, nullStr, maxLen);
            return;
        }

    char secStr[5], minStr[5], hrStr[5];
    IntToString(second, secStr, 2, 5);
    IntToString(minute, minStr, 2, 5);

    bool doAMPM = (tf == tfHHMMSSAP) || (tf == tfHHMMAP);
    if (doAMPM)
        {
            if (hour > 12)
                IntToString(hour - 12, hrStr, 5);
            else if (hour > 0)
                IntToString(hour, hrStr, 5);
            else
                CopyString(hrStr, "12", 5);
        }
    else
        IntToString(hour, hrStr, 2, 5);

    switch (tf)
        {
        case tfHHMMSS24: case tfHHMMSSAP: {
            MakeString(timeStr, hrStr, ":", minStr, ":", secStr, maxLen);
            break;
        }
        case tfHHMM24: case tfHHMMAP:  {
            MakeString(timeStr, hrStr, ":", minStr, maxLen);
            break;
        }
        case tfMMSS: {
            MakeString(timeStr, minStr, ":", secStr, maxLen);
            break;
        }
        }

    if (doAMPM)
        {
            if (hour > 11)
                ConcatString(timeStr, " PM", maxLen);
            else
                ConcatString(timeStr, " AM", maxLen);
        }


}


DC_DateTimeSpec::DC_DateTimeSpec()
{
    timeSpecType = tstExcelDays;
    timeSpecUnits = DC_DateTime::tuSeconds;  // not used for tatExcelDays
    calendarYearStart = 2003;
}

DC_DateTimeSpec::DC_DateTimeSpec(const DC_DateTimeSpec& a)
{
    LocalCopy(a);
}

DC_DateTimeSpec&  DC_DateTimeSpec::operator=(const DC_DateTimeSpec& a)
{
    if (this != &a)
        LocalCopy(a);
    return *this;
}


void DC_DateTimeSpec::LocalCopy(const DC_DateTimeSpec& a)
{
    timeSpecType = a.timeSpecType;
    timeSpecUnits = a.timeSpecUnits;
    calendarYearStart = a.calendarYearStart;
    arbitraryZeroDT = a.arbitraryZeroDT;
}



// ********************** conversion support

DC_DateTimeConv::DC_DateTimeConv()
{
    ClearParseChars();
    SetToNull(parsedStr);
}

bool DC_DateTimeConv::StringToDate(const char* tempStr)
{
    SetParseChars("/-");
    CopyString(parsedStr, tempStr, maxStrLen);
    PrepareParseString(parsedStr, parseChars);

    month = GetNextInt();
    day = GetNextInt();
    year = GetNextInt();

    // check for y m d conversion
    if (month > 12)
        {
            int ty = month;
            int tm = day;
            day = year;
            month = tm;
            year = ty;
        }

    // kluge assuming 2 character tim3e
    if ((year >= 0) && (year < 100))
        if (year > 50)
            year += 1900;
        else
            year += 2000;

    return TimeOK();
}

bool DC_DateTimeConv::StringToTime(const char* tempStr)
{
    SetParseChars(":");
    CopyString(parsedStr, tempStr, maxStrLen);
    PrepareParseString(parsedStr, parseChars);

    hour = GetNextInt();
    minute = GetNextInt();
    second = GetNextInt();

    return DateOK();
}

void DC_DateTimeConv::SetParseChars(const char newParseChars[])
{
    ClearParseChars();
    ConcatString(parseChars, newParseChars, maxParseChars);
}

void DC_DateTimeConv::ClearParseChars()
{
    parseChars[0] = ' ';
    parseChars[1] = '\t';
    parseChars[2] = '\0';
}


int DC_DateTimeConv::GetNextInt()
{
    char tempStr[maxStrLen];
    if (!ParseGetNextString(parsedStr, tempStr, maxStrLen))
        return -1;

    int tempnum = -1;
    if (!IsValidInt(tempStr, tempnum))
        tempnum = -1;

    return tempnum;
}




DC_DateFormat::DC_DateFormat()
{
    dateOrder = doDMY;
    monthFormat = mfJan;
    dayHasLeadingZero = true;
    yearIsFull = false;
    CopyString(delim1, "-", delimStrLen);
    CopyString(delim2, "-", delimStrLen);
}

DC_DateFormat::DC_DateFormat(const DC_DateFormat& a)
{
    LocalCopy(a);
}

DC_DateFormat&  DC_DateFormat::operator=(const DC_DateFormat& a)
{
    if (this != &a)
        LocalCopy(a);
    return *this;
}


void DC_DateFormat::LocalCopy(const DC_DateFormat& a)
{
    dateOrder               = a.dateOrder;
    monthFormat             = a.monthFormat;
    dayHasLeadingZero       = a.dayHasLeadingZero;
    yearIsFull              = a.yearIsFull;
    CopyString(delim1, a.delim1, delimStrLen);
    CopyString(delim2, a.delim2, delimStrLen);
}

void DC_DateFormat::DateToString(char*  dateStr, int maxLen,
                                 const DC_DateTime& dtData) const
{
    if (!dtData.DateOK())
        {
            CopyString(dateStr, nullStr, maxLen);
            return;
        }

    int day = dtData.day;
    int month = dtData.month;
    int year = dtData.year;

    char dayStr[5];
    if (dayHasLeadingZero)
        IntToString(day, dayStr, 2, 5);
    else
        IntToString(day, dayStr, 5);

    char monStr[20];

    switch (monthFormat) {
    case mf01: {
        IntToString(month, monStr, 2, 5);
        break;
    }
    case mf1: {
        IntToString(month, monStr, 5);
        break;
    }
    case mfJan : {
        static const char* months[] = {"Jan", "Feb", "Mar", "Apr",
                                       "May", "Jun", "Jul", "Aug",
                                       "Sep", "Oct", "Nov", "Dec"};
        CopyString(monStr, months[month - 1], 5);
        break;
    }
    case mfJAN : {
        static const char* months[] = {"JAN", "FEB", "MAR", "APR",
                                       "MAY", "JUN", "JUL", "AUG",
                                       "SEP", "OCT", "NOV", "DEC"};
        CopyString(monStr, months[month - 1], 5);
        break;
    }
    case mfJanuary : {
        static const char* months[] = {"January", "February", "March", "April",
                                       "May", "June", "July", "August",
                                       "September", "October", "November", "December"};
        CopyString(monStr, months[month - 1], 20);
        break;
    }
    }



    char yearStr[5];
    if (yearIsFull)
        IntToString(year, yearStr, 2, 5);
    else
        if (year < 2000)
            IntToString(year - 1900, yearStr, 2, 5);
        else
            IntToString(year - 2000, yearStr, 2, 5);


    switch (dateOrder)
        {
        case doDMY: {
            MakeString(dateStr, dayStr, delim1, monStr, delim2, yearStr, maxLen);
            break;
        }
        case doMDY: {
            MakeString(dateStr, monStr, delim1, dayStr, delim2, yearStr, maxLen);
            break;
        }
        case doYMD: {
            MakeString(dateStr, yearStr, delim1, monStr, delim2, dayStr, maxLen);
            break;
        }
        case doMY: {
            MakeString(dateStr, monStr, delim1, yearStr, maxLen);
            break;
        }
        case doYM: {
            MakeString(dateStr, yearStr, delim1, monStr, maxLen);
            break;
        }
        case doMD: {
            MakeString(dateStr, monStr, delim1, dayStr, maxLen);
            break;
        }
        case doDM: {
            MakeString(dateStr, dayStr, delim1, monStr, maxLen);
            break;
        }
        }
}

