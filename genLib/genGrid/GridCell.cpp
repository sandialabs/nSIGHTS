///////////////////////////////////////////////////////////////////////////////////
//
// GridCell.cpp
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
// DESCRIPTION: Windows specific code for implementing basic spreadsheet type
//              data entry/display.
//              Base class for individual cells
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>

#include "GridCell.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGridCell::CGridCell() :
    m_bReadOnly(false)
{
}

CGridCell::~CGridCell()
{
}

CGridCell::CGridCell(const CGridCell& /*rhs*/)
{
}

const CGridCell& CGridCell::operator=(const CGridCell& rhs)
{
    if (&rhs == this) return *this;
    return *this;
}

void CGridCell::DrawUnconstrained(CDC* /* pDC */, int /* x */, int /* y */) const
{
}

void CGridCell::DrawConstrained(CDC* /* pDC */, int /*x*/, int /*y*/, int /*cx*/, int /*cy*/) const
{
}

CSize CGridCell::GetExtent(CDC* /*pDC*/) const
{
    return CSize(0,0);
}

CSize CGridCell::GetNewlineTextExtent(CDC* pDC, const char* str)
{
    CSize sizeRet(0,0);
    ASSERT(str);
    const char* strstart = str;
    const char* strpos = str;
    const char* strend = str + StringLength(str);
    while (strstart != 0) {
        CSize tempSize;
        strpos = strchr(strstart, '\n');
        if (strpos != 0) {
            tempSize = pDC->GetTextExtent(strstart, (int) (strpos - strstart));
            strstart = strpos + 1;
        } else {
            tempSize = pDC->GetTextExtent(strstart, (int) (strend - strstart));
            strstart = strpos;
        }
        sizeRet.cx = max(sizeRet.cx, tempSize.cx);
        sizeRet.cy += tempSize.cy;
    }
    return sizeRet;
}

