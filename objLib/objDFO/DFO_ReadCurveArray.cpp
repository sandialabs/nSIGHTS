///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadCurveArray.cpp
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
//      reads curve data from ASCII file
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <objDFO/DFO_ReadCurveArray.h>

DFO_ReadCurveArray :: DFO_ReadCurveArray() : 
    FuncObjC("Read Curve Array")
{
    curveArrayFname[0]    = '\0';
    InitCommon();
}

DFO_ReadCurveArray::DFO_ReadCurveArray(const DFO_ReadCurveArray& a) : 
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ReadCurveArray::InitCommon()
{
    curveArrayDO.curveArray = &curveArray;
    fileNameDO.SetTypeLabel("Curve file");
    AddOutPort(curveArrayDO);
    AddOutPort(fileNameDO);
    DoStatusChk();
}

DFO_ReadCurveArray& DFO_ReadCurveArray::operator= (const DFO_ReadCurveArray& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ReadCurveArray::LocalCopy(const DFO_ReadCurveArray& a)
{
    CopyString(curveArrayFname, a.curveArrayFname, stdFileStrLen);
}

void  DFO_ReadCurveArray:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    if (!curveArrayDO.DataOK())
        SetObjErrMsg("No data read");
}

void DFO_ReadCurveArray:: CalcOutput(FOcalcType  calcType)
{
    if ((calcType == foc_Full) || (calcType == foc_Apply))
    {
        curveArray.DeAlloc();
        objStatus = os_OK;

        if (!CheckFileExists(curveArrayFname))
            return;

        if (!curveArray.ReadFromFile(curveArrayFname))
            return;

        SetDefaultFileID(curveArrayFname);
        fileNameDO.SetFileValueLabel(curveArrayFname);
    }

    DoStatusChk();
    if (objStatus != os_OK)
    {
        curveArray.DeAlloc();
        return;
    }
}

