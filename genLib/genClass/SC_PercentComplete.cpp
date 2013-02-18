///////////////////////////////////////////////////////////////////////////////////
//
// SC_PercentComplete.cpp
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

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>


#include <genClass/SC_PercentComplete.h>



SC_PercentComplete::SC_PercentComplete(const char* id, int maxOp, int opModulus, int minOpForDisplay)
    : msgID(id), displayModulus(opModulus)
{
    lastDisplayed = -1;
    doUpdate = maxOp > minOpForDisplay;
    pctMultiplier = 100.0 / double(maxOp);
}

SC_PercentComplete::SC_PercentComplete(const char* id, int maxOp)
    : msgID(id), displayModulus(1)
{
    lastDisplayed = -1;
    doUpdate = true;
    pctMultiplier = 100.0 / double(maxOp);
}

SC_PercentComplete::~SC_PercentComplete()
{
    if (doUpdate)
        GenAppInfoMsg(msgID, " calculation done");
}

void SC_PercentComplete::Update(int currOp)
{
    if (!doUpdate)
        return;

    int currPct = int(double(currOp) * pctMultiplier);

    if (((currPct % displayModulus) == 0) && (currPct > lastDisplayed))
        {
            lastDisplayed = currPct;
            char msgStr[80];
            IntToString(currPct, msgStr, 80);
            ConcatString(msgStr, "% complete", 80);
            GenAppInfoMsg(msgID, msgStr);
        }
}

