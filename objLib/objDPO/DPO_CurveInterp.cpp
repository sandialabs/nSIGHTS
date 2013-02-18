///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CurveInterp.cpp
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

#ifndef ADCONSOLEAPP
#include "DPO_CurveInterpPS.h"
#endif
#include "DPO_CurveInterp.h"

const char* DPO_CurveInterp::objLongDesc = "Interpolate on Curve";

DPO_CurveInterp :: DPO_CurveInterp() :
    MenuObjC(this, objLongDesc)
{
}

DPO_CurveInterp :: DPO_CurveInterp(const DPO_CurveInterp& a) :
    DFO_CurveInterp(a),
    MenuObjC(this, objLongDesc)
{
}

DPO_CurveInterp& DPO_CurveInterp::operator= (const DPO_CurveInterp& a)
{
    if (&a != this)
       *((DFO_CurveInterp*) this) = a;
    return *this;
}


bool  DPO_CurveInterp::ReadFromFile()
{
    ReadObjectHeader();

    ReadFuncObjRef(curveObjRef);
    ReadFuncObjRef(xyInputObjRef);

    if ( currentObjReadMajorVersion == 0)
    {
        bool isXYInput = ReadBool();
        bool isLinear  = ReadBool();
        if (isXYInput)
        {
            interpData.interpPointsOp = DSC_InterpPoints::ipoAtInput;
        }
        else if (isLinear)
        {
            interpData.interpPointsOp = DSC_InterpPoints::ipoLinear;
        }
        else
        {
            interpData.interpPointsOp = DSC_InterpPoints::ipoLogRelative;
        }
        interpData.useDataLimits  = ReadBool();
        interpData.minUserLimit = ReadDouble();
        interpData.maxUserLimit = ReadDouble();
        interpData.numInterpPoints = ReadInt();
    }
    else
        ReadInterpPoints(interpData);

    CheckForInputFailure("Reading DPO_CurveInterp");
    return true;
}


void DPO_CurveInterp::WriteToFile()
{
    const int objMajVer = 1;            // added new interp class
//    const int objMajVer = 0;
    const int objMinVer = 0;

    WriteObjectHeader(objMajVer, objMinVer);

    WriteFuncObjRef(curveObjRef);
    WriteFuncObjRef(xyInputObjRef);
    WriteInterpPoints(interpData);
}

#ifndef ADCONSOLEAPP


DPO_CurveInterpPS& DPO_CurveInterp::MenuRef()
{
    ASSERT(assocMenu);
    return static_cast<DPO_CurveInterpPS&>(*assocMenu);
}


void DPO_CurveInterp::PropUpdate()
{
    if (UpdateOK())
        MenuRef().SetMenuData(*this);
}


void DPO_CurveInterp::PropOpenMenu()
{
    MenuObjC::PropOpenMenu();
    assocMenu = new DPO_CurveInterpPS();
    MenuRef().InitMenu();
    MenuRef().MenuOpen(*this);

    PropUpdate();
}


void DPO_CurveInterp::PropApply()
{
    MenuRef().GetMenuData(*this);
    MenuObjC::PropApply();
}

#endif //!ADCONSOLEAPP

