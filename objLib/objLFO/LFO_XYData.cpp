///////////////////////////////////////////////////////////////////////////////////
//
// LFO_XYData.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genClass/DO_XYData.h>

#include <objLFO/LFO_XYData.h>

LFO_XYData :: LFO_XYData(ListDefC& assLst)
    : ListObjC("List XY Data", assLst)
{
    listXYDataID        = false;
    listHeaders         = true;
    xyDataColWidth      = 15;
    xyDataFormat        = SC_SciFormat(5);

    InitCommon();
}

LFO_XYData::LFO_XYData(const LFO_XYData& a, ListDefC& assLst) : ListObjC(a, assLst)
{
    LocalCopy(a);
    InitCommon();
}

LFO_XYData :: ~LFO_XYData ()
{
}

void LFO_XYData::InitCommon()
{
    AddInPort(xyDataObjRef, typeid(DO_XYData));
    DoStatusChk();
}

LFO_XYData& LFO_XYData::operator= (const LFO_XYData& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void LFO_XYData::LocalCopy(const LFO_XYData& a)
{
    xyDataObjRef    = a.xyDataObjRef;
    listXYDataID    = a.listXYDataID;
    listHeaders     = a.listHeaders;
    xyDataColWidth  = a.xyDataColWidth;
    xyDataFormat    = a.xyDataFormat;
}


void LFO_XYData::DoStatusChk()
{
    FuncObjC::DoStatusChk();
    CheckInputFO(xyDataObjRef, "XYData");
}


void LFO_XYData::CreateListing()
{
    const DC_XYData* inXYDataDC = static_cast<DO_XYData*>(GetInPortData(xyDataObjRef))->xyData;

    if (xyDataColWidth > listingLineLen / 2)
        xyDataColWidth = listingLineLen / 2 - 1;

    char tempStr[80];
    if (listXYDataID)
    {
        MakeString(tempStr, "XYData: ", inXYDataDC->GetID(), 80);
        TextStdTitle(tempStr);
    }

    SC_IntArray  colWidth(2, xyDataColWidth);
    SC_BoolArray leftJust(2, false);

    TableSetup(colWidth, leftJust, 0);


    if (listHeaders)
    {
        SetTableCol(0, "X");
        SetTableCol(1, "Y");
        AddNextLine();
        AddBlankLine();
    }

    for (int i = 0; i <  inXYDataDC->Size(); i++)
    {
        SetTableReal(0, inXYDataDC->xData[i], xyDataFormat);
        SetTableReal(1, inXYDataDC->yData[i], xyDataFormat);
        AddNextLine();
    }
    AddBlankLine();
}


