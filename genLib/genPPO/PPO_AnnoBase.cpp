///////////////////////////////////////////////////////////////////////////////////
//
// PPO_AnnoBase.cpp
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

#include "genPPOConfigFile.h"
#include <genApp\C_MenuPage.h>
#include <genApp\C_PlotPage.h>

#include "PPO_AnnoBase.h"

PPO_AnnoBase :: PPO_AnnoBase(AnnoObjC* bPO, const char* longDesc) :
    MenuObjC(bPO, longDesc),
    basePO(*bPO)
{
}


bool  PPO_AnnoBase::ReadFromFile()
{
    int baseReadMaj = ReadInt();
    int baseReadMin = ReadInt();

    basePO.doPlot = ReadBool();
    if (baseReadMaj < 3)
    {
        ReadInt();  // plot layer
        if (baseReadMaj > 0)
            ReadDouble(); //poly offset
    }
    if (baseReadMaj > 3)
        basePO.transGroup = ReadInt();

    return true;
}

void PPO_AnnoBase::WriteToFile()
{
    const int objMajVer = 4;              // transparency
//    const int objMajVer = 3;            // first ver as anno base
//    const int objMajVer = 2;            // added offset coord
//    const int objMajVer = 1;            // added polyOffset
    const int objMinVer = 0;

    WriteInt(objMajVer);
    WriteInt(objMinVer);
    WriteLine();

    WriteBool(basePO.doPlot);
    WriteInt(basePO.transGroup);  // added v4
    WriteLine();

}


void PPO_AnnoBase::ChangePage(MenuPageC& currParent,
                               MenuPageC& newParent)
{
    // must be OK
    const PlotPageC& currPlotPage = dynamic_cast<const PlotPageC&>(*currParent.GetActualParent());
    const PlotPageC& newPlotPage = dynamic_cast<const PlotPageC&>(*newParent.GetActualParent());

    PlotDefC* newPlot = newPlotPage.GetPlot();

    basePO.assocPlot = newPlot;
    basePO.penSetObjRef.objRef = newPlot;
    newPlot->annoObjList.AddTo(&basePO);

    PlotDefC* currPlot = currPlotPage.GetPlot();
    currPlot->annoObjList.DeleteFrom(&basePO);
}

