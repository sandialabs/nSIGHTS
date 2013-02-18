///////////////////////////////////////////////////////////////////////////////////
//
// PPO_CubeColorBasePS.cpp
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
//      File I/O and user interface for all objPFOGL objects
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genPPO/PPO_Base.h>

#include <objClass/DO_CubeData.h>

#include "PPO_CubeColorBasePS.h"


PPO_CubeColorBasePS::PPO_CubeColorBasePS() :
    PPO_ColorBasePS(typeid(DO_CubeData))
{
}

void PPO_CubeColorBasePS::ResetLabelsCB(void* inObj)
{
    static_cast<PPO_CubeColorBasePS*>(inObj)->ResetLabels();
}

void PPO_CubeColorBasePS::ResetLabels()
{
    coordMapUI.SetLabels(cubeDataUI.cubeLabels);
}

void PPO_CubeColorBasePS::InitMenu(const PPO_Base&   inData)
{
    const char* viewStrings[] = {"All data", "Cube in View"};

    ArgListC args;

    PPO_BasePS::InitMenu(inData);
    cubeDataUI.StdInit(PPform.GetWidget(), inputDataObjRefUI, topRowForm);
    inputDataObjRefUI.AddOtherObjectCallback(ResetLabelsCB, this);

    args.StdTopObjectAttach(topRowForm.GetWidget());

    PPO_ColorBasePS::InitMenu(inData, args, viewStrings, 2);
}

void PPO_CubeColorBasePS::InitMapAndOffset(const PPO_Base&  inData,
                                                 ArgListC&  attachments)
{
    coordMapUI.StdInit(PPform.GetWidget(), attachments);

    ArgListC args;
    args.StdTopObjectAttach(coordMapUI.coordMapFrame);
    offsetCoordUI.StdInit("Offset", PPform.GetWidget(), args);
}

void PPO_CubeColorBasePS::MenuOpen(PFO_CubeColorBase& inData)
{
    PPO_ColorBasePS::MenuOpen(inData);
    cubeDataUI.MenuOpen(inData, inData);
}

void PPO_CubeColorBasePS::SetMenuData(PPO_Base&         inBase,
                                      PFO_CubeColorBase& inData)
{
    PPO_ColorBasePS::SetMenuData(inBase, inData);
    cubeDataUI.Set3DCubeData(inData);
    cubeDataUI.ResetLabels();
    coordMapUI.Set3DCoordMap(inData.coordMap);
    coordMapUI.SetLabels(cubeDataUI.cubeLabels);
}

bool PPO_CubeColorBasePS::UIdataOK()
{
    PPO_ColorBasePS::UIdataOK();

    cubeDataUI.UIdataOK(allOK);

    return allOK;
}

void PPO_CubeColorBasePS::GetMenuData(PPO_Base&         outBase,
                                   PFO_CubeColorBase& outData)
{
    PPO_ColorBasePS::GetMenuData(outBase, outData);
    cubeDataUI.Get3DCubeData(outData);
    coordMapUI.Get3DCoordMap(outData.coordMap);
}


