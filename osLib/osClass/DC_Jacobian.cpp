///////////////////////////////////////////////////////////////////////////////////
//
// DC_Jacobian.cpp
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
//      optimizer results (Jacobian matrices) for a single fit.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <osClass/DC_Jacobian.h>

DC_Jacobian::DC_Jacobian() :
    derivSpan(0.0)
{
    SetToNull(runID);
    SetToNull(caseID);
    SetToNull(fitID);
}

DC_Jacobian::DC_Jacobian(const DC_Jacobian& a)
{
    LocalCopy(a);
}

DC_Jacobian&  DC_Jacobian::operator=(const DC_Jacobian& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}

void DC_Jacobian::LocalCopy(const DC_Jacobian& a)
{
    CopyString(runID, a.runID, idLen);
    CopyString(caseID, a.caseID, idLen);
    CopyString(fitID, a.fitID, idLen);

    jacobData = a.jacobData;
    derivSpan = a.derivSpan;
    fitDesc   = a.fitDesc;
    parDesc   = a.parDesc;
}

bool DC_Jacobian::ExtractFrom(const OptSimRunResults&  optRes,
                                    int                caseIndex,
                                    int                fitIndex)
{
    CopyString(runID, optRes.runID, idLen);
    CopyString(caseID, optRes.runResults[caseIndex].caseDesc, idLen);
    CopyString(fitID, optRes.fitDescriptions[fitIndex].fitID, idLen);

    const OptSingleFit& selFit = optRes.runResults[caseIndex].caseResults[fitIndex];
    if (!selFit.fitCompletedOK)
        return false;

    derivSpan = selFit.derivSpan;
    parDesc = optRes.varDescriptions;
    fitDesc = optRes.fitDescriptions[fitIndex];

    int nfitComp = selFit.fitComponents.Size();

    jacobData.AllocAndSetSize(nfitComp);
    for (int i = 0; i < nfitComp; i++)
        jacobData[i] = selFit.fitComponents[i].fitJacobian;

    return true;
}

