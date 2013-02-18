///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ReadMiniTroll.cpp
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
//      Reads results from MiniTroll formatted file in raw text or post-processed
//  CSV format.  alculates time as elapsed calendar time.
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>

#include <genClass/C_ConvFile.h>
#include <nsDFO/DFO_ReadMiniTroll.h>

DFO_ReadMiniTroll :: DFO_ReadMiniTroll() :
    FuncObjC("ReadMiniTroll")
{
    SetToNull(mtFname);

    fileType        = mtCSV;
    readAllColumns  = false;
    dasReadID       = false;
    dasReadKey      = false;
    yColumn         = 1;
    minimumDeltaT   = 0.0;
    InitCommon();
}

DFO_ReadMiniTroll::DFO_ReadMiniTroll(const DFO_ReadMiniTroll& a) : FuncObjC(a)
{
    LocalCopy(a);
    InitCommon();
}

void DFO_ReadMiniTroll::InitCommon()
{
    AddOutPort(tableDO);
    AddOutPort(westBayPressureTimeDO);
    AddOutPort(westBayPressureDepthDO);
    AddOutPort(xyDataArrayDO);
    AddOutPort(singleXYDO);
    AddOutPort(fileNameDO);
    AddOutPort(timeSpecDO);

    xyDataArrayDO.xyDataArray = &xyTableData;
    tableDO.tableData = &tableData;
    fileNameDO.SetTypeLabel("MiniTroll file");

    westBayPressureTimeDO.tableData = &westBayPressureTimeDC;
    westBayPressureDepthDO.tableData = &westBayPressureDepthDC;

    timeSpecDO.dateTimeSpec = &timeSpecDC;

    DoStatusChk();
}

DFO_ReadMiniTroll& DFO_ReadMiniTroll::operator= (const DFO_ReadMiniTroll& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_ReadMiniTroll::LocalCopy(const DFO_ReadMiniTroll& a)
{
    CopyString(mtFname, a.mtFname, stdFileStrLen);
    timeSpecDC = a.timeSpecDC;

    fileType        = a.fileType;
    dasReadID       = a.dasReadID;
    dasReadKey      = a.dasReadKey;
    readAllColumns  = a.readAllColumns;
    yColumn         = a.yColumn;
    minimumDeltaT   = a.minimumDeltaT;
}

void  DFO_ReadMiniTroll:: DoStatusChk()
{
    FuncObjC::DoStatusChk();
    if (!tableDO.DataOK())
        SetObjErrMsg("No data read");
}

void DFO_ReadMiniTroll:: CalcOutput(FOcalcType  calcType)
{
    if ((calcType == foc_Full) || (calcType == foc_Apply))
    {
        xyTableData.DeAlloc();
        tableData.Alloc(0, 0);
        objStatus = os_OK;
        singleXYDO.xyData = 0;

        // clear Westbay tables
        westBayPressureTimeDC.ClearAll();
        westBayPressureDepthDC.ClearAll();

        if (!CheckFileExists(mtFname))
            return;

        if (!ReadMiniTroll())
            return;

        if ((tableData.GetNCols() > 1) && (tableData.GetNRows() > 0))
        {
            xyTableData.AllocAndSetSize(tableData.GetNCols() - 1);
            for (int i = 1; i < tableData.GetNCols(); i++)
            {
                xyTableData[i - 1].CreateFrom(tableData.dataTable[0], tableData.dataTable[i]);
                xyTableData[i - 1].SetID(tableData.columnDesc[i]);
            }
            singleXYDO.xyData = &(xyTableData[0]);
        }

        if (fileType == wbMOSDAQ)
            SetWestbayTables();
        SetDefaultFileID(mtFname);
        tableData.SetID(GetID());
        fileNameDO.SetFileValueLabel(mtFname);
    }

    DoStatusChk();
    if (StatusNotOK())
    {
        tableData.Alloc(0, 0);
        return;
    }
}

void DFO_ReadMiniTroll::SetWestbayTables()
{
    int nwbTran  = (tableData.GetNCols() - 1) / 2;  // assumes temp & pressure 
    int nwbTime = tableData.GetNRows();

    int maxDesc = IMax(tableData.rowDesc.GetMaxStringLen(), tableData.columnDesc.GetMaxStringLen()) + 2;

    westBayPressureTimeDC.Alloc(nwbTran + 1, nwbTime, maxDesc);
    westBayPressureTimeDC.SetID("Westbay Pressure v Time");
    westBayPressureTimeDC.columnDesc += "Time";
    westBayPressureTimeDC.rowDesc = tableData.rowDesc;
    westBayPressureTimeDC.dataTable[0] = tableData.dataTable[0];
    for (int i = 0; i < nwbTran; i++)
    {
        int pressCol = i * 2 + 1;
        westBayPressureTimeDC.columnDesc += tableData.columnDesc[pressCol];
        westBayPressureTimeDC.dataTable[i + 1] = tableData.dataTable[pressCol];
    }
    westBayPressureTimeDC.SetSize(nwbTran + 1, nwbTime);

    westBayPressureDepthDC.Alloc(nwbTime + 1, nwbTran, maxDesc);
    westBayPressureDepthDC.SetID("Westbay Pressure v Depth");
    westBayPressureDepthDC.columnDesc += "Depth";
    westBayPressureDepthDC.dataTable[0] = westbayDepths;
    for (int i = 0; i < nwbTran; i++)
    {
        int pressCol = i * 2 + 1;
        westBayPressureDepthDC.rowDesc += tableData.columnDesc[pressCol];
    }
    
    for (int i = 0; i < nwbTime; i++)
    {
        westBayPressureDepthDC.columnDesc += tableData.rowDesc[i];
        SC_DoubleArray& currCol = westBayPressureDepthDC.dataTable[i + 1]; 
        for (int j = 0; j < nwbTran; j++)
        {
            int pressCol = j * 2 + 1;
            currCol += tableData.dataTable[pressCol][i];
        }
    }
    westBayPressureDepthDC.SetSize(nwbTime + 1, nwbTran);


}

