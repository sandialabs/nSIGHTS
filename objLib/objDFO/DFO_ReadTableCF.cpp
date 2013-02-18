///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadTableCF.cpp
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

#include <genClass/C_ConvFile.h>

#include <objDFO/DFO_ReadTable.h>

//TODO: move this to a header file!
class TableFile :  public ConvFileC {
    private:
        int             nextInc;
        int             nextCount;

    public:
        DC_TableData&   tableData;
        DFO_ReadTable&  inDFO;

    public:
                        TableFile(DFO_ReadTable& inObj);
                        ~TableFile(){};

        void            Init();
        void            ReadBasicTable(bool readColIDs, bool readRowIDs);
        void            ReadCSVTable(bool readColIDs, bool readRowIDs);
        void            ReadTecplotTable();
        void            ReadMultRows(bool readColIDs);


};

TableFile :: TableFile(DFO_ReadTable& inObj)
: ConvFileC(inObj.tableFname, 8 * 1024 * 1024), inDFO(inObj), tableData(inObj.tableData)
{
    if (inDFO.ignoreComments)
        CFsetComment(inDFO.commentChars);
}

void TableFile :: Init()
{
    nextInc = 5;
    nextCount = 50;
    tableData.Alloc(0, 0);
}

void  TableFile::ReadBasicTable(bool readColIDs, bool readRowIDs)
{
    // count cols
    CFreadNonBlank();
    int colCount = 0;
    char tempStr[80];
    int idLen = 10;

    if (inDFO.averageMultipleColumns)
    {
        using namespace std;

        istringstream muStr(lastLineRead);
        while (!muStr.fail())
        {
            muStr >> tempStr;
            colCount++;
        }
    }
    else
    {
        while (CFgetNextStr(tempStr, 80))
        {
            colCount++;
            idLen = IMax(StringLength(tempStr) ,idLen);
        }
    }

    if (readRowIDs)
        colCount--;
    if (colCount < 1)
        CFtextError("No data in header");

    // count rows
    int rowCount = 1;                   // already read first line
    while(true)
    {
        if (CFeof())
            break;
        if (CFreadText())
        {
            rowCount++;
            if (readRowIDs)
            {
                CFgetNextStr(tempStr, 80);
                idLen = IMax(StringLength(tempStr) ,idLen);
            }
        }
    }

    if (readColIDs)
        rowCount--;
    if (rowCount < 1)
        CFtextError("No data in file");

    int totalColCount = colCount;
    if (inDFO.averageMultipleColumns)
    {
        colCount--; // lose time col
        colCount /= inDFO.multipleColumnCount;
        colCount++; // add back in
        CFsetMsgStuff("ReadTable", "Multiple Col TableFile", 1);

    }

    if (!tableData.Alloc(colCount, rowCount, idLen + 2))
        CFtextError("could not alloc table");

    CFreset();

    if (readColIDs)
    {
        CFreadNonBlank();

        // first col header is for row IDs -- skip
        if (readRowIDs)
            CFgetNextStr(tempStr, 80);

        if (inDFO.averageMultipleColumns)
        {
            CFgetNextStr(tempStr, 80);
            tableData.columnDesc += tempStr;
            for (int i = 1; i < colCount; i++)
            {
                IntToString(i, tempStr, 5, 80);
                tableData.columnDesc += tempStr;
            }
        }
        else
        {
            //  add all col headers
            while (CFgetNextStr(tempStr, 80))
                tableData.columnDesc += tempStr;
        }
    }
    else
    {
        for (int i = 0; i < colCount; i++)
        {
            IntToString(i, tempStr, 6, 80);
            tableData.columnDesc += tempStr;
        }
    }

    // read rows
    while(true)
    {
        if (CFeof() || tableData.rowDesc.IsFull())  // in case table is being written
            break;

        if (CFreadText())
        {
            // get row ID
            if (readRowIDs)
                CFgetNextStr(tempStr, 80);
            else
                IntToString(tableData.rowDesc.Size(), tempStr, 6, 80);
            tableData.rowDesc += tempStr;

            // get colCount data
            double tVal;

            using namespace std;

            // used by averageMultipleColumns only
            istringstream muStr(lastLineRead);
            int ntotalRead = 1;
            for (int i = 0; i < colCount; i++)
            {
                if (inDFO.averageMultipleColumns)
                {
                    if (i == 0)
                    {
                        muStr >> tVal;
                        if (muStr.fail())
                            CFtextError("error reading multiple col 1");
                    }
                    else
                    {
                        tVal = 0.0;
                        int avgCount = 0;
                        for (int j = 0; j < inDFO.multipleColumnCount; j++)
                        {
                            double colVal;
                            muStr >> colVal;


                            if (muStr.fail())
                                CFtextError("error reading multiple col n");
                            tVal += colVal;
                            avgCount++;

                            ntotalRead++;
                            if (ntotalRead == totalColCount)
                                break;
                        }
                        tVal /= double(avgCount);
                    }
                }
                else
                {
                    if (!CFgetNextStr(tempStr, 80))
                        CFtextError("insufficient columns of data");
                    if (!IsValidReal(tempStr, tVal))
                        tVal = nullReal;
                    if (inDFO.setNullsToZero && RealIsNull(tVal))
                        tVal = 0.0;
                }

                tableData.dataTable[i] += tVal;
            }
        }
    }

    tableData.dataTable.SetSize(colCount);
}



