///////////////////////////////////////////////////////////////////////////////////
//
// UFO_WriteTable.cpp
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
//      Basic utility classes for writing data.  Writes table to a file.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/DO_TableData.h>

#include <genClass/U_Str.h>

#include <objUFO/UFO_WriteTable.h>


UFO_WriteTable :: UFO_WriteTable() : FuncObjC("Write Table")
{
    writeColHeaders = true;
    writeRowDescs   = false;
    SetToNull(outFname);
    InitCommon();
}

UFO_WriteTable::UFO_WriteTable(const UFO_WriteTable& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void UFO_WriteTable::InitCommon()
{
    AddInPort(tableObjRef,   typeid(DO_TableData));
    mpiFiles += NodeFile(outFname, "table file", false);

    DoStatusChk();
}

UFO_WriteTable& UFO_WriteTable::operator= (const UFO_WriteTable& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  UFO_WriteTable::LocalCopy(const UFO_WriteTable& a)
{
    CopyString(outFname, a.outFname , stdFileStrLen);
    tableObjRef = a.tableObjRef;
    writeColHeaders = a.writeColHeaders;
    writeRowDescs   = a.writeRowDescs  ;
    mpiFiles.CopyFrom(a.mpiFiles);

}

void  UFO_WriteTable::DoStatusChk()
{
    FuncObjC::DoStatusChk();
    CheckInputFO(tableObjRef, "Table data");
    mpiFiles.StdCheckAndUpdate(*this);
}


void UFO_WriteTable:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if ((objStatus == os_OK) && (calcType == foc_Apply))
        WriteTableFile();
}

