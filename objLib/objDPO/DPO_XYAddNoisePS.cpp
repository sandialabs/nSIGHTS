///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYAddNoisePS.cpp
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

#include <genClassPS/PS_UIBase.h>

#include "DPO_XYAddNoise.h"
#include "DPO_XYAddNoisePS.h"

DPO_XYAddNoisePS::DPO_XYAddNoisePS()
{
}

void DPO_XYAddNoisePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_XYAddNoisePS*>(inObj)->SetAllSensitive();
}

void DPO_XYAddNoisePS::SetAllSensitive()
{
    bool isNormalOp = noiseDistributionOpUI.GetChoice();

    stdDeviationField.SetSensitive(isNormalOp);
    noiseRangeField.SetSensitive(!isNormalOp);
}

void DPO_XYAddNoisePS::InitMenu()
{
    const char* operationStrings[] = {"Uniform", "Normal"};

    DPO_XYBasePS::InitMenu("XY Add Noise");

    ArgListC args;

    // frame
    args.Clear();
    args.StdFormAttach();
    addNoiseFrame.InitFramedUI("Add Noise", PPform.GetWidget(), args);

    // add noise operation
    args.Clear();
    args.StdFormAttach();
    noiseDistributionOpUI.PreInitSetUp(operationStrings, 2);
    noiseDistributionOpUI.InitLabeledUI("Noise Distribution", addNoiseFrame.GetForm(), args);
    noiseDistributionOpUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    // Std Deviation
    args.Clear();
    args.StdTopObjectAttach(noiseDistributionOpUI);
    stdDeviationField.InitLabeledUI("Standard Deviation", addNoiseFrame.GetForm(), args);

    // Noise Range
    args.Clear();
    args.StdTopObjectAttach(stdDeviationField);
    noiseRangeField.InitLabeledUI("Noise Range", addNoiseFrame.GetForm(), args);

    // Random Seed
    args.Clear();
    args.StdTopObjectAttach(noiseRangeField);
    randomSeedField.InitLabeledUI("Random Seed", addNoiseFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(addNoiseFrame);
    SetOutputDescUI(args);
}

void DPO_XYAddNoisePS::MenuOpen(DPO_XYAddNoise& inData)
{
    DPO_XYBasePS::MenuOpen(inData);
}


void DPO_XYAddNoisePS::SetMenuData(DPO_XYAddNoise&  inData)
{
    DPO_XYBasePS::SetMenuData(inData, inData);

    noiseDistributionOpUI.SetChoice(inData.noiseDistributionOp);
    stdDeviationField.SetFieldValue(inData.stdDeviation);
    noiseRangeField.SetFieldValue(inData.noiseRange);
    randomSeedField.SetFieldValue(inData.randomSeed);

    SetAllSensitive();
}

bool DPO_XYAddNoisePS::UIdataOK()
{
    DPO_XYBasePS::UIdataOK();
    if (noiseDistributionOpUI.GetChoice())
        stdDeviationField.FieldValueOK(allOK, "Standard Deviation");
    else
        noiseRangeField.FieldValueOK(allOK, "Noise Range");
    randomSeedField.FieldValueOK(allOK, "Random Seed", 0, INT_MAX);
    return allOK;
}

void DPO_XYAddNoisePS::GetMenuData(DPO_XYAddNoise&  outData)
{
    DPO_XYBasePS::GetMenuData(outData, outData);
    outData.noiseDistributionOp = DFO_XYAddNoise::AddNoiseOp(noiseDistributionOpUI.GetChoice());
    if (outData.noiseDistributionOp == DFO_XYAddNoise::anUniform)
        outData.noiseRange = noiseRangeField.GetFieldValue();
    else
        outData.stdDeviation = stdDeviationField.GetFieldValue();
    outData.randomSeed = randomSeedField.GetFieldValue();
}

