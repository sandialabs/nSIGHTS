///////////////////////////////////////////////////////////////////////////////////
//
// C_ConvFile.cpp
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
// DESCRIPTION:  Platform independent and application independent base classes
//              and support classes.
//              Special support for reading and converting ASCII files produced
//              by numeric models.
//
///////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_File.h>

#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_StringArray.h>

#include <genClass/C_ConvFile.h>


ConvFileC::CFReadError::CFReadError(const char* eMsg)
{
    CopyString(errMsg, eMsg, maxErrLen);
}

void ConvFileC::InitCommon()
{
    nlinesRead   = 0;
    inputF       = 0;
    currFname[0] = '\0';

    lastLineRead = new char[maxLineLen + 1];
    lastLineRead[0] = '\0';

    parsedLine = new char[maxLineLen + 1];
    parsedLine[0] = '\0';
    beenParsed = false;

    infoMsgSrc[0] = '\0';
    infoMsgHdr[0] = '\0';
    infoMsgMod    = 100;

    commentChars[0] = '\0';
    processComments = false;
    ncommentChars = 0;

    CFsetNormal();

    ClearParseChars();

    trapEOFseperately = false;
}

ConvFileC :: ConvFileC(const char* fName) : maxLineLen(4096)
{
    InitCommon();
    CFopenText(fName);
}

ConvFileC :: ConvFileC(const char* fName, int maxLen) : maxLineLen(maxLen)
{
    InitCommon();
    CFopenText(fName);
}

ConvFileC :: ConvFileC() : maxLineLen(4096)
{
    InitCommon();
}

ConvFileC :: ConvFileC(int maxLen) : maxLineLen(maxLen)
{
    InitCommon();
}

ConvFileC :: ~ConvFileC()
{
    if (inputF)
        {
            CFcloseText();
            delete inputF;
        }
    delete [] lastLineRead;
    delete [] parsedLine;
}

bool ConvFileC::CFfileExists(const char* fN)
{
    return FileExists(fN);
}

void ConvFileC::CFsetDirect()
{
    isDirect = true;
    directRecordNum = 0;
}
void ConvFileC::CFsetNormal()
{
    isDirect = false;
    directRecordNum = 0;
}

void ConvFileC :: CFsetMsgStuff(const char*   msgSrc,
                                const char*   msgHdr,
                                int           msgMod)
{
    CopyString(infoMsgSrc, msgSrc, msgStrLen);
    CopyString(infoMsgHdr, msgHdr, msgStrLen);
    infoMsgMod = msgMod;
}

void ConvFileC::CFsetComment(const char* inChars)
{
    if (IsNull(inChars))
        {
            commentChars[0] = '\0';
            processComments = false;
            ncommentChars = 0;
        }
    else
        {
            CopyString(commentChars, inChars, 40);
            processComments = true;
            ncommentChars = StringLength(commentChars);
        }
}
void   ConvFileC:: CFopenText(const char* fN)
{
    if (inputF)
        {
            CFcloseText();
            //        inputF->open(fN);             // MS doesn't like to open again
            delete inputF;
            inputF = 0;
        }
    //    else

    inputF = new std::ifstream(fN);

    if (! (*inputF))
        {
            char msgStr[80];
            CopyString(msgStr, fN, 80);
            ConcatString(msgStr, "does not exist", 80);
            CFtextError(msgStr);
        }

    CopyString(currFname, fN, stdFileStrLen);
    CFreset();
}

void   ConvFileC:: CFreset()
{
    using namespace std;

    nlinesRead = 0;
    lastLineRead[0] = '\0';
    parsedLine[0] = '\0';
    beenParsed = false;
    directRecordNum = 0;

    //      clear fail bit - VS .Net change
    inputF->clear();
    inputF->seekg(0,ios_base::beg );

    if (inputF->bad())
        throw CFReadError("Failure on reset");

    inputF->clear();
}
void   ConvFileC:: CFcloseText()
{
    inputF->close();
}