void  TableFile::ReadCSVTable(bool readColIDs, bool readRowIDs)
{
    SetParseChars(",");
    ReadBasicTable(readColIDs, readRowIDs);
}


void TableFile::ReadTecplotTable()
{
    CFsetComment("#");

    // read header info
    char tempStr[80];
    char firstStr[80];

    SC_StringArray headerStrings;
    while(true)
    {
        if (CFeof())
            CFtextError("variables not found");
        if (!CFreadText())
            continue;

        CFgetNextStr(tempStr, 80);

        PrepareParseString(tempStr, "=");
        ParseGetNextString(tempStr, firstStr, 80);
        if (SameUCString(firstStr, "TITLE"))
        {
            if (!headerStrings.ExtractQuotedStrings(lastLineRead))
                CFtextError("expecting quoted title ??");
            tableData.SetID(headerStrings[0]);
        }

        if (SameUCString(firstStr, "VARIABLES"))
        {
            if (!headerStrings.ExtractQuotedStrings(lastLineRead))
                CFtextError("expecting quoted variables");
            break;
        }
    }


    int ncol = headerStrings.Size();
    if (!tableData.ResizableAlloc(ncol, 1000, headerStrings.GetMaxStringLen()))
        CFtextError("allocation error");

    tableData.dataTable.SetSize(ncol);
    tableData.columnDesc = headerStrings;

    // read data
    int rowSize;
    while(true)
    {
        if (CFeof())
            break;
        if (CFreadText())
        {
            rowSize = tableData.rowDesc.Size();
            IntToString(rowSize, tempStr, 6, 80);
            tableData.rowDesc += tempStr;

            // get colCount data
            double tVal;
            for (int i = 0; i < ncol; i++)
            {
                if (!CFgetNextStr(tempStr, 80))
                    CFtextError("insufficient columns of data");

                if (SameUCString(tempStr, "ZONE") ||
                    SameUCString(tempStr, "TEXT") ||
                    SameUCString(tempStr, "GEOMETRY") ||
                    SameUCString(tempStr, "CUSTOMLABELS") ||
                    SameUCString(tempStr, "DATASETAUXDATA") ||
                    SameUCString(tempStr, "VARAUXDATA"))
                {
                    tableData.rowDesc.SetSize(rowSize);   //reset row indx size
                    break;  //read next line
                }

                if (!IsValidReal(tempStr, tVal))
                    tVal = nullReal;
                tableData.dataTable[i] += tVal;
            }
        }
    }

}

void TableFile::ReadMultRows(bool readColIDs)
{
    // count cols
    CFreadNonBlank();
    int colCount = CFgetNextInt();

    if (colCount < 1)
        CFtextError("No data in header");

    if (!tableData.ResizableAlloc(colCount, 128, 40))
        CFtextError("could not alloc table");


    char tempStr[80];
    if (readColIDs)
    {
        CFreadNonBlank();
        // first col header is for row IDs -- skip
        for (int i = 0; i < colCount; i++)
        {
            if (!CFgetNextStr(tempStr, 80))
            {
                CFreadNonBlank();
                if (!CFgetNextStr(tempStr, 80))
                    CFtextError("could not read col IDs");
            }
            //  add all col headers
            tableData.columnDesc += tempStr;
        }
    }
    else
    {
        for (int i = 0; i < colCount; i++)
        {
            IntToString(i, tempStr, 6, 80);
            tableData.columnDesc += tempStr;
        }
    }

    // read rows
    int nRow = 0;
    while(true)
    {
        if (CFeof())
            break;
        if (CFreadText())
        {
            // get row ID
            IntToString(tableData.rowDesc.Size(), tempStr, 6, 80);
            tableData.rowDesc += tempStr;
            nRow ++;

            // get colCount data
            double tVal;
            for (int i = 0; i < colCount; i++)
            {
                tVal = CFstreamDouble(false);
                if (inDFO.setNullsToZero && RealIsNull(tVal))
                    tVal = 0.0;
                tableData.dataTable[i] += tVal;
            }
        }
    }

    tableData.SetSize(colCount, nRow);
}


bool DFO_ReadTable::ReadTable()
{
    TableFile cmFile(*this);
    cmFile.CFsetMsgStuff("ReadTable", "TableFile", 500);
    try
    {
        cmFile.Init();
        switch (tableFileFormat)
        {
            case tffBasic : {
                cmFile.ReadBasicTable(readColumnIDs, readRowIDs);
                break;
            }
            case tffCSV : {
                cmFile.ReadCSVTable(readColumnIDs, readRowIDs);
                break;
            }
            case tffTecplot : {
                cmFile.ReadTecplotTable();
                break;
            }
            case tffMultRows : {
                cmFile.ReadMultRows(readColumnIDs);
                break;
            }
            default: {
                cmFile.CFtextError("other is unsupported");
                break;
            }

        }
    }
    catch (ConvFileC::CFReadError re) {
        SetObjErrMsg(re.errMsg);
        return false;
    }

    using namespace std;

    ostringstream ostr;
    ostr <<  "Table read OK: " << tableData.columnDesc.Size() << " cols " << tableData.rowDesc.Size() << " rows "  << ends;
    GenAppInfoMsg("ReadTable", ostr.str().c_str());

    return true;
}

