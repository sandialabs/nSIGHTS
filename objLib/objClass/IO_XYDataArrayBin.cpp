///////////////////////////////////////////////////////////////////////////////////
//
// IO_XYDataArrayBin.cpp
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
//      I/O for DC_XXX.
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Msg.h>

#include <objClass/IO_XYDataArrayBin.h>


// constructors / destructors - empty for now
IO_XYDataArrayBin::IO_XYDataArrayBin()
: BufFileC(8192)
{};

IO_XYDataArrayBin::~IO_XYDataArrayBin(){};

static void InfoMsg(const char* msgStr)
{
    GenAppInfoMsg("IO_XYDataArrayBin", msgStr);
}

enum {fileIDLen = 80};
static const char  fileID[fileIDLen]  = "INTERA Binary XY Data Array File";

bool IO_XYDataArrayBin::WriteXYDataArrayFile(const char  fName[],
                                                    const DC_XYDataArray& xyArrayData)
{
    try
    {
        OpenFile(fName, false);
        WriteXYDataArrayHeader(xyArrayData);
        WriteXYDataArrayBody(xyArrayData);
        FlushBuffer();
        CloseFile();
    }
    catch (BufFileC::BufferFileError re) {
        GenAppErrorMsg("IO_XYDataArrayBin", re.errMsg);
        return false;
    }

    InfoMsg(" File written successfully ");

    return true;
}


void IO_XYDataArrayBin::WriteXYDataArrayHeader(const DC_XYDataArray& xyArrayData)
{
    WriteStdFileHeader(fileID, fileIDLen, currWriteMajVer, currWriteMinVer);
    WriteInt(xyArrayData.Size());
}



void IO_XYDataArrayBin::WriteXYDataArrayBody(const DC_XYDataArray& xyArrayData)
{
    for (int i = 0; i < xyArrayData.Size(); i++)
    {
        DC_XYData& currXY = xyArrayData[i];

        WriteString(currXY.dataID, DC_XYData::dataIDLen);
        WriteInt(currXY.Size());

        for (int j = 0; j < currXY.Size(); j++)
        {
            WriteDouble(currXY.xData[j]);
            WriteDouble(currXY.yData[j]);
        }
    }
}


bool IO_XYDataArrayBin::ReadXYDataArrayHeader(DC_XYDataArray& xyArrayData)
{
    ReadStdFileHeader(fileID, fileIDLen, currReadMajVer, currReadMinVer);

    int nxy = ReadInt();

    return xyArrayData.ChkAlloc(nxy);
}


bool IO_XYDataArrayBin::ReadXYDataArrayBody(DC_XYDataArray& xyArrayData)
{
    xyArrayData.SetSizeToAlloc();

    for (int i = 0; i < xyArrayData.Size(); i++)
    {
        DC_XYData& currXY = xyArrayData[i];

        ReadString(currXY.dataID, DC_XYData::dataIDLen);
        int nxy = ReadInt();
        if (!currXY.Alloc(nxy))
            return false;

        currXY.SetSize(nxy);

        for (int j = 0; j < nxy; j++)
        {
            currXY.xData[j]= ReadDouble();
            currXY.yData[j]= ReadDouble();
        }
    }

    return true;
}

bool IO_XYDataArrayBin::ReadXYDataArrayFile(const char fName[],
                                                     DC_XYDataArray& xyArrayData)
{
    bool readOK = false;
    xyArrayData.DeAlloc();
    try
    {
        InfoMsg(" Opening file .. ");

        OpenFile(fName, true);

        if (ReadXYDataArrayHeader(xyArrayData) && ReadXYDataArrayBody(xyArrayData))
        {
            readOK = true;
        }
    }
    catch (BufFileC::BufferFileError re) {
        GenAppErrorMsg("XYDataArrayBin", re.errMsg);
    }
    catch (SC_Array::AllocationError re) {
        GenAppErrorMsg("XYDataArrayBin", re.errMsg);
    }
    CloseFile();

    if (!readOK)
        return false;

    InfoMsg(" File read successfully ");
    return true;

}

