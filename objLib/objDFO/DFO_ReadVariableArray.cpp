///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadVariableArray.cpp
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
#include <genClass/C_MessageLog.h>

#include <objDFO/DFO_ReadVariableArray.h>


DFO_ReadVariableArray :: DFO_ReadVariableArray() : FuncObjC("Read Variable Array")
{
    SetToNull(varArrayFname);
    varArrayFileFormat   = IO_VariableArray::vffPaCalc;

    InitCommon();
}

DFO_ReadVariableArray::DFO_ReadVariableArray(const DFO_ReadVariableArray& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ReadVariableArray::InitCommon()
{
    AddOutPort(fileNameDO);
    varDO.variableArrayData = &varDC;
    AddOutPort(varDO);
    mpiFiles += NodeFile(varArrayFname, "variable array file");
    varDO.SetTypeLabel("Variables read");
    DoStatusChk();
}

DFO_ReadVariableArray& DFO_ReadVariableArray::operator= (const DFO_ReadVariableArray& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ReadVariableArray::LocalCopy(const DFO_ReadVariableArray& a)
{
    CopyString(varArrayFname, a.varArrayFname, stdFileStrLen);
    varArrayFileFormat = a.varArrayFileFormat;
    mpiFiles.CopyFrom(a.mpiFiles);
}

int DFO_ReadVariableArray::GetNVars() const
{
    return varDC.Size();
}


void  DFO_ReadVariableArray:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    if (!varDO.DataOK())
        SetObjErrMsg("No data read");

    fileNameDO.SetTypeLabel("Variable file");
}

void DFO_ReadVariableArray:: CalcOutput(FOcalcType  calcType)
{
    if ((calcType == foc_Full) || (calcType == foc_Apply))
    {
        objStatus = os_OK;
        mpiFiles.StdCheckAndUpdate(*this);
        if (StatusNotOK())
            return;

        IO_VariableArray inFile;
        if (!inFile.ReadVariableFile(varArrayFileFormat, varArrayFname, varDC))
        {
            SetObjErrMsg(MessageLogC::GetLastMessage());
            return;
        }

        SetDefaultFileID(varArrayFname);
        fileNameDO.SetFileValueLabel(varArrayFname);
    }

    DoStatusChk();
}

