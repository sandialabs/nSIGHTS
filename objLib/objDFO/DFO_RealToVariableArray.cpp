///////////////////////////////////////////////////////////////////////////////////
//
// DFO_RealToVariableArray.cpp
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

#include <objDFO/DFO_RealToVariableArray.h>

DFO_RealToVariableArray::RealSel::RealSel() :
    doReal(false)
{
    SetToNull(realID);
}
DFO_RealToVariableArray::RealSel::RealSel(const RealSel& a)
{
    LocalCopy(a);
}
DFO_RealToVariableArray::RealSel& DFO_RealToVariableArray::RealSel::operator= (const RealSel& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}
void DFO_RealToVariableArray::RealSel::LocalCopy(const RealSel& a)
{
    doReal     = a.doReal;
    realObjRef = a.realObjRef;
    CopyString(realID, a.realID, DC_Variable::varIDLen);
}

DFO_RealToVariableArray :: DFO_RealToVariableArray()
    : FuncObjC("Convert Reals to Variable Array")
{
    InitCommon();
}

DFO_RealToVariableArray::DFO_RealToVariableArray(const DFO_RealToVariableArray& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_RealToVariableArray :: ~DFO_RealToVariableArray ()
{
}

void DFO_RealToVariableArray::InitCommon()
{
    outputVarDO.variableArrayData = &outputVarDC;
    AddOutPort(outputVarDO);

     for (int i = 0; i < maxRealSel; i++)
        AddUseFlagInPort(realSelData[i].realObjRef, typeid(DO_Real), realSelData[i].doReal);

    DoStatusChk();
}

DFO_RealToVariableArray& DFO_RealToVariableArray::operator= (const DFO_RealToVariableArray& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void  DFO_RealToVariableArray::LocalCopy(const DFO_RealToVariableArray& a)
{
    for (int i = 0; i < maxRealSel; i++)
        realSelData[i] = a.realSelData[i];
}



void  DFO_RealToVariableArray:: DoStatusChk()
{
    outputVarDO.Reset("Combined reals");

    // base class check
    FuncObjC::DoStatusChk();

    // check input objects
    nrealSel = 0;
    for (int i = 0; i < maxRealSel; i++)
        if (realSelData[i].doReal)
        {
            if (IsNull(realSelData[i].realID))
                SetObjErrMsg("variable ID not set for real");

            if (!CheckInputFO(realSelData[i].realObjRef, "Real"))
                return;
            DO_Real* tmpDO = static_cast<DO_Real*>(GetInPortData(realSelData[i].realObjRef));

            realSelData[i].realDataDO = tmpDO;
            nrealSel++;
        }

    if (nrealSel == 0)
    {
        SetObjErrMsg("no reals selected");
        return;
    }

}


void DFO_RealToVariableArray:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    outputVarDC.AllocAndSetSize(nrealSel);
    nrealSel = 0;
    for (int i = 0; i < maxRealSel; i++)
        if (realSelData[i].doReal)
        {
            outputVarDC[nrealSel].SetID(realSelData[i].realID);
            outputVarDC[nrealSel++].SetValue(realSelData[i].realDataDO->realVal);
        }
}


