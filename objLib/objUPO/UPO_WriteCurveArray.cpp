///////////////////////////////////////////////////////////////////////////////////
//
// UPO_WriteCurveArray.cpp
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
//      File I/O and user-interface for objUFO.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genApp/AppConfigFile.h>

#ifndef ADCONSOLEAPP
#include "UPO_WriteCurveArrayPS.h"
#endif
#include "UPO_WriteCurveArray.h"

const char* UPO_WriteCurveArray::objLongDesc = "Write Curve Array";

UPO_WriteCurveArray :: UPO_WriteCurveArray() :
    MenuObjC(this, objLongDesc)
{
    RegisterFileName(outFname);
}

UPO_WriteCurveArray :: UPO_WriteCurveArray(const UPO_WriteCurveArray& a) :
    UFO_WriteCurveArray(a),
    MenuObjC(this, objLongDesc)
{
    RegisterFileName(outFname);
}

UPO_WriteCurveArray& UPO_WriteCurveArray::operator= (const UPO_WriteCurveArray& a)
{
    if (&a != this)
       *((UFO_WriteCurveArray*) this) = a;
    return *this;
}

bool  UPO_WriteCurveArray::ReadFromFile()
{
    ReadObjectHeader();

    ReadFileName(outFname);

    int nSel = ReadInt();
    int i;
    for (i = 0; i < nSel; i++)
    {
        curveData[i].doCurve   = true;
        ReadFuncObjRef(curveData[i].curveObjRef);
        ReadText(curveData[i].curveID, DC_CurveWithID::curveIDlen);
    }

    for (i = nSel; i < UFO_WriteCurveArray::maxCurveData; i++)
        curveData[i] = CurveData();


    CheckForInputFailure("Reading UPO_WriteCurveArray");
    return true;
}

void UPO_WriteCurveArray::WriteToFile()
{
    // ver 0
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    WriteText(outFname);

    int nSel = 0;
    int i;
    for (i = 0; i < UFO_WriteCurveArray::maxCurveData; i++)
        if (curveData[i].doCurve)
            nSel++;

    WriteInt(nSel);
    for (i = 0; i < UFO_WriteCurveArray::maxCurveData; i++)
        if (curveData[i].doCurve)
        {
            WriteFuncObjRef(curveData[i].curveObjRef);
            WriteText(curveData[i].curveID);
        }
    WriteLine();
}


#ifndef ADCONSOLEAPP

UPO_WriteCurveArrayPS& UPO_WriteCurveArray::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<UPO_WriteCurveArrayPS&>(*assocMenu);
}

void UPO_WriteCurveArray::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}

void UPO_WriteCurveArray::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new UPO_WriteCurveArrayPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void UPO_WriteCurveArray::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP




