///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadMiniTrollCF.cpp
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

#include <sstream>

#include <genClass/C_ConvFile.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>
#include <genClass/U_Msg.h>
#include <genClass/DC_DateTime.h>
#include <genClass/SC_RealFormat.h>

#include <genDataClass/DSC_TimeBase.h>

#include <nsDFO/DFO_ReadMiniTroll.h>


//KSB 2/15/12 why isn't this in a header file?
class MiniTrollFile :  public ConvFileC {

    public:
        DFO_ReadMiniTroll&  mfObj;

    private:
        double                  zeroTime;
        DC_DateTime::TimeUnits  elTimeUnits;
        int                     westbayNullCount;

    public:
                        MiniTrollFile(DFO_ReadMiniTroll&    inObj);

        void            ReadRawMT();
        void            ReadRaw0708MT(bool is08);
        void            ReadCSVMT();
        void            ReadCSVDAS();
        void            ReadWestbay();
    private:
        void            ElapsedTimeSetup();
        double          WestbayGetNextDouble();

};

MiniTrollFile :: MiniTrollFile(DFO_ReadMiniTroll&   inObj)
    : mfObj(inObj)
{
}

void MiniTrollFile::ElapsedTimeSetup()
{
    // set for calendar time ...
    // for 1900 assume Excel time 01 Jan 1900 = 1.0
    zeroTime = 0.0;
    elTimeUnits = mfObj.timeSpecDC.timeSpecUnits;
    if (mfObj.timeSpecDC.timeSpecType == DC_DateTimeSpec::tstExcelDays)
        elTimeUnits = DC_DateTime::tuDays;
    else
    {
        DC_DateTimeConv currDT;
        currDT.year = mfObj.timeSpecDC.calendarYearStart;
        zeroTime = currDT.GetExcelTime(elTimeUnits);
    }

}
void  MiniTrollFile::ReadRawMT()
{
    CFopenText(mfObj.mtFname);

    if (!CFreadUntil("TOTAL", 1, false))
        CFtextError("Data header TOTAL not found");

    // read header info

    // skip "TOTAL DATA SAMPLES"
    CFskipNextEntry(3);
    int rowCount = CFgetNextInt();


    // get # of channels and data type
    SC_StringArray searchStrs;

    searchStrs.Alloc(2);
    searchStrs += "Channel";
    searchStrs += "---";

    SC_StringArray measTypeStrs;
    measTypeStrs.SetResizable(8);

    int readType = 1;
    while (true) {

        int readType = CFreadUntil(searchStrs, 1, false);
        if (readType < 0)
            CFtextError("Channel number [x] or data start ----- not found");
        if (readType == 1)  // data start
            break;

        if (!CFreadUntil("Measurement", 3, false))
            CFtextError("Measurement type: not found");

        // skip "Measurement type:"
        CFskipNextEntry(2);

        // get channel type
        char tempStr[80];
        CFgetNextStr(tempStr, 80);

        measTypeStrs += tempStr;
    }
    if (measTypeStrs.IsEmpty())
        CFtextError("No Channels found");

    int colCount = measTypeStrs.Size();
    int readCol = 1;
    if (mfObj.readAllColumns)
        readCol = colCount;

    DC_TableData& currTable = mfObj.tableData;

    if (!currTable.ResizableAlloc(readCol + 1, rowCount, 40))
        CFtextError("table allocation error");

    currTable.SetTitle(0, "Time");

    int col = 1;
    for (int i = 0; i < colCount; i++)
    {
        if (mfObj.readAllColumns)
            col = i + 1;
        else
        {
            if (i != mfObj.yColumn)
                continue;
        }
        currTable.SetTitle(col, measTypeStrs[i]);
    }

    currTable.dataTable.SetSize(readCol);


    ElapsedTimeSetup();

    // read data
    char timeStr[80], dateStr[80], fullStr[80];
    DC_DateTimeConv currDT;

    for (int i = 0; i < rowCount; i++)
    {
        if (CFeof())
            CFtextError("EOF before total number of rows read.");
        if (!CFreadText())
            CFtextError("Blank line in data ??");

        CFgetNextStr(dateStr, 80);      //date
        currDT.StringToDate(dateStr);

        CFgetNextStr(timeStr, 80);      //time
        currDT.StringToTime(timeStr);

        MakeString(fullStr, dateStr, " ", timeStr, 80);
        currTable.rowDesc += fullStr;

        double currTime = currDT.GetExcelTime(elTimeUnits);

        if ((i == 0) && (mfObj.timeSpecDC.timeSpecType == DC_DateTimeSpec::tstArbitraryZero))
        {
            mfObj.timeSpecDC.arbitraryZeroDT = currDT;
            zeroTime = currTime;
        }

        currTable.dataTable[0] += currTime - zeroTime;

        // skip mt elapsed time
        CFskipNextEntry(1);

        char tempStr[80];
        if (mfObj.readAllColumns)
        {
            for (int i = 0; i < colCount; i++)
            {
                if (!CFgetNextStr(tempStr, 80))
                    CFtextError("insufficient columns of data");

                double dVal;
                if (!IsValidReal(tempStr, dVal))
                    CFtextError("error converting data value");

                currTable.dataTable[i + 1] += dVal;
            }
        }
        else
        {
            CFskipNextEntry(mfObj.yColumn);

            if (!CFgetNextStr(tempStr, 80))
                CFtextError("insufficient columns of data");

            double dVal;
            if (!IsValidReal(tempStr, dVal))
                CFtextError("error converting data value");

            currTable.dataTable[1] += dVal;
        }

    }

}

