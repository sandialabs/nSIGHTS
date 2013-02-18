///////////////////////////////////////////////////////////////////////////////////
//
// C_Text.cpp
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
// DESCRIPTION: Platform independent and application independent base claseess and
//              support classes.
//              Base class for reading/writing text formatted data (file or clipboard)
//
///////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <iostream>
#include <ios>

#include <genClass/C_Common.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genClass/C_Text.h>

//  ----------------------------------------- constructor/destructor

TextC::TextC()
{
    solidTextDelimChar = ' ';
    newLineStart = true;
}

TextC::~TextC()
{
}

//  ----------------------------------------- error check

TextC::TextError::TextError(const char* eMsg)
{
    CopyString(errMsg, eMsg, textFileErrLen);
}

void TextC::ThrowTextError(const char* errMsg)
{
    throw TextError(errMsg);
}

//  ----------------------------------------- misc writing routines

void  TextC::WriteTextWithTrailingBlanks(const char* fText)
{
    char tempStr[512];
    CopyString(tempStr, fText, 512);
    int len = StringLength(tempStr);
    for (int i = len - 1; i >= 0; i--)
        {
            if (tempStr[i] != ' ')
                break;
            tempStr[i] = '_';
        }
    WriteText(tempStr);
}

void TextC::WriteQuotedText(const char* fText)
{
    char tempStr[256];
    MakeString(tempStr, "\"", fText, "\"", 256);
    WriteText(tempStr);
}

void TextC::WriteQuotedSolidText(const char* fText)
{
    char tempStr[256];
    MakeString(tempStr, "\"", fText, "\"", 256);
    WriteSolidText(tempStr);
}


void TextC::WriteBool(bool fBool)
{
    if (fBool)
        WriteSolidText("1");
    else
        WriteSolidText("0");
}

void TextC::WriteInt(int fInt)
{
    char tempStr[80];
    IntToString(fInt, tempStr, 80);
    WriteSolidText(tempStr);
}

void  TextC::WritePointer(const void* fPtr)
{
    using namespace std;
    ostringstream temp;
    temp << fPtr << ends;
    WriteSolidText(temp.str().c_str());
}

void TextC::WriteDouble(const double& fDouble)
{
    char temp[40];

    // necessary kluge -- otherwise , is delimiter on stream read
    bool wasEuropean = SC_RealFormat::european;
    SC_RealFormat::european = false;
    doubleConvFormat.RealToString(fDouble, temp, 40);
    SC_RealFormat::european = wasEuropean;
    WriteSolidText(temp);
}

void TextC::WriteNullDouble(const double& fDouble)
{
    if (RealIsNull(fDouble))
        {
            WriteSolidText("null");
        }
    else
        {
            WriteDouble(fDouble);
        }
}


void TextC::WriteIntArray(const SC_IntArray& intData,
                          bool               writeSize,
                          int                lineModulus)
{
    if (writeSize)
        WriteInt(intData.Size());

    for (int i = 0; i < intData.Size(); i++)
        {
            WriteInt(intData[i]);
            if ((lineModulus > 0) && (((i + 1) % lineModulus) == 0))
                WriteLine();
        }
    if ((lineModulus > 0) && ((intData.Size() % lineModulus) != 0))
        WriteLine();
}

void TextC::WriteMappedIntArray(const SC_IntArray& intData,
                                const SC_IntArray& validMap,
                                int                lineModulus)
{
    for (int i = 0; i < validMap.Size(); i++)
        {
            WriteInt(intData[validMap[i]]);
            if ((lineModulus > 0) && (((i + 1) % lineModulus) == 0))
                WriteLine();
        }
    if ((lineModulus > 0) && ((validMap.Size() % lineModulus) != 0))
        WriteLine();
}

void TextC::WriteDoubleArray(const SC_DoubleArray& dblData,
                             bool                  writeSize,
                             int                   lineModulus)
{
    if (writeSize)
        WriteInt(dblData.Size());
    for (int i = 0; i < dblData.Size(); i++)
        {
            WriteDouble(dblData[i]);
            if ((lineModulus > 0) && (((i + 1) % lineModulus) == 0))
                WriteLine();
        }
    if ((lineModulus > 0) && ((dblData.Size() % lineModulus) != 0))
        WriteLine();
}

