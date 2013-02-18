///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ReadColorMapPS.cpp
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
// DESCRIPTION: File I/O and user interface for all genDFO functional objects.
//              All code is platform independent, but relies upon platform dependent
//              code in genApp and genClassPS.
//              User interface for DFO_XXXX objects.
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClassPS/PS_Color.h>

#include <genApp/U_Menu.h>
#include <genClass/U_Str.h>
#include <genApp/C_MenuPage.h>

#include "DPO_ReadColorMap.h"
#include "DPO_ReadColorMapPS.h"

//  draws current color map in drawing area
void DPO_ReadColorMapPS::RedrawColorMapCB(void* inObj)
{
    static_cast<DPO_ReadColorMapPS*>(inObj)->RedrawColorMap();
}

void DPO_ReadColorMapPS::RedrawColorMap()
{
    DPO_ReadColorMap* currObj = static_cast<DPO_ReadColorMap*> (MenuObjC::currMenuObj);
    if (currObj->colorMap.ncolorMap == 0)
        return;

    colorMapDraw.DrawColorMap(currObj->colorMap);
}

DPO_ReadColorMapPS::DPO_ReadColorMapPS()
{
}

void DPO_ReadColorMapPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC   args;

    args.Clear();
    args.StdFormAttach();
    usePVwaveFormatUI.rightOffset = 150;
    usePVwaveFormatUI.PreInitSetUp(" mView output format  ", " PV Wave native format      ", true);
    usePVwaveFormatUI.InitFramedUI("Color Map File Format", PPform.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(usePVwaveFormatUI);
    dataFile.InitFramedUI("Color Map File", PPform.GetWidget(), args);
    dataFile.SetPromptAndMask("Color map file to read",
                              "CMap Files (*.cmap)|*.cmap|All Files (*.*)|*.*|||");


    args.Clear();
    args.StdTopObjectAttach(dataFile);
    args.BottomFormAttach();
    colorMapDraw.InitWidget(PPform.GetWidget(), args);
}


void DPO_ReadColorMapPS::SetMenuData(DPO_ReadColorMap&  inData)
{
    bool isOK = (inData.GetStatus() == os_OK);

    dataFile.SetFieldString(inData.colorMapFname);
    usePVwaveFormatUI.SetBoolChoice(inData.usePVwaveFormat);

    MenuBaseC::SetMenuData(inData);

    RedrawColorMap();
}

void DPO_ReadColorMapPS::MenuOpen()
{
    MenuBaseC::MenuOpen(false, true);
}

void DPO_ReadColorMapPS::MenuClose()
{
    dataFile.CloseDialog();
    MenuBaseC::MenuClose();
}

bool DPO_ReadColorMapPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    dataFile.FieldFileOK(allOK, "Color map file", true, false);
    return allOK;
}

void DPO_ReadColorMapPS::GetMenuData(DPO_ReadColorMap&  outData)
{
    MenuBaseC::GetMenuData(outData);
    dataFile.GetFieldFile(outData.colorMapFname);
    outData.usePVwaveFormat = usePVwaveFormatUI.GetBoolChoice();
}

void DPO_ReadColorMapPS::MenuClear()
{
    dataFile.SetFieldString(NULL);
    usePVwaveFormatUI.SetBoolChoice(false);
}

