///////////////////////////////////////////////////////////////////////////////////
//
// UFO_WriteXY.cpp
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
//      Basic utility classes for writing data.  Writes XY data to a file.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/DO_XYData.h>
#include <genClass/IO_XYData.h>

#include <genClass/U_Str.h>

#include <objUFO/UFO_WriteXY.h>


UFO_WriteXY :: UFO_WriteXY() : FuncObjC("Write XY")
{
    writeColHeaders = true;
    SetToNull(outFname);
    InitCommon();
}

UFO_WriteXY::UFO_WriteXY(const UFO_WriteXY& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void UFO_WriteXY::InitCommon()
{
    AddInPort(xyObjRef,   typeid(DO_XYData));
    mpiFiles += NodeFile(outFname, "XY file", false);

    DoStatusChk();
}

UFO_WriteXY& UFO_WriteXY::operator= (const UFO_WriteXY& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  UFO_WriteXY::LocalCopy(const UFO_WriteXY& a)
{
    CopyString(outFname, a.outFname , stdFileStrLen);
    xyObjRef = a.xyObjRef;
    writeColHeaders = a.writeColHeaders;
    mpiFiles.CopyFrom(a.mpiFiles);
}

void  UFO_WriteXY::DoStatusChk()
{
    FuncObjC::DoStatusChk();
    CheckInputFO(xyObjRef, "XY data");
    mpiFiles.StdCheckAndUpdate(*this);
}


void UFO_WriteXY:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK() && (calcType == foc_Apply))
    {
        IO_XYData outFile;
        const DC_XYData& outXY = *(static_cast<DO_XYData*>(GetInPortData(xyObjRef)))->xyData;
        outFile.WriteXYFile(outFname, writeColHeaders, outXY);
    }
}