bool ConvFileC::CFreadText()
{
    if (inputF->eof())
        if (trapEOFseperately)
            throw CFEofError();
        else
            throw CFReadError("Attempting to read past EOF");

    if (isDirect)
        {
            long recStartPos = long(directRecordNum) * long(maxLineLen);
            inputF->seekg(recStartPos);

            inputF->read(lastLineRead, maxLineLen);

            // add terminator
            lastLineRead[maxLineLen] = '\0';

            directRecordNum++;
        }
    else
        {
            inputF->getline(lastLineRead, maxLineLen);
        }
    lastLineLen = StringLength(lastLineRead);

    // kluge for reading DOS files - strip off CR
    if ((lastLineLen) && (lastLineRead[lastLineLen - 1] < ' '))
        {
            lastLineLen--;
            lastLineRead[lastLineLen] = '\0';
        }


    // process comments
    if (processComments && (lastLineLen > 0))
        {
            for (int i = 0; i < ncommentChars; i++)
                if (lastLineRead[0] == commentChars[i])
                    {
                        lastLineRead[0] = '\0';
                        lastLineLen = 0;
                        break;
                    }
        }

    TrimLeft(lastLineRead);
    lastLineLen = StringLength(lastLineRead);

    CFresetParseStr();

    nlinesRead++;
    if (!(nlinesRead % infoMsgMod))
        {
            char intStr[40];
            IntToString(nlinesRead, intStr, 40);

            char msgStr[120];
            MakeString(msgStr, infoMsgHdr, " : Line# ", intStr, 120);

            GenAppProlificInfoMsg(infoMsgSrc, msgStr);
        }

    return (lastLineLen > 0) ;
}

bool ConvFileC::CFreadText(int minLen)
{
    bool isNonBlank = CFreadText();

    if (lastLineLen < minLen)
        {
            LeftJustify(lastLineRead, minLen);
            lastLineLen = minLen;
            CFresetParseStr();
        }
    return isNonBlank;
}


void   ConvFileC:: CFreadNonBlank()
{
    do
        {
            CFreadText();
        } while (! StringLength(lastLineRead));
}


bool ConvFileC::ReadUntilCommon(const char matchStr[],
                                bool abortEOF)
{
    if ( inputF->eof())
        if ( abortEOF)
            {
                char msgStr[80];
                MakeString(msgStr, "Not found :", matchStr, 80);
                CFtextError(msgStr);
            }
        else
            return(false);

    CFreadText();

    return true;
}

bool  ConvFileC:: CFreadUntil(const char matchStr[],
                              bool abortEOF)
{
    while (true)
        {
            if (!ReadUntilCommon(matchStr, abortEOF))
                return false;

            if (strstr(lastLineRead, matchStr))
                return true;
        }
}

bool  ConvFileC:: CFreadUntil(const char matchStr[],
                              int  stCol,
                              bool abortEOF)
{
    int endCol = stCol + StringLength(matchStr) - 1;
    char tempStr[80];
    while (true)
        {
            if (!ReadUntilCommon(matchStr, abortEOF))
                return false;

            if ( endCol <= lastLineLen)
                {
                    CFgetSubStr(tempStr, stCol, endCol);
                    if (!strcmp(matchStr, tempStr))
                        return true;
                }
        }
}

bool  ConvFileC:: CFreadUntil(const char matchStr[],
                              int  stCol,
                              int  endCol,
                              bool abortEOF)
{
    char tempStr[80];
    while (true)
        {
            if (!ReadUntilCommon(matchStr, abortEOF))
                return false;

            if (endCol <= lastLineLen)
                {
                    CFgetSubStr(tempStr, stCol, endCol);
                    FullTrim(tempStr);
                    if (! strcmp(matchStr, tempStr))
                        return(true);
                }
        }
}


int  ConvFileC::CFreadUntil(const SC_StringArray& matchStr,  // must match starting at stcol
                            int  stCol,                      // stCol is FORTRAN column number
                            bool abortEOF)                   // if false then will return false on EOF
{
    char tempStr[80];
    while (true)
        {
            if (!ReadUntilCommon("multi string", abortEOF))
                return false;

            for (int i = 0; i < matchStr.Size(); i++)
                {
                    int endCol = stCol + StringLength(matchStr[i]) - 1;

                    if (endCol <= lastLineLen)
                        {
                            CFgetSubStr(tempStr, stCol, endCol);
                            FullTrim(tempStr);
                            if (SameString(matchStr[i], tempStr))
                                return i;
                        }
                }
        }

    return -1;
}

bool  ConvFileC::CFstrIsDouble(char    inStr[],
                               double& dVal,
                               bool    throwError)
{
    if (IsValidReal(inStr, dVal))
        return true;

    if (!throwError)
        return false;

    char msgStr[80];
    MakeString(msgStr, "BadReal: ", inStr, 80);
    CFtextError(msgStr);

    // never gets here
    return false;
}


