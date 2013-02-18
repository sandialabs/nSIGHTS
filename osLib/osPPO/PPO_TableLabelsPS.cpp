///////////////////////////////////////////////////////////////////////////////////
//
// PPO_TableLabelsPS.cpp
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
//      File I/O and user interface for all osPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genClass/DO_TableData.h>

#include "PPO_TableLabelsPS.h"

// ****************************************

PPO_TableLabelsPS::PPO_TableLabelsPS() :
    labelSpecUI(typeid(DO_TableData))
{
}

void PPO_TableLabelsPS::InitMenu(const PPO_TableLabels& inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    labelSpecUI.StdInit(PPform.GetWidget(), args);

    args.StdTopObjectAttach(labelSpecUI.optionsFrame.GetForm());
    bottomRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(65);
    convFormatUI.InitFramedUI("Conversion Format", bottomRow.GetWidget(), args);

    args.StdSplitFinish(35);
    static const char* mpoStrings[] = {"Full", "Upper Only", "Lower Only",
                                        "Upper Diagonal", "Lower Diagonal", "Diagonal"};
    matrixPlotOptionUI.PreInitSetUp(mpoStrings, 6);
    matrixPlotOptionUI.InitFramedUI("Matrix Plot Options", bottomRow.GetWidget(), args);

}


void PPO_TableLabelsPS::MenuOpen(PPO_TableLabels&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    labelSpecUI.MenuOpen(inData);
    labelSpecUI.dataSource.StdMenuInit(inData.tableObjRef);
}

void PPO_TableLabelsPS::SetMenuData(PPO_TableLabels&    inData)
{
    PPO_BasePS::SetMenuData(inData);

    labelSpecUI.dataSource.SetFuncObjRef(inData.tableObjRef);
    labelSpecUI.SetXYLabelSpec(inData);

    matrixPlotOptionUI.SetChoice(int(inData.matrixPlotOption));
    convFormatUI.SetRealFormatSettings(inData.convFormat);

}

bool PPO_TableLabelsPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    labelSpecUI.UIdataOK(allOK);

    return allOK;
}

void PPO_TableLabelsPS::GetMenuData(PPO_TableLabels&  outData)
{
    PPO_BasePS::GetMenuData(outData);

    outData.tableObjRef = labelSpecUI.dataSource.GetFuncObjRef();
    labelSpecUI.GetXYLabelSpec(outData);
    convFormatUI.GetRealFormatSettings(outData.convFormat);

    outData.matrixPlotOption = PFO_TableLabels::MatrixPlotOption(matrixPlotOptionUI.GetChoice());
}

