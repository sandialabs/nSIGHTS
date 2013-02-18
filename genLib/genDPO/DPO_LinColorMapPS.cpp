///////////////////////////////////////////////////////////////////////////////////
//
// DPO_LinColorMapPS.cpp
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
#include <stdlib.h>

#include <genClassPS/PS_Color.h>

#include <genApp/U_Menu.h>
#include <genClass/U_Str.h>

#include "DPO_LinColorMapPS.h"


// ****************************************

void ColorMapSliderUIC :: InitLabeledUI(      char              label[],
                                              Widget             parent,
                                        const ArgListC&          inArgs,
                                              DPO_LinColorMapPS* cbObj)

{
    BaseUIC::InitLabeledUI(label, parent, inArgs);

    ArgListC args;
    args.BottomFormAttach();
    args.RightOffset(10);
    args.SetPlaceHolder();

    args.LeftPosAttach(10);
    args.RightPosAttach(55);
    startVal.InitFormUI(form.GetWidget(), args);

    args.ResetPlace();
    args.LeftUICAttach(startVal);
    args.RightFormAttach();
    args.LeftOffset(10);
    endVal.InitFormUI(form.GetWidget(), args);

    startVal.valSlider.AddOtherObjectCallback(DPO_LinColorMapPS::RedrawColorMapCB, cbObj);
    startVal.valDisplay.AddOtherObjectCallback(DPO_LinColorMapPS::RedrawColorMapCB, cbObj);
    endVal.valSlider.AddOtherObjectCallback(DPO_LinColorMapPS::RedrawColorMapCB, cbObj);
    endVal.valDisplay.AddOtherObjectCallback(DPO_LinColorMapPS::RedrawColorMapCB, cbObj);

}

void DPO_LinColorMapPS::RedrawColorMap()
{
    SC_ColorSpec mapStart, mapEnd;
    rMap.GetStartEnd(mapStart.RH, mapEnd.RH);
    gMap.GetStartEnd(mapStart.GS, mapEnd.GS);
    bMap.GetStartEnd(mapStart.BV, mapEnd.BV);

    DC_ColorMap newMap = colorMapDraw.colorMap;
    newMap.ncolorMap = nShades.valSlider.GetValue();

    if (useHSV.GetBoolChoice())
        newMap.MakeLinearHSVMap(mapStart, mapEnd);
    else
        newMap.MakeLinearRGBMap(mapStart, mapEnd);

    colorMapDraw.DrawColorMap(newMap);

}
void DPO_LinColorMapPS::RedrawColorMapCB(void* inObj)
{
    static_cast<DPO_LinColorMapPS*>(inObj)->RedrawColorMap();
}


// updates row labels based on HSV/RGB toggle

void DPO_LinColorMapPS::SetRowLabels()
{
    // updates row labels based on HSV/RGB toggle
    BaseUIC* sliderLabels[3];
    sliderLabels[0] = &rMap;
    sliderLabels[1] = &gMap;
    sliderLabels[2] = &bMap;
    PropSetColorLabels(sliderLabels, useHSV.GetBoolChoice());
}

void  DPO_LinColorMapPS::UseHSV()
{
    SetRowLabels();
    RedrawColorMap();
}

void DPO_LinColorMapPS::UseHSVCB(void* inObj)
{
    static_cast<DPO_LinColorMapPS*>(inObj)->UseHSV();
}


