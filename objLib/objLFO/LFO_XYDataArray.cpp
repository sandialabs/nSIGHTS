///////////////////////////////////////////////////////////////////////////////////
//
// LFO_XYDataArray.cpp
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

#include <objClass/DO_XYDataArray.h>

#include <objLFO/LFO_XYDataArray.h>

LFO_XYDataArray :: LFO_XYDataArray(ListDefC& assLst)
    : ListObjC("List XY Array", assLst)
{
    listXYDataID        = false;
    listHeaders         = true;
    xyDataColWidth      = 15;
    xyDataFormat        = SC_SciFormat(5);

    InitCommon();
}

LFO_XYDataArray::LFO_XYDataArray(const LFO_XYDataArray& a, ListDefC& assLst) : ListObjC(a, assLst)
{
    LocalCopy(a);
    InitCommon();
}

LFO_XYDataArray :: ~LFO_XYDataArray ()
{
}

void LFO_XYDataArray::InitCommon()
{
    AddInPort(xyDataObjRef, typeid(DO_XYDataArray));
    DoStatusChk();
}

LFO_XYDataArray& LFO_XYDataArray::operator= (const LFO_XYDataArray& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void LFO_XYDataArray::LocalCopy(const LFO_XYDataArray& a)
{
    xyDataObjRef    = a.xyDataObjRef;
    listXYDataID    = a.listXYDataID;
    listHeaders     = a.listHeaders;
    xyDataColWidth  = a.xyDataColWidth;
    xyDataFormat    = a.xyDataFormat;
}


void LFO_XYDataArray::DoStatusChk()
{
    FuncObjC::DoStatusChk();
    CheckInputFO(xyDataObjRef, "XYData");
}


void LFO_XYDataArray::CreateListing()
{
    DC_XYDataArray& inXYDataArrayDC = *(static_cast<DO_XYDataArray*>(GetInPortData(xyDataObjRef))->xyDataArray);

    if (xyDataColWidth > listingLineLen / 2)
        xyDataColWidth = listingLineLen / 2 - 1;

    for (int i = 0; i < inXYDataArrayDC.Size(); i++)
    {
        DC_XYData& inXYDataDC = inXYDataArrayDC[i];
        char tempStr[80];
        if (listXYDataID)
        {
            MakeString(tempStr, "XYData: ", inXYDataDC.GetID(), 80);
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

        for (int j = 0; j < inXYDataDC.Size(); j++)
        {
            SetTableReal(0, inXYDataDC.xData[j], xyDataFormat);
            SetTableReal(1, inXYDataDC.yData[j], xyDataFormat);
            AddNextLine();
        }
        AddBlankLine();
    }
    AddBlankLine();
}


