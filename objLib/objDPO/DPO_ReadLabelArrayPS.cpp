///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadLabelArrayPS.cpp
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

#include <genClass/U_Str.h>

#include <genApp/U_Menu.h>

#include "DPO_ReadLabelArrayPS.h"
#include "DPO_ReadLabelArray.h"

DPO_ReadLabelArrayPS::DPO_ReadLabelArrayPS()
{
}

void DPO_ReadLabelArrayPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.StdFormAttach();
    dataFile.InitFramedUI("XYZ Label Input File ", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(dataFile);
    static const char* formatLabels[] = {"Points", "DXF", "Other"};
    labelFileFormatUI.PreInitSetUp(formatLabels, 3);
    labelFileFormatUI.InitFramedUI("File Format", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(labelFileFormatUI);
    statusFrame.InitFramedUI("Data Status", PPform.GetWidget(), args);

    args.Clear();
    args.LeftOffset(10);
    args.RightOffset(10);
    args.StdFormAttach(5);
    nLabels.InitLabeledUI("# of labels read", statusFrame.GetForm(), args);

}

void DPO_ReadLabelArrayPS::MenuOpen(DPO_ReadLabelArray& inData)
{
    MenuBaseC::MenuOpen(true, false);
}

void DPO_ReadLabelArrayPS::MenuClose()
{
    dataFile.CloseDialog();
    MenuBaseC::MenuClose();
}


void DPO_ReadLabelArrayPS::SetMenuData(DPO_ReadLabelArray&  inData)
{
    MenuBaseC::SetMenuData(inData);

    dataFile.SetFieldString(inData.labelsFname);
    labelFileFormatUI.SetChoice(int(inData.labelFileFormat));

    bool isOK = (inData.GetStatus() == os_OK);
    if (isOK)
    {
        nLabels.SetFieldValue(inData.GetNLabels());
    }
    else
    {
        nLabels.SetStatusNA();
    }
    statusFrame.SetSensitive(isOK);
}

bool DPO_ReadLabelArrayPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    dataFile.FieldFileOK(allOK, "Data input", true, false);
    return allOK;
}

void DPO_ReadLabelArrayPS::GetMenuData(DPO_ReadLabelArray&  outData)
{
    MenuBaseC::GetMenuData(outData);
    dataFile.GetFieldFile(outData.labelsFname);
    outData.labelFileFormat = IO_LabelArray::LabelFileFormat(labelFileFormatUI.GetChoice());
}


void DPO_ReadLabelArrayPS::MenuDefault()
{
    MenuBaseC::MenuDefault();

    dataFile.SetFieldString(NULL);
}

