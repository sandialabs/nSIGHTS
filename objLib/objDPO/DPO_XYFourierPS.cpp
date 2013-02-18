///////////////////////////////////////////////////////////////////////////////////
//
// DPO_XYFourierPS.cpp
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

#include "DPO_XYFourier.h"
#include "DPO_XYFourierPS.h"

DPO_XYFourierPS::DPO_XYFourierPS()
{
}

void DPO_XYFourierPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_XYFourierPS*>(inObj)->SetAllSensitive();
}

void DPO_XYFourierPS::SetAllSensitive()
{
    bool doInverse = forwardFFTUI.GetBoolChoice();
    scaleInverseYUI.SetSensitive(doInverse);
    inverseXScaleUI.SetSensitive(doInverse);
}

void DPO_XYFourierPS::InitMenu()
{

    DPO_XYBasePS::InitMenu("XY Fourier Transform");

    ArgListC args;

    // frame
    args.Clear();
    args.StdFormAttach();
    args.RightOffset(50);
    forwardFFTUI.PreInitSetUp("   Forward FFT   ", "   Inverse FFT   ");  // reversed
    forwardFFTUI.InitFramedUI("Operation", PPform.GetWidget(), args);
    forwardFFTUI.AddOtherObjectCallback(SetAllSensitiveCB, this);


    args.Clear();
    args.StdTopObjectAttach(forwardFFTUI);
    optionFrame.InitFramedUI("Inversion Options", PPform.GetWidget(), args);

    args.StdToggleSetup(5);
    scaleInverseYUI.StdToggleInit("Scale Y by 2/n", optionFrame.GetForm(), args);

    args.StdSplitSetup();
    args.StdSplitFinish();
    inverseXScaleUI.InitLabeledUI("X Scaling", optionFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(optionFrame);

    SetOutputDescUI(args);

    args.Clear();
    args.StdTopObjectAttach(outputDescUI);
    nfftPoints.InitFramedUI("# of FFT points", PPform.GetWidget(), args);
}

void DPO_XYFourierPS::MenuOpen(DPO_XYFourier& inData)
{
    DPO_XYBasePS::MenuOpen(inData);
}


void DPO_XYFourierPS::SetMenuData(DPO_XYFourier&  inData)
{
    DPO_XYBasePS::SetMenuData(inData, inData);

    forwardFFTUI.SetBoolChoice(!inData.forwardFFT);
    scaleInverseYUI.SetValue(inData.scaleInverseY);
    inverseXScaleUI.SetFieldValue(inData.inverseXScale);

    if (inData.StatusOK())
    {
        nfftPoints.SetFieldValue(inData.outputXY.Size());
    }
    else
    {
        nfftPoints.SetStatusNA();
    }

    SetAllSensitive();
}

bool DPO_XYFourierPS::UIdataOK()
{
    MenuBaseC::UIdataOK();

    if (forwardFFTUI.GetBoolChoice())
        inverseXScaleUI.FieldValueOK(allOK, "X Scaling");

    return allOK;
}

void DPO_XYFourierPS::GetMenuData(DPO_XYFourier&  outData)
{
    DPO_XYBasePS::GetMenuData(outData, outData);
    outData.forwardFFT = !forwardFFTUI.GetBoolChoice();
    if (!outData.forwardFFT)
    {
        outData.scaleInverseY = scaleInverseYUI.GetValue();
        outData.inverseXScale = inverseXScaleUI.GetFieldValue();
    }

}