void  MiniTrollFile::ReadCSVMT()
{
    CFopenText(mfObj.mtFname);

    SetParseChars(",", true);  // just commas

    char tempStr[80];
    int colCount = 0;
    if (mfObj.readAllColumns)
    {
        // skip down to data start to get ncol
        CFskipLines(4);
        CFskipNextEntry(1); // date/time
        while (CFgetNextStr(tempStr, 80))
        {
            double dummy;
            if (!IsValidReal(tempStr, dummy))
                break;
            colCount++;
        }
        CFreset(); // back to start
    }
    else
        colCount = 1;

    // allocate 1000 rows to start
    DC_TableData& currTable = mfObj.tableData;
    if (!currTable.ResizableAlloc(colCount + 1, 8000, 40))
        CFtextError("table allocation error");

    // this header is known
    currTable.SetTitle(0, "Time");


    // read first line for remaining header data
    CFreadText();
    CFskipNextEntry(1); // date/time
    if (mfObj.readAllColumns)
    {
        for (int i = 0; i < colCount; i++)
        {
            CFgetNextStr(tempStr, 80);
            currTable.SetTitle(i + 1, tempStr);
        }
    }
    else
    {
        CFskipNextEntry(mfObj.yColumn - 1);
        CFgetNextStr(tempStr, 80);
        currTable.SetTitle(1, tempStr);
    }

    // skip units & ---- lines
    CFskipLines(2);

    ElapsedTimeSetup();

    // read data
    char timeStr[80], dateStr[80];
    DC_DateTimeConv currDT;

    while (true) {

        if (CFeof())
            break;

        if (!CFreadText())
            break;

        CFgetNextStr(dateStr, 80);      //date/time
        FullTrim(dateStr);

        if (IsNull(dateStr))
            break;

        currTable.rowDesc += dateStr;  // add to desc as is


        CopyString(timeStr, dateStr, 80);

        // split at ' '
        int splitPos = CharPositn(dateStr, ' ', 1);
        if (splitPos < 0)
            CFtextError("No blank between time & date ?");
        dateStr[splitPos] = '\0';
        DeleteChar(timeStr, 0, splitPos);
        FullTrim(timeStr);

        currDT.StringToDate(dateStr);
        currDT.StringToTime(timeStr);
        double currTime = currDT.GetExcelTime(elTimeUnits);
        if (currTable.dataTable[0].IsEmpty() && (mfObj.timeSpecDC.timeSpecType == DC_DateTimeSpec::tstArbitraryZero))
        {
            mfObj.timeSpecDC.arbitraryZeroDT = currDT;
            zeroTime = currTime;
        }

        currTable.dataTable[0] += currTime - zeroTime;

        if (mfObj.readAllColumns)
        {
            for (int i = 0; i < colCount; i++)
            {
                if (!CFgetNextStr(tempStr, 80))
                    CFtextError("insufficient columns of data");

                double dVal;
                if (!IsValidReal(tempStr, dVal))
                    CFtextError("error converting data value");

                currTable.dataTable[i + 1] += dVal;
            }
        }
        else
        {
            CFskipNextEntry(mfObj.yColumn - 1);
            currTable.dataTable[1] += CFgetNextDouble();
        }
    }
}

