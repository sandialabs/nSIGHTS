///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectExtendedProfile.cpp
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

#include <nsDFO/DFO_SelectExtendedProfile.h>


DFO_SelectExtendedProfile :: DFO_SelectExtendedProfile() :
    DFO_SelectBase("Select Single Profile", baseMS)
{
    InitCommon();
}

DFO_SelectExtendedProfile::DFO_SelectExtendedProfile(const DFO_SelectExtendedProfile& a) :
    DFO_SelectBase(a, baseMS)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SelectExtendedProfile::InitCommon()
{
    InitMSPorts();

    extendedProfileDO.profileResults = &extendedProfileDC;
    AddOutPort(extendedProfileDO);

    waterTableDC.SetID("Water Table");
    waterTableDO.xyData = &waterTableDC;
    AddOutPort(waterTableDO);


    AddOutPort(profileTimeDO);

    AddInPort(extendedProfileCaseObjRef, typeid(DO_ExtendedProfileCase));

    DoStatusChk();
}

DFO_SelectExtendedProfile& DFO_SelectExtendedProfile::operator= (const DFO_SelectExtendedProfile& a)
{
    if (&a != this)
    {
        DFO_SelectBase::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectExtendedProfile::LocalCopy(const DFO_SelectExtendedProfile& a)
{
    extendedProfileCaseObjRef  = a.extendedProfileCaseObjRef;
}

void  DFO_SelectExtendedProfile:: DoStatusChk()
{
    DFO_SelectBase::DoStatusChk();
    indexMS.SetTypeLabel("Selected Results");
    profileTimeDO.Reset("Profile time");

    //  check input objects
    if (!CheckInputFO(extendedProfileCaseObjRef, "Profile case"))
        return;

    extendedProfileCase = static_cast<DO_ExtendedProfileCase*>(GetInPortData(extendedProfileCaseObjRef))->profileCaseResults;

    if (IndexRangeOK(extendedProfileCase->Size()))
    {
        indexMS.indexDO.SetValueLabel("??");
    }
}

void DFO_SelectExtendedProfile:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    const ExtendedProfileTimeData& temp = (*extendedProfileCase)[indexMS.GetCurrentIndex()];

    extendedProfileDC.ExtendedProfileGeomData::operator = (*extendedProfileCase);
    extendedProfileDC.ExtendedProfileTimeData::operator = (temp);
    extendedProfileDC.UpdateTriangulation();

    profileTimeDO.realVal = temp.profileTime;
    profileTimeDO.SetValueLabel(temp.profileTime);

    if (temp.waterTablePos.IsNotEmpty())
        waterTableDC.CreateFrom(extendedProfileCase->nodeRadii, temp.waterTablePos);
    else
        waterTableDC.SetEmpty();
}

