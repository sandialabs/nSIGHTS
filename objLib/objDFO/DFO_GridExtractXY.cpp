///////////////////////////////////////////////////////////////////////////////////
//
// DFO_GridExtractXY.cpp
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
//      Extracts line of data from grid.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>
#include <genClass/U_Str.h>

#include <objDFO/DFO_GridExtractXY.h>

DFO_GridExtractXY :: DFO_GridExtractXY() :
    FuncObjC("Extract XY from Grid"), extractValMS("Extract XY")
{
    extractX = true;
    logInterp = false;
    InitCommon();
}

DFO_GridExtractXY::DFO_GridExtractXY(const DFO_GridExtractXY& a) :
    FuncObjC(a), extractValMS(a.extractValMS), DSC_GridInput(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_GridExtractXY :: ~DFO_GridExtractXY ()
{
}

void DFO_GridExtractXY::InitCommon()
{
    extractValMS.AddPort(*this);
    DSC_GridInput::AddPort(*this);
    outputXYDO.xyData = &outputXYDC;
    AddOutPort(outputXYDO);
    DoStatusChk();
}

DFO_GridExtractXY& DFO_GridExtractXY::operator= (const DFO_GridExtractXY& a)
{
    if (&a != this)
    {
        FuncObjC::operator=(a);
        DSC_GridInput::operator=(a);
        extractValMS = a.extractValMS;
        LocalCopy(a);
    }
    return *this;
}


void  DFO_GridExtractXY::LocalCopy(const DFO_GridExtractXY& a)
{
    extractX  = a.extractX;
    logInterp = a.logInterp;
}

void  DFO_GridExtractXY::DoStatusChk()
{
    FuncObjC::DoStatusChk();
    DSC_GridInput::DoCheck(*this);
    extractValMS.DoCheck("Extract XY Source", *this);

    if (StatusNotOK())
        return;

    const DC_DataCoord* currXY;
    if (extractX)
        currXY = &(gridInputDC->xData);
    else
        currXY = &(gridInputDC->yData);

    extractVal = extractValMS.GetCurrentValue();

    int dummy;
    if (!currXY->GetIndex(extractVal, dummy))
        SetObjErrMsg("value out of grid XY range");
}


void DFO_GridExtractXY:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    if (!gridInputDC->ExtractXY(extractVal, extractX, logInterp, outputXYDC))
        SetObjErrMsg("extraction error -- log ??");
}