bool ConvFileC::CFstrIsInt(char inStr[],
                           int& iVal,
                           bool throwError)
{
    FullTrim(inStr);
    //      strip leading zeroes to prevent octal conversion
    bool zeroFound = false;
    unsigned int maxStrip = StringLength(inStr) - 1;
    for (unsigned int i = 0; (i < maxStrip) && (inStr[i] == '0'); i++)
        {
            inStr[i] = ' ';
            zeroFound = true;
        }

    // trim again if necessary
    if (zeroFound)
        FullTrim(inStr);


    std::istringstream iStr(inStr);
    iStr >> iVal;
    bool convOK = ! iStr.fail();
    if ((!throwError) || convOK)
        return convOK;

    char msgStr[80];
    CopyString(msgStr, "BadInt: ", 80);
    ConcatString(msgStr, inStr, 80);
    CFtextError(msgStr);

    // never gets here
    return false;
}

void ConvFileC:: CFskipLines(int nSkip)
{
    for ( int i = 0; i < nSkip; i++)
        {
            if (CFeof())
                return;
            CFreadText();
        }
}


void ConvFileC:: CFgetSubStr(char  inStr[],
                             int   stCol,
                             int   endCol)
{
    using namespace std;

    if (endCol > (lastLineLen + 1))
        {
            cout << "Line: stCol " << stCol << " endCol " << endCol << " maxCol " << lastLineLen << endl << lastLineRead << endl;
            CFtextError("expecting longer line");
        }

    int inPos = 0;
    for ( int i = stCol; i <= endCol; i++)
        {
            inStr[inPos] = lastLineRead[i - 1];
            inPos++;
        }
    inStr[inPos] = '\0';
}


bool ConvFileC::CFisDouble(int     stCol,
                           int     endCol,
                           double& dVal)
{
    if ( endCol > lastLineLen)
        return(false);

    char    tempStr[80];
    CFgetSubStr(tempStr, stCol, endCol);

    return CFstrIsDouble(tempStr, dVal, false);
}

bool ConvFileC::CFisInt(int     stCol,
                        int     endCol,
                        int& iVal)
{
    if ( endCol > lastLineLen)
        return(false);

    char    tempStr[80];
    CFgetSubStr(tempStr, stCol, endCol);

    return CFstrIsInt(tempStr, iVal, false);
}

double ConvFileC:: CFgetDouble(int     stCol,
                               int     endCol)
{
    char tempStr[80];
    CFgetSubStr(tempStr, stCol, endCol);

    double tmpVal;
    CFstrIsDouble(tempStr, tmpVal);

    return tmpVal;
}

int ConvFileC:: CFgetInt(int     stCol,
                         int     endCol)
{
    char tempStr[80];
    CFgetSubStr(tempStr, stCol, endCol);

    int tmpVal;
    CFstrIsInt(tempStr, tmpVal);

    return tmpVal;
}


//  blank delimited routines

void ConvFileC::SetParseChars(const char newParseChars[],
                              bool useJustNew)  // blank and tab are default - not used if = true

{
    if (!useJustNew)
        ClearParseChars();
    else
        SetToNull(parseChars);

    ConcatString(parseChars, newParseChars, maxParseChars);
}

void ConvFileC::ClearParseChars()
{
    parseChars[0] = ' ';
    parseChars[1] = '\t';
    parseChars[2] = '\0';
}

bool ConvFileC:: CFgetNextStr(char inStr[],
                              int     maxLen)
{
    // clear leading parse chars
    /*
      for (i = 0; i < maxLen; i++)
      {
      char nextChar = parsedLine[i];
      if ((nextChar == '\0') || (strchr(parseChars, nextChar) == NULL))
      break;
      parsedLine[i] = ' ';
      }
    */
    FullTrim(parsedLine);
    beenParsed = true;

    //  copy up to blank or NULL
    int lastCopy = maxLen - 1;
    for (int i = 0; i < maxLen; i++)
        {
            inStr[i] = parsedLine[i];

            //      last portion ?
            if (inStr[i] == '\0')
                return (i > 0);

            // clear char from input
            parsedLine[i] = ' ';

            //      parse char ?
            if (strchr(parseChars, inStr[i]) != NULL)
                {
                    lastCopy = i;
                    // clear successive parse chars
                    char* nextChar = &parsedLine[i + 1];
                    while (((*nextChar) != '\0') && (strchr(parseChars, *nextChar) != NULL))
                        {
                            *nextChar = ' ';
                            nextChar++;
                        }
                    break;
                }
        }
    //  set end of output str
    inStr[lastCopy] = '\0';

    return lastCopy > 0;
}


double ConvFileC::CFgetNextDouble()
{
    char tempStr[80];
    if (! CFgetNextStr(tempStr, 80))
        CFtextError("Expecting real");

    double tmpVal;
    CFstrIsDouble(tempStr, tmpVal);

    return tmpVal;
}

