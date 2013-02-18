///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadLabelArray.cpp
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
//      Reads label data for plotting
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/C_MessageLog.h>

#include <objDFO/DFO_ReadLabelArray.h>


DFO_ReadLabelArray :: DFO_ReadLabelArray() : FuncObjC("Read Label Array")
{
    labelsFname[0]    = '\0';
    labelFileFormat   = IO_LabelArray::lffPoints;

    InitCommon();
}

DFO_ReadLabelArray::DFO_ReadLabelArray(const DFO_ReadLabelArray& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ReadLabelArray::InitCommon()
{
    AddOutPort(labelsDO);
    AddOutPort(fileNameDO);
    tableDO.tableData = &tableDC;
    AddOutPort(tableDO);

    DoStatusChk();
}

DFO_ReadLabelArray& DFO_ReadLabelArray::operator= (const DFO_ReadLabelArray& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ReadLabelArray::LocalCopy(const DFO_ReadLabelArray& a)
{
    CopyString(labelsFname, a.labelsFname, stdFileStrLen);
    labelFileFormat = a.labelFileFormat;
}


void  DFO_ReadLabelArray:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    if (!labelsDO.LabelsAvailable())
        SetObjErrMsg("No data read");

    labelsDO.SetTypeLabel("Labels");
    fileNameDO.SetTypeLabel("Label file");
}

void DFO_ReadLabelArray:: CalcOutput(FOcalcType  calcType)
{
    if ((calcType == foc_Full) || (calcType == foc_Apply))
    {
        labelsDO.Clear();
        tableDC.Alloc(0, 0);

        objStatus = os_OK;
        if (!CheckFileExists(labelsFname))
            return;

        IO_LabelArray inFile;
        if (!inFile.ReadLabelFile(labelFileFormat, labelsFname, labelsDO))
        {
            SetObjErrMsg(MessageLogC::GetLastMessage());
            return;
        }

        tableDC.Alloc(3, labelsDO.Size());
        tableDC.SetTitle(0, "X");
        tableDC.SetTitle(1, "Y");
        tableDC.SetTitle(2, "Z");
        tableDC.dataTable.SetSize(3);
        for (int i = 0; i < labelsDO.Size(); i++)
        {
            Coord3D& currCoord = labelsDO.dataCoords[i];

            tableDC.dataTable[0] += currCoord.cX;
            tableDC.dataTable[1] += currCoord.cY;
            tableDC.dataTable[2] += currCoord.cZ;
        }

        SetDefaultFileID(labelsFname);
        fileNameDO.SetFileValueLabel(labelsFname);
    }

    DoStatusChk();
    if (StatusNotOK())
    {
        labelsDO.Clear();
        tableDC.Alloc(0, 0);
        return;
    }
}

