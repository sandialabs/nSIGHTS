///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYDualScaleTransformPS.cpp
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

#include <stdlib.h>

#include <genClass/C_Common.h>

#include <genClass/U_Str.h>
#include <genClass/DO_XYData.h>

#include "DPO_XYDualScaleTransform.h"
#include "DPO_XYDualScaleTransformPS.h"

void DPO_XYDualScaleTransformPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_XYDualScaleTransformPS*>(inObj)->SetAllSensitive();
}

void DPO_XYDualScaleTransformPS::SetAllSensitive()
{
    xscaleValueMSUI.SetAllSensitive();
    xoffsetValueMSUI.SetAllSensitive();
    yscaleValueMSUI.SetAllSensitive();
    yoffsetValueMSUI.SetAllSensitive();
}


DPO_XYDualScaleTransformPS::DPO_XYDualScaleTransformPS() :
    xyInputObjRefUI(typeid(DO_XYData)),
    xscaleValueMSUI(typeid(DO_Real), false),
    xoffsetValueMSUI(typeid(DO_Real), false),
    yscaleValueMSUI(typeid(DO_Real), false),
    yoffsetValueMSUI(typeid(DO_Real), false)
{
}

void DPO_XYDualScaleTransformPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    xyInputObjRefUI.InitFramedUI("Data To Scale/Transform", URform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    xFrame.InitFramedUI("X Scale/Transform", PPform.GetWidget(), args);


    // same atachments
    xtopRowForm.InitWidget(xFrame.GetForm(), args);


    args.StdSplitSetup(35);
    xscaleFirstUI.PreInitSetUp("Transform -> Scale", "Scale -> Transform");
    xscaleFirstUI.InitFramedUI("Operation Order", xtopRowForm.GetWidget(), args);

    static const char* scalLabels[]  = {"D * Sc + Off", "(D + Off) * Sc",
                                    "D / Sc + Off", "(D + Off) / Sc"};

    args.ResetPlace();
    args.LeftPosAttach(35);
    args.RightPosAttach(70);
    xscaleOpUI.PreInitSetUp(scalLabels, 4);
    xscaleOpUI.InitFramedUI("Scale Operation", xtopRowForm.GetWidget(), args);

    args.StdSplitFinish(30);
    static const char* tranLabels[]  = {"None", "ln(Data)", "log10(Data)", "Sqrt(Data)", "1 / Data",
                                    "exp(Data)", "10**(Data)", "Data * Data", "Abs(Data)"};

    xtranOpUI.PreInitSetUp(tranLabels, 9);
    xtranOpUI.InitFramedUI("Transform", xtopRowForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(xtopRowForm.GetWidget());
    xscaleValueMSUI.InitUI("Scale", args, xFrame.GetForm(), true, true);

    args.Clear();
    args.StdTopObjectAttach(xscaleValueMSUI);
    xoffsetValueMSUI.InitUI("Offset", args, xFrame.GetForm(), true, true);


    args.Clear();
    args.StdTopObjectAttach(xFrame);
    yFrame.InitFramedUI("Y Scale/Transform", PPform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    ytopRowForm.InitWidget(yFrame.GetForm(), args);

    args.StdSplitSetup(35);
    yscaleFirstUI.PreInitSetUp("Transform -> Scale", "Scale -> Transform");
    yscaleFirstUI.InitFramedUI("Operation Order", ytopRowForm.GetWidget(), args);

    args.ResetPlace();
    args.LeftPosAttach(35);
    args.RightPosAttach(70);
    yscaleOpUI.PreInitSetUp(scalLabels, 4);
    yscaleOpUI.InitFramedUI("Scale Operation", ytopRowForm.GetWidget(), args);

    args.StdSplitFinish(30);
    ytranOpUI.PreInitSetUp(tranLabels, 9);
    ytranOpUI.InitFramedUI("Transform", ytopRowForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(ytopRowForm.GetWidget());
    yscaleValueMSUI.InitUI("Scale", args, yFrame.GetForm(), true, true);

    args.Clear();
    args.StdTopObjectAttach(yscaleValueMSUI);
    yoffsetValueMSUI.InitUI("Offset", args, yFrame.GetForm(), true, true);

    args.Clear();
    args.StdTopObjectAttach(yFrame);
    optionFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.LeftFormAttach();
    args.LeftOffset(10);
    positiveXOnlyUI.StdToggleInit("+ve X only", optionFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(optionFrame);
    outputTypeDescUI.SetFieldLen(40);
    outputTypeDescUI.InitFramedUI("Output Description", PPform.GetWidget(), args);

    SetMainformScrolled(true, 500, 500);
}

void DPO_XYDualScaleTransformPS::SetMenuData(DPO_XYDualScaleTransform&  inData)
{
    MenuBaseC::SetMenuData(inData);

    xyInputObjRefUI.SetFuncObjRef(inData.xyInputObjRef);

    xscaleFirstUI.SetBoolChoice(inData.xST.scaleFirst);
    xscaleOpUI.SetChoice(int(inData.xST.scaleOp));
    xtranOpUI.SetChoice(int(inData.xST.tranOp));
    xscaleValueMSUI.SetValue(inData.xscaleMS);
    xoffsetValueMSUI.SetValue(inData.xoffsetMS);

    yscaleFirstUI.SetBoolChoice(inData.yST.scaleFirst);
    yscaleOpUI.SetChoice(int(inData.yST.scaleOp));
    ytranOpUI.SetChoice(int(inData.yST.tranOp));
    yscaleValueMSUI.SetValue(inData.yscaleMS);
    yoffsetValueMSUI.SetValue(inData.yoffsetMS);

    positiveXOnlyUI.SetValue(inData.positiveXOnly);

    outputTypeDescUI.SetFieldString(inData.outputTypeDesc);

    SetAllSensitive();
}

void DPO_XYDualScaleTransformPS::MenuOpen(DPO_XYDualScaleTransform& inData)
{
    MenuBaseC::MenuOpen(true, false);
    xyInputObjRefUI.StdMenuInit(inData.xyInputObjRef);
    xscaleValueMSUI.MenuInit(inData.xscaleMS);
    xoffsetValueMSUI.MenuInit(inData.xoffsetMS);
    yscaleValueMSUI.MenuInit(inData.yscaleMS);
    yoffsetValueMSUI.MenuInit(inData.yoffsetMS);
}


bool DPO_XYDualScaleTransformPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    xyInputObjRefUI.FuncObjRefOK(allOK, "Data source");
    xscaleValueMSUI.UIdataOK(allOK);
    xoffsetValueMSUI.UIdataOK(allOK);
    yscaleValueMSUI.UIdataOK(allOK);
    yoffsetValueMSUI.UIdataOK(allOK);

    return allOK;
}

void DPO_XYDualScaleTransformPS::GetMenuData(DPO_XYDualScaleTransform&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.xyInputObjRef    = xyInputObjRefUI.GetFuncObjRef();
    outData.xST.scaleFirst = xscaleFirstUI.GetBoolChoice();
    outData.xST.scaleOp    = DSC_ScaleTransform::ScaleType(xscaleOpUI.GetChoice());
    outData.xST.tranOp     = DSC_ScaleTransform::TransformType(xtranOpUI.GetChoice());
    xscaleValueMSUI.GetValue(outData.xscaleMS);
    xoffsetValueMSUI.GetValue(outData.xoffsetMS);

    outData.yST.scaleFirst = yscaleFirstUI.GetBoolChoice();
    outData.yST.scaleOp    = DSC_ScaleTransform::ScaleType(yscaleOpUI.GetChoice());
    outData.yST.tranOp     = DSC_ScaleTransform::TransformType(ytranOpUI.GetChoice());
    yscaleValueMSUI.GetValue(outData.yscaleMS);
    yoffsetValueMSUI.GetValue(outData.yoffsetMS);

    outData.positiveXOnly = positiveXOnlyUI.GetValue();

    outputTypeDescUI.GetFieldString(outData.outputTypeDesc, stdMsgStrLen);
}

void DPO_XYDualScaleTransformPS::MenuDefault()
{
    MenuBaseC::MenuDefault();

    xscaleValueMSUI.realValueUI.SetFieldValue(1.0);
    xoffsetValueMSUI.realValueUI.SetFieldValue(0.0);
    yscaleValueMSUI.realValueUI.SetFieldValue(1.0);
    yoffsetValueMSUI.realValueUI.SetFieldValue(0.0);
    xtranOpUI.SetChoice(int(DFO_ScaleTransform::tt_None));
    ytranOpUI.SetChoice(int(DFO_ScaleTransform::tt_None));

}


