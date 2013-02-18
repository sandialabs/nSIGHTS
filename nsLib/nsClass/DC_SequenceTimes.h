///////////////////////////////////////////////////////////////////////////////////
//
// DC_SequenceTimes.h
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

#ifndef DC_SEQUENCETIMES_H
#define DC_SEQUENCETIMES_H

#include <genClass/SC_StringArray.h>
#include <genClass/SC_DoubleArray.h>

class DC_SequenceTimes {
    public:
        //  constructed as resizable and allocd at 10
        SC_StringArray          sequenceIDs;         // ID of sequence
        SC_DoubleArray          sequenceStartTimes;  // start time of sequence
                                           // sequenceStartTimes.Size() == sequenceIDs.Size() + 1
                                           // sequenceStartTimes.LastIndex() == end time of last sequence
    public:
                                DC_SequenceTimes();
                                DC_SequenceTimes(const DC_SequenceTimes& a);

                                ~DC_SequenceTimes();
        DC_SequenceTimes&       operator= (const DC_SequenceTimes& a);

        void                    Alloc(int nSequence);
        void                    Clear();
        int         Size()      const {return sequenceIDs.Size();}
        bool        IsEmpty()   const {return sequenceIDs.IsEmpty();}

        void                    AddSequence(const char*     seqID,
                                            const double&   seqStartTime);
        void                    AddEndTime (const double&   lastSeqEndTime);


    private:
        void                    LocalCopy(const DC_SequenceTimes& a);  // implementation of copy

};


#endif // DC_SEQUENCETIMES_H

