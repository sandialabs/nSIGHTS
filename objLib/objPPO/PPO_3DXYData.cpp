///////////////////////////////////////////////////////////////////////////////////
//
// PPO_3DXYData.cpp
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
#include <objPPO/objPPOConfigFile.h>

#include "PPO_3DXYDataPS.h"
#include "PPO_3DXYData.h"

const char* PPO_3DXYData::objLongDesc = "Series from Single XY Data";

PPO_3DXYData :: PPO_3DXYData(PlotDefC* assPlt)
                   : PFO_3DXYDataGL(*assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_3DXYData :: PPO_3DXYData(PlotDefC* assPlt, const PPO_3DXYData& a)
                            : PFO_3DXYDataGL (a, *assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_3DXYData& PPO_3DXYData::operator= (const PPO_3DXYData& a)
{
    if (&a != this)
       *((PFO_3DXYData*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_3DXYDataPS& PPO_3DXYData::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_3DXYDataPS&>(*assocMenu);
}


void PPO_3DXYData::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_3DXYData::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_3DXYDataPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_3DXYData::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}




bool  PPO_3DXYData::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();
    ReadFuncObjRef(penSetObjRef);
    ReadFuncObjRef(xyDataObjRef);

    Read3DSeriesSpec(xySeries);
    ReadRealMSSpec(zValue);

    if (currentObjReadMajorVersion > 0)
        Read3DCoordMap(coordMap);
    else
        ReadInt();

    ReadLegendOverride(seriesLabel);


    CheckForInputFailure("Reading PPO_3DXYData");
    return true;
}

void PPO_3DXYData::WriteToFile()
{

    const int objMajVer = 1;        // changed plane too coord map
 //   const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();

    WriteFuncObjRef(penSetObjRef);
    WriteFuncObjRef(xyDataObjRef);

    Write3DSeriesSpec(xySeries);
    WriteRealMSSpec(zValue);
    Write3DCoordMap(coordMap);
    WriteLegendOverride(seriesLabel);

}

