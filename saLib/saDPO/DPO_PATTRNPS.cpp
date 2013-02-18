///////////////////////////////////////////////////////////////////////////////////
//
// DPO_PATTRNPS.cpp
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "SaLibAfx.h"

#include <genClassPS/PS_UIBase.h>

#include "DPO_PATTRNPS.h"


void DPO_PATTRNPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_PATTRNPS*>(inObj)->SetAllSensitive();
}


void DPO_PATTRNPS::SetSel()
{
    testSel.Alloc(DFO_PATTRN::ptLastTest);
    for (int i = 0; i < DFO_PATTRN::ptLastTest; i++)
        testSel[i] = testsUI[i].GetValue();
}

bool DPO_PATTRNPS::NreplicatesSel()
{
    return true;

//  return  testSel[DFO_PATTRN::ptDR] ||
//          testSel[DFO_PATTRN::ptFR]  ||
//          MonteCarloSel();
}
bool DPO_PATTRNPS::NxintSel()
{
    return true;

/*  return  testSel[DFO_PATTRN::ptCIQ] ||
            testSel[DFO_PATTRN::ptCIQMC] ||
            testSel[DFO_PATTRN::ptCL] ||
            testSel[DFO_PATTRN::ptCLMC] ||
            testSel[DFO_PATTRN::ptCMD] ||
            testSel[DFO_PATTRN::ptCMDMC] ||
            testSel[DFO_PATTRN::ptCMN] ||
            testSel[DFO_PATTRN::ptCMNMC] ||
            testSel[DFO_PATTRN::ptCV] ||
            testSel[DFO_PATTRN::ptCVMC] ||
            testSel[DFO_PATTRN::ptSI] ||
            testSel[DFO_PATTRN::ptSIMC]; */

}
bool DPO_PATTRNPS::NyintSel()
{
    return true;

//  return  testSel[DFO_PATTRN::ptSI] ||
//          testSel[DFO_PATTRN::ptSIMC];

}
bool DPO_PATTRNPS::MonteCarloSel()
{
    return  testSel[DFO_PATTRN::ptCCMC] ||
            testSel[DFO_PATTRN::ptCIQMC] ||
            testSel[DFO_PATTRN::ptCLMC] ||
            testSel[DFO_PATTRN::ptCMDMC] ||
            testSel[DFO_PATTRN::ptCMNMC] ||
            testSel[DFO_PATTRN::ptCVMC] ||
            testSel[DFO_PATTRN::ptDRMC] ||
            testSel[DFO_PATTRN::ptFRMC] ||
            testSel[DFO_PATTRN::ptRCCMC] ||
            testSel[DFO_PATTRN::ptSIMC];

}


bool DPO_PATTRNPS::LogarithmsSel()
{
    return  testSel[DFO_PATTRN::ptCMN] ||
            testSel[DFO_PATTRN::ptCMNMC] ||
            testSel[DFO_PATTRN::ptCV] ||
            testSel[DFO_PATTRN::ptCVMC];
}

bool DPO_PATTRNPS::AlphaCutoffSel()
{
    for (int i = 0; i < DFO_PATTRN::ptLastTest; i++)
    {
        if ((i == DFO_PATTRN::ptPCC) || (i == DFO_PATTRN::ptPRCC) ||
            (i == DFO_PATTRN::ptSRC) ||(i == DFO_PATTRN::ptSRRC))
            continue;

        if (testSel[i])
            return true;
    }

    return false;
}

void DPO_PATTRNPS::SetAllSensitive()
{
    SetSel();
    nreplicatesUI.SetSensitive(NreplicatesSel());
    nxintUI.SetSensitive(NxintSel());
    nyintUI.SetSensitive(NyintSel());
    randSeedUI.SetSensitive(MonteCarloSel());
    nrandomPatternsUI.SetSensitive(MonteCarloSel());
    logarithmsUI.SetSensitive(LogarithmsSel());
    alphaCutoffUI.SetSensitive(AlphaCutoffSel());

    verboseMCUI.SetSensitive(MonteCarloSel());
    verboseCVUI.SetSensitive(testSel[DFO_PATTRN::ptCV] || testSel[DFO_PATTRN::ptCVMC]);
    coefficientsUI.SetSensitive(testSel[DFO_PATTRN::ptCC] || testSel[DFO_PATTRN::ptPCC]
                                || testSel[DFO_PATTRN::ptSRC]);

    rankCoefficientsUI.SetSensitive(testSel[DFO_PATTRN::ptRCC] || testSel[DFO_PATTRN::ptPRCC]
                                || testSel[DFO_PATTRN::ptSRRC]);


    mcRepStrUI.SetSensitive(MonteCarloSel() && verboseMCUI.GetValue());
}

