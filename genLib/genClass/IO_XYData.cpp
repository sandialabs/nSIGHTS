///////////////////////////////////////////////////////////////////////////////////
//
// IO_XYData.cpp
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
// DESCRIPTION: Platform independent and application independent base classes
//              and support classes: Reads/Writes DC_XYData in a variety of formats.
//
///////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClass/IO_XYData.h>

DSC_XYInputSpec::DSC_XYInputSpec()
{
    xyFileFormat = xyffAppDev;
    readColumnID = false;
    tableXcol    = 1;
    tableYcol    = 2;

    dateSpec.timeSpecUnits= DC_DateTime::tuYears;

}

DSC_XYInputSpec::DSC_XYInputSpec(const DSC_XYInputSpec& a)
{
    LocalCopy(a);
}
DSC_XYInputSpec& DSC_XYInputSpec::operator= (const DSC_XYInputSpec& a)
{
    if (&a != this)
        {
            LocalCopy(a);
        }
    return *this;
}
void  DSC_XYInputSpec::LocalCopy(const DSC_XYInputSpec& a)
{
    xyFileFormat    = a.xyFileFormat;
    readColumnID    = a.readColumnID;
    tableXcol       = a.tableXcol;
    tableYcol       = a.tableYcol;
    dateSpec        = a.dateSpec;
}


IO_XYData::I_XYDataFile::I_XYDataFile(const char* inputFile)
    : ConvFileC(inputFile, 8192)
{
}

void IO_XYData::I_XYDataFile::Init()
{
    nextInc = 5;
    nextCount = 50;
}


void IO_XYData::I_XYDataFile::ReadTableXY(bool          readColIDs,
                                          int           tableXcol,
                                          int           tableYcol,
                                          DC_XYData&    xyData)
{
    xyData.DeAlloc();
    SetParseChars(",");

    // count cols
    CFreadNonBlank();
    int colCount = 0;
    char tempStr[80];
    int idLen = 10;
    while (CFgetNextStr(tempStr, 80))
        {
            colCount++;
            if (int(StringLength(tempStr)) > idLen)
                idLen = StringLength(tempStr);
        }

    // input assume col 1 is first (users are all FORTRAN trained ...)
    tableXcol--;
    tableYcol--;
    int maxCol = tableYcol;
    if (tableXcol > tableYcol)
        maxCol = tableXcol;

    if (colCount < maxCol)
        CFtextError("Insufficient columns in file");

    // count rows
    int rowCount = 1;                   // already read first line
    while(true)
        {
            if (CFeof())
                break;
            if (CFreadText())
                rowCount++;
        }

    if (readColIDs)
        rowCount--;
    if (rowCount < 1)
        CFtextError("No data in file");

    if (!xyData.Alloc(rowCount))
        CFtextError("could not allocate xy");

    CFreset();

    if (readColIDs)
        {
            CFreadNonBlank();

            //  get y col headers
            int headerCount = 0;
            while (CFgetNextStr(tempStr, 80))
                {
                    if (headerCount == tableYcol)
                        {
                            CopyString(xyData.dataID, tempStr, DC_XYData::dataIDLen);
                            break;
                        }
                    headerCount++;
                }
        }

    // read rows
    SC_DoubleArray colVals(maxCol + 1);
    while(true)
        {
            if (CFeof())
                break;
            if (CFreadText())
                {
                    colVals.FillToAlloc(nullReal);
                    // read entire line - this code is for case where x and y col data may be incomplete
                    for (int i = 0; i <= maxCol; i++)
                        {
                            if (!CFgetNextStr(tempStr, 80))
                                break;
                            if (!IsValidReal(tempStr, colVals[i]))
                                colVals[i] = nullReal;
                        }

                    if ((!RealIsNull(colVals[tableXcol])) && (!RealIsNull(colVals[tableYcol])))
                        {
                            xyData.xData += colVals[tableXcol];
                            xyData.yData += colVals[tableYcol];
                        }
                }
        }
}

