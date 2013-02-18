///////////////////////////////////////////////////////////////////////////////////
//
// C_TxtFile.cpp
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
// DESCRIPTION:  Platform independent and application independent base classes and
//              support classes.
//              Derived from C_Text for file I/O
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <iostream>

#include <genClass/C_TxtFile.h>

//  ----------------------------------------- constructor/destructor

TxtFileC::TxtFileC()
{
    txtFile         = 0;
    openWasOK       = false;
}

TxtFileC::TxtFileC(const char* fName, bool openForRead)
{
    txtFile         = 0;
    openWasOK       = false;
    OpenFile(fName, openForRead);
}
TxtFileC::~TxtFileC()
{
    if (txtFile != 0)
        {
            txtFile->close();
            delete txtFile;
        }
}

void TxtFileC::OpenFile(const char* fName, bool openForRead)
{
    if (txtFile != 0)
        GenAppInternalError("TxtFileC::OpenFile - file already open");

    using namespace std;

    txtFile = new fstream();
    if (txtFile == 0)
        TextError("error creating file");


    if (openForRead)
        {
            txtFile->open(fName, ios::in);
            char fileName[stdFileStrLen];
            ExtractFileName(fileName, fName, stdFileStrLen);
            char tempStr[80];
            MakeString(tempStr, " error opening ", fileName, " (bad path or file name ?)", 80);

            CheckTextError(tempStr);
            //CheckTextError("error opening text file (bad path or file name ?)");
        }
    else
        {
            txtFile->open(fName, ios::in | ios::out | ios::trunc);
            CheckTextError("error opening text file (readonly? bad path/file name?)");
        }

    openWasOK = true;
}

void TxtFileC::CloseFile()
{
    if (openWasOK)
        if (txtFile == 0)
            GenAppInternalWarning("TxtFileC::CloseFile - file NULL");
        else
            txtFile->close();

    delete txtFile;
    openWasOK  = false;
    txtFile = 0;
}

void TxtFileC::Close()
{
    CloseFile();
}

//  ----------------------------------------- error check

void TxtFileC::CheckTextError(const char* errMsg)
{
    if (txtFile->fail())
        {
            //              std::cout << "rdstate: " << txtFile->rdstate() << std::endl;
            ThrowTextError(errMsg);
        }
}

//  ----------------------------------------- TextC virtuals

bool TxtFileC::IsEOF()
{
    return txtFile->eof() != 0;
}

void TxtFileC::WriteLine()
{
    using namespace std;

    *txtFile << endl;
    newLineStart = true;
}

static const char nullText[] = "NULL";

//  ensures no blank lines to confuse ReadNextLine

void TxtFileC::WriteText(const char* fText)
{
    int nChar = StringLength(fText);
    bool blank = true;
    for (int i = 0; i < nChar; i++)
        if (fText[i] != ' ')
            {
                blank = false;
                break;
            }

    using namespace std;

    if (blank)
        *txtFile << nullText << endl;
    else
        *txtFile << fText << endl;
    newLineStart = true;
}


void TxtFileC::WriteSolidText(const char* fText)
{
    if ((!newLineStart) && (solidTextDelimChar != '\0'))
        *txtFile << solidTextDelimChar;
    *txtFile << fText;
    newLineStart = false;
}

//write text with blank lines
void TxtFileC::WriteTextIncBlank(const char* fText)
{
    using namespace std;

    *txtFile << fText << endl;
    newLineStart = true;
}

//write string arrays, each string is followed by an endline
void TxtFileC::WriteStringArray(const SC_StringArray fText)
{
    using namespace std;

    for (int i = 0; i < fText.Size(); i++)
        *txtFile << fText[i] << endl;
    newLineStart = true;
}



void TxtFileC::ReadText(char* fText,
                        int   maxLen)
{
    //  kluge to get around mixing >> and getline
    //  sometimes after >>, getline reads just EOL char or trailing blanks

    char    temp[8192];
    do
        {
            txtFile->getline(temp, 8192);
            TrimLeft(temp);
        }
    while ((StringLength(temp) == 0)&&(!IsEOF()));

    if (strcmp(temp, nullText) == 0)
        fText[0] = '\0';
    else
        CopyString(fText, temp, maxLen);
}


void TxtFileC::ReadSolidText(char* rText)
{
    *txtFile >> rText;
}


bool TxtFileC::ReadBool()
{
    int rBool;
    *txtFile >> rBool;
    return rBool != 0; // RK: more efficient and no warnings
}

int TxtFileC::ReadInt()
{
    int rInt;
    *txtFile >> rInt;
    return rInt;
}

double TxtFileC::ReadDouble()
{
    double rDouble;
    *txtFile >> rDouble;
    return rDouble;
}