int ConvFileC:: CFgetNextInt()
{
    char tempStr[80];
    if (! CFgetNextStr(tempStr, 80))
        CFtextError("Expecting integer");

    int tmpVal;
    CFstrIsInt(tempStr, tmpVal);

    return tmpVal;
}

void  ConvFileC::CFskipNextEntry(int nskip)
{
    char tempStr[80];
    for ( int i = 0; i < nskip; i++)
        if (!CFgetNextStr(tempStr, 80))
            CFtextError("expecting to skip");
}

void ConvFileC::CFstreamText(char* txtVal, int maxLen)
{
    if (!CFgetNextStr(txtVal, maxLen))
        {
            CFreadNonBlank();
            if (!CFgetNextStr(txtVal, maxLen))
                CFtextError("expecting text");
        }
}


double  ConvFileC::CFstreamDouble(bool throwError)
{
    char tempStr[80];
    if (!CFgetNextStr(tempStr, 80))
        {
            CFreadNonBlank();
            if (!CFgetNextStr(tempStr, 80))
                CFtextError("expecting double");
        }

    double tmpVal;
    if (CFstrIsDouble(tempStr, tmpVal, throwError))
        return tmpVal;

    // only gets here if false and throw
    return nullReal;
}


int     ConvFileC::CFstreamInt()
{
    char tempStr[80];
    if (!CFgetNextStr(tempStr, 80))
        {
            CFreadNonBlank();
            if (!CFgetNextStr(tempStr, 80))
                CFtextError("expecting int");
        }

    int tmpVal;
    CFstrIsInt(tempStr, tmpVal);

    return tmpVal;
}


void ConvFileC::CFreadDoubleArray(SC_DoubleArray& inVec,
                                  bool                    nullOK)
{
    char tempStr[80];
    for ( int i = 0; i < inVec.Size(); i++)
        {
            if (!CFgetNextStr(tempStr, 80))
                {
                    CFreadNonBlank();
                    if (!CFgetNextStr(tempStr, 80))
                        CFtextError("expecting real vector");
                }
            if (CFstrIsDouble(tempStr, inVec[i], false))
                continue;

            if (!nullOK)
                CFtextError("null/invalid real found");

            inVec[i] = nullReal;
        }
}

void ConvFileC::CFreadIntArray(SC_IntArray& inVec)
{
    char tempStr[80];
    for ( int i = 0; i < inVec.Size(); i++)
        {
            if (!CFgetNextStr(tempStr, 80))
                {
                    CFreadNonBlank();
                    if (!CFgetNextStr(tempStr, 80))
                        CFtextError("expecting int vector");
                }
            CFstrIsInt(tempStr, inVec[i], true);
        }
}


void ConvFileC::CFresetParseStr()
{
    CopyString(parsedLine, lastLineRead, maxLineLen);
    beenParsed = false;
}

void ConvFileC::CFtrim()
{
    FullTrim(parsedLine);

}
bool ConvFileC::CFmatches(const char matchStr[])
{
    return (strcmp(matchStr, parsedLine) == 0);
}

bool  ConvFileC::CFmatchStringFound(const char matchStr[])
{
    return (strstr(parsedLine, matchStr)!= NULL);
}


void   ConvFileC::CFextractToCol(int stCol)
{
    stCol = stCol - 1;
    for ( int i = 0; i < stCol; i++)
        {
            if ( parsedLine[i] == '\0')
                break;
            parsedLine[i] = ' ';
            beenParsed = true;
        }
}


bool   ConvFileC:: CFeof()
{
    return (inputF->eof() ? true : false);
}


void   ConvFileC:: CFtextError(const char errMsg[])
{
    char tempStr[120];
    char intStr[40];
    IntToString(nlinesRead, intStr, 40);
    MakeString(tempStr, "Line: ", intStr, " ", errMsg, 120);
    GenAppErrorMsg(infoMsgSrc, tempStr);

    throw CFReadError(tempStr);
}

void ConvFileC:: CFimportOK(int nConv)
{
    char msgStr[80];
    char intStr[40];
    IntToString(nConv, intStr, 40);
    MakeString(msgStr, "Read OK: ", intStr, " data read", 80);
    GenAppInfoMsg(infoMsgSrc, msgStr);
}


void ConvFileC::CFreadDXFCoord(Coord3D& readCoord)
{
    CFreadText();
    readCoord.cX = CFgetNextDouble();

    // 20 or 21
    CFreadText();
    CFreadText();
    readCoord.cY = CFgetNextDouble();

    // 30 or 31 or 0 or 6
    CFreadText();
    if (CFmatches("  0") || CFmatches("  6"))
        readCoord.cZ = 0.0;
    else
        {
            CFreadText();
            readCoord.cZ = CFgetNextDouble();
        }
}

