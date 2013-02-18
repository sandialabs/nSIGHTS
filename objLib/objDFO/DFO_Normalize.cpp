///////////////////////////////////////////////////////////////////////////////////
//
// DFO_Normalize.cpp
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
#include <objClass/DO_Normalize.h>

#include <objDFO/DFO_Normalize.h>


DFO_Normalize :: DFO_Normalize(const char* inID) :
    FuncObjC(inID),
    normPowerMS("Power")
{
    isMaster = true;
    normPowerMS.enteredRealValue = normalizeDC.normPower;
    InitCommon();
}

DFO_Normalize::DFO_Normalize(const DFO_Normalize& a) :
    FuncObjC(a),
    normPowerMS(a.normPowerMS)
{
    LocalCopy(a);
    InitCommon();
}

DFO_Normalize :: ~DFO_Normalize()
{
}

void DFO_Normalize::InitCommon()
{
    AddMSInPort(masterObjRef, typeid(DO_Normalize), isMaster);

    normPowerMS.AddPort(*this);
    powerCheckRef = GetLastInPortCheckRef();

    AddOutPort(normalizeDO);
    normalizeDO.normalizeDC = &normalizeDC;

    DoStatusChk();
}

DFO_Normalize& DFO_Normalize::operator= (const DFO_Normalize& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        LocalCopy(a);
    }
    return *this;
}

void  DFO_Normalize::LocalCopy(const DFO_Normalize& a)
{
    isMaster        = a.isMaster;
    masterObjRef    = a.masterObjRef;
    normalizeDC     = a.normalizeDC;
    normPowerMS     = a.normPowerMS;
}


void  DFO_Normalize:: DoStatusChk()
{
    // set flags for data to use
    *powerCheckRef =isMaster && (normalizeDC.normalizeOp != DC_Normalize::noLimit);
    FuncObjC::DoStatusChk();

    if ((!isMaster) && (CheckInputFO(masterObjRef, "Master spec")))
    {
        DO_Normalize*  inputDO = static_cast<DO_Normalize*>(GetInPortData(masterObjRef));
        normalizeDC = *(inputDO->normalizeDC);
    }

    if (*powerCheckRef)
    {
        normPowerMS.DoCheck("power value", *this);
        normalizeDC.normPower = normPowerMS.GetLimitedValue(-10.0, 10.0);
    }
}


