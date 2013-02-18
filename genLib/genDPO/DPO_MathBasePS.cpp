///////////////////////////////////////////////////////////////////////////////////
//
// DPO_MathBasePS.cpp
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

#include <genClassPS/PS_UIBase.h>

#include "DPO_MathBasePS.h"

DPO_MathBasePS::DPO_MathBasePS()
{
}


void DPO_MathBasePS::InitMenu(FuncObjRefUIC& inputAUI,
                              FuncObjRefUIC& inputBUI)
{
    InitABUI(inputAUI, inputBUI);

    ArgListC args;
    args.StdTopObjectAttach(topRowForm.GetWidget());
    InitOpUI(args);

    args.Clear();
    args.StdTopObjectAttach(mathOpUI);
    InitDescUI(args);
}


void DPO_MathBasePS::InitABUI(FuncObjRefUIC& inputAUI,
                              FuncObjRefUIC& inputBUI)
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    inputAUI.InitFramedUI("Data Source A", topRowForm.GetWidget(), args);

    args.StdSplitFinish();
    inputBUI.InitFramedUI("Data Source B", topRowForm.GetWidget(), args);
}


void DPO_MathBasePS::InitOpUI(ArgListC&  args)
{
    args.RightOffset(50);

    static const char* mathOpLabels[]  = {"   A + B    ",
                                          "   A - B    ",
                                          "   A * B    ",
                                          "   A / B    ",
                                          "   Max(A,B) ",
                                          "   Min(A,B) "};

    mathOpUI.PreInitSetUp(mathOpLabels, 6, true);
    mathOpUI.InitFramedUI("Operation", PPform.GetWidget(), args);
}

void  DPO_MathBasePS::InitDescUI(const ArgListC&  args)
{
    outputTypeDescUI.SetFieldLen(40);
    outputTypeDescUI.InitFramedUI("Output Description", PPform.GetWidget(), args);
}


void DPO_MathBasePS::SetMenuData(DFO_MathBase&  inData)
{
    mathOpUI.SetChoice(int(inData.mathOp));
    outputTypeDescUI.SetFieldString(inData.outputTypeDesc);
}


void DPO_MathBasePS::GetMenuData(DFO_MathBase&  outData)
{
    outData.mathOp    = DFO_MathBase::OpType(mathOpUI.GetChoice());
    outputTypeDescUI.GetFieldString(outData.outputTypeDesc, DFO_MathBase::descLen);
}