void DPO_PATTRNPS::InitMenu()
{
    DPO_SAObjBasePS::InitMenu();

    ArgListC args;

    args.StdTopObjectAttach(varForm.GetWidget());

    obsStrUI.AttachObjectToFrame();
    obsStrUI.SetFieldLen(DFO_PATTRN::inpStrLen);
    obsStrUI.InitFramedUI("Observations", PPform.GetWidget(), args);

    args.StdTopObjectAttach(obsStrUI);
    testFrame.InitFramedUI("PATTRN Tests", PPform.GetWidget(), args);



    int ptIndx = 0;
    args.StdFormAttach();
    for (int i = 0; i < 4; i++)
    {
        testForm[i].InitWidget(testFrame.GetForm(), args);
        for (int j = 0; j < 6; j++)
        {
            args.StdEvenSplitSetup(j, 6);
            args.LeftOffset(5);
            testsUI[ptIndx].StdToggleInit(DFO_PATTRN::testStrs[ptIndx], testForm[i].GetWidget(), args);
            testsUI[ptIndx].AddOtherObjectCallback(SetAllSensitiveCB, this);
            ptIndx++;
        }
        args.StdTopObjectAttach(testForm[i].GetWidget());
    }

    args.StdTopObjectAttach(testFrame);
    parForm1.InitWidget(PPform.GetWidget(), args);
    args.StdEvenSplitSetup(0, 4);
    nreplicatesUI.InitFramedUI("# replicates", parForm1.GetWidget(), args);
    args.StdEvenSplitSetup(1, 4);
    nxintUI.InitFramedUI("NXINT", parForm1.GetWidget(), args);
    args.StdEvenSplitSetup(2, 4);
    nyintUI.InitFramedUI("NYINT", parForm1.GetWidget(), args);
    args.StdEvenSplitSetup(3, 4);
    randSeedUI.InitFramedUI("RNG Seed", parForm1.GetWidget(), args);

    args.StdTopObjectAttach(parForm1.GetWidget());
    parForm2.InitWidget(PPform.GetWidget(), args);

    args.StdEvenSplitSetup(0, 4);
    nrandomPatternsUI.InitFramedUI("# random patterns", parForm2.GetWidget(), args);

    args.StdEvenSplitSetup(1, 4);
    alphaCutoffUI.InitFramedUI("Alpha Cutoff", parForm2.GetWidget(), args);

    args.StdEvenSplitSetup(2, 4);
    mtest1UI.PreInitSetUp(DFO_PATTRN::testStrs, DFO_PATTRN::ptLastTest);
    mtest1UI.InitFramedUI("Sorting Test", parForm2.GetWidget(), args);

    args.StdEvenSplitSetup(3, 4);
    optionForm.InitFramedUI("Options", parForm2.GetWidget(), args);

    args.StdToggleSetup(5);
    logarithmsUI.StdToggleInit("LOGARITHM", optionForm.GetForm(), args);

    args.StdTopObjectAttach(parForm2.GetWidget());
    outputFrame.InitFramedUI("Output Control", PPform.GetWidget(), args);

    args.StdToggleSetup(2);
    verboseUI.StdToggleInit("VERBOSE", outputFrame.GetForm(), args);

    args.StdToggleSetup(20);
    verboseMCUI.StdToggleInit("VERBOSEMC", outputFrame.GetForm(), args);
    verboseMCUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(40);
    verboseCVUI.StdToggleInit("VERBOSECV", outputFrame.GetForm(), args);

    args.StdToggleSetup(60);
    coefficientsUI.StdToggleInit("COEFF.", outputFrame.GetForm(), args);

    args.StdToggleSetup(80);
    rankCoefficientsUI.StdToggleInit("RANK COEFF.", outputFrame.GetForm(), args);

    args.StdTopObjectAttach(outputFrame);
    mcRepStrUI.AttachObjectToFrame();
    mcRepStrUI.SetFieldLen(DFO_PATTRN::inpStrLen);
    mcRepStrUI.InitFramedUI("MC Realizations to Report", PPform.GetWidget(), args);

    args.StdTopObjectAttach(mcRepStrUI);
    coeffFormatUI.InitFramedUI("Coefficient Listing Format", PPform.GetWidget(), args);
}

void DPO_PATTRNPS::MenuOpen(DPO_PATTRN& inData)
{
    DPO_SAObjBasePS::MenuOpen(inData);
}

