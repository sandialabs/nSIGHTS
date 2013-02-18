///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadCubeDataPS.cpp
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

#include "DPO_ReadCubeDataPS.h"

void DPO_ReadCubeDataPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.Clear();
    args.StdFormAttach();
    cubeDataFnameUI.InitFramedUI("Cube Data File ", PPform.GetWidget(), args);
    cubeDataFnameUI.SetPromptAndMask("Cube file to read",
        "Cube Files (*.cube)|*.cube |All Files (*.*)|*.*|||");

    args.Clear();
    args.StdTopObjectAttach(cubeDataFnameUI);
    static const char* formatLabels[] = {"Std", "Other"};
    cubeFileFormatUI.PreInitSetUp(formatLabels, 2);
    cubeFileFormatUI.InitFramedUI("File Format", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(cubeFileFormatUI);
    optionsFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    xForm.InitWidget(optionsFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(xForm.GetWidget());
    yForm.InitWidget(optionsFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(yForm.GetWidget());
    zForm.InitWidget(optionsFrame.GetForm(), args);


    args.Clear();
    args.TopFormAttach();
    args.LeftFormAttach();
    args.BottomFormAttach(5);
    args.LeftOffset(10);
    setXLogUI.StdToggleInit("X is Log", xForm.GetWidget(), args);
    setYLogUI.StdToggleInit("Y is Log", yForm.GetWidget(), args);
    setZLogUI.StdToggleInit("Z is Log", zForm.GetWidget(), args);


    args.StdSplitSetup();
    args.StdSplitFinish(50);
    setXIDUI.SetFieldLen(10);
    setXIDUI.InitLabeledUI("X ID: ", xForm.GetWidget(), args);
    setYIDUI.SetFieldLen(10);
    setYIDUI.InitLabeledUI("Y ID: ", yForm.GetWidget(), args);
    setZIDUI.SetFieldLen(10);
    setZIDUI.InitLabeledUI("Z ID: ", zForm.GetWidget(), args);


    args.Clear();
    args.StdTopObjectAttach(optionsFrame);
    xcoordStatus.StdInit("Grid X Status", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(xcoordStatus.coordFrame);
    ycoordStatus.StdInit("Grid Y Status", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(ycoordStatus.coordFrame);
    zcoordStatus.StdInit("Grid Z Status", PPform.GetWidget(), args);
}

void  DPO_ReadCubeDataPS::MenuClose()
{
    cubeDataFnameUI.CloseDialog();
    MenuBaseC::MenuClose();
}

void DPO_ReadCubeDataPS::SetMenuData(DPO_ReadCubeData&  inData)
{
    MenuBaseC::SetMenuData(inData);

    cubeDataFnameUI.SetFieldString(inData.cubeDataFname);
    cubeFileFormatUI.SetChoice(inData.cubeFileFormat);

    setXLogUI.SetValue(inData.setXLog);
    setYLogUI.SetValue(inData.setYLog);
    setZLogUI.SetValue(inData.setZLog);
    setXIDUI.SetFieldString(inData.setXID);
    setYIDUI.SetFieldString(inData.setYID);
    setZIDUI.SetFieldString(inData.setZID);

    if (inData.StatusOK())
    {
        xcoordStatus.SetOKStatus(inData.cubeDataDC.xData);
        ycoordStatus.SetOKStatus(inData.cubeDataDC.yData);
        zcoordStatus.SetOKStatus(inData.cubeDataDC.zData);
    }
    else
    {
        xcoordStatus.SetBadStatus();
        ycoordStatus.SetBadStatus();
        zcoordStatus.SetBadStatus();
    }
}

bool DPO_ReadCubeDataPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    cubeDataFnameUI.FieldFileOK(allOK, "Cube Data file", true, false);
    return allOK;
}

void DPO_ReadCubeDataPS::GetMenuData(DPO_ReadCubeData&  outData)
{
    MenuBaseC::GetMenuData(outData);
    cubeDataFnameUI.GetFieldFile(outData.cubeDataFname);
    outData.cubeFileFormat = IO_CubeData::CubeFileFormat(cubeFileFormatUI.GetChoice());

    outData.setXLog = setXLogUI.GetValue();
    outData.setYLog = setYLogUI.GetValue();
    outData.setZLog = setZLogUI.GetValue();

    setXIDUI.GetFieldString(outData.setXID, DC_CubeData::cubeIDLen);
    setYIDUI.GetFieldString(outData.setYID, DC_CubeData::cubeIDLen);
    setZIDUI.GetFieldString(outData.setZID, DC_CubeData::cubeIDLen);
}

void DPO_ReadCubeDataPS::MenuClear()
{
    cubeDataFnameUI.SetFieldString(NULL);
}

