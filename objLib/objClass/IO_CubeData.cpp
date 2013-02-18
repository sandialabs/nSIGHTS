///////////////////////////////////////////////////////////////////////////////////
//
// IO_CubeData.cpp
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
//      I/O for DC_CubeData
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <objClass/IO_CubeData.h>

IO_CubeData::I_CubeDataFile::I_CubeDataFile(const char* inputFile)
    : ConvFileC(inputFile, 8192)
{
}

void IO_CubeData::I_CubeDataFile::Init()
{
    nextInc = 5;
    nextCount = 50;
}

void IO_CubeData::I_CubeDataFile::ReadAppDevCube(DC_CubeData&    cubeDataDC)
{
    cubeDataDC.DeAlloc();

    CFreadNonBlank();
    CopyString(cubeDataDC.cubeID, lastLineRead, DC_CubeData::cubeIDLen);

    // data starts after header
    CFreadNonBlank();

    int nX = CFgetNextInt();
    int nY = CFgetNextInt();
    int nZ = CFgetNextInt();

    if ((nX < 1) || (nY < 1) || (nZ < 1) || (nX > 1000) || (nY > 1000) || (nZ > 1000) || (nX * nY * nZ > 1000000))
        CFtextError("Grid dimension bad (<1 or >1000 or total > 1E6) ");

    if (!cubeDataDC.Alloc(nX, nY, nZ))
        CFtextError("could not allocate grid data");

    CFreadDoubleArray(cubeDataDC.xData, false);
    CFreadDoubleArray(cubeDataDC.yData, false);
    CFreadDoubleArray(cubeDataDC.zData, false);
    if (!(cubeDataDC.xData.IsAscending() && cubeDataDC.yData.IsAscending() && cubeDataDC.yData.IsAscending()))
        CFtextError("X, Y & Z locations must be ascending");

    for (int i = 0; i < nX; i++)
        for (int j = 0; j < nY; j++)
            CFreadDoubleArray(cubeDataDC.cData[i][j], true);
}


IO_CubeData::O_CubeDataFile::O_CubeDataFile(const char* outputFile)
    :TxtFileC(outputFile, false)
{
}

void IO_CubeData::O_CubeDataFile::Init()
{
}

void IO_CubeData::O_CubeDataFile::WriteAppDevCube(const DC_CubeData& cubeData)
{
    WriteText(cubeData.GetID());

    WriteInt(cubeData.xData.Size());
    WriteInt(cubeData.yData.Size());
    WriteInt(cubeData.zData.Size());
    WriteLine();

    WriteDoubleArray(cubeData.xData, false);
    WriteDoubleArray(cubeData.yData, false);
    WriteDoubleArray(cubeData.zData, false);

    for (int i = 0; i < cubeData.xData.Size(); i++)
        for (int j = 0; j < cubeData.yData.Size(); j++)
            WriteDoubleArray(cubeData.cData[i][j], false);
};



bool IO_CubeData::ReadCubeFile(      CubeFileFormat cubeFileFormat,
                               const char*          inputFileName,
                                     DC_CubeData&   cubeData)
{
    try
    {
        I_CubeDataFile cmFile(inputFileName);
        cmFile.CFsetMsgStuff("ReadCubeData", "CubeDataFile", 100);
        cmFile.Init();
        switch (cubeFileFormat)
        {
            case cffAppDev : {
                cmFile.ReadAppDevCube(cubeData);
                break;
            }
            case cffOther : {
                GenAppErrorMsg("ReadCubeData", "other not supported");
                return false;
                break;
            }
        }
    }
    catch (ConvFileC::CFReadError re) {
        cubeData.DeAlloc();
        GenAppErrorMsg("ReadCubeData", re.errMsg);
        return false;
    }

    {
        using namespace std;
        ostringstream ostr;
        ostr <<  "Cube Data read OK: " << cubeData.xData.Size() << " rows " << " by " << cubeData.yData.Size() << " columns"<< ends;
        GenAppInfoMsg("ReadCubeData", ostr.str().c_str());
    }
    return true;
}

void IO_CubeData::WriteCubeFile(const char*          outputFileName,
                                const DC_CubeData&   cubeData)
{
    try
    {
        O_CubeDataFile cmFile(outputFileName);
        cmFile.Init();
        cmFile.WriteAppDevCube(cubeData);
    }
    catch (TextC::TextError re) {
        GenAppErrorMsg("WriteCubeData", re.errMsg);
        return;
    }

    GenAppInfoMsg("WriteCubeData", "file written successfully");
}

