///////////////////////////////////////////////////////////////////////////////////
//
// PPO_CovarLimits.cpp
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
//      File I/O and user interface for all osPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genPPO/genPPOConfigFile.h>

#include "PPO_CovarLimitsPS.h"
#include "PPO_CovarLimits.h"

const char* PPO_CovarLimits::objLongDesc = "Covariance Matrix Confidence Limits - Two Parameter";

PPO_CovarLimits :: PPO_CovarLimits(PlotDefC* assPlt)
                   : PFO_CovarLimitsGL(*assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_CovarLimits :: PPO_CovarLimits(PlotDefC* assPlt, const PPO_CovarLimits& a)
                            : PFO_CovarLimitsGL (a, *assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_CovarLimits& PPO_CovarLimits::operator= (const PPO_CovarLimits& a)
{
    if (&a != this)
       *((PFO_CovarLimits*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_CovarLimitsPS& PPO_CovarLimits::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_CovarLimitsPS&>(*assocMenu);
}


void PPO_CovarLimits::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_CovarLimits::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_CovarLimitsPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_CovarLimits::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


bool  PPO_CovarLimits::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();
    ReadFuncObjRef(penSetObjRef);

    ReadFuncObjRef(covarArrayObjRef);
    xDataIndx = ReadInt();
    yDataIndx = ReadInt();
    zDataIndx = ReadInt();

    covarPlotType   = CovarPlotType(ReadInt());
    covarPlotFormat = CovarPlotFormat(ReadInt());
    cLimits         = DC_Covar::ConfidenceLimits(ReadInt());

    covarPen        = ReadInt();
    covarLineThk    = ReadInt();

    ReadTubeExtrusionSpec(tubeData);

    ndualPts        = ReadInt();
    ntripleSlice    = ReadInt();
    ntripleMeridian = ReadInt();

    ReadLegendOverride(legendOverride);

    CheckForInputFailure("Reading PPO_CovarLimits");
    return true;
}

void PPO_CovarLimits::WriteToFile()
{
    // ver 0
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();

    WriteFuncObjRef(penSetObjRef);
    WriteFuncObjRef(covarArrayObjRef);
    WriteInt(xDataIndx);
    WriteInt(yDataIndx);
    WriteInt(zDataIndx);
    WriteLine();

    WriteInt(int(covarPlotType));
    WriteInt(int(covarPlotFormat));
    WriteInt(int(cLimits));
    WriteLine();

    WriteInt(covarPen);
    WriteInt(covarLineThk);
    WriteLine();

    WriteTubeExtrusionSpec(tubeData);

    WriteInt(ndualPts);
    WriteInt(ntripleSlice);
    WriteInt(ntripleMeridian);
    WriteLine();

    WriteLegendOverride(legendOverride);

}

