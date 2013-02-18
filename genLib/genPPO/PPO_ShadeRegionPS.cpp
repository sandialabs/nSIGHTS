///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ShadeRegionPS.cpp
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
//      File I/O and user interface for all genPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "PPO_ShadeRegionPS.h"

// ****************************************

PPO_ShadeRegionPS::PPO_ShadeRegionPS() :
    regionXMinUI(typeid(DO_Real), false),
    regionXMaxUI(typeid(DO_Real), false),
    regionYMinUI(typeid(DO_Real), false),
    regionYMaxUI(typeid(DO_Real), false),
    colorMapObjRefUI(typeid(DO_ColorMap)),
    labelFontUI(true)
{
}

void PPO_ShadeRegionPS::SetNeedXY()
{
    needXMin = (xRegionSpecUI.GetChoice() == 1) || (xRegionSpecUI.GetChoice() == 2);
    needXMax = (xRegionSpecUI.GetChoice() == 2) || (xRegionSpecUI.GetChoice() == 3);
    needYMin = (yRegionSpecUI.GetChoice() == 1) || (yRegionSpecUI.GetChoice() == 2);
    needYMax = (yRegionSpecUI.GetChoice() == 2) || (yRegionSpecUI.GetChoice() == 3);
}


void PPO_ShadeRegionPS::SetAllSensitive()
{
    SetNeedXY();

    regionXMinUI.SetSensitive(needXMin);
    regionXMaxUI.SetSensitive(needXMax);
    regionYMinUI.SetSensitive(needYMin);
    regionYMaxUI.SetSensitive(needYMax);

    bool doShade = shadeRegionUI.GetValue();
    shadeIsColorMapUI.SetSensitive(doShade);

    bool doCM = doShade && shadeIsColorMapUI.GetBoolChoice();
    colorMapObjRefUI.SetSensitive(doCM);
    shadeCMIndexUI.SetSensitive(doCM);
    shadePenUI.SetSensitive(!doCM);

    bool doLabel = labelRegionUI.GetValue();
    labelFormatFrame.SetSensitive(doLabel);
    labelXPositionUI.SetSensitive(doLabel);
    labelYPositionUI.SetSensitive(doLabel);
    labelPenUI.SetSensitive(doLabel);
    halignUI.SetSensitive(doLabel);
    valignUI.SetSensitive(doLabel);
    labelOffsetUI.SetSensitive(doLabel);

    labelFontUI.SetSensitive(doLabel);
    regionLabelUI.SetSensitive(doLabel);

    regionStippleUI.SetSensitive(stippleRegionUI.GetValue());

    frameLineUI.SetSensitive(frameRegionUI.GetValue());

}

void PPO_ShadeRegionPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_ShadeRegionPS*>(inObj)->SetAllSensitive();
}


