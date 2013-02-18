///////////////////////////////////////////////////////////////////////////////////
//
// LSC_Listing.cpp
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
//      Platform independent base classes to allow creation of listings in HTML browser windows.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>

#include <genClass/C_FuncObj.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genListClass/LSC_Listing.h>

LSC_Listing :: LSC_Listing()
{
    InitCommon();
}


LSC_Listing :: LSC_Listing(const LSC_Listing& a)
{
    InitCommon();
    LocalCopy(a);
}

LSC_Listing::~LSC_Listing()
{
}

void LSC_Listing::InitCommon()
{
    listingData.SetStringLen(listingLineLen);
    listingData.SetResizable(64);
    SetToNull(currStr);
    lastLineBlank = true;

    doingPreFormat = false;
    suppressHTML = false;

    enumStrs.SetResizable(5);
    enumMap.SetResizable(5);

    ResetStdLeftMargin();

    stdRightPos  = 55;
    stdDataLen   = 15;
    wideRightPos = 75;
    unitLeftPos  = 60;
}


LSC_Listing& LSC_Listing::operator= (const LSC_Listing& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}


void  LSC_Listing::LocalCopy(const LSC_Listing& a)
{
}


void LSC_Listing::InitListingData()
{
    doingPreFormat = false;
    lastLineBlank = true;
    ResetCurrStr();
    listingData.SetEmpty();
}


void LSC_Listing::CloseListingData()
{
    EndPreFormatting();

//  listingData += "<A HREF=\"http://www.infoworld.com\">Test URL</A>";
}

void LSC_Listing::AddHTML(const char* inStr)
{
    if (!suppressHTML)
        EndPreFormatting();
    listingData += inStr;
}


void LSC_Listing::AddHTML(const char* h1, const char* h2, const char* h3)
{
    char tempStr[listingLineLen];
    if (!suppressHTML)
        MakeString(tempStr, h1, h2, h3, listingLineLen);
    else
        CopyString(tempStr, h2, listingLineLen);
    listingData += tempStr;
}

void LSC_Listing::AddTitleHTML(const char* h1, const char* h2, const char* h3)
{
    EndPreFormatting();
    AddHTML(h1, h2, h3);
}


void LSC_Listing::StartPreFormatting()
{
    if (!doingPreFormat && (!suppressHTML))
        listingData += "<pre>";
    doingPreFormat = true;
}

void LSC_Listing::EndPreFormatting()
{
    if (doingPreFormat && (!suppressHTML))
        listingData += "</pre>";
    doingPreFormat = false;
}

void LSC_Listing::ResetCurrStr()
{
    currStr[0] = '\0';
    LeftJustify(currStr, listingLineLen - 2);
}

void LSC_Listing::AddBlankLine()
{
    currStr[0] = '\0';
    AddNextLine();
}


void  LSC_Listing::AddConditionalBlankLine()
{
    if (!lastLineBlank)
        AddBlankLine();
}

void LSC_Listing::AddNextLine()
{
    TrimLeft(currStr);
    lastLineBlank = (currStr[0] == '\0');

    StartPreFormatting();

    if (!suppressHTML)
        ConcatString(currStr, "\n", listingLineLen);
    listingData += currStr;

    ResetCurrStr();
}

void LSC_Listing::AddText(const char inStr[],
                             int  atPos)
{
    if (atPos < 0)
        atPos = 0;
    int len = StringLength(inStr);
    for (int i = 0; i < len; i++)
   {
        if (currStr[atPos] == '\0')
            break;
        currStr[atPos++] = inStr[i];
    }
}

void LSC_Listing::AddText(const char inStr[],
                            int  atPos,
                            int  fieldLen,
                            bool leftJustify)
{
    char tempStr[listingLineLen];
    CopyString(tempStr, inStr, listingLineLen);
    if (leftJustify)
        LeftJustify(tempStr, fieldLen);
    else
        RightJustify(tempStr, fieldLen);
    AddText(tempStr, atPos);
}



void LSC_Listing::BigTitle(const char title[])
{
    AddTitleHTML("<H3>", title, "</H3>");
}

void LSC_Listing::StdTitle(const char title[])
{
    AddTitleHTML("<H4>", title, "</H4>");
}


