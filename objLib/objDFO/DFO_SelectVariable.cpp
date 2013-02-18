///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectVariable.cpp
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
#include <genClass/SC_DoubleArray.h>

#include <objClass/DO_VariableArray.h>

#include <objDFO/DFO_SelectVariable.h>


DFO_SelectVariable :: DFO_SelectVariable() :
    DFO_SelectBase("SelectVariable", baseMS)
{
    InitCommon();
}

DFO_SelectVariable::DFO_SelectVariable(const DFO_SelectVariable& a) :
    DFO_SelectBase(a, baseMS)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SelectVariable::InitCommon()
{
    InitMSPorts();

    AddOutPort(realDO);
    AddInPort(varDataArrayObjRef, typeid(DO_VariableArray));
    realDO.SetTypeLabel("Selected variable");

    DoStatusChk();
}

DFO_SelectVariable& DFO_SelectVariable::operator= (const DFO_SelectVariable& a)
{
    if (&a != this)
    {
        DFO_SelectBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectVariable::LocalCopy(const DFO_SelectVariable& a)
{
    varDataArrayObjRef  = a.varDataArrayObjRef;
}

void  DFO_SelectVariable:: DoStatusChk()
{
    DFO_SelectBase::DoStatusChk();
    indexMS.SetTypeLabel("Selected Variable");

    if (!CheckInputFO(varDataArrayObjRef, "Data source"))
        return;

    varDataDO = static_cast<DO_VariableArray*>(GetInPortData(varDataArrayObjRef));

    if (IndexRangeOK(varDataDO->variableArrayData->Size()))
    {
        indexMS.indexDO.SetValueLabel((*varDataDO->variableArrayData)[indexMS.GetCurrentIndex()].varID);
    }

}

void DFO_SelectVariable:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    int index = indexMS.GetCurrentIndex();

    realVal = (*varDataDO->variableArrayData)[index].varValue;

    realDO.realVal = realVal;
    SetDefaultID((*varDataDO->variableArrayData)[index].varID);
}

