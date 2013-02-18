///////////////////////////////////////////////////////////////////////////////////
//
// DSC_RealMSSpec.cpp
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

#include <genDataClass/DSC_RealMSSpec.h>


DSC_RealMSSpec::DSC_RealMSSpec(const char* valID, DO_Real& inDO) :
    valueID(valID), valueDO(inDO)
{
    valueIsMaster    = true;
    enteredRealValue = 0.0;
    slaveRealValue   = 0.0;
}


DSC_RealMSSpec& DSC_RealMSSpec::operator= (const DSC_RealMSSpec& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

void DSC_RealMSSpec::Clear()
{
    valueIsMaster       = true;
    enteredRealValue    = 0.0;
    slaveRealValue      = 0.0;
}

void DSC_RealMSSpec::LocalCopy(const DSC_RealMSSpec& a)
{
    valueIsMaster     = a.valueIsMaster;
    valueMasterObjRef = a.valueMasterObjRef;
    enteredRealValue  = a.enteredRealValue;
    slaveRealValue    = a.slaveRealValue;
}

void  DSC_RealMSSpec::DoCheck(const char*      specID,
                              FuncObjC&        specFO)
{
    valueDO.Reset(valueID);
    if ((!valueIsMaster) && specFO.CheckInputFO(valueMasterObjRef,  specID))
        slaveRealValue  = (static_cast<DO_Real*> (specFO.GetInPortData(valueMasterObjRef)))->realVal;

    valueDO.realVal = GetCurrentValue();
    valueDO.SetValueLabel(valueDO.realVal);
}

void DSC_RealMSSpec::AddPort(FuncObjC&   specFO)
{
    specFO.AddMSInPort(valueMasterObjRef, typeid(valueDO), valueIsMaster);
    specFO.AddOutPort(valueDO);
    valueDO.Reset(valueID);
}
double  DSC_RealMSSpec::GetLimitedValue(const double& minVal,
                                        const double& maxVal) const
{
    double temp = GetCurrentValue();
    if (temp < minVal)
        temp = minVal;
    if (temp > maxVal)
        temp = maxVal;
    return temp;
}

double DSC_RealMSSpec::GetCurrentValue() const
{
    if (valueIsMaster)
        return enteredRealValue;

    return slaveRealValue;
}