void LSC_Listing::SubTitle(const char title[])
{
    AddTitleHTML("<H5>", title, "</H5>");
}


void LSC_Listing::AddTextTitle(const char title[],
                                   char underChar,
                                       char overChar,
                                       bool blankAfter)
{
    int nchar = StringLength(title);
    char tempStr[listingLineLen];
    if (overChar != ' ')
    {
        FillWithChar(tempStr, overChar, nchar);
        AddLine(tempStr);
    }
    AddLine(title);
    FillWithChar(tempStr, underChar, nchar);
    AddLine(tempStr);
    if (blankAfter)
        AddBlankLine();
}

void LSC_Listing::AddTextTitle(const char title[],
                                     char underChar,
                                    bool blankAfter)
{
    AddTextTitle(title, underChar, ' ', blankAfter);
}


void LSC_Listing::TextBigTitle(const char title[])
{
    AddTextTitle(title, '*', '*', true);
}

void LSC_Listing::TextStdTitle(const char title[])
{
    AddTextTitle(title, '=', true);
}

void LSC_Listing::TextSubTitle(const char title[])
{
    AddTextTitle(title, '-', false);
}


void LSC_Listing::AddLine(const char inStr[],
                              int  atPos)
{
    AddText(inStr, atPos);
    AddNextLine();
}

void LSC_Listing::AddConditionalBlank()
{
    if (!lastLineBlank)
        AddBlankLine();
}

void LSC_Listing::SetStdLeftMargin(int newMargin)
{
    stdLeftMargin = newMargin;
}

void LSC_Listing::ResetStdLeftMargin()
{
    stdLeftMargin = 0;
}

void LSC_Listing::AddStdText(const char* dataDesc,
                          const char* dataStr)
{
    AddText(dataDesc, stdLeftMargin);
    int dataLen = StringLength(dataStr);
    AddText(dataStr, stdRightPos - dataLen, dataLen, false);
    AddNextLine();
}

void LSC_Listing::AddWideText(const char* dataDesc,
                           const char* dataStr)
{
    AddText(dataDesc, stdLeftMargin);
    int dataLen = StringLength(dataStr);
    AddText(dataStr, wideRightPos - dataLen, dataLen, false);
    AddNextLine();
}


void LSC_Listing::AddBoolText(const char* dataDesc,
                          bool  flag,
                        const char* trueStr,
                        const char* falseStr)
{
    if (flag)
        AddStdText(dataDesc, trueStr);
    else
        AddStdText(dataDesc, falseStr);
}

void LSC_Listing::AddYesNo(const char* dataDesc,
                          bool  flag)
{
    AddBoolText(dataDesc, flag, "yes", "no");
}

void LSC_Listing::AddStatus(const char* dataDesc,
                          bool  okFlag)
{
    AddBoolText(dataDesc, okFlag, "OK", "BAD");
}


void LSC_Listing::AddWideBoolText(const char* dataDesc,
                                              bool  flag,
                                        const char* trueStr,
                                        const char* falseStr)
{
    if (flag)
        AddWideText(dataDesc, trueStr);
    else
        AddWideText(dataDesc, falseStr);
}

void LSC_Listing::AddStdInt(const char* dataDesc,
                            int   dataVal)
{
    char tempStr[listingLineLen];
    IntToString(dataVal, tempStr, listingLineLen);
    AddStdText(dataDesc, tempStr);
}



void LSC_Listing::AddStdReal(const char*   dataDesc,
                          const double& dataVal,
                          const SC_RealFormat& dataFormat)
{
    if (RealIsNull(dataVal))
        AddStdText(dataDesc, nullStr);
    else
    {
        char tempStr[listingLineLen];
        dataFormat.RealToString(dataVal, tempStr, listingLineLen);
        AddStdText(dataDesc, tempStr);
    }
}

void LSC_Listing::AddStdReal(const char*   dataDesc,
                          const double& dataVal)
{
    AddStdReal(dataDesc, dataVal, defaultRealFormat);
}

void LSC_Listing::AddRealText(const double& dataVal,
                          const SC_RealFormat& dataFormat,
                          int  atPos)
{
    if (RealIsNull(dataVal))
        AddText(nullStr, atPos);
    else
    {
        char tempStr[listingLineLen];
        dataFormat.RealToString(dataVal, tempStr, listingLineLen);
        AddText(tempStr, atPos);
    }
    AddNextLine();
}

