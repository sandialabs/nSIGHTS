///////////////////////////////////////////////////////////////////////////////////
//
// DFO_GridInterpolate.cpp
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

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <objDFO/DFO_GridInterpolate.h>

DFO_GridInterpolate :: DFO_GridInterpolate() : FuncObjC("Interpolate Grid")
{
    InitCommon();
}

DFO_GridInterpolate::DFO_GridInterpolate(const DFO_GridInterpolate& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_GridInterpolate :: ~DFO_GridInterpolate ()
{
}

void DFO_GridInterpolate::InitCommon()
{
    inputA.AddPort(*this);
    inputB.AddPort(*this);
    outputDataDO.gridData = &outputData;
    AddOutPort(outputDataDO);
    DoStatusChk();
}

DFO_GridInterpolate& DFO_GridInterpolate::operator= (const DFO_GridInterpolate& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_GridInterpolate::LocalCopy(const DFO_GridInterpolate& a)
{
    inputA = a.inputA;
    inputB = a.inputB;
    logInterp = a.logInterp;
}

void  DFO_GridInterpolate:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    inputA.DoCheck(*this);
    inputB.DoCheck(*this);

    if (StatusNotOK())
        return;

}

void DFO_GridInterpolate:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    if (!outputData.CreateFrom(*inputB.gridInputDC))
    {
        SetObjErrMsg("memory allocation ??");
        return;
    }

    const DC_GridData& inputGrd = *inputA.gridInputDC;

    for ( int i = 0; i < outputData.xData.Size(); i++)
    {
        double xVal = outputData.xData[i];
        for ( int j = 0; j < outputData.yData.Size(); j++)
        {
            double yVal = outputData.yData[j];
            double temp;
            if (!inputGrd.ExtractPt(Point2D(xVal, yVal), logInterp, temp))
                temp = nullReal;
            outputData.gData[i][j] = temp;
        }
    }

    //  update ID
    outputData.SetID(GetID());
}

