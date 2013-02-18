///////////////////////////////////////////////////////////////////////////////////
//
// UFO_WriteLabelArray.cpp
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

#include <genClass/DO_LabelArray.h>
#include <genClass/IO_LabelArray.h>

#include <genClass/U_Str.h>

#include <objUFO/UFO_WriteLabelArray.h>


UFO_WriteLabelArray :: UFO_WriteLabelArray() : FuncObjC("Write Label Array")
{
    outFname[0] = '\0';
    InitCommon();
}

UFO_WriteLabelArray::UFO_WriteLabelArray(const UFO_WriteLabelArray& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void UFO_WriteLabelArray::InitCommon()
{
    AddInPort(labelObjRef,   typeid(DO_LabelArray));
    DoStatusChk();
}

UFO_WriteLabelArray& UFO_WriteLabelArray::operator= (const UFO_WriteLabelArray& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  UFO_WriteLabelArray::LocalCopy(const UFO_WriteLabelArray& a)
{
    CopyString(outFname, a.outFname , stdFileStrLen);
    labelObjRef = a.labelObjRef;
}

void  UFO_WriteLabelArray::DoStatusChk()
{
    FuncObjC::DoStatusChk();
    CheckInputFO(labelObjRef, "label data");
}


void UFO_WriteLabelArray:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK() && (calcType == foc_Apply))
    {
        IO_LabelArray outFile;
        DO_LabelArray& outLab = *(static_cast<DO_LabelArray*>(GetInPortData(labelObjRef)));
        outFile.WriteLabelFile(outFname, outLab);
    }
}


