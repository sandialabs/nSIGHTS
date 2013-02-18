///////////////////////////////////////////////////////////////////////////////////
//
// DPO_PenSet.cpp
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
// DESCRIPTION: File I/O and user interface for all genDFO functional objects.
//              All code is platform independent, but relies upon platform dependent
//              code in genApp and genClassPS.
//              File I/O for DFO_XXXX objects.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#ifndef ADCONSOLEAPP
#include "DPO_PenSetPS.h"
#endif

#include <genClass/U_Str.h>

#include <genApp/AppConfigFile.h>

#include "DPO_PenSet.h"

// **************************************** constructor/destructor
const char* DPO_PenSet::objLongDesc = "Create Pen Set";

DPO_PenSet :: DPO_PenSet() :
    DFO_PenSet(),
    MenuObjC(this, objLongDesc)
{
    useHSVspec  = false;
}

DPO_PenSet :: DPO_PenSet(const DPO_PenSet& a) :
    DFO_PenSet(a),
    MenuObjC(this, objLongDesc)
{
    useHSVspec = a.useHSVspec;
}

DPO_PenSet& DPO_PenSet::operator= (const DPO_PenSet& a)
{
    if (&a != this)
        {
            *((DFO_PenSet*) this) = a;
            useHSVspec = a.useHSVspec;
        }
    return *this;
}



bool  DPO_PenSet::ReadFromFile()
{
    ReadObjectHeader();

    useHSVspec     = ReadBool();
    if (currentObjReadMajorVersion < 2)
        {
            bool useScreenPens  = ReadBool();
            int npenread = 12;
            if (currentObjReadMajorVersion == 0)
                npenread = 8;

            for ( int i = 0; i < npenread; i++)
                {
                    ReadText(penSet.penLabel[i], stdInputStrLen);
                    // read screen
                    penSet.penColor[i].RH      = ReadDouble();
                    penSet.penColor[i].GS      = ReadDouble();
                    penSet.penColor[i].BV      = ReadDouble();

                    // save
                    SC_ColorSpec temp = penSet.penColor[i];

                    // read hard copy
                    penSet.penColor[i].RH      = ReadDouble();
                    penSet.penColor[i].GS      = ReadDouble();
                    penSet.penColor[i].BV      = ReadDouble();

                    if (useScreenPens)
                        penSet.penColor[i] = temp;
                }
        }
    else
        {
            int npenread = DC_PenSet::maxPenSet;
            for ( int i = 0; i < npenread; i++)
                {
                    ReadText(penSet.penLabel[i], stdInputStrLen);
                    // read screen
                    penSet.penColor[i].RH      = ReadDouble();
                    penSet.penColor[i].GS      = ReadDouble();
                    penSet.penColor[i].BV      = ReadDouble();
                }
        }

    CheckForInputFailure("Reading DPO_PenSet");
    return true;
}

void DPO_PenSet::WriteToFile()
{
    //    const int objMajVer = 0;
    //    const int objMajVer = 1;      // changed maxPenSet to 12 from 8
    const int objMajVer = 2;            // changed maxPenSet to 24 from 12, dropped seperate pens
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    WriteBool(useHSVspec);
    WriteLine();

    SetRealConvFormat(SC_RealFormat(ncf_Decimal, 3));
    for ( int i = 0; i < DC_PenSet::maxPenSet; i++)
        {
            WriteText(penSet.penLabel[i]);
            WriteDouble(penSet.penColor[i].RH);
            WriteDouble(penSet.penColor[i].GS);
            WriteDouble(penSet.penColor[i].BV);
            WriteLine();
        }
}

#ifndef ADCONSOLEAPP

DPO_PenSetPS& DPO_PenSet::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_PenSetPS&>(*assocMenu);
}


void DPO_PenSet::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_PenSet::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_PenSetPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(true, false);

    PropUpdate();
}


void DPO_PenSet::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif

