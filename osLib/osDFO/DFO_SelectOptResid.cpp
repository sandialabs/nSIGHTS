///////////////////////////////////////////////////////////////////////////////////
//
// DFO_SelectOptResid.cpp
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
//      selects one or more residual results from optimizer results.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/SC_DoubleArray.h>

#include <osDFO/DFO_SelectOptResid.h>


DFO_SelectOptResid :: DFO_SelectOptResid() : DFO_SelectOpt("SelectOptResid", true, false)
{
    InitCommon();
}

DFO_SelectOptResid::DFO_SelectOptResid(const DFO_SelectOptResid& a) : DFO_SelectOpt(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_SelectOptResid::InitCommon()
{
    residDO.xyData = &xyData;
    AddOutPort(residDO);

    includeSubFits = true;

    DoStatusChk();
}

DFO_SelectOptResid& DFO_SelectOptResid::operator= (const DFO_SelectOptResid& a)
{
    if (&a != this)
    {
        DFO_SelectOpt::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_SelectOptResid::LocalCopy(const DFO_SelectOptResid& a)
{
}

void  DFO_SelectOptResid:: DoStatusChk()
{
    DFO_SelectOpt::DoStatusChk();
    indexMS.indexDO.SetTypeLabel("Selected Residuals");

    if (StatusNotOK())
        return;

    // make sure that file contains residuals
    if (!optSimResultsDC->containsResiduals)
        SetObjErrMsg("no residuals in input");
}

void DFO_SelectOptResid:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    SC_DoubleArray& residX =
        (*optSimResultsDC)[runIndex[0]].
                fitDescriptions[fitIndex[0]].
                        fitComponentDescription[subfitIndex[0]].fitResidJacobX;

    SC_DoubleArray& residY =
        (*optSimResultsDC)[runIndex[0]].
                runResults[caseIndex[0]].
                        caseResults[fitIndex[0]].
                            fitComponents[subfitIndex[0]].fitResiduals;

    residDO.noptPararameters = optSimResultsDC->noptVar;

    if (!xyData.CreateFrom(residX, residY))
    {
        SetObjErrMsg("all input null or memory allocation ??");
        return;
    }

    CopyString(xyData.dataID, "Residuals", DC_XYData::dataIDLen);
    SetDefaultID(xyData.dataID);


}

