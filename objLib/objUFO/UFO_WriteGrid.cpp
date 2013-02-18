///////////////////////////////////////////////////////////////////////////////////
//
// UFO_WriteGrid.cpp
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
//      Basic utility classes for writing data.  Write grid to data file.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>

#include <objClass/DO_GridData.h>

#include <objUFO/UFO_WriteGrid.h>


UFO_WriteGrid :: UFO_WriteGrid() : FuncObjC("Write Grid")
{
    gridFileFormat = IO_GridData::gffAppDev;
    outFname[0] = '\0';
    InitCommon();
}

UFO_WriteGrid::UFO_WriteGrid(const UFO_WriteGrid& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void UFO_WriteGrid::InitCommon()
{
    AddInPort(gridObjRef,   typeid(DO_GridData));
    DoStatusChk();
}

UFO_WriteGrid& UFO_WriteGrid::operator= (const UFO_WriteGrid& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  UFO_WriteGrid::LocalCopy(const UFO_WriteGrid& a)
{
    CopyString(outFname, a.outFname , stdFileStrLen);
    gridObjRef = a.gridObjRef;
    gridFileFormat = a.gridFileFormat;
}

void  UFO_WriteGrid::DoStatusChk()
{
    FuncObjC::DoStatusChk();
    CheckInputFO(gridObjRef, "Grid data");
}



void UFO_WriteGrid:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if ((objStatus == os_OK) && (calcType == foc_Apply))
    {
        const DC_GridData& tmpDC = *(static_cast<DO_GridData*>(GetInPortData(gridObjRef)))->gridData;

        IO_GridData gridOut;

        gridOut.WriteGridFile(gridFileFormat, outFname, tmpDC);
    }
}

