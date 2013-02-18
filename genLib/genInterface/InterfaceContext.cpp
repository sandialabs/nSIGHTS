///////////////////////////////////////////////////////////////////////////////////
//
// InterfaceContext.cpp
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
// DESCRIPTION: Windows specific code for implementing Outlook style
//              application with dialogs and object trees.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "InterfaceContext.h"

#include <genApp/AppConfigFile.h>
#include <genApp/ProjectUtil.h>

InterfaceContext interfaceContext;

const char* InterfaceContext::groupHeader = "Interface";
const int  InterfaceContext::majorVersion = 0;
const int  InterfaceContext::minorVersion = 0;


////////////////////////////////////////////////////////////////////////////
InterfaceContext::InterfaceContext() :
    m_hWndLastFocus(NULL),
    m_pDialogLastFocus(NULL)
{
}

InterfaceContext::~InterfaceContext()
{
}

void InterfaceContext::Push(int nSelection)
{
    m_contextStack.Add(nSelection);
}

int InterfaceContext::Pop()
{
    if (m_contextStack.GetSize() == 0) {
        return -1;
    }
    int nSelection = m_contextStack[m_contextStack.GetSize()-1];
    m_contextStack.RemoveAt(m_contextStack.GetSize()-1);
    return nSelection;
}

void InterfaceContext::Clear()
{
    m_contextStack.RemoveAll();
    m_pDialogLastFocus = NULL;
    m_hWndLastFocus = NULL;
}

void InterfaceContext::WriteToFile()
{
    WriteGroupHeader(groupHeader, majorVersion, minorVersion);

    WriteLine();
    int nSize = (int) m_contextStack.GetSize();
    WriteInt(nSize);
    for (int i = 0; i < nSize; i++)
        {
            WriteInt(m_contextStack[i]);
        }
    WriteLine();
    WriteBool(projectUtilities::OutbarIconsAreSmall());
    WriteLine();
}

bool InterfaceContext::ReadFromFile(bool oldVer)
{
    if (!oldVer)
        ReadGroupHeader(groupHeader, majorVersion);

    ReadLine();
    int nSize = ReadInt();
    for (int i = 0; i < nSize; i++)
        {
            int nNode = ReadInt();
            m_contextStack.Add(nNode);
        }
    ReadLine();
    // Keep it but don't use it
    //bSmallOutBarIcon = ReadBool();
    ReadBool();
    ReadLine();
    return true;
}

