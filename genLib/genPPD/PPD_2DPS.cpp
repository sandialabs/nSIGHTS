///////////////////////////////////////////////////////////////////////////////////
//
// PPD_2DPS.cpp
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
//      derived from PPD_Base.  Adds components for all 2D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "PPD_2DPS.h"

void PPD_2DPS::InitMenu()
{
    PPD_BasePS::InitMenu();

    ArgListC args;

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.LeftFormAttach();
    args.RightUICAttach(marginFrame);
    formatFrame.InitFramedUI("Format", lastRowForm.GetWidget(), args);

    args.Clear();
    args.TopFormAttach(10);
    args.LeftFormAttach(10);
    showReportAreaUI.StdToggleInit("Show Report Area", formatFrame.GetForm(), args);

    SetMainformScrolled(true, 520, 220);
}

void PPD_2DPS::MenuOpen(PPD_2D& inData)
{
    PPD_BasePS::MenuOpen(inData);
}


void PPD_2DPS::SetMenuData(PPD_2D&  inData)
{
    showReportAreaUI.SetValue(inData.base2DPD.showReportArea);

    PPD_BasePS::SetMenuData(inData);
}

bool PPD_2DPS::UIdataOK()
{
    PPD_BasePS::UIdataOK();
    return allOK;
}

void PPD_2DPS::GetMenuData(PPD_2D& outData)
{
    PPD_BasePS::GetMenuData(outData);
    outData.base2DPD.showReportArea = showReportAreaUI.GetValue();
}

void  PPD_2DPS::MenuDefault()
{
    PPD_BasePS::MenuDefault();
    showReportAreaUI.SetValue(true);
}

