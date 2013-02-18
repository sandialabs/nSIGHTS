///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CreateDataLabelPS.cpp
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

#include "DPO_CreateDataLabelPS.h"
#include "DPO_CreateDataLabel.h"



void DPO_CreateDataLabelPS::LabelUIC::InitLabel()
{
    ArgListC args;

    args.StdSplitSetup(65);
    dataTypeLabelUI.SetFieldLen(30);
    dataTypeLabelUI.InitWidget(outsideForm.GetWidget(), args);

    args.StdSplitFinish(35);
    dataValueLabelUI.SetFieldLen(20);
    dataValueLabelUI.InitWidget(outsideForm.GetWidget(), args);
}

void DPO_CreateDataLabelPS::LabelUIC::SetLabel(const DO_Label& labelData)
{
    dataTypeLabelUI.SetFieldString(labelData.dataTypeLabel);
    dataValueLabelUI.SetFieldString(labelData.dataValueLabel);
}


void DPO_CreateDataLabelPS::LabelUIC::GetLabel(DO_Label& labelData)
{
    dataTypeLabelUI.GetFieldString(labelData.dataTypeLabel, DO_Label::maxLabelLen);
    dataValueLabelUI.GetFieldString(labelData.dataValueLabel, DO_Label::maxLabelLen);
}

DPO_CreateDataLabelPS::DPO_CreateDataLabelPS()
{
}


void DPO_CreateDataLabelPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.Clear();
    args.StdFormAttach();
    topForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(65);
    descLab.StdLabelInit("Description", topForm.GetWidget(), args);

    args.StdSplitFinish(35);
    valueLab.StdLabelInit("Value", topForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topForm.GetWidget());

    for (int i = 0; i < DFO_CreateDataLabel::maxLabelData; i++)
    {
        labelDataUI[i].outsideForm.InitWidget(PPform.GetWidget(), args);
        labelDataUI[i].InitLabel();

        args.Clear();
        args.StdTopObjectAttach(labelDataUI[i].outsideForm.GetWidget());
        labelDataUI[i].hSep.InitWidget(PPform.GetWidget(), args);

        args.Clear();
        args.StdTopObjectAttach(labelDataUI[i].hSep.GetWidget());
    }
}


void DPO_CreateDataLabelPS::SetMenuData(DPO_CreateDataLabel&  inData)
{
    for (int i = 0; i < DFO_CreateDataLabel::maxLabelData; i++)
        labelDataUI[i].SetLabel(inData.labelData[i]);

    MenuBaseC::SetMenuData(inData);
}


void DPO_CreateDataLabelPS::GetMenuData(DPO_CreateDataLabel&  outData)
{
    MenuBaseC::GetMenuData(outData);
    for (int i = 0; i < DFO_CreateDataLabel::maxLabelData; i++)
        labelDataUI[i].GetLabel(outData.labelData[i]);
}