void PPO_ShadeRegionPS::InitMenu(const PPO_ShadeRegion& inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    topRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    const char* rstStrs[] = {"None", "< Min", "Min to Max", "> Max"};
    xRegionSpecUI.PreInitSetUp(rstStrs, 4);
    xRegionSpecUI.InitFramedUI("X Region", topRow.GetWidget(), args);
    xRegionSpecUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    yRegionSpecUI.PreInitSetUp(rstStrs, 4);
    yRegionSpecUI.InitFramedUI("Y Region", topRow.GetWidget(), args);
    yRegionSpecUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(topRow.GetWidget());
    regionXMinUI.InitUI("X Minimum", args, PPform.GetWidget(), true, true, SetAllSensitiveCB, this);

    args.StdTopObjectAttach(regionXMinUI);
    regionXMaxUI.InitUI("X Maximum", args, PPform.GetWidget(), true, true, SetAllSensitiveCB, this);

    args.StdTopObjectAttach(regionXMaxUI);
    regionYMinUI.InitUI("Y Minimum", args, PPform.GetWidget(), true, true, SetAllSensitiveCB, this);

    args.StdTopObjectAttach(regionYMinUI);
    regionYMaxUI.InitUI("Y Maximum", args, PPform.GetWidget(), true, true, SetAllSensitiveCB, this);

    args.StdTopObjectAttach(regionYMaxUI);
    optionFrame.InitFramedUI("Region Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    shadeRegionUI.StdToggleInit("shade", optionFrame.GetForm(), args);
    shadeRegionUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(30);
    labelRegionUI.StdToggleInit("label", optionFrame.GetForm(), args);
    labelRegionUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(55);
    stippleRegionUI.StdToggleInit("pattern", optionFrame.GetForm(), args);
    stippleRegionUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(80);
    frameRegionUI.StdToggleInit("frame", optionFrame.GetForm(), args);
    frameRegionUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(optionFrame);
    shadeFormRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(15);
    shadeIsColorMapUI.PreInitSetUp("Std Pens", "Color Map");
    shadeIsColorMapUI.InitFramedUI("Shade Source", shadeFormRow.GetWidget(), args);
    shadeIsColorMapUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitMiddle(15, 50);
    colorMapObjRefUI.InitFramedUI("Color Map", shadeFormRow.GetWidget(), args);

    args.StdSplitMiddle(50, 75);
    shadeCMIndexUI.InitFramedUI("CM Index", shadeFormRow.GetWidget(), args);

    args.StdSplitFinish(25);
    shadePenUI.InitFramedUI("Pen", shadeFormRow.GetWidget(), args);

    args.StdTopObjectAttach(shadeFormRow.GetWidget());
    labelFormatFrame.InitFramedUI("Region Label", PPform.GetWidget(), args);

    args.StdSplitSetup(20);
    labelXPositionUI.SetFieldLen(5);
    labelXPositionUI.format = SC_RealFormat(ncf_Decimal, 3);
    labelXPositionUI.AttachLabelToObject();
    labelXPositionUI.InitLabeledUI("XPos", labelFormatFrame.GetForm(), args);

    args.StdSplitMiddle(20, 40);
    labelYPositionUI.SetFieldLen(5);
    labelYPositionUI.format = SC_RealFormat(ncf_Decimal, 3);
    labelYPositionUI.AttachLabelToObject();
    labelYPositionUI.InitLabeledUI("YPos", labelFormatFrame.GetForm(), args);

    args.StdSplitMiddle(40, 60);
    halignUI.AttachLabelToObject();
    halignUI.InitLabeledUI("H", labelFormatFrame.GetForm(), args);

    args.StdSplitMiddle(60, 80);
    valignUI.AttachLabelToObject();
    valignUI.InitLabeledUI("V", labelFormatFrame.GetForm(), args);

    args.StdSplitFinish(20);
    labelOffsetUI.SetFieldLen(3);
    labelOffsetUI.AttachLabelToObject();
    labelOffsetUI.InitLabeledUI("Off", labelFormatFrame.GetForm(), args);

    args.StdTopObjectAttach(labelFormatFrame);
    penFontRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(25);
    labelPenUI.InitFramedUI("Label Pen", penFontRow.GetWidget(), args);

    args.StdSplitFinish(75);
    labelFontUI.InitFramedUI("Label Font", penFontRow.GetWidget(), args);

    args.StdTopObjectAttach(penFontRow.GetWidget());
    regionLabelUI.AttachObjectToFrame();
    regionLabelUI.InitFramedUI("Label",  PPform.GetWidget(), args);

    args.StdTopObjectAttach(regionLabelUI);
    regionStippleUI.InitFramedUI("Pattern", PPform.GetWidget(), args);

    args.StdTopObjectAttach(regionStippleUI);
    frameLineUI.InitFramedUI("Frame", PPform.GetWidget(), args);

}


void PPO_ShadeRegionPS::MenuOpen(PPO_ShadeRegion&   inData)
{
    MenuBaseC::MenuOpen(false, false);
    regionXMinUI.MenuInit(inData.regionXMin);
    regionXMaxUI.MenuInit(inData.regionXMax);
    regionYMinUI.MenuInit(inData.regionYMin);
    regionYMaxUI.MenuInit(inData.regionYMax);
    colorMapObjRefUI.StdMenuInit(inData.colorMapObjRef);
    shadePenUI.ResetPens(inData.objPenSet);
    labelPenUI.ResetPens(inData.objPenSet);
    regionStippleUI.ResetPens(inData.objPenSet);
    frameLineUI.ResetPens(inData.objPenSet);
}

void PPO_ShadeRegionPS::SetMenuData(PPO_ShadeRegion&    inData)
{
    PPO_BasePS::SetMenuData(inData);

    xRegionSpecUI.SetChoice(inData.xRegionSpec);
    yRegionSpecUI.SetChoice(inData.yRegionSpec);

    regionXMinUI.SetValue(inData.regionXMin);
    regionXMaxUI.SetValue(inData.regionXMax);
    regionYMinUI.SetValue(inData.regionYMin);
    regionYMaxUI.SetValue(inData.regionYMax);

    shadeRegionUI.SetValue(inData.shadeRegion);
    shadeIsColorMapUI.SetBoolChoice(inData.shadeIsColorMap);
    colorMapObjRefUI.SetFuncObjRef(inData.colorMapObjRef);
    shadeCMIndexUI.SetFieldValue(inData.shadeCMIndex);
    shadePenUI.SetPenSelection(inData.shadePen);

    labelRegionUI.SetValue(inData.labelRegion);
    labelXPositionUI.SetFieldValue(inData.labelXPosition);
    labelYPositionUI.SetFieldValue(inData.labelYPosition);
    halignUI.SetHAlign(inData.halign);
    valignUI.SetVAlign(inData.valign);
    labelOffsetUI.SetFieldValue(inData.labelOffset);

    labelPenUI.SetPenSelection(inData.labelPen);
    labelFontUI.SetFontSettings(inData.labelFont);
    regionLabelUI.SetFieldString(inData.regionLabel);

    stippleRegionUI.SetValue(inData.stippleRegion);
    regionStippleUI.SetStippleSpec(inData.regionStipple);

    frameRegionUI.SetValue(inData.frameRegion);
    frameLineUI.SetLineSpec(inData.frameLine);

    SetAllSensitive();
}

bool PPO_ShadeRegionPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    SetNeedXY();

    if (needXMin)
        regionXMinUI.UIdataOK(allOK);
    if (needXMax)
        regionXMaxUI.UIdataOK(allOK);
    if (needYMin)
        regionYMinUI.UIdataOK(allOK);
    if (needYMax)
        regionYMaxUI.UIdataOK(allOK);

    if (shadeRegionUI.GetValue() && shadeIsColorMapUI.GetBoolChoice())
    {
        colorMapObjRefUI.FuncObjRefOK(allOK, "shade color map");
        shadeCMIndexUI.FieldValueOK(allOK, "shade color index", 0, 255);
    }

    if (labelRegionUI.GetValue())
    {
        labelXPositionUI.FieldValueOK(allOK, "X Position", 0.0, 100.0);
        labelYPositionUI.FieldValueOK(allOK, "Y Position", 0.0, 100.0);
        labelOffsetUI.FieldValueOK(allOK, "Offset", 0, 100);
    }

    return allOK;
}