void DPO_LinColorMapPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC   args;

    args.AllFormAttach();
    useHSV.rightOffset = 45;
    useHSV.PreInitSetUp("Red/Green/Blue", "Hue/Sat/Val", true);
    useHSV.InitFramedUI("Color Specification Method", URform.GetWidget(), args);
    useHSV.AddOtherObjectCallback(UseHSVCB, this);

    args.Clear();
    args.LeftFormAttach();
    args.RightFormAttach();
    args.SetPlaceHolder();
    args.BottomFormAttach();
    sliderFrame.InitFramedUI("Colour Map Limits", PPform.GetWidget(), args);

    bMap.InitLabeledUI("Blue   ", sliderFrame.GetForm(), args, this);

    args.ResetPlace();
    args.BottomUICAttach(bMap);
    gMap.InitLabeledUI("Green  ", sliderFrame.GetForm(), args, this);

    args.ResetPlace();
    args.BottomUICAttach(gMap);
    rMap.InitLabeledUI("Red    ", sliderFrame.GetForm(), args, this);

    //  form for labels
    args.ResetPlace();
    args.BottomUICAttach(rMap);
    labForm.InitFormUI(sliderFrame.GetForm(), args);

    //  labels
    args.Clear();
    args.BottomFormAttach();
    args.LeftPosAttach(15);
    startLab.StdLabelInit("Map Start", labForm.GetForm(), args);

    args.Clear();
    args.BottomFormAttach();
    args.LeftPosAttach(55);
    endLab.StdLabelInit("Map End", labForm.GetForm(), args);

    //  ncolors
    args.Clear();
    args.LeftFormAttach();
    args.RightFormAttach();
    args.BottomUICAttach(sliderFrame);

    nShades.SetSliderVals(3.0, double(DC_ColorMap::maxCMcolors), 3, DC_ColorMap::maxCMcolors, 20);
    nShades.InitFramedUI("Number of Colors in Map", PPform.GetWidget(), args);
    nShades.valSlider.AddOtherObjectCallback(RedrawColorMapCB, this);
    nShades.valDisplay.AddOtherObjectCallback(RedrawColorMapCB, this);
    nShades.valDisplay.SetFieldFormat(SC_DecFormat(0));

    args.Clear();
    args.StdFormAttach();
    args.BottomUICAttach(nShades);
    args.LeftOffset(3);
    args.RightOffset(3);
    args.BottomOffset(3);
    colorMapDraw.InitWidget(PPform.GetWidget(), args);

    SetMainformScrolled(true, 520, 250);
}

void DPO_LinColorMapPS::SetMenuData(DPO_LinColorMap&    inData)
{
    useHSV.SetBoolChoice(inData.useHSVspec);
    nShades.SetValues(inData.nmapShades);
    rMap.SetStartEnd(inData.mapStart.RH, inData.mapEnd.RH);
    gMap.SetStartEnd(inData.mapStart.GS, inData.mapEnd.GS);
    bMap.SetStartEnd(inData.mapStart.BV, inData.mapEnd.BV);

    SetRowLabels();
    RedrawColorMap();

    MenuBaseC::SetMenuData(inData);
}

bool DPO_LinColorMapPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    return allOK;
}

void DPO_LinColorMapPS::GetMenuData(DPO_LinColorMap&    outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.useHSVspec  = useHSV.GetBoolChoice();
    outData.nmapShades = nShades.valSlider.GetValue();
    rMap.GetStartEnd(outData.mapStart.RH, outData.mapEnd.RH);
    gMap.GetStartEnd(outData.mapStart.GS, outData.mapEnd.GS);
    bMap.GetStartEnd(outData.mapStart.BV, outData.mapEnd.BV);
}

// kluge -- doesn't clear at all - sets end to start
void DPO_LinColorMapPS::MenuClear()
{
    SC_ColorSpec    mapStart;
    SC_ColorSpec    mapEnd;

    rMap.GetStartEnd(mapStart.RH, mapEnd.RH);
    gMap.GetStartEnd(mapStart.GS, mapEnd.GS);
    bMap.GetStartEnd(mapStart.BV, mapEnd.BV);

    rMap.SetStartEnd(mapStart.RH, mapStart.RH);
    gMap.SetStartEnd(mapStart.GS, mapStart.GS);
    bMap.SetStartEnd(mapStart.BV, mapStart.BV);

    RedrawColorMap();
}
void DPO_LinColorMapPS::MenuDefault()
{
    MenuBaseC::MenuDefault();
    useHSV.SetBoolChoice(true);
    nShades.SetValues(50);
    rMap.SetStartEnd(0.0, 0.85);
    gMap.SetStartEnd(1.0, 1.0);
    bMap.SetStartEnd(1.0, 1.0);
    SetRowLabels();
    RedrawColorMap();
}

