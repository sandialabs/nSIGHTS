///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadXYDataArray.cpp
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
//      Reads XY data from ASCII file in several formats
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <objClass/IO_XYDataArray.h>

#include <objDFO/DFO_ReadXYDataArray.h>



DFO_ReadXYDataArray :: DFO_ReadXYDataArray() : FuncObjC("Read XY Array")
{
    SetToNull(xyDataArrayFname);
    InitCommon();
}

DFO_ReadXYDataArray::DFO_ReadXYDataArray(const DFO_ReadXYDataArray& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ReadXYDataArray::InitCommon()
{
    AddOutPort(fileNameDO);
    AddOutPort(xyDataArrayDO);

    fileNameDO.SetTypeLabel("XY Data array file");
    mpiFiles += NodeFile(xyDataArrayFname, "XY Array file");

    DoStatusChk();
}

DFO_ReadXYDataArray& DFO_ReadXYDataArray::operator= (const DFO_ReadXYDataArray& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ReadXYDataArray::LocalCopy(const DFO_ReadXYDataArray& a)
{
    CopyString(xyDataArrayFname, a.xyDataArrayFname, stdFileStrLen);
    mpiFiles.CopyFrom(a.mpiFiles);
}


void  DFO_ReadXYDataArray:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    xyDataArrayDO.xyDataArray = &xyDataArray;

    mpiFiles.StdCheckAndUpdate(*this);
}


void DFO_ReadXYDataArray::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if ((objStatus == os_Bad) || (objStatus == os_NotSet) || (calcType == foc_PortOnly))
        return;

    IO_XYDataArray xyRead;
    if (xyRead.ReadXYDataArrayFile(xyDataArrayFname, xyDataArray))
    {
        char tempStr[stdFileStrLen];
        ExtractFileName(tempStr, xyDataArrayFname, stdFileStrLen);
        SetDefaultID(tempStr);
        fileNameDO.SetValueLabel(tempStr);
        DoStatusChk();
    }
    else
    {
        SetObjErrMsg("File is not a xy data array file");
        xyDataArray.DeAlloc();
    }
}

