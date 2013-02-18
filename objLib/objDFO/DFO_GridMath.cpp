///////////////////////////////////////////////////////////////////////////////////
//
// DFO_GridMath.cpp
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
//      add/subtract/multiply/divide contents of to grids
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <objDFO/DFO_GridMath.h>

DFO_GridMath :: DFO_GridMath() : DFO_MathBase("Grid Math")
{
    InitCommon();
}

DFO_GridMath::DFO_GridMath(const DFO_GridMath& a) : DFO_MathBase(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_GridMath :: ~DFO_GridMath ()
{
}

void DFO_GridMath::InitCommon()
{
    inputA.AddPort(*this);
    inputB.AddPort(*this);
    outputDataDO.gridData = &outputData;
    AddOutPort(outputDataDO);
    DoStatusChk();
}

DFO_GridMath& DFO_GridMath::operator= (const DFO_GridMath& a)
{
    if (&a != this)
    {
        DFO_MathBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_GridMath::LocalCopy(const DFO_GridMath& a)
{
    inputA = a.inputA;
    inputB = a.inputB;
}

void  DFO_GridMath:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    inputA.DoCheck(*this);
    inputB.DoCheck(*this);

    if (StatusNotOK())
        return;

    if (!inputA.gridInputDC->IsCompatible(*inputB.gridInputDC))
        SetObjErrMsg("A and B must be the same size");
}


void DFO_GridMath:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    if (!outputData.CreateFrom(*inputA.gridInputDC))
    {
        SetObjErrMsg("memory allocation ??");
        return;
    }

    for ( int i = 0; i < outputData.Size(); i++)
    {
        outputData[i] = MathResult((*inputA.gridInputDC)[i], (*inputB.gridInputDC)[i]);
    }

    //  update ID
    outputData.SetID(GetCurrID());

}

