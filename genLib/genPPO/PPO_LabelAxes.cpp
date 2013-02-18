///////////////////////////////////////////////////////////////////////////////////
//
// PPO_LabelAxes.cpp
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

#include <genPPO/genPPOConfigFile.h>

#include "PPO_LabelAxesPS.h"
#include "PPO_LabelAxes.h"

const char* PPO_LabelAxes::objLongDesc = "Label Axis";

PPO_LabelAxes :: PPO_LabelAxes(PlotDefC* assPlt)
     : PFO_LabelAxesGL(*assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_LabelAxes :: PPO_LabelAxes(PlotDefC* assPlt, const PPO_LabelAxes& a)
                            : PFO_LabelAxesGL (a, *assPlt), PPO_Base(this, objLongDesc)
{
}

PPO_LabelAxes& PPO_LabelAxes::operator= (const PPO_LabelAxes& a)
{
    if (&a != this)
        PFO_LabelAxesGL::operator=(a);
    return *this;
}

// ************************* menu ops

PPO_LabelAxesPS& PPO_LabelAxes::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<PPO_LabelAxesPS&>(*assocMenu);
}


void PPO_LabelAxes::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void PPO_LabelAxes::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new PPO_LabelAxesPS();
    MenuRef().InitMenu(*this);
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void PPO_LabelAxes::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


bool  PPO_LabelAxes::ReadFromFile()
{
    ReadObjectHeader();
    PPO_Base::ReadFromFile();
    ReadFuncObjRef(penSetObjRef);

    ReadFuncObjRef(labelArrayObjRef);
    if (currentObjReadMajorVersion > 1)
    {
        ReadFuncObjRef(tableObjRef);
        labelSource = LabelSource(ReadInt());   // v2
        skipCol0 = ReadBool();  // v2
    }

    isXAxis = ReadBool();

    if (currentObjReadMajorVersion > 0)
    {
        limTxtLen = ReadBool();
        maxTxtLen = ReadInt();
    }

    ReadText(axesLabel, maxLabelLen);

    CheckForInputFailure("Reading PPO_LabelAxes");
    return true;
}

void PPO_LabelAxes::WriteToFile()
{
    const int objMajVer = 2;  //added table row/col desc
//  const int objMajVer = 1;  //added increment label length
//  const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    PPO_Base::WriteToFile();

    WriteFuncObjRef(penSetObjRef);

    WriteFuncObjRef(labelArrayObjRef);
    WriteFuncObjRef(tableObjRef); // v2

    WriteInt(labelSource);  // v2
    WriteBool(skipCol0);    // v2
    WriteBool(isXAxis);
    WriteLine();

    WriteBool(limTxtLen);
    WriteInt(maxTxtLen);
    WriteLine();

    WriteText(axesLabel);

}

