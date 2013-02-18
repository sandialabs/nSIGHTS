///////////////////////////////////////////////////////////////////////////////////
//
// C_TimeProcess.cpp
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
//      Definition of data required for Horner/Argawal/Bourdet time processing.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_Common.h>

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <nsClass/C_TimeProcess.h>

TimeProcessDescription::TimeProcessDescription()
{
    timeProcessType     = TimeProcess::tpHorner;
    hornerTval          = 1.0;
    argawalTpVal        = 1.0;
    addReplaceLastQ     = false;
    newLastQ            = 0.0;
}

TimeProcessDescription::TimeProcessDescription(const TimeProcessDescription& a)
{
    LocalCopy(a);
}

TimeProcessDescription::~TimeProcessDescription()
{
    // empty for now
}

TimeProcessDescription& TimeProcessDescription::operator= (const TimeProcessDescription& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

bool  TimeProcessDescription::TimeProcessSetupOK(SC_SetupErr& errData)
{
    if ((timeProcessType == TimeProcess::tpHorner) && (hornerTval < stdEps))
        errData.SetInternalError("Horner t must be +ve non-zero");

    if ((timeProcessType == TimeProcess::tpArgawal) && (argawalTpVal < stdEps))
        errData.SetInternalError("Argawal t must be +ve non-zero");

    return errData.NoErrors();
}


void  TimeProcessDescription::LocalCopy(const TimeProcessDescription& a)
{
    timeProcessType     = a.timeProcessType;
    hornerTval          = a.hornerTval;
    argawalTpVal        = a.argawalTpVal;
    addReplaceLastQ     = a.addReplaceLastQ;
    newLastQ            = a.newLastQ;

}

