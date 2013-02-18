///////////////////////////////////////////////////////////////////////////////////
//
// PPO_TableHorsetail.cpp
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

#include <objDPO/objDPOConfigFile.h>
#include <objPPO/objPPOConfigFile.h>

#include "PPO_TableHorsetailPS.h"
#include "PPO_TableHorsetail.h"

const char* PPO_TableHorsetail::objLongDesc = "Table Horsetail";

PPO_TableHorsetail :: PPO_TableHorsetail(PlotDefC* assPlt) :
    PFO_TableHorsetailGL(*assPlt),
    PPO_Base(this, objLongDesc)
{ }

PPO_TableHorsetail :: PPO_TableHorsetail(PlotDefC* assPlt, const PPO_TableHorsetail& a) :
    PFO_TableHorsetailGL (a, *assPlt),
    PPO_Base(this, objLongDesc)
{ }

PPO_TableHorsetail& PPO_TableHorsetail::operator= (const PPO_TableHorsetail& a)
{
    if (&a != this)
       *((PFO_TableHorsetail*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_TableHorsetailPS& PPO_TableHorsetail::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_TableHorsetailPS&>(*assocMenu);
}

void PPO_TableHorsetail::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void PPO_TableHorsetail::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_TableHorsetailPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_TableHorsetail::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool  PPO_TableHorsetail::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();
    ReadFuncObjRef(penSetObjRef);

    ReadTableColSel(*this);

    ReadHorsetail(*this);

    CheckForInputFailure("Reading PPO_TableHorsetail");
    return true;
}

void PPO_TableHorsetail::WriteToFile()
{
    // ver 0
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();

    WriteFuncObjRef(penSetObjRef);

    WriteTableColSel(*this);

    WriteHorsetail(*this);
}

