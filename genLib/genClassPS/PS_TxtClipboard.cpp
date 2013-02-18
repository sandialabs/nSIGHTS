///////////////////////////////////////////////////////////////////////////////////
//
// PS_TxtClipboard.cpp
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
// DESCRIPTION: Platform specific but application independent code.
//              Derived from TextC. Encapsulates/implements clipboard I/O
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include "PS_TxtClipboard.h"

PS_TxtClipboard::PS_TxtClipboard(int maxChars) :
    m_bNextEOF(false),
    maxOutputStringLen(maxChars)
{
    openedForWriting = false;
    openOK = false;
    textToWrite.SetStringLen(maxOutputStringLen);

    currLine = new char[maxOutputStringLen];
    tempLine = new char[maxOutputStringLen];

    SetToNull(currLine);
}

PS_TxtClipboard::~PS_TxtClipboard()
{
    CloseTxtClipboard();
    delete currLine;
    delete tempLine;
}


bool PS_TxtClipboard::OpenTxtClipboard(bool forWrite)
{
    openedForWriting = forWrite;
    if (openedForWriting)
        {
            textToWrite.SetResizable(512);  // most are shorter than this
            return true;
        }

    // set up reading
    openOK = false;
    m_bNextEOF = false;
    if (!m_obj.AttachClipboard())
        {
            return false;
        }
    if (!m_obj.IsDataAvailable(CF_TEXT))
        {
            m_obj.Release();
            return false;
        }

    openOK = true;
    m_hMem = m_obj.GetGlobalData(CF_TEXT);
    m_nImportLength = (int) ::GlobalSize(m_hMem);
    m_memFile.Attach((BYTE*)::GlobalLock(m_hMem), (UINT) ::GlobalSize(m_hMem));
    return true;
}


void PS_TxtClipboard::WriteToClipboard()
{
    // count chars to copy
    int nChars = 0;
    int i;
    for (i = 0; i < textToWrite.Size(); i++)
        {
            // no extra trailing blanks required
            TrimLeft(textToWrite[i]);
            nChars += StringLength(textToWrite[i]);
            nChars += 2; // for newline ??
        }
    // something to write ??
    if (nChars == 0)
        return;

    //code to copy to the clipboard
    if (!::OpenClipboard(NULL))
        return;
    ::EmptyClipboard();

    // allocate the global memory object
    HGLOBAL memObj = GlobalAlloc(GMEM_MOVEABLE, (nChars + 1) * sizeof(TCHAR));
    if (memObj == NULL)
        {
            ::CloseClipboard();
            return;
        }

    // Lock onto the handle
    LPTSTR  memHandle = (TCHAR*) GlobalLock(memObj);

    // copy the text to the buffer
    int cch = 0;
    for (i = 0; i < textToWrite.Size(); i++)
        {
            const char* currStr = textToWrite[i];
            int currLen = StringLength(currStr);
            for (int j = 0; j < currLen; j++)
                {
                    TCHAR currChar = currStr[j];
                    memHandle[cch++] = currChar;
                }
            // new line
            memHandle[cch++] = TCHAR('\n');
        }

    // and terminate
    memHandle[cch] = (TCHAR) 0;    // null character

    GlobalUnlock(memObj);

    // Place the handle on the clipboard.
    SetClipboardData(CF_TEXT, memObj);

    // close clipboard
    ::CloseClipboard();
}


void PS_TxtClipboard::CloseTxtClipboard()
{
    if (openedForWriting)
        {
            WriteToClipboard();
            textToWrite.DeAlloc();
            openedForWriting = false;
        }
    else if (openOK)
        {
            ::GlobalUnlock(m_hMem);
            m_memFile.Detach();
            m_obj.Release();
        }
    openOK = false;
}

void PS_TxtClipboard::Close()
{
    CloseTxtClipboard();
}


bool PS_TxtClipboard::IsEOF()
{
    return m_bNextEOF;
}

void PS_TxtClipboard::ReadClipboardLine(char* fText, int maxLen)
{
    if (m_bNextEOF)
        ThrowTextError("Attempt to read past end of clipboard");

    char cChar;
    if (m_memFile.Read(&cChar, 1) == 0)
        ThrowTextError("Could not read from clipboard");

    SetToNull(fText);
    int nChar = 0;
    if (cChar != '\n')
        {
            // read from last char from previous line of necessery
            fText[nChar++] = cChar;
        }

    while (true)
        {
            if (m_memFile.Read(&cChar, 1) == 0)
                {
                    m_bNextEOF = true;
                    fText[nChar] = '\0';
                    break;
                }

            if (cChar == '\r' || cChar == '\n')
                {
                    fText[nChar] = '\0';
                    break;
                }

            fText[nChar++] = cChar;

            // overflow next ?
            if (nChar == maxLen)
                {
                    // reset last to null and return -- char gets lost
                    fText[maxLen - 1] = '\0';
                    nChar = maxLen - 1;
                    break;
                }
        }

    // sometimes we get garbage - clear all invalid chars
    for (int i = 0; i < nChar; i++)
        if (fText[i] < 0)
            fText[i] = ' ';
}

void PS_TxtClipboard::CheckTextError(const char* errMsg)
{
    // any errors should have already been processed and thrown
    return;
}

//  ----------------------------------------- TextC virtuals

void PS_TxtClipboard::WriteLine()
{
    textToWrite += currLine;
    SetToNull(currLine);
}

static const char nullText[] = "NULL";

//  ensures no blank lines to confuse ReadNextLine

void PS_TxtClipboard::WriteText(const char* fText)
{
    int nChar = StringLength(fText);
    bool blank = true;
    for (int i = 0; i < nChar; i++)
        if (fText[i] != ' ')
            blank = false;

    if (blank)
        {
            textToWrite += nullText;
        }
    else
        {
            textToWrite += fText;
        }
    SetToNull(currLine);
}


void PS_TxtClipboard::WriteSolidText(const char* fText)
{
    ConcatString(currLine, fText, maxOutputStringLen);
    ConcatString(currLine, " ", maxOutputStringLen);
}


void PS_TxtClipboard::ReadText(char* fText,
                               int   maxLen)
{
    //  kluge to get around mixing >> and getline
    //  sometimes after >>, getline reads just EOL char or trailing blanks
    do
        {
            ReadClipboardLine(tempLine, maxOutputStringLen);
            TrimLeft(tempLine);
        }
    while (StringLength(tempLine) == 0);

    if (strcmp(tempLine, nullText) == 0)
        fText[0] = '\0';
    else
        CopyString(fText, tempLine, maxLen);
}


void PS_TxtClipboard::ReadSolidText(char* fText)
{
    if (m_bNextEOF)
        ThrowTextError("Attempt to read past end of clipboard");
    char cChar;

    SetToNull(fText);
    int nChar = 0;
    while (true)
        {
            if (m_memFile.Read(&cChar, 1) == 0)
                {
                    m_bNextEOF = true;
                    fText[nChar] = '\0';
                    return;
                }

            if (NotWhiteSpace(cChar) && (cChar != '\r') && (cChar != '\n'))
                {
                    fText[nChar++] = cChar;
                }
            else
                if (nChar > 0)
                    {
                        fText[nChar] = '\0';
                        return;
                    }
        }
}