void DPO_PATTRNPS::SetMenuData(DPO_PATTRN&  inData)
{
    MenuBaseC::SetMenuData(inData);
    DPO_SAObjBasePS::SetMenuData(inData);

    obsStrUI.SetFieldString(inData.obsStr);

    for (int i = 0; i < DFO_PATTRN::ptLastTest; i++)
        testsUI[i].SetValue(inData.tests[i]);

    nreplicatesUI.SetFieldValue(inData.nreplicates);
    nxintUI.SetFieldValue(inData.nxintInp);
    nyintUI.SetFieldValue(inData.nyintInp);
    randSeedUI.SetFieldValue(inData.randSeed);

    nrandomPatternsUI.SetFieldValue(inData.nrandomPatterns);
    alphaCutoffUI.SetFieldValue(inData.alphaCutoff);
    mtest1UI.SetChoice(inData.mtest1);
    logarithmsUI.SetValue(inData.logarithms);

    verboseUI.SetValue(inData.verbose);
    verboseMCUI.SetValue(inData.verboseMC);
    verboseCVUI.SetValue(inData.verboseCV);
    coefficientsUI.SetValue(inData.coefficients);
    rankCoefficientsUI.SetValue(inData.rankCoefficients);

    coeffFormatUI.SetRealFormatSettings(inData.coeffFormat);

    mcRepStrUI.SetFieldString(inData.mcRepStr);

    SetAllSensitive();
}

bool DPO_PATTRNPS::UIdataOK()
{
    DPO_SAObjBasePS::UIdataOK();

    SetSel();

    if (NreplicatesSel())
        nreplicatesUI.FieldValueOK(allOK, "NREPLICATES", 1, 10000);
    if (NxintSel())
        nxintUI.FieldValueOK(allOK, "NXINT", 1, 10000);
    if (NyintSel())
        nyintUI.FieldValueOK(allOK, "NYINT", 1, 10000);
    if (MonteCarloSel())
        randSeedUI.FieldValueOK(allOK, "RNG Seed", -2000000000, 2000000000);

    if (MonteCarloSel())
        nrandomPatternsUI.FieldValueOK(allOK, "N RANDOM PATTERNS", 1, 10000);

    if (AlphaCutoffSel())
        alphaCutoffUI.FieldValueOK(allOK, "ALPHA_CUTOFF", 0.0, 1.0);

    return allOK;
}

void DPO_PATTRNPS::GetMenuData(DPO_PATTRN&  outData)
{
    MenuBaseC::GetMenuData(outData);
    DPO_SAObjBasePS::GetMenuData(outData);

    obsStrUI.GetFieldString(outData.obsStr, DFO_PATTRN::inpStrLen);

    SetSel();
    for (int i = 0; i < DFO_PATTRN::ptLastTest; i++)
        outData.tests[i] = testSel[i];


    if (NreplicatesSel())
        outData.nreplicates = nreplicatesUI.GetFieldValue();
    if (NxintSel())
        outData.nxintInp = nxintUI.GetFieldValue();
    if (NyintSel())
        outData.nyintInp = nyintUI.GetFieldValue();
    if (MonteCarloSel())
        outData.randSeed = randSeedUI.GetFieldValue();

    if (MonteCarloSel())
        outData.nrandomPatterns = nrandomPatternsUI.GetFieldValue();

    if (AlphaCutoffSel())
        outData.alphaCutoff = alphaCutoffUI.GetFieldValue();

    if (LogarithmsSel())
        outData.logarithms = logarithmsUI.GetValue();

    outData.verbose = verboseUI.GetValue();
    if (MonteCarloSel())
    {
        outData.verboseMC = verboseMCUI.GetValue();
        if (outData.verboseMC)
            mcRepStrUI.GetFieldString(outData.mcRepStr, DFO_PATTRN::inpStrLen);
    }

    if (testSel[DFO_PATTRN::ptCV] || testSel[DFO_PATTRN::ptCVMC])
        outData.verboseCV = verboseCVUI.GetValue();

    if (testSel[DFO_PATTRN::ptCC] || testSel[DFO_PATTRN::ptPCC] || testSel[DFO_PATTRN::ptSRC])
        outData.coefficients = coefficientsUI.GetValue();

    if (testSel[DFO_PATTRN::ptRCC] || testSel[DFO_PATTRN::ptPRCC] || testSel[DFO_PATTRN::ptSRRC])
        outData.rankCoefficients = rankCoefficientsUI.GetValue();

    outData.mtest1 = mtest1UI.GetChoice();


    coeffFormatUI.GetRealFormatSettings(outData.coeffFormat);

}

