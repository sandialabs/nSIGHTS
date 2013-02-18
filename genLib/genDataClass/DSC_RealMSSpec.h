///////////////////////////////////////////////////////////////////////////////////
//
// DSC_RealMSSpec.h
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

#ifndef DSC_REALMSSPEC_H
#define DSC_REALMSSPEC_H

#include <genClass/C_FuncObj.h>
#include <genClass/DO_Real.h>

class DSC_RealMSSpec {
public:
    bool            valueIsMaster;
    FuncObjRef      valueMasterObjRef;
    double          enteredRealValue;

    DO_Real&        valueDO;

    const char*     valueID;

private:
    double          slaveRealValue;

public:
    DSC_RealMSSpec(const char* valID, DO_Real& valueDO);
    ~DSC_RealMSSpec() {};

    DSC_RealMSSpec&  operator= (const DSC_RealMSSpec& a);

    void            DoCheck(const char* specID,
                            FuncObjC&   specFO);

    void            AddPort(FuncObjC&   specFO);

    double          GetLimitedValue(const double& minVal,
                                    const double& maxVal) const;
    double          GetCurrentValue() const;

    void            Clear();

    //  easier access to valueDO
    void            SetTypeLabel(const char* typeLabel)     {valueDO.SetTypeLabel(typeLabel);}
    void            Reset(const char* typeLabel)            {valueDO.Reset(typeLabel);}
    void            InitLabelAndValue(const double& inVal)  {valueDO.InitLabelAndValue(inVal);}

private:
    void            LocalCopy(const DSC_RealMSSpec& a);

};

#endif // DSC_REALMSSPEC_H