void  MiniTrollFile::ReadCSVDAS()
{
    CFopenText(mfObj.mtFname);

    SetParseChars(",\"", true);  // commas & quotes

    // allocate 1000 rows to start
    DC_TableData& currTable = mfObj.tableData;

    int ncol = 2;
    if (mfObj.dasReadID)
        ncol++;
    if (mfObj.dasReadKey)
        ncol++;

    if (!currTable.ResizableAlloc(ncol, 8000, 40))
        CFtextError("table allocation error");

    // this header is known
    currTable.SetTitle(0, "Time");
    currTable.SetTitle(1, "Data");
    ncol = 2;
    int keyCol, idCol;
    if (mfObj.dasReadID)
    {
        idCol = ncol;
        currTable.SetTitle(ncol++, "ID");
    }
    if (mfObj.dasReadKey)
    {
        keyCol = ncol;
        currTable.SetTitle(ncol, "Key");
    }

    // skip header line
    CFskipLines(1);

    ElapsedTimeSetup();

    // read data
    char timeStr[80], dateStr[80];
    DC_DateTimeConv currDT;

    while (true) {

        if (CFeof())
            break;

        if (!CFreadText())
            break;

        // kluge - clear leading quote
        parsedLine[0] = ' ';

        // key
        if (mfObj.dasReadKey)
        {
            currTable.dataTable[keyCol] += CFgetNextDouble();
        }
        else
            CFskipNextEntry(1);
        // ID
        if (mfObj.dasReadID)
        {
            currTable.dataTable[idCol] += CFgetNextDouble();
        }
        else
            CFskipNextEntry(1);

        CFgetNextStr(dateStr, 80);      //date/time
        FullTrim(dateStr);

        if (IsNull(dateStr))
            break;

        currTable.rowDesc += dateStr;  // add to desc as is


        CopyString(timeStr, dateStr, 80);

        // split at ' '
        int splitPos = CharPositn(dateStr, ' ', 1);
        if (splitPos < 0)
            CFtextError("No blank between time & date ?");
        dateStr[splitPos] = '\0';
        DeleteChar(timeStr, 0, splitPos);
        FullTrim(timeStr);

        currDT.StringToDate(dateStr);
        currDT.StringToTime(timeStr);
        double currTime = currDT.GetExcelTime(elTimeUnits);
        if (currTable.dataTable[0].IsEmpty() && (mfObj.timeSpecDC.timeSpecType == DC_DateTimeSpec::tstArbitraryZero))
        {
            mfObj.timeSpecDC.arbitraryZeroDT = currDT;
            zeroTime = currTime;
        }

        currTable.dataTable[0] += currTime - zeroTime;

        // skip ms
        CFskipNextEntry(1);

        // get data
        currTable.dataTable[1] += CFgetNextDouble();
    }
}

