///////////////////////////////////////////////////////////////////////////////////
//
// DC_DateTime.h
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
//              framework: Date and time data structures.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_DATETIME_H
#define DC_DATETIME_H

class DC_DateFormat;

class DC_DateTime       {
public:
  enum    TimeUnits       {tuYears, tuDays, tuHours, tuMinutes, tuSeconds};
  enum    TimeFormat      {tfHHMMSS24, tfHHMMSSAP, tfHHMM24, tfHHMMAP, tfMMSS};

  int                     year;
  int                     month;
  int                     day;
  int                     hour;
  int                     minute;
  int                     second;

  static DC_DateFormat defaultDF;

public:
  DC_DateTime(); // init to 1/1/1900 00:00:00
  DC_DateTime(const DC_DateTime& a);
  ~DC_DateTime(){};
  DC_DateTime&        operator=(const DC_DateTime& a);


  double              GetExcelTime(TimeUnits tUnits = tuSeconds) const;
  // time since 1900 -duplicates Excel 1900 bug
  // 01 jan 1900 = 1.0days

  void                SetExcelTime(const double& inExcel,
                      TimeUnits tUnits = tuSeconds);

  void                DateToString(char*      dateStr, int maxLen,
                      const DC_DateFormat& df = defaultDF) const;

  void                TimeToString(char*      timeStr, int maxLen,
                      TimeFormat tf = tfHHMMSS24) const;

  bool                DateOK() const;
  bool                TimeOK() const;
  bool                IsOK() const {return DateOK() && TimeOK();}

private:
  void                LocalCopy(const DC_DateTime& a);  // implementation of copy

};

class DC_DateTimeSpec {
public:
  enum    TimeSpecType            {tstArbitraryZero, tstCalendarYear, tstExcelDays};

  TimeSpecType                    timeSpecType;
  DC_DateTime::TimeUnits          timeSpecUnits;  // not used for tatExcelDays
  int                             calendarYearStart;
  DC_DateTime                     arbitraryZeroDT;

public:
  DC_DateTimeSpec();
  DC_DateTimeSpec(const DC_DateTimeSpec& a);
  ~DC_DateTimeSpec(){};
  DC_DateTimeSpec&    operator=(const DC_DateTimeSpec& a);

private:
  void                LocalCopy(const DC_DateTimeSpec& a);  // implementation of copy

};



class DC_DateTimeConv : public DC_DateTime {

private:
  enum                {maxParseChars = 40, maxStrLen = 80};
  char                parseChars[maxParseChars];
  char                parsedStr[maxStrLen];

public:
  DC_DateTimeConv(); // init to 1/1/1900 00:00:00
  ~DC_DateTimeConv(){};

  bool                StringToDate(const char* tempStr); // converts mm/dd/yy
  bool                StringToTime(const char* tempStr); // converts hh:mm:ss

private:
  DC_DateTimeConv(const DC_DateTimeConv& a) {};
  DC_DateTimeConv& operator=(const DC_DateTimeConv& a) {return *this;}

  void                SetParseChars(const char newParseChars[]);
  void                ClearParseChars();
  int                 GetNextInt();
};


class DC_DateFormat {
public:
  enum    DateOrder {doDMY, doMDY, doYMD, doMY, doYM, doMD, doDM};
  enum    MonthFormat {mf01, mf1, mfJan, mfJAN, mfJanuary};
  enum    {delimStrLen = 5};

  DateOrder       dateOrder;
  MonthFormat     monthFormat;
  bool            dayHasLeadingZero;
  bool            yearIsFull;
  char            delim1[delimStrLen];
  char            delim2[delimStrLen];

public:
  DC_DateFormat();

  DC_DateFormat(const DC_DateFormat& a);
  ~DC_DateFormat(){};
  DC_DateFormat&      operator=(const DC_DateFormat& a);


  void                DateToString(char* dateStr, int maxLen,
                      const DC_DateTime& dtData) const;

private:
  void                LocalCopy(const DC_DateFormat& a);


};


#endif // DC_DATETIME_H