void PPO_ShadeRegionPS::GetMenuData(PPO_ShadeRegion&  outData)
{
    PPO_BasePS::GetMenuData(outData);

    outData.xRegionSpec = PFO_ShadeRegion::RegionSpecType(xRegionSpecUI.GetChoice());
    outData.yRegionSpec = PFO_ShadeRegion::RegionSpecType(yRegionSpecUI.GetChoice());
    SetNeedXY();

    if (needXMin)
        regionXMinUI.GetValue(outData.regionXMin);
    if (needXMax)
        regionXMaxUI.GetValue(outData.regionXMax);
    if (needYMin)
        regionYMinUI.GetValue(outData.regionYMin);
    if (needYMax)
        regionYMaxUI.GetValue(outData.regionYMax);

    outData.shadeRegion = shadeRegionUI.GetValue();
    if (outData.shadeRegion)
    {
        outData.shadeIsColorMap = shadeIsColorMapUI.GetBoolChoice();
        if (outData.shadeIsColorMap)
        {
            outData.colorMapObjRef = colorMapObjRefUI.GetFuncObjRef();
            outData.shadeCMIndex = shadeCMIndexUI.GetFieldValue();
        }
        else
            outData.shadePen = shadePenUI.GetPenSelection();
    }

    outData.labelRegion = labelRegionUI.GetValue();
    if (outData.labelRegion)
    {
        outData.labelXPosition = labelXPositionUI.GetFieldValue();
        outData.labelYPosition = labelYPositionUI.GetFieldValue();
        outData.labelOffset = labelOffsetUI.GetFieldValue();
        outData.halign = halignUI.GetHAlign();
        outData.valign = valignUI.GetVAlign();

        outData.labelPen = labelPenUI.GetPenSelection();
        labelFontUI.GetFontSettings(outData.labelFont);
        regionLabelUI.GetFieldString(outData.regionLabel, PFO_ShadeRegion::maxLabelLen);
    }

    outData.stippleRegion = stippleRegionUI.GetValue();
    if (outData.stippleRegion)
    {
        regionStippleUI.GetStippleSpec(outData.regionStipple);
    }

    outData.frameRegion = frameRegionUI.GetValue();
    if (outData.frameRegion)
        frameLineUI.GetLineSpec(outData.frameLine);
}


