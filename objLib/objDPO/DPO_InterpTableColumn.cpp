///////////////////////////////////////////////////////////////////////////////////
//
// DPO_InterpTableColumn.cpp
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <objDPO/objDPOConfigFile.h>

#include "DPO_InterpTableColumn.h"

#ifndef ADCONSOLEAPP
#include "DPO_InterpTableColumnPS.h"
#endif

static const char* objLongDesc = "Interpolate Real Value from Table";

DPO_InterpTableColumn :: DPO_InterpTableColumn() :
    MenuObjC(this, objLongDesc),
    exposedInterpXVal(*this, "Extract XY", interpXValMS.enteredRealValue)
{
}

DPO_InterpTableColumn :: DPO_InterpTableColumn(const DPO_InterpTableColumn& a) :
    DFO_InterpTableColumn(a),
    MenuObjC(this, objLongDesc),
    exposedInterpXVal(*this, "Extract XY", interpXValMS.enteredRealValue)
{
}

DPO_InterpTableColumn& DPO_InterpTableColumn::operator= (const DPO_InterpTableColumn& a)
{
    if (&a != this)
       *((DFO_InterpTableColumn*) this) = a;
    return *this;
}


bool DPO_InterpTableColumn::ReadFromFile()
{
    ReadObjectHeader();

    ReadTableColSel(*this);
    selectedYCol = ReadInt();

    logInterpX = ReadBool();
    logInterpY = ReadBool();

    ReadRealMSSpec(interpXValMS);
    exposedInterpXVal.ReadExposed();

    CheckForInputFailure("Reading DPO_InterpTableColumn");
    return true;
}

void DPO_InterpTableColumn::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteTableColSel(*this);
    WriteInt(selectedYCol);
    WriteBool(logInterpX);
    WriteBool(logInterpY);
    WriteLine();

    WriteRealMSSpec(interpXValMS);
    exposedInterpXVal.WriteExposed();

}

#ifndef ADCONSOLEAPP

DPO_InterpTableColumnPS& DPO_InterpTableColumn::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_InterpTableColumnPS&>(*assocMenu);
}

void DPO_InterpTableColumn::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_InterpTableColumnPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}

void DPO_InterpTableColumn::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void DPO_InterpTableColumn::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

bool DPO_InterpTableColumn::ContainsExposedObject()
{
    return exposedInterpXVal.IsExposed();
}

void DPO_InterpTableColumn::CalcOutput(FOcalcType  calcType)
{
    DFO_InterpTableColumn::CalcOutput(calcType);
    exposedInterpXVal.UpdateIfCreated();
    if (UpdateOK())
        MenuRef().UpdateStatus(*this);
}


#endif //!ADCONSOLEAPP

