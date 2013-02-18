///////////////////////////////////////////////////////////////////////////////////
//
// UFO_VariableProcess.cpp
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

#include <objClass/DO_VariableArray.h>
#include <objDataClass/DSC_VariableProcess.h>

#include <objUFO/UFO_VariableProcess.h>


UFO_VariableProcess :: UFO_VariableProcess() : FuncObjC("Pre-Process Variables")
{
    SetToNull(directiveFileName);
    openOutputFile = true;
    SetToNull(outputFileName);
    InitCommon();
}

UFO_VariableProcess::UFO_VariableProcess(const UFO_VariableProcess& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void UFO_VariableProcess::InitCommon()
{
    AddInPort(variableObjRef,   typeid(DO_VariableArray));

    mpiFiles += NodeFile(directiveFileName, "directive file");
    mpiFiles += NodeFile(outputFileName, "output file", false);

    DoStatusChk();
}

UFO_VariableProcess& UFO_VariableProcess::operator= (const UFO_VariableProcess& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  UFO_VariableProcess::LocalCopy(const UFO_VariableProcess& a)
{
    CopyString(outputFileName, a.outputFileName , stdFileStrLen);
    CopyString(directiveFileName, a.directiveFileName, stdFileStrLen);
    variableObjRef = a.variableObjRef;
    openOutputFile = a.openOutputFile;
    mpiFiles.CopyFrom(a.mpiFiles);
}

void  UFO_VariableProcess::DoStatusChk()
{
    FuncObjC::DoStatusChk();
    CheckInputFO(variableObjRef, "Variable array data");

    mpiFiles.SetUsed(1, openOutputFile);
    mpiFiles.StdCheckAndUpdate(*this);
}



void UFO_VariableProcess:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusOK() && (calcType == foc_Apply))
    {
        const DC_VariableArray& tmpDC = static_cast<DO_VariableArray*>(GetInPortData(variableObjRef))->GetArrayRef();

        SC_SetupErr processErrs;
        DSC_VariableProcess varProc;
        char* outFile = NULL;
        if (openOutputFile)
            outFile = outputFileName;

        if (!varProc.ProcessVariables(tmpDC, directiveFileName, outFile, processErrs))
            SetObjErrMsg(processErrs.GetErrorText());
    }
}

