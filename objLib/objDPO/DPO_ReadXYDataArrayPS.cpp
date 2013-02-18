///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadXYDataArrayPS.cpp
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

#include "DPO_ReadXYDataArrayPS.h"
#include "DPO_ReadXYDataArray.h"

DPO_ReadXYDataArrayPS::DPO_ReadXYDataArrayPS()
{
}

void DPO_ReadXYDataArrayPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.StdFormAttach();
    xyDataArrayFnameUI.InitFramedUI("XY Data Array File (.XYA)", PPform.GetWidget(), args);
    xyDataArrayFnameUI.SetPromptAndMask("XY Data array file to read",
                "XY data array files (*.nXYA)|*.nXYA|All Files (*.*)|*.*|||");

    args.StdTopObjectAttach(xyDataArrayFnameUI);
    mpiFilesUI.nodeFileID += "XY Array file";
    mpiFilesUI.StdInit(PPform.GetWidget(), args);

    args.StdTopObjectAttach(mpiFilesUI);
    statusFrame.InitFramedUI("Status", PPform.GetWidget(), args);

    args.StdFormAttach();
    nXY.SetFieldLen(8);
    nXY.InitLabeledUI("Number of XY data sets", statusFrame.GetForm(), args);

    args.StdTopObjectAttach(nXY);
    nPoints.InitLabeledUI("Total # of XY points", statusFrame.GetForm(), args);
}

void DPO_ReadXYDataArrayPS::MenuOpen(DPO_ReadXYDataArray& inData)
{
    MenuBaseC::MenuOpen(true, false);
}

void DPO_ReadXYDataArrayPS::MenuClose()
{
    xyDataArrayFnameUI.CloseDialog();
    MenuBaseC::MenuClose();
}


void DPO_ReadXYDataArrayPS::SetMenuData(DPO_ReadXYDataArray&  inData)
{
    MenuBaseC::SetMenuData(inData);

    xyDataArrayFnameUI.SetFieldString(inData.xyDataArrayFname);
    mpiFilesUI.SetNodeFileArray(inData.mpiFiles);

    bool isOK = inData.StatusOK();
    if (isOK)
    {
        nXY.SetFieldValue(inData.xyDataArray.Size());
        int npts = 0;
        for (int i = 0; i < inData.xyDataArray.Size(); i++)
            npts += inData.xyDataArray[i].Size();
        nPoints.SetFieldValue(npts);
    }
    else
    {
        nXY.SetStatusNA();
        nPoints.SetStatusNA();
    }

    statusFrame.SetSensitive(isOK);
}

bool DPO_ReadXYDataArrayPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    xyDataArrayFnameUI.FieldFileOK(allOK, "XY data file", true, false);
    return allOK;
}

void DPO_ReadXYDataArrayPS::GetMenuData(DPO_ReadXYDataArray&  outData)
{
    MenuBaseC::GetMenuData(outData);
    xyDataArrayFnameUI.GetFieldFile(outData.xyDataArrayFname);
    mpiFilesUI.GetNodeFileArray(outData.mpiFiles);
}


void DPO_ReadXYDataArrayPS::MenuDefault()
{
    MenuBaseC::MenuDefault();
    xyDataArrayFnameUI.SetFieldString(NULL);
}

