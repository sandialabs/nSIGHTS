///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadColorMap.cpp
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
// DESCRIPTION: Basic functional objects (all derived from FuncObjC)
//              used by all applications, primarily concerned with
//              defining/operating on pen sets and colour maps:
//              Reads a color map from a file.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/C_ConvFile.h>
#include <genClass/U_Str.h>

#include <genDFO/DFO_ReadColorMap.h>


DFO_ReadColorMap :: DFO_ReadColorMap() : FuncObjC("Read Color Map")
{
    colorMapFname[0]    = '\0';
    usePVwaveFormat     = false;
    InitCommon();
}

DFO_ReadColorMap::DFO_ReadColorMap(const DFO_ReadColorMap& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ReadColorMap::InitCommon()
{
    AddOutPort(colorMapDO);
    AddOutPort(fileNameDO);
    colorMapDO.colorMapData = &colorMap;
    fileNameDO.SetTypeLabel("Color map file");

    DoStatusChk();
}

DFO_ReadColorMap& DFO_ReadColorMap::operator= (const DFO_ReadColorMap& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ReadColorMap::LocalCopy(const DFO_ReadColorMap& a)
{
    CopyString(colorMapFname, a.colorMapFname, stdFileStrLen);
    usePVwaveFormat     = a.usePVwaveFormat;
}


void  DFO_ReadColorMap:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
}

void DFO_ReadColorMap:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if ((calcType == foc_Full) || (calcType == foc_Apply))
    {
        colorMap.ncolorMap = 0;
        objStatus = os_OK;

        if (!CheckFileExists(colorMapFname))
            return;

        if (!ReadColorMap())
            return;

        SetDefaultFileID(colorMapFname);
        fileNameDO.SetFileValueLabel(colorMapFname);
    }
}