void  MiniTrollFile::ReadRaw0708MT(bool is08)
{
    CFopenText(mfObj.mtFname);

    if (is08)
        SetParseChars(", ", true);  // commas

    if (!CFreadUntil("Record", 1, false))
        CFtextError("Data header ""Record Count"" not found");

    // read header info
    // skip "Record Count
    CFskipNextEntry(2);
    int rowCount = CFgetNextInt();


    // go to sensor IDs
    if (!CFreadUntil("Date", 1, false))
        CFtextError("Data header ""Date"" not found");

    // skip "Date and Time Seconds
    CFskipNextEntry(4);

    SC_StringArray measTypeStrs;
    measTypeStrs.SetResizable(8);
    char typeStr[80];

    while (CFgetNextStr(typeStr, 80)) {
        char unitStr[80];
        if (!CFgetNextStr(unitStr, 80))
            CFtextError("Expecting sensor units");
        char sensorStr[80];
        MakeString(sensorStr, typeStr, unitStr, 80);
        measTypeStrs += sensorStr;
    }

    int readCol = measTypeStrs.Size();

    DC_TableData& currTable = mfObj.tableData;

    if (!currTable.ResizableAlloc(readCol + 2, rowCount, 40))
        CFtextError("table allocation error");

    currTable.SetTitle(0, "DateTime");
    currTable.SetTitle(1, "ElapsedSec");
    for (int i = 0; i < readCol; i++)
        currTable.SetTitle(i + 2, measTypeStrs[i]);
    currTable.dataTable.SetSize(readCol + 1);

    ElapsedTimeSetup();

    // read data
    char timeStr[80], dateStr[80], ampmStr[80], fullStr[80];
    DC_DateTimeConv currDT;

    if (!is08)
        CFskipLines(1); // skip ------- line

    for (int i = 0; i < rowCount; i++)
    {
        if (CFeof())
            CFtextError("EOF before total number of rows read.");
        if (!CFreadText())
            CFtextError("Blank line in data ??");

        CFgetNextStr(dateStr, 80);      //date
        currDT.StringToDate(dateStr);

        CFgetNextStr(timeStr, 80);      //time
        currDT.StringToTime(timeStr);

        CFgetNextStr(ampmStr, 80);      //AM PM
        if ((ampmStr[0] == 'P') && (currDT.hour < 12))
            currDT.hour += 12;
        else if ((ampmStr[0] == 'A') && (currDT.hour == 12))
            currDT.hour = 0;

        MakeString(fullStr, dateStr, " ", timeStr, " ", ampmStr, 80);
        currTable.rowDesc += fullStr;

        double currTime = currDT.GetExcelTime(elTimeUnits);

        if ((i == 0) && (mfObj.timeSpecDC.timeSpecType == DC_DateTimeSpec::tstArbitraryZero))
        {
            mfObj.timeSpecDC.arbitraryZeroDT = currDT;
            zeroTime = currTime;
        }

        currTable.dataTable[0] += currTime - zeroTime;

        currTable.dataTable[1] += CFgetNextDouble();

        for (int j = 0; j < readCol; j++)
            currTable.dataTable[j + 2] += CFgetNextDouble();

    }

}

double  MiniTrollFile::WestbayGetNextDouble()
{
    char tempStr[80];
    if (!CFgetNextStr(tempStr, 80))
        CFtextError("Expecting westbay port data");

    double tmpVal;
    if (CFstrIsDouble(tempStr, tmpVal, false))
        return tmpVal;

    if (SameString(tempStr, "n/a"))
    {
        westbayNullCount++;
        return nullReal;
    }

    CFtextError("Expecting real or n/a");
    return 0.0;
}

