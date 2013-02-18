///////////////////////////////////////////////////////////////////////////////////
//
// UFO_WriteCurveArray.cpp
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
//      Basic utility classes for writing data.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <objClass/DC_Curve.h>

#include <objUFO/UFO_WriteCurveArray.h>

UFO_WriteCurveArray::CurveData::CurveData() :
    doCurve(false), curveDataDC(0)
{
    curveID[0] = '\0';
}
UFO_WriteCurveArray::CurveData::CurveData(const CurveData& a)
{
    LocalCopy(a);
}
UFO_WriteCurveArray::CurveData& UFO_WriteCurveArray::CurveData::operator= (const CurveData& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}
void UFO_WriteCurveArray::CurveData::LocalCopy(const CurveData& a)
{
    doCurve     = a.doCurve;
    curveObjRef = a.curveObjRef;
    CopyString(curveID, a.curveID, DC_CurveWithID::curveIDlen);
}

UFO_WriteCurveArray :: UFO_WriteCurveArray()
    : FuncObjC("Write Curve Array")
{
    outFname[0] = '\0';
    InitCommon();
}

UFO_WriteCurveArray::UFO_WriteCurveArray(const UFO_WriteCurveArray& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

UFO_WriteCurveArray :: ~UFO_WriteCurveArray ()
{
}

void UFO_WriteCurveArray::InitCommon()
{
    for (int i = 0; i < maxCurveData; i++)
        AddUseFlagInPort(curveData[i].curveObjRef, typeid(DO_Curve), curveData[i].doCurve);

    DoStatusChk();
}

UFO_WriteCurveArray& UFO_WriteCurveArray::operator= (const UFO_WriteCurveArray& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  UFO_WriteCurveArray::LocalCopy(const UFO_WriteCurveArray& a)
{
    CopyString(outFname, a.outFname, stdFileStrLen);
    for (int i = 0; i < maxCurveData; i++)
        curveData[i] = a.curveData[i];
}


void UFO_WriteCurveArray::SetDefault()
{
    for ( int i = 0; i < maxCurveData; i++)
        curveData[i].doCurve = false;
}

void  UFO_WriteCurveArray:: DoStatusChk()
{
    // base class check
    FuncObjC::DoStatusChk();

    // check input objects
    nSelected = 0;
    for (int i = 0; i < maxCurveData; i++)
        if (curveData[i].doCurve)
        {
            if (!CheckInputFO(curveData[i].curveObjRef, "Curve"))
                return;
            if (StringLength(curveData[i].curveID) == 0)
            {
                SetObjErrMsg("no ID for curve");
                return;
            }

            curveData[i].curveDataDC = static_cast<DO_Curve*>(GetInPortData(curveData[i].curveObjRef))->curve;
            nSelected++;
        }

    if (nSelected == 0)
        SetObjErrMsg("no curves selected to write");

    if (StringLength(outFname) == 0)
        SetObjErrMsg("File name not set");
}


void UFO_WriteCurveArray:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if ((objStatus == os_OK) && (calcType == foc_Apply))

    {
        outCurve.AllocAndSetSize(nSelected);
        nSelected = 0;
        for (int i = 0; i < maxCurveData; i++)
            if (curveData[i].doCurve)
            {
                outCurve[nSelected] = *(curveData[i].curveDataDC);
                CopyString(outCurve[nSelected].curveID, curveData[i].curveID, DC_CurveWithID::curveIDlen);
                nSelected++;
            }

        outCurve.WriteToFile(outFname);
        SetDefaultFileID(outFname);
    }
}


