///////////////////////////////////////////////////////////////////////////////////
//
// PPO_EnterXYOnPlot.cpp
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
//      File I/O and user interface for all objPFOGL objects
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genPPO/genPPOConfigFile.h>

#include "PPO_EnterXYOnPlotPS.h"
#include "PPO_EnterXYOnPlot.h"

static const char* objLongDesc = "Enter/Edit XY on Plot";

PPO_EnterXYOnPlot :: PPO_EnterXYOnPlot(PlotDefC* assPlt) :
    PFO_EnterXYOnPlotGL(*assPlt),
    PPO_Base(this, objLongDesc)
{
}

PPO_EnterXYOnPlot :: PPO_EnterXYOnPlot(PlotDefC* assPlt, const PPO_EnterXYOnPlot& a) :
    PFO_EnterXYOnPlotGL(a, *assPlt),
    PPO_Base(this, objLongDesc)
{
}

PPO_EnterXYOnPlot& PPO_EnterXYOnPlot::operator= (const PPO_EnterXYOnPlot& a)
{
    if (&a != this)
       *((PPO_EnterXYOnPlot*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_EnterXYOnPlotPS& PPO_EnterXYOnPlot::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_EnterXYOnPlotPS&>(*assocMenu);
}


void PPO_EnterXYOnPlot::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_EnterXYOnPlot::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_EnterXYOnPlotPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_EnterXYOnPlot::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  PPO_EnterXYOnPlot::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();

    ReadFuncObjRef(inputXYObjRef);
    ReadSymbolSpec(currentPtSymbol);
    ReadSymbolSpec(enteredPtSymbol);
    ReadSymbolSpec(deletedPtSymbol);
    currLinePen = ReadInt();

    int nXYRead = ReadInt();
    xyDataDC.Alloc(nXYRead);
    pointStatus.Alloc(nXYRead);
    for ( int i = 0; i < nXYRead; i++)
    {
        xyDataDC.xData[i] = ReadDouble();
        xyDataDC.yData[i] = ReadDouble();
        pointStatus[i] = PointStatusType(ReadInt());
    }
    xyDataDC.SetSize(nXYRead);
    pointStatus.SetSize(nXYRead);

    if (!xyDataDC.IsEmpty())
    {
        lastSelectedXDO.InitLabelAndValue(xyDataDC.xData.LastIndex());
        lastSelectedYDO.InitLabelAndValue(xyDataDC.yData.LastIndex());
    }

    CheckForInputFailure("Reading PPO_EnterXYOnPlot");
    return true;
}


void PPO_EnterXYOnPlot::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();

    WriteFuncObjRef(inputXYObjRef);
    WriteSymbolSpec(currentPtSymbol);
    WriteSymbolSpec(enteredPtSymbol);
    WriteSymbolSpec(deletedPtSymbol);
    WriteInt(currLinePen);

    WriteInt(xyDataDC.Size());
    WriteLine();
    SetRealConvFormat(SC_SciFormat(9));

    for (int i = 0; i < xyDataDC.Size(); i++)
    {
        WriteDouble(xyDataDC.xData[i]);
        WriteDouble(xyDataDC.yData[i]);
        WriteInt(int(pointStatus[i]));
        WriteLine();
    }

    WriteLine();
}