void LSC_Listing::AddRealText(const double& dataVal,
                          int  atPos)
{
    AddRealText(dataVal, defaultRealFormat, atPos);
}

void LSC_Listing::AddUnitText(const char* dataDesc,
                          const char* dataStr,
                          const char* unitStr)
{
    AddText(dataDesc, stdLeftMargin);
    AddText(dataStr, stdRightPos - stdDataLen, stdDataLen, false);
    AddText(unitStr, unitLeftPos, stdDataLen, true);
    AddNextLine();
}


void LSC_Listing::AddUnitConst(const char* dataDesc,
                           const double& dataVal,
                           const char* unitStr)
{
    char dataStr[listingLineLen];
    SC_SciFormat sciConv(6);
    sciConv.RealToString(dataVal, dataStr, listingLineLen);
    AddUnitText(dataDesc, dataStr, unitStr);
}


void  LSC_Listing::AddFileName(const char* dataDesc,
                            const char* fileStr)
{
    AddText(dataDesc, 0);
    AddNextLine();
    int fileLen = StringLength(fileStr);
    AddText(fileStr, stdRightPos - fileLen, fileLen, false);
    AddNextLine();
}
void LSC_Listing::ClearEnumStrs()
{
    enumStrs.SetEmpty();
    enumMap.SetEmpty();
}

void LSC_Listing::FirstEnumStr(const char* enumStr, int enumVal)
{
    ClearEnumStrs();
    AddEnumStr(enumStr, enumVal);
}

void LSC_Listing::AddEnumStr(const char* enumStr, int enumVal)
{
    enumStrs += enumStr;
    enumMap += enumVal;
}

void LSC_Listing::AddEnumStr(const char* enumStr)
{
    enumMap += enumStrs.Size();
    enumStrs += enumStr;
}

void LSC_Listing::AddStdEnum(const char* dataDesc, int enumVal)
{
    for (int i = 0; i < enumStrs.Size(); i++)
        if (enumVal == enumMap[i])
            AddStdText(dataDesc, enumStrs[i]);

}


void  LSC_Listing::AddFuncObjRef(const char*        dataDesc,
                              const FuncObjRef& inRef)
{
    if (inRef.objRef == 0)
        AddStdText(dataDesc, "ref not set");
    else
        AddStdText(dataDesc, inRef.objRef->GetID());
}

void LSC_Listing::TableSetup(const SC_IntArray&    colWidth,
                          const SC_BoolArray&   leftJust,
                                int             tabInc,
                                int             startCol)
{
    tableColWidth = colWidth;
    tableLeftJustify = leftJust;
    tableColPos.Alloc(tableColWidth.Size());

    tableColPos += startCol;
    for (int i = 0; i < tableColWidth.UpperBound(); i++)
        tableColPos +=  tableColPos[i] + tableColWidth[i] + tabInc;
}

void LSC_Listing::TableSetup(const int* cWidth,
                             const bool* lJust,
                                   int    tabInc,
                                   int startCol)
{
    int ncol = 0;
    while (cWidth[ncol] > 0)
        ncol++;

    SC_IntArray colWidth(ncol);
    SC_BoolArray leftJust(ncol);
    for (int i = 0; i < ncol; i++)
    {
        colWidth[i] = cWidth[i];
        leftJust[i] = lJust[i];
    }
    TableSetup(colWidth, leftJust, tabInc, startCol);

}

void LSC_Listing::TableSetup(int cWidth, int tabInc, int startCol, int nCol)
{
    SC_IntArray colWidth(nCol);
    SC_BoolArray leftJust(nCol);
    for (int i = 0; i < nCol; i++)
    {
        colWidth[i] = cWidth;
        leftJust[i] = false;
    }

    TableSetup(colWidth, leftJust, tabInc, startCol);
}

void LSC_Listing::TableSetup(const SC_IntArray&    colWidth,
                                int             tabInc,
                                int             startCol)
{
    SC_BoolArray leftJust(colWidth.Size(), false);
    leftJust[0] = true;
    TableSetup(colWidth, leftJust, tabInc, startCol);
}


