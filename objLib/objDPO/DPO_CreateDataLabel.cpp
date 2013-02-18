///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CreateDataLabel.cpp
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

#include <genApp/AppConfigFile.h>

#include "DPO_CreateDataLabel.h"

#ifndef ADCONSOLEAPP
#include "DPO_CreateDataLabelPS.h"
#endif

const char* DPO_CreateDataLabel::objLongDesc = "Create Data Labels";

DPO_CreateDataLabel :: DPO_CreateDataLabel() : MenuObjC(this, objLongDesc)
{
}

DPO_CreateDataLabel :: DPO_CreateDataLabel(const DPO_CreateDataLabel& a) :
    DFO_CreateDataLabel(a),
    MenuObjC(this, objLongDesc)
{
}


DPO_CreateDataLabel& DPO_CreateDataLabel::operator= (const DPO_CreateDataLabel& a)
{
    if (&a != this)
       *((DFO_CreateDataLabel*) this) = a;
    return *this;
}


bool  DPO_CreateDataLabel::ReadFromFile()
{
    ReadObjectHeader();

    int nRead = maxLabelData;
    if (currentObjReadMajorVersion == 0)
        nRead = 1;

    for (int i = 0; i < nRead; i++)
    {
        ReadText(labelData[i].dataTypeLabel, DO_Label::maxLabelLen);
        ReadText(labelData[i].dataValueLabel, DO_Label::maxLabelLen);
    }

    CheckForInputFailure("Reading DPO_CreateDataLabel");
    return true;
}


void DPO_CreateDataLabel::WriteToFile()
{
    const int objMajVer = 1;  // added multiple labels
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    for (int i = 0; i < maxLabelData; i++)
    {
        WriteText(labelData[i].dataTypeLabel);
        WriteText(labelData[i].dataValueLabel);
    }

}

#ifndef ADCONSOLEAPP

DPO_CreateDataLabelPS& DPO_CreateDataLabel::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_CreateDataLabelPS&>(*assocMenu);
}


void DPO_CreateDataLabel::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_CreateDataLabel::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_CreateDataLabelPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(true, false);

    PropUpdate();
}


void DPO_CreateDataLabel::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}


#endif //!ADCONSOLEAPP

