///////////////////////////////////////////////////////////////////////////////////
//
// DPO_GridStatisticsPS.cpp
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

#include "DPO_GridStatistics.h"
#include "DPO_GridStatisticsPS.h"


DPO_GridStatisticsPS::DPO_GridStatisticsPS() :
    DPO_StatisticsPS()
{
}

void DPO_GridStatisticsPS::InitMenu()
{
    DPO_StatisticsPS::InitMenu();

    gridInputUI.StdInit(URform.GetWidget());

    ArgListC args;
    args.StdFormAttach();
    InitStatsCommon(args);

    args.Clear();
    args.StdTopObjectAttach(statusFrame);
    xcoordStatus.StdInit("Grid X Status", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(xcoordStatus.coordFrame);
    ycoordStatus.StdInit("Grid Y Status", PPform.GetWidget(), args);
}

void DPO_GridStatisticsPS::SetMenuData(DPO_GridStatistics&  inData)
{
    gridInputUI.SetGridInput(inData);
    DPO_StatisticsPS::SetMenuData(inData);

    if (inData.StatusOK())
    {
        xcoordStatus.SetOKStatus(inData.gridInputDC->xData);
        ycoordStatus.SetOKStatus(inData.gridInputDC->yData);
    }
    else
    {
        xcoordStatus.SetBadStatus();
        ycoordStatus.SetBadStatus();
    }
}


void DPO_GridStatisticsPS::MenuOpen(DPO_GridStatistics& inData)
{
    DPO_StatisticsPS::MenuOpen(inData);
    gridInputUI.MenuOpen(inData);
}


bool DPO_GridStatisticsPS::UIdataOK()
{
    DPO_StatisticsPS::UIdataOK();
    gridInputUI.UIdataOK(allOK);
    return allOK;
}

void DPO_GridStatisticsPS::GetMenuData(DPO_GridStatistics&  outData)
{
    DPO_StatisticsPS::GetMenuData(outData);
    gridInputUI.GetGridInput(outData);
}


