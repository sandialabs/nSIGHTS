///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadCubeData.cpp
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
//      reads cube data from ASCII file
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Real.h>
#include <genClass/U_Str.h>
#include <genClass/C_MessageLog.h>

#include <objDFO/DFO_ReadCubeData.h>

DFO_ReadCubeData :: DFO_ReadCubeData() :
    FuncObjC("Read Cube Data")
{
    cubeDataFname[0]    = '\0';
    cubeFileFormat = IO_CubeData::cffAppDev;

    setXLog = false;
    setYLog = false;
    setZLog = false;
    CopyString(setXID, "X Data", DC_CubeData::cubeIDLen);
    CopyString(setYID, "Y Data", DC_CubeData::cubeIDLen);
    CopyString(setZID, "Z Data", DC_CubeData::cubeIDLen);

    InitCommon();
}

DFO_ReadCubeData::DFO_ReadCubeData(const DFO_ReadCubeData& a) :
    FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ReadCubeData::InitCommon()
{
    cubeDataDCDO.cubeData = &cubeDataDC;
    fileNameDO.SetTypeLabel("Cube data file");
    AddOutPort(cubeDataDCDO);
    AddOutPort(fileNameDO);
    DoStatusChk();
}

DFO_ReadCubeData& DFO_ReadCubeData::operator= (const DFO_ReadCubeData& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ReadCubeData::LocalCopy(const DFO_ReadCubeData& a)
{
    CopyString(cubeDataFname, a.cubeDataFname, stdFileStrLen);
    CopyString(setXID, a.setXID, DC_CubeData::cubeIDLen);
    CopyString(setYID, a.setYID, DC_CubeData::cubeIDLen);
    CopyString(setZID, a.setZID, DC_CubeData::cubeIDLen);
    setXLog = a.setXLog;
    setYLog = a.setYLog;
    setZLog = a.setZLog;
    cubeFileFormat = a.cubeFileFormat;
}

void  DFO_ReadCubeData:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    if (!cubeDataDCDO.DataOK())
        SetObjErrMsg("No data read");
}

void DFO_ReadCubeData:: CalcOutput(FOcalcType  calcType)
{
    if ((calcType == foc_Full) || (calcType == foc_Apply))
    {
        cubeDataDC.DeAlloc();
        objStatus = os_OK;

/*
        cubeDataDC.Alloc(10, 15, 20);
        int i, j, k;
        for (i = 0; i < cubeDataDC.xData.Size(); i++) 
            cubeDataDC.xData[i] *= 5.0;
        for (i = 0; i < cubeDataDC.yData.Size(); i++) 
            cubeDataDC.yData[i] *= 8.0;
        for (i = 0; i < cubeDataDC.zData.Size(); i++) 
            cubeDataDC.zData[i] *= 3.0;

        for (i = 0; i < cubeDataDC.xData.Size(); i++) 
            for (j = 0; j < cubeDataDC.yData.Size(); j++) 
                for (k = 0; k < cubeDataDC.zData.Size(); k++) 
                {
                    double xdist = 25.0 - cubeDataDC.xData[i];
                    double ydist = 60.0 - cubeDataDC.yData[j];
                    double zdist = 30.0 - cubeDataDC.zData[k];
                    cubeDataDC.cData[i][j][k] = sqrt(xdist * xdist + ydist * ydist + zdist* zdist);
                }   

        IO_CubeData outFile;
        outFile.WriteCubeFile(cubeDataFname, cubeDataDC);

*/

        if (!CheckFileExists(cubeDataFname))
            return;

        IO_CubeData outFile;
        if (!outFile.ReadCubeFile(cubeFileFormat, cubeDataFname, cubeDataDC))
        {
            SetObjErrMsg(MessageLogC::GetLastMessage());
            return;
        }

        if (setXLog)
            CheckLogSet(cubeDataDC.xData);
        if (setYLog)
            CheckLogSet(cubeDataDC.yData);
        if (setZLog)
            CheckLogSet(cubeDataDC.zData);

        cubeDataDC.SetXID(setXID);
        cubeDataDC.SetYID(setYID);
        cubeDataDC.SetZID(setZID);

        SetDefaultFileID(cubeDataFname);
        fileNameDO.SetFileValueLabel(cubeDataFname);
        cubeDataDC.SetID(GetID());
    }

    DoStatusChk();
    if (StatusNotOK())
    {
        cubeDataDC.DeAlloc();
        return;
    }
}


void DFO_ReadCubeData::CheckLogSet(DC_DataCoord& inCoord)
{
    double min, max;
    inCoord.CalcMinMax(min, max);
    if (min > 0.0)
        inCoord.dataIsLog = true;
}

