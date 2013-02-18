///////////////////////////////////////////////////////////////////////////////////
//
// PPO_TableLabels.cpp
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

#include "PPO_TableLabelsPS.h"
#include "PPO_TableLabels.h"

const char* PPO_TableLabels::objLongDesc = "Table Labels";

PPO_TableLabels :: PPO_TableLabels(PlotDefC* assPlt)
                   : PFO_TableLabelsGL(*assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_TableLabels :: PPO_TableLabels(PlotDefC* assPlt, const PPO_TableLabels& a)
                            : PFO_TableLabelsGL (a, *assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_TableLabels& PPO_TableLabels::operator= (const PPO_TableLabels& a)
{
    if (&a != this)
       *((PFO_TableLabels*) this) = a;
    return *this;
}

// ************************* menu ops

PPO_TableLabelsPS& PPO_TableLabels::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_TableLabelsPS&>(*assocMenu);
}


void PPO_TableLabels::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_TableLabels::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_TableLabelsPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_TableLabels::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


bool  PPO_TableLabels::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();

    ReadFuncObjRef(tableObjRef);
    matrixPlotOption = MatrixPlotOption(ReadInt());
    ReadXYLabelSpec(*this);
    if (currentObjReadMajorVersion > 0)
        ReadRealFormat(convFormat);

    CheckForInputFailure("Reading PPO_TableLabels");
    return true;
}

void PPO_TableLabels::WriteToFile()
{
    const int objMajVer = 1;  // add conv
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();

    WriteFuncObjRef(tableObjRef);
    WriteInt(int(matrixPlotOption));
    WriteLine();

    WriteXYLabelSpec(*this);

    WriteRealFormat(convFormat);

}