void LSC_Listing::SetTableCol(    int    colNum,
                            const char*  colText)
{
    AddText(colText, tableColPos[colNum], tableColWidth[colNum], tableLeftJustify[colNum]);
}

void LSC_Listing::SetTableCol(const char*  colText[])
{
    for (int i = 0; i < tableColWidth.Size(); i++)
    {
        if (colText[i] == NULL)
            return;
        SetTableCol(i, colText[i]);
    }
}

void LSC_Listing::SetTableCol(const char*  col0Text)
{
    SetTableCol(0, col0Text);
    AddNextLine();
}

void LSC_Listing::SetTableCol(const char*  col0Text,
                           const char*  col1Text)
{
    SetTableCol(0, col0Text);
    SetTableCol(1, col1Text);
    AddNextLine();
}

void LSC_Listing::SetTableCol(const char*  col0Text,
                        const char*  col1Text,
                        const char*  col2Text)
{
    SetTableCol(0, col0Text);
    SetTableCol(1, col1Text);
    SetTableCol(2, col2Text);
    AddNextLine();
}

void LSC_Listing::SetTableCol(const char*  col0Text,
                        const char*  col1Text,
                        const char*  col2Text,
                        const char*  col3Text)
{
    SetTableCol(0, col0Text);
    SetTableCol(1, col1Text);
    SetTableCol(2, col2Text);
    SetTableCol(3, col3Text);
    AddNextLine();
}

void LSC_Listing::SetTableCol(const char*  col0Text,
                        const char*  col1Text,
                        const char*  col2Text,
                        const char*  col3Text,
                        const char*  col4Text)
{
    SetTableCol(0, col0Text);
    SetTableCol(1, col1Text);
    SetTableCol(2, col2Text);
    SetTableCol(3, col3Text);
    SetTableCol(4, col4Text);
    AddNextLine();
}

void LSC_Listing::SetTableCol(const char*  col0Text,
                        const char*  col1Text,
                        const char*  col2Text,
                        const char*  col3Text,
                        const char*  col4Text,
                        const char*  col5Text)
{
    SetTableCol(0, col0Text);
    SetTableCol(1, col1Text);
    SetTableCol(2, col2Text);
    SetTableCol(3, col3Text);
    SetTableCol(4, col4Text);
    SetTableCol(5, col5Text);
    AddNextLine();
}

void LSC_Listing::SetTableCol(const char*  col0Text,
                        const char*  col1Text,
                        const char*  col2Text,
                        const char*  col3Text,
                        const char*  col4Text,
                        const char*  col5Text,
                        const char*  col6Text)
{
    SetTableCol(0, col0Text);
    SetTableCol(1, col1Text);
    SetTableCol(2, col2Text);
    SetTableCol(3, col3Text);
    SetTableCol(4, col4Text);
    SetTableCol(5, col5Text);
    SetTableCol(6, col6Text);
    AddNextLine();
}


void LSC_Listing::SetTableReal(   int            colNum,
                            const double&        dataVal,
                            const SC_RealFormat& dataFormat)
{
    char realStr[40];
    if (RealIsNull(dataVal))
        CopyString(realStr, "---", 40);
    else
        dataFormat.RealToString(dataVal, realStr, 40);
    SetTableCol(colNum, realStr);
}

void LSC_Listing::SetTableReal(   int            colNum,
                            const double&        dataVal)
{
    SetTableReal(colNum, dataVal, defaultRealFormat);
}

void LSC_Listing::SetTableInt(int   colNum,
                           int dataVal)
{
    char intStr[40];
    IntToString(dataVal, intStr, 40);
    SetTableCol(colNum, intStr);
}

void LSC_Listing::SetTableInt(int   colNum,
                           int dataVal,
                           int padLen)
{
    char intStr[40];
    IntToString(dataVal, intStr, padLen, 40);
    SetTableCol(colNum, intStr);
}


void  LSC_Listing::SetTableBool(       int      colNum,
                                   bool     dataVal,
                             const char*    trueText,
                            const char* falseText)
{
    if (dataVal)
        SetTableCol(colNum, trueText);
    else
        SetTableCol(colNum, falseText);
}

