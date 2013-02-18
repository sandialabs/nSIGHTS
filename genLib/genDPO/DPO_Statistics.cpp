///////////////////////////////////////////////////////////////////////////////////
//
// DPO_Statistics.cpp
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
// DESCRIPTION: File I/O and user interface for all genDFO functional objects.
//              All code is platform independent, but relies upon platform dependent
//              code in genApp and genClassPS.
//              File I/O for DFO_XXXX objects.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#ifndef ADCONSOLEAPP
#include "DPO_StatisticsPS.h"
#endif

#include <genApp/AppConfigFile.h>

#include "DPO_Statistics.h"

DPO_Statistics :: DPO_Statistics(DFO_Statistics* bPO, const char* longDesc) :
    MenuObjC(bPO, longDesc),
    bpRef(*bPO)
{
}

bool  DPO_Statistics::ReadFromFile(FuncObjRef& dataInputObjRef)
{
    int stVer = ReadInt();
    if (stVer == 0)
        ReadFuncObjRef(dataInputObjRef);
    bpRef.value1Op = DFO_Statistics::StatisticOp(ReadInt());
    bpRef.value2Op = DFO_Statistics::StatisticOp(ReadInt());
    bpRef.value3Op = DFO_Statistics::StatisticOp(ReadInt());
    bpRef.value4Op = DFO_Statistics::StatisticOp(ReadInt());

    if (stVer > 1)
        {
            bpRef.calcModeAndMedian = ReadBool();
            bpRef.calcLogData = ReadBool();
            if (stVer > 2)
                {
                    bpRef.useLog10 = ReadBool();
                    if (stVer > 3)
                        bpRef.presentTransformed = ReadBool();
                }
        }

    CheckForInputFailure("Reading DPO_Statistics");
    return true;
}

void DPO_Statistics::WriteToFile()
{
    const int stVer = 4;      // presentTransformed
    //    const int stVer = 3;    // added useLog10
    //    const int stVer = 2;    // added optional mode/med calc
    //    const int stVer = 1;   // removed input obj ref
    //    const int stVer = 0;
    WriteInt(stVer);

    WriteInt(int(bpRef.value1Op));
    WriteInt(int(bpRef.value2Op));
    WriteInt(int(bpRef.value3Op));
    WriteInt(int(bpRef.value4Op));

    // added v2
    WriteBool(bpRef.calcModeAndMedian);
    WriteBool(bpRef.calcLogData);

    // added v3
    WriteBool(bpRef.useLog10);

    // added v4
    WriteBool(bpRef.presentTransformed);

    WriteLine();
}

