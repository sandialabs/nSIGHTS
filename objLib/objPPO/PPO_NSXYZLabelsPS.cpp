///////////////////////////////////////////////////////////////////////////////////
//
// PPO_NSXYZLabelsPS.cpp
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

#include "PPO_NSXYZLabelsPS.h"


PPO_NSXYZLabelsPS::PPO_NSXYZLabelsPS() :
    labelSpecUI(typeid(DO_LabelArray))
{
}


void PPO_NSXYZLabelsPS::InitMenu(const PPO_NSXYZLabels&   inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC  args;
    args.StdFormAttach();

    labelSpecUI.StdInit(PPform.GetWidget(), args);

    args.StdTopObjectAttach(labelSpecUI.optionsFrame);
    offsetCoordUI.StdInit("Offset", PPform.GetWidget(), args);
}

void PPO_NSXYZLabelsPS::MenuOpen(PPO_NSXYZLabels& inData)
{
    MenuBaseC::MenuOpen(true, false);
    labelSpecUI.MenuOpen(inData);
    labelSpecUI.dataSource.StdMenuInit(inData.labelsObjRef);
}

void PPO_NSXYZLabelsPS::SetMenuData(PPO_NSXYZLabels& inData)
{
    PPO_BasePS::SetMenuData(inData);

    labelSpecUI.dataSource.SetFuncObjRef(inData.labelsObjRef);
    labelSpecUI.SetXYZLabelSpec(inData);
}

bool PPO_NSXYZLabelsPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    labelSpecUI.UIdataOK(allOK);
    return allOK;
}

void PPO_NSXYZLabelsPS::GetMenuData(PPO_NSXYZLabels& outData)
{
    PPO_BasePS::GetMenuData(outData);

    outData.labelsObjRef = labelSpecUI.dataSource.GetFuncObjRef();
    labelSpecUI.GetXYZLabelSpec(outData);

}