void  MiniTrollFile::ReadWestbay()
{
    CFopenText(mfObj.mtFname);

    SetParseChars(",\"", true);  // commas & quotes

    // allocate 1000 rows to start
    DC_TableData& currTable = mfObj.tableData;
    if (!currTable.ResizableAlloc(50, 8000, 60))
        CFtextError("table allocation error");

    // this header is known
    currTable.SetTitle(0, "Time/Date");

    ElapsedTimeSetup();


    // skip to Port information for Depth
    CFreadUntil("Port Information");

    CFskipLines(1);  // next blank

    // read port data and get depths
    SetParseChars("#: ", true);  //

    mfObj.westbayDepths.SetResizable(10);

    SC_IntArray westbayPorts;
    westbayPorts.SetResizable(10);

    while (true) {

        if (CFeof())
            CFtextError("EOF while reading ports");

        if (!CFreadText())
            break;

        // skip "Port
        CFskipNextEntry(1);
        westbayPorts += CFgetNextInt();

        // - Depth:"  - 2 fields
        CFskipNextEntry(2);
        mfObj.westbayDepths += CFgetNextDouble(); //date/time
    }

    // assume P & T for each tran
    int nport = mfObj.westbayDepths.Size();
    SC_IntArray depthMap;
    depthMap.AllocAndIndex(nport);
    mfObj.westbayDepths.Sort(true, depthMap);

    for (int i = 0; i < nport; i++)
    {
        char titleStr[80], depthStr[80], portStr[80];
        RealToString(SC_DecFormat(3), mfObj.westbayDepths[i], depthStr, 80);
        IntToString(westbayPorts[depthMap[i]], portStr, 2, 80);
        MakeString(titleStr, "P", portStr, ":",depthStr,"P", 80);
        currTable.SetTitle(i * 2 + 1, titleStr);
        MakeString(titleStr, "P", portStr, ":",depthStr,"T", 80);
        currTable.SetTitle(i * 2 + 2, titleStr);
    }


    SetParseChars(",\"", true);  // commas & quotes

    // skip to Data
    CFreadUntil("[Data]");
    CFskipLines(4);  // title lines

    // read data
    char timeStr[80], dateStr[80];
    DC_DateTimeConv currDT;
    while (true) {

        if (CFeof())
            break;

        if (!CFreadText())
            break;

        // kluge - clear leading quote
        parsedLine[0] = ' ';

        CFgetNextStr(dateStr, 80);      //date/time
        FullTrim(dateStr);

        if (IsNull(dateStr))
            break;

        currTable.rowDesc += dateStr;  // add to desc as is

        CopyString(timeStr, dateStr, 80);

        // split at ' '
        int splitPos = CharPositn(dateStr, ' ', 1);
        if (splitPos < 0)
            CFtextError("No blank between time & date ?");
        dateStr[splitPos] = '\0';
        DeleteChar(timeStr, 0, splitPos);
        FullTrim(timeStr);

        currDT.StringToDate(dateStr);
        currDT.StringToTime(timeStr);
        double currTime = currDT.GetExcelTime(elTimeUnits);
        if (currTable.dataTable[0].IsEmpty() && (mfObj.timeSpecDC.timeSpecType == DC_DateTimeSpec::tstArbitraryZero))
        {
            mfObj.timeSpecDC.arbitraryZeroDT = currDT;
            zeroTime = currTime;
        }

        double nextTime = currTime - zeroTime;
        if (currTable.dataTable[0].IsNotEmpty())
        {
            double deltaT = nextTime - currTable.dataTable[0].LastIndex();
            if (deltaT < mfObj.minimumDeltaT)
            {
                currTable.rowDesc.nlistData--;
                continue;
            }
        }

        currTable.dataTable[0] += nextTime;

        westbayNullCount = 0;
        for (int i = 0; i < nport; i++)
        {
            int portNum = depthMap[i] * 2 + 1;
            currTable.dataTable[portNum++] += WestbayGetNextDouble();
            currTable.dataTable[portNum] += WestbayGetNextDouble();
        }

        if (westbayNullCount > nport)  // more than half bad
        {
            // drop last row
            currTable.dataTable[0].nlistData--;
            for (int i = 0; i < nport; i++)
            {
                int portNum = depthMap[i] * 2 + 1;
                currTable.dataTable[portNum++].nlistData--;
                currTable.dataTable[portNum].nlistData--;
            }
            currTable.rowDesc.nlistData--;
        }
    }
}


bool DFO_ReadMiniTroll::ReadMiniTroll()
{
    MiniTrollFile cmFile(*this);
    cmFile.CFsetMsgStuff("ReadMiniTroll", "MiniTrollFile", 100);
    try
    {
        switch (fileType) {
            case mtRawText : {
                cmFile.ReadRawMT();
                break;
             }
            case mtCSV : {
                cmFile.ReadCSVMT();
                break;
             }
            case dasCSV : {
                cmFile.ReadCSVDAS();
                break;
             }
            case mtRawText07 : {
                cmFile.ReadRaw0708MT(false);
                break;
             }
            case wbMOSDAQ : {
                cmFile.ReadWestbay();
                break;
             }
            case mtRawText08 : {
                cmFile.ReadRaw0708MT(true);
                break;
             }
        }
    }
    catch (ConvFileC::CFReadError re) {
        SetObjErrMsg(re.errMsg);
        return false;
    }

    {
        using namespace std;
        ostringstream ostr;
        ostr <<  "MiniTroll read OK: " << tableData.columnDesc.Size() << " cols " << tableData.rowDesc.Size() << " rows "  << ends;
        GenAppInfoMsg("ReadMiniTroll", ostr.str().c_str());
    }

    return true;
}

