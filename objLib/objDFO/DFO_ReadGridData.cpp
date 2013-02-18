///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadGridData.cpp
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
//      Reads grid data from ASCII file in several formats
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/C_MessageLog.h>

#include <objDFO/DFO_ReadGridData.h>

DFO_ReadGridData :: DFO_ReadGridData() :
    FuncObjC("Read Grid")
{
    gridDataFname[0]    = '\0';
    gridFileFormat = IO_GridData::gffAppDev;

    setXLog = false;
    setYLog = false;
    CopyString(setXID, "X Data", DC_GridData::gridIDLen);
    CopyString(setYID, "Y Data", DC_GridData::gridIDLen);

    decimationModulus = 1;

    InitCommon();
}

DFO_ReadGridData::DFO_ReadGridData(const DFO_ReadGridData& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ReadGridData::InitCommon()
{
    gridDataDO.gridData = &gridData;
    fileNameDO.SetTypeLabel("Grid Data file");
    AddOutPort(gridDataDO);
    AddOutPort(fileNameDO);
    DoStatusChk();
}

DFO_ReadGridData& DFO_ReadGridData::operator= (const DFO_ReadGridData& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ReadGridData::LocalCopy(const DFO_ReadGridData& a)
{
    CopyString(gridDataFname, a.gridDataFname, stdFileStrLen);
    CopyString(setXID, a.setXID, DC_GridData::gridIDLen);
    CopyString(setYID, a.setYID, DC_GridData::gridIDLen);
    setXLog = a.setXLog;
    setYLog = a.setYLog;
    gridFileFormat = a.gridFileFormat;
    decimationModulus = a.decimationModulus;
}

void  DFO_ReadGridData:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    if (!gridDataDO.DataOK())
        SetObjErrMsg("No data read");
}

void DFO_ReadGridData:: CalcOutput(FOcalcType  calcType)
{
    if ((calcType == foc_Full) || (calcType == foc_Apply))
    {
        gridData.DeAlloc();
        objStatus = os_OK;

        if (!CheckFileExists(gridDataFname))
            return;

        IO_GridData outFile;
        if (!outFile.ReadGridFile(gridFileFormat, gridDataFname, gridData))
        {
            SetObjErrMsg(MessageLogC::GetLastMessage());
            return;
        }

        gridData.Decimate(decimationModulus);

        if (setXLog)
            CheckLogSet(gridData.xData);
        if (setYLog)
            CheckLogSet(gridData.yData);
        gridData.SetXID(setXID);
        gridData.SetYID(setYID);

        SetDefaultFileID(gridDataFname);
        fileNameDO.SetFileValueLabel(gridDataFname);
        gridData.SetID(GetID());
    }

    DoStatusChk();
    if (StatusNotOK())
    {
        gridData.DeAlloc();
        return;
    }
}


void DFO_ReadGridData::CheckLogSet(DC_DataCoord& inCoord)
{
    double min, max;
    inCoord.CalcMinMax(min, max);
    if (min > 0.0)
        inCoord.dataIsLog = true;
}

