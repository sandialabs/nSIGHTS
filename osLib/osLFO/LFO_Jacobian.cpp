///////////////////////////////////////////////////////////////////////////////////
//
// LFO_Jacobian.cpp
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
//      list Jacobian results.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <osClass/DO_Jacobian.h>

#include <osLFO/LFO_Jacobian.h>


LFO_Jacobian :: LFO_Jacobian(ListDefC& assLst)
    : LFO_OptBase("Jacobian", assLst)
{
    InitCommon();
}

LFO_Jacobian::LFO_Jacobian(const LFO_Jacobian& a, ListDefC& assLst) : LFO_OptBase(a, assLst)
{
    LocalCopy(a);
    InitCommon();
}

LFO_Jacobian :: ~LFO_Jacobian ()
{
}

void LFO_Jacobian::InitCommon()
{
    AddInPort(jacobObjRef, typeid(DO_Jacobian));
    DoStatusChk();
}

LFO_Jacobian& LFO_Jacobian::operator= (const LFO_Jacobian& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}


void LFO_Jacobian::LocalCopy(const LFO_Jacobian& a)
{
}


void LFO_Jacobian::DoStatusChk()
{
    // base class check
    FuncObjC::DoStatusChk();
    CheckInputFO(jacobObjRef, "Jacobian");
}


void LFO_Jacobian::CreateListing()
{
    DC_Jacobian& jacData = *(static_cast<DO_Jacobian*>(GetInPortData(jacobObjRef))->jacobianData);

    char titleStr[80];
    MakeString(titleStr, GetID(), ": Jacobian: ", jacData.runID, "/", jacData.caseID, "/", jacData.fitID, 80);
    TextStdTitle(titleStr);

    ListJacobian(jacData);
}