void IO_XYData::I_XYDataFile::ReadDateXY(const DC_DateTimeSpec& dateSpec,
                                         DC_XYData&             xyData)
{
    xyData.DeAlloc();
    SetParseChars(",");

    // count rows
    int rowCount = 0;
    while(true)
        {
            if (CFeof())
                break;
            if (CFreadText())
                rowCount++;
        }

    if (rowCount < 1)
        CFtextError("No data in file");

    if (!xyData.Alloc(rowCount))
        CFtextError("could not allocate xy");

    CFreset();

    DC_DateTime inputDate;
    double                                  zeroTime;
    DC_DateTime::TimeUnits  elTimeUnits;

    // set for calendar time ...
    // for 1900 assume Excel time 01 Jan 1900 = 1.0
    zeroTime = 0.0;
    elTimeUnits = dateSpec.timeSpecUnits;
    if (dateSpec.timeSpecType == DC_DateTimeSpec::tstExcelDays)
        elTimeUnits = DC_DateTime::tuDays;
    else
        {
            DC_DateTimeConv currDT;
            currDT.year = dateSpec.calendarYearStart;
            zeroTime = currDT.GetExcelTime(elTimeUnits);
        }

    SC_DoubleArray& xVal = xyData.xData;
    SC_DoubleArray& yVal = xyData.yData;

    while(true)
        {
            if (CFeof())
                break;
            if (CFreadText())
                {
                    inputDate.year = CFgetNextInt();
                    inputDate.month = CFgetNextInt();
                    inputDate.day = CFgetNextInt();

                    double currTime = inputDate.GetExcelTime(elTimeUnits);

                    if (xVal.IsEmpty() && (dateSpec.timeSpecType == DC_DateTimeSpec::tstArbitraryZero))
                        {
                            zeroTime = currTime;
                        }
                    xVal  += currTime - zeroTime;

                    yVal += CFgetNextDouble();
                }
        }
}


IO_XYData::O_XYDataFile::O_XYDataFile(const char* outputFile)
    :TxtFileC(outputFile, false)
{
}

void IO_XYData::O_XYDataFile::Init()
{
}

void IO_XYData::O_XYDataFile::WriteAppDevXY(bool             writeColHeaders,
                                            const DC_XYData& xyData)
{
    const int colWidth = 16;

    char tempStr[160];
    if (writeColHeaders)
        {
            CopyString(tempStr, "XData", 80);
            RightJustify(tempStr, colWidth);
            WriteSolidText(tempStr);

            CopyString(tempStr, "YData", 80);
            RightJustify(tempStr, colWidth);
            WriteSolidText(tempStr);

            WriteLine();
        }

    SC_RealFormat xyFormat(ncf_Scientific, 9);

    for (int i = 0; i < xyData.Size(); i++)
        {
            xyFormat.RealToString(xyData.xData[i], tempStr, 80);
            RightJustify(tempStr, colWidth);
            WriteSolidText(tempStr);

            xyFormat.RealToString(xyData.yData[i], tempStr, 80);
            RightJustify(tempStr, colWidth);
            WriteSolidText(tempStr);

            WriteLine();
        }
};



bool IO_XYData::ReadXYFile(const  char*             inputFileName,
                           const  DSC_XYInputSpec&  inputSpec,
                           DC_XYData&               xyData)
{
    try
        {
            I_XYDataFile cmFile(inputFileName);
            cmFile.CFsetMsgStuff("ReadXY", "xyFile", 100);
            cmFile.Init();
            switch (inputSpec.xyFileFormat)
                {
                case DSC_XYInputSpec::xyffAppDev : {
                    cmFile.ReadTableXY(inputSpec.readColumnID, 1, 2, xyData);
                    break;
                }
                case DSC_XYInputSpec::xyffTable : {
                    cmFile.ReadTableXY(inputSpec.readColumnID, inputSpec.tableXcol, inputSpec.tableYcol, xyData);
                    break;
                }
                case DSC_XYInputSpec::xyffDate : {
                    cmFile.ReadDateXY(inputSpec.dateSpec, xyData);
                    break;
                }
                default: {
                    cmFile.CFtextError("other is unsupported");
                    break;
                }

                }
        }
    catch (ConvFileC::CFReadError re) {
        xyData.DeAlloc();
        GenAppErrorMsg("ReadXYData", re.errMsg);
        return false;
    }

    {
        using namespace std;
        ostringstream ostr;
        ostr <<  "XY read OK: " << xyData.Size() << " rows " << ends;
        GenAppInfoMsg("ReadXY", ostr.str().c_str());
    }
    return true;

}

void IO_XYData::WriteXYFile(const char*      outputFileName,
                            bool             writeColIDs,
                            const DC_XYData& xyData)
{
    try
        {
            O_XYDataFile cmFile(outputFileName);
            cmFile.Init();
            cmFile.WriteAppDevXY(writeColIDs, xyData);
        }
    catch (TextC::TextError re) {
        GenAppErrorMsg("WriteXYData", re.errMsg);
        return;
    }

    GenAppInfoMsg("WriteXYData", "file written successfully");
    return;
}

