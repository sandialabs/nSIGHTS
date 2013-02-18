///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYSmoothFilter.cpp
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

#ifndef ADCONSOLEAPP
#include "DPO_XYSmoothFilterPS.h"
#endif
#include "DPO_XYSmoothFilter.h"

static const char* objLongDesc = "Smooth/Filter XY Data";

DPO_XYSmoothFilter :: DPO_XYSmoothFilter() :
    MenuObjC(this, objLongDesc)
{
}

DPO_XYSmoothFilter :: DPO_XYSmoothFilter(const DPO_XYSmoothFilter& a) :
    DFO_XYSmoothFilter(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_XYSmoothFilter& DPO_XYSmoothFilter::operator= (const DPO_XYSmoothFilter& a)
{
    if (&a != this)
       *((DFO_XYSmoothFilter*) this) = a;
    return *this;
}


bool  DPO_XYSmoothFilter::ReadFromFile()
{
    ReadObjectHeader();
    ReadFuncObjRef(xyInputObjRef);
    ReadText(outputDesc, DFO_XYBase::outputDescLen);

    smoothFilterOp = DFO_XYSmoothFilter::SmoothFilterOp(ReadInt());
    double ddummy = ReadDouble(); //used to be fftSmoothParameter but we ripped out the FFT filter, KSB 10/26/12
    ptsInHalfWindow = ReadInt();
    int idummy = ReadInt();

    CheckForInputFailure("Reading DPO_XYSmoothFilter");
    return true;
}


void DPO_XYSmoothFilter::WriteToFile()
{
    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);
    WriteFuncObjRef(xyInputObjRef);
    WriteLine();
    WriteText(outputDesc);

    WriteInt(smoothFilterOp);
    double ddummy = 0.0;
    int idummy = 0;
    WriteDouble(ddummy);
    WriteInt(ptsInHalfWindow);
    WriteInt(idummy);

    WriteLine();
}

#ifndef ADCONSOLEAPP

DPO_XYSmoothFilterPS& DPO_XYSmoothFilter::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_XYSmoothFilterPS&>(*assocMenu);
}


void DPO_XYSmoothFilter::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_XYSmoothFilter::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_XYSmoothFilterPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_XYSmoothFilter::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

