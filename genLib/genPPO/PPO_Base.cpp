///////////////////////////////////////////////////////////////////////////////////
//
// PPO_Base.cpp
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
//      File I/O and user interface for all genPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genPlotClass/PC_Report.h>
#include "genPPOConfigFile.h"

#include "PPO_Base.h"

PPO_Base :: PPO_Base(PlotObjC* bPO, const char* longDesc) :
    MenuObjC(bPO, longDesc),
    basePO(*bPO)
{
    // most plot objects can't be dragged
    objectCanChangePages = false;
}


bool  PPO_Base::ReadFromFile()
{
    int baseReadMaj = ReadInt();
    int baseReadMin = ReadInt();

    basePO.doPlot = ReadBool();
    basePO.plotLayer = ReadInt();
    if (baseReadMaj > 3)
        basePO.transGroup = ReadInt();

    if (baseReadMaj > 0)
        basePO.polyOffset = ReadDouble();

    // v3 report handling
    bool readReport = basePO.reportOK;
    if (baseReadMaj > 2)
        readReport = ReadBool();
    if (readReport)
    {
        bool showRep = ReadBool();
        if (basePO.reportOK)
            basePO.cursorReport->showReport = showRep;
    }

    if (basePO.activeOK)
        basePO.isActive = ReadBool();

    if ((baseReadMaj > 2) ||(basePO.is3DObject && (baseReadMaj ==2)))
        ReadCoord(basePO.offsetCoord);

    return true;
}

void PPO_Base::WriteToFile()
{
    const int objMajVer = 4;              // transparency group
//    const int objMajVer = 3;            // offset coord for 2D & 3D to allow copy/paste
                                          // also added handling for reportOK
//    const int objMajVer = 2;            // added offset coord
//    const int objMajVer = 1;            // added polyOffset
    const int objMinVer = 0;

    WriteInt(objMajVer);
    WriteInt(objMinVer);
    WriteLine();

    WriteBool(basePO.doPlot);
    WriteInt(basePO.plotLayer);
    WriteInt(basePO.transGroup);  // added v4
    WriteDouble(basePO.polyOffset);

    WriteBool(basePO.reportOK);  // added v3
    if (basePO.reportOK)
    {
        WriteBool(basePO.cursorReport->showReport);
    }

    if (basePO.activeOK)
        WriteBool(basePO.isActive);
    WriteLine();

//    if (basePO.is3DObject) // deleted v3
    {
        SetRealConvFormat(SC_RealFormat(ncf_Decimal, 3));
        WriteCoord(basePO.offsetCoord);
    }

}


