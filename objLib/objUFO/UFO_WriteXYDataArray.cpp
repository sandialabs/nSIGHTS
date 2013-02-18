///////////////////////////////////////////////////////////////////////////////////
//
// UFO_WriteXYDataArray.cpp
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

#include <objClass/DO_XYDataArray.h>
#include <objClass/IO_XYDataArray.h>

#include <objUFO/UFO_WriteXYDataArray.h>


UFO_WriteXYDataArray :: UFO_WriteXYDataArray() : FuncObjC("Write XY Array")
{
    SetToNull(outFname);
    writeAsText = true;
    InitCommon();
}

UFO_WriteXYDataArray::UFO_WriteXYDataArray(const UFO_WriteXYDataArray& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void UFO_WriteXYDataArray::InitCommon()
{
    AddInPort(xyDataArrayObjRef,   typeid(DO_XYDataArray));
    mpiFiles += NodeFile(outFname, "XY Array file", false);

    DoStatusChk();
}

UFO_WriteXYDataArray& UFO_WriteXYDataArray::operator= (const UFO_WriteXYDataArray& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void  UFO_WriteXYDataArray::LocalCopy(const UFO_WriteXYDataArray& a)
{
    CopyString(outFname, a.outFname, stdFileStrLen);
    xyDataArrayObjRef = a.xyDataArrayObjRef;
    writeAsText = a.writeAsText;
    mpiFiles.CopyFrom(a.mpiFiles);
}

void  UFO_WriteXYDataArray::DoStatusChk()
{
    FuncObjC::DoStatusChk();
    if (CheckInputFO(xyDataArrayObjRef, "XY data"))
    {
        xyData = static_cast<DO_XYDataArray*>(GetInPortData(xyDataArrayObjRef))->xyDataArray;
    }
    mpiFiles.StdCheckAndUpdate(*this);
}


void UFO_WriteXYDataArray:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if ((objStatus == os_OK) && (calcType == foc_Apply))
    {
        IO_XYDataArray xyFile;
        xyFile.WriteXYDataArrayFile(outFname, *xyData, writeAsText);
    }

}