void TextC::WriteMappedDoubleArray(const SC_DoubleArray& dblData,
                                   const SC_IntArray&    validMap,
                                   int                   lineModulus)
{
    for (int i = 0; i < validMap.Size(); i++)
        {
            WriteDouble(dblData[validMap[i]]);
            if ((lineModulus > 0) && (((i + 1) % lineModulus) == 0))
                WriteLine();
        }
    if ((lineModulus > 0) && ((validMap.Size() % lineModulus) != 0))
        WriteLine();
}


void TextC::ReadTextWithTrailingBlanks(char* fText,
                                       int   maxLen)
{
    ReadText(fText, maxLen);
    int len = StringLength(fText);
    for (int i = len - 1; i >= 0; i--)
        {
            if (fText[i] != '_')
                break;
            fText[i] = ' ';
        }
}


void TextC::ReadFileName(char* fName)
{
    ReadText(fName, stdFileStrLen);
}

bool TextC::ReadBool()
{
    int rBool = ReadInt();
    return rBool != 0;
}

int TextC::ReadInt()
{
    char tStr[128];
    ReadSolidText(tStr);

    int rInt;
    if (!IsValidInt(tStr, rInt))
        ThrowTextError("invalid integer");

    return rInt;
}

void*  TextC::ReadPointer()
{
    char tStr[128];
    ReadSolidText(tStr);
          std::cout << "Pointer: " << tStr << std::endl;

    std::istringstream istr(tStr);
    void* tmpP;
    istr >> tmpP;
    if (istr.fail())
        {
            // kluge to deal with 64 bit pointers on 32 bit - FB395
            if (strlen(tStr) > 8)
                {
                    istr.clear();
                    istr.seekg(8);
                    istr >> tmpP;
                }

            if (istr.fail())
                {
                    // backward compatibility - try again as integer
                    istr.clear();
                    istr.seekg(0);
                    int tmpI;
                    istr >> tmpI;
                    if (istr.fail())
                        {
                                  std::cout << "Pointer bad" << tStr << std::endl;

                            ThrowTextError("invalid pointer");
                        }
                    tmpP = reinterpret_cast<void*>(tmpI);
                }
        }

    return tmpP;
}


double TextC::ReadDouble()
{
    char tStr[128];
    ReadSolidText(tStr);

    double rDouble;
    if (!IsValidReal(tStr, rDouble))
        ThrowTextError("invalid real");

    return rDouble;
}

double TextC::ReadNullDouble()
{
    // klugy approach
    char tStr[128];
    ReadSolidText(tStr);
    if (tStr[0] == 'n')
        return nullReal;

    double rDouble;
    if (!IsValidReal(tStr, rDouble))
        ThrowTextError("invalid null real");
    return rDouble;
}

void TextC::ReadIntArray(SC_IntArray& intData,
                         bool         readSize)
{
    int nread = intData.Size();
    if (readSize)
        {
            nread = ReadInt();
            intData.AllocAndSetSize(nread);
        }

    for (int i = 0; i < nread; i++)
        intData[i] = ReadInt();
}

void TextC::ReadMappedIntArray(SC_IntArray& intData,
                               const SC_IntArray& validMap)
{
    for (int i = 0; i < validMap.Size(); i++)
        intData[validMap[i]] = ReadInt();
}

void TextC::ReadDoubleArray(SC_DoubleArray& dblData,
                            bool            readSize)
{
    int nread = dblData.Size();
    if (readSize)
        {
            nread = ReadInt();
            dblData.AllocAndSetSize(nread);
        }

    for (int i = 0; i < nread; i++)
        dblData[i] = ReadDouble();
}

void TextC::ReadMappedDoubleArray(SC_DoubleArray&       dblData,
                                  const SC_IntArray&    validMap)
{
    for (int i = 0; i < validMap.Size(); i++)
        dblData[validMap[i]] = ReadDouble();
}



void TextC:: WriteFileHeader(const char* headerText, int majVer, int minVer)
{
    WriteText(headerText);
    WriteInt(majVer);
    WriteInt(minVer);
    WriteLine();
}
void TextC::ReadFileHeader(const char* headerToMatch, int& majVer, int& minVer, const char* fileDesc)
{
    char tempHeader[stdFileStrLen];
    static char errMsg[80];

    ReadFileName(tempHeader);
    if (strcmp(tempHeader, headerToMatch) != 0)
        {
            CopyString(errMsg, "File is not an ", 80);
            ConcatString(errMsg, fileDesc, 80);
            ThrowTextError(errMsg);
        }
    majVer = ReadInt();
    minVer = ReadInt();
    CheckTextError("Reading file header");
}

