///////////////////////////////////////////////////////////////////////////////////
//
// PC_ReportPS.cpp
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
//      implements reporting window management
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genPlotClass/C_PlotDef.h>
#include <genPlotClass/PD_2D.h>

#include <genPlotClassPS/PC_Platform2DPS.h>

#include <genPlotClassPS/PC_ReportPS.h>

PC_ReportPS::PC_ReportPS(PD_2D&  assPlt) :
    PC_Report(assPlt)
{
    showReport = false;
}

PC_ReportPS::~PC_ReportPS()
{
    LayoutManager* pLayoutManager = reportFrame.GetForm();
    pLayoutManager = pLayoutManager->layoutManager; // Get parent layout
    if (pLayoutManager != NULL) {
        pLayoutManager->AdjustChildren(reportFrame.GetForm(), rightPosition);
        pLayoutManager->RemoveChild(reportFrame.GetForm());
    }

    PC_Platform2DPS* platform2D = dynamic_cast<PC_Platform2DPS*>(assocPlot->platformData);
    if (platform2D == 0) {
        GenAppInternalError("PC_ReportPS");
    }

    // force screen update to create widgets
    platform2D->layoutStatusBar.ResetForm();
}

void PC_ReportPS::ReportInit(const char* maxTitle)
{
    if (reportInitialized)
        return;

    PC_Platform2DPS* platform2D = dynamic_cast<PC_Platform2DPS*>(assocPlot->platformData);
    if (platform2D == 0)
        GenAppInternalError("PC_ReportPS::platform2D");

    LayoutManager& parent = platform2D->statusForm;

    ArgListC  args;

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.SetPlaceHolder();
    if (!platform2D->lastWidgetAttached)
    {
        // first to be attached, do not attach to the right
        args.LeftFormAttach();
        args.RightPosAttach(rightPosition);
    }
    else
    {
        // 2nd, 3rd, etc.. to be attached
        args.LeftUICAttach(*(platform2D->lastWidgetAttached));
        args.RightPosAttach(rightPosition + platform2D->lastWidgetAttached->frame.rightAttach.attachPos);
    }

    reportFrame.InitFramedUI("Report", parent.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    reportDataX.SetOffsets(0, 0, 0, 0);
    reportDataX.AttachObjectToLabel();
    reportDataX.InitLabeledUI(maxTitle, reportFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(reportDataX);
    reportDataY.SetOffsets(0, 0, 0, 0);
    reportDataY.AttachObjectToLabel();
    reportDataY.InitLabeledUI(maxTitle, reportFrame.GetForm(), args);

    platform2D->lastWidgetAttached = &reportFrame;

    // force screen update to create widgets
    platform2D->layoutStatusBar.ResetForm();
    reportInitialized = true;
}


void PC_ReportPS::ReportTitleSetup(const char* title)
{
    if (!reportInitialized)
        return;

    reportFrame.ChangeLabel(title);

    if (showReport)
        reportFrame.Manage();
    else
        reportFrame.Unmanage();
}

void PC_ReportPS::ReportLabelSetup(int rowNum, const char* rowLab)
{
    if (reportInitialized) {
        if (rowNum == 0)
            reportDataX.ChangeLabel(rowLab);
        else
            reportDataY.ChangeLabel(rowLab);
    }
}

void PC_ReportPS::ReportUpdate(int rowNum, const char* rowLab)
{
    if (showReport && reportInitialized)
    {
        if (rowNum == 0)
            reportDataX.SetFieldString(rowLab);
        else
            reportDataY.SetFieldString(rowLab);
    }
}

