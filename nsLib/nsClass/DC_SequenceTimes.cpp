///////////////////////////////////////////////////////////////////////////////////
//
// DC_SequenceTimes.cpp
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
//      Sequence names and start/end times
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <nsClass/DC_SequenceTimes.h>


DC_SequenceTimes::DC_SequenceTimes()
{
    sequenceIDs.SetResizable();
    sequenceIDs.SetStringLen(80);
    Alloc(10);
    sequenceStartTimes.SetResizable();
}

DC_SequenceTimes::DC_SequenceTimes(const DC_SequenceTimes& a) :
    sequenceIDs(a.sequenceIDs),
    sequenceStartTimes(a.sequenceStartTimes)
{
}

DC_SequenceTimes::~DC_SequenceTimes()
{
    // empty
}

DC_SequenceTimes& DC_SequenceTimes::operator= (const DC_SequenceTimes& a)
{
    if (&a != this)
    {
        LocalCopy(a);
    }
    return *this;
}


void  DC_SequenceTimes::LocalCopy(const DC_SequenceTimes& a)
{
    sequenceIDs         = a.sequenceIDs;
    sequenceStartTimes  = a.sequenceStartTimes;
}

void DC_SequenceTimes::Alloc(int nSequence)
{
    sequenceIDs.Alloc(nSequence);
    sequenceStartTimes.Alloc(nSequence + 1);
}
void DC_SequenceTimes::Clear()
{
    sequenceIDs.SetEmpty();
    sequenceStartTimes.SetEmpty();
}

void  DC_SequenceTimes::AddSequence(const char*     seqID,
                                    const double&   seqStartTime)
{
    sequenceIDs += seqID;
    sequenceStartTimes += seqStartTime;
}

void DC_SequenceTimes::AddEndTime (const double&    lastSeqEndTime)
{
    sequenceStartTimes += lastSeqEndTime;
}

