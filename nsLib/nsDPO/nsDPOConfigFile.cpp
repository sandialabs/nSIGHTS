///////////////////////////////////////////////////////////////////////////////////
//
// nsDPOConfigFile.cpp
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

#include "nsLibAfx.h"

#include "nsDPOConfigFile.h"

namespace nsDPOConfigFile {


    void  ReadTimeLimits(DSC_TimeLimits& inLimits)
    {
        int readVer = ReadInt();

        ReadFuncObjRef(inLimits.seqTimesObjRef);
        inLimits.timeLimitSelection = DSC_TimeLimits::TimeLimitSelection(ReadInt());
        inLimits.startSeq  = ReadInt();
        inLimits.endSeq    = ReadInt();
        inLimits.startVal  = ReadDouble();
        inLimits.endVal    = ReadDouble();
    }


    void  WriteTimeLimits(const DSC_TimeLimits& outLimits)
    {
        static const int tlVer = 0;
        WriteInt(tlVer);

        WriteFuncObjRef(outLimits.seqTimesObjRef);

        WriteInt(int(outLimits.timeLimitSelection));
        WriteInt(outLimits.startSeq);
        WriteInt(outLimits.endSeq);
        WriteLine();

        WriteDouble(outLimits.startVal);
        WriteDouble(outLimits.endVal);
        WriteLine();
    }


};

