///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CreateXYArray.cpp
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
//      Combines XY data into DC_XYDataArray
//
///////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <genClass/U_Str.h>

#include <genClass/DO_XYData.h>

#include <objDFO/DFO_CreateXYArray.h>

DFO_CreateXYArray::XYDataDesc::XYDataDesc() :
    doXY(false), xyDataDC(0)
{
    newID[0] = '\0';
}
DFO_CreateXYArray::XYDataDesc::XYDataDesc(const XYDataDesc& a)
{
    LocalCopy(a);
}
DFO_CreateXYArray::XYDataDesc& DFO_CreateXYArray::XYDataDesc::operator= (const XYDataDesc& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}
void DFO_CreateXYArray::XYDataDesc::LocalCopy(const XYDataDesc& a)
{
    doXY     = a.doXY;
    xyObjRef = a.xyObjRef;
    CopyString(newID, a.newID, DC_XYData::dataIDLen);
}

DFO_CreateXYArray :: DFO_CreateXYArray()
    : FuncObjC("Create XY Array")
{
    InitCommon();
}

DFO_CreateXYArray::DFO_CreateXYArray(const DFO_CreateXYArray& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_CreateXYArray :: ~DFO_CreateXYArray ()
{
}

void DFO_CreateXYArray::InitCommon()
{
    outputDO.xyDataArray = &outputDC;
    AddOutPort(outputDO);

    for (int i = 0; i < maxXYDataDesc; i++)
        AddUseFlagInPort(xyData[i].xyObjRef, typeid(DO_XYData), xyData[i].doXY);

    DoStatusChk();
}

DFO_CreateXYArray& DFO_CreateXYArray::operator= (const DFO_CreateXYArray& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  DFO_CreateXYArray::LocalCopy(const DFO_CreateXYArray& a)
{
    for (int i = 0; i < maxXYDataDesc; i++)
        xyData[i] = a.xyData[i];
}


void DFO_CreateXYArray::SetDefault()
{
    for (int i = 0; i < maxXYDataDesc; i++)
        xyData[i].doXY = false;
}

void  DFO_CreateXYArray:: DoStatusChk()
{
    // check input objects
    nSelected = 0;
    for (int i = 0; i < maxXYDataDesc; i++)
        if (xyData[i].doXY)
        {
            // we still build the array even if the input is not OK
            if (CheckInputFO(xyData[i].xyObjRef, "XY"))  
                xyData[i].xyDataDC = static_cast<DO_XYData*>(GetInPortData(xyData[i].xyObjRef))->xyData;
            else
                xyData[i].xyDataDC = 0;
            nSelected++;
        }

    // slight kluge -- reset status to clear any errors from XY check
    // base class check
    FuncObjC::DoStatusChk();

    // only error ... 
    if (nSelected == 0)
        SetObjErrMsg("no XY selected");
}


void DFO_CreateXYArray:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK())
    {
        outputDC.AllocAndSetSize(nSelected);
        nSelected = 0;
        int nbad = 0;
        for (int i = 0; i < maxXYDataDesc; i++)
            if (xyData[i].doXY)
            {
                DC_XYData& currXY = outputDC[nSelected++];
                if (xyData[i].xyDataDC != 0)
                {
                    currXY = *(xyData[i].xyDataDC);
                    if (StringLength(xyData[i].newID))
                        currXY.SetID(xyData[i].newID);
                }
                else
                {
                    // stick a single 0.0 in
                    currXY.Alloc(1);
                    currXY.xData += 0.0; 
                    currXY.yData += 0.0; 
                    currXY.SetID("InputBAD");
                    nbad++;
                }
            }
//      std::cout << "*** CreateXYArray " << GetID() << " nsel " << nSelected << " n bad << " << nbad << std::endl;
    }
}


