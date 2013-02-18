///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableSamplePS.cpp
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

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>
#include <genClass/DO_XYCDF.h>
#include <objClass/DO_SampleTable.h>

#include "DPO_TableSample.h"
#include "DPO_TableSamplePS.h"


DPO_TableSamplePS::DPO_TableSamplePS()
    :   existingTableObjRefUI(typeid(DO_SampleTable)),
        userCDFInputObjRefUI(typeid(DO_XYCDF))

{
}

void DPO_TableSamplePS::SetAllSensitive()
{
    bool createNew = createNewTableUI.GetBoolChoice();
    topRowForm.SetSensitive(createNew);
    existingTableObjRefUI.SetSensitive(!createNew);

    SampleDistributionBase::SVDistribution currDist =
        SampleDistributionBase::SVDistribution(svDistributionUI.GetChoice());

    distMeanUI.SetSensitive(SampleDistributionBase::UsesMean(currDist));
    distStdDevUI.SetSensitive(SampleDistributionBase::UsesStdDev(currDist) &&
                                SampleDistributionBase::IsLinear(currDist));
    distStdDevLogUI.SetSensitive(SampleDistributionBase::IsNormal(currDist) &&
                                SampleDistributionBase::IsLog(currDist));;

    poissonLambdaUI.SetSensitive(SampleDistributionBase::IsPoisson(currDist));

    weibullShapeUI.SetSensitive(SampleDistributionBase::IsWeibull(currDist));
    weibullScaleUI.SetSensitive(SampleDistributionBase::IsWeibull(currDist));

    distMinUI.SetSensitive(SampleDistributionBase::UsesMinMax(currDist));
    distPeakUI.SetSensitive(SampleDistributionBase::IsTriangular(currDist));
    distMaxUI.SetSensitive(SampleDistributionBase::UsesMinMax(currDist));

    betaPUI.SetSensitive(SampleDistributionBase::IsBeta(currDist));
    betaQUI.SetSensitive(SampleDistributionBase::IsBeta(currDist));

    userCDFInputObjRefUI.SetSensitive(SampleDistributionBase::IsUserCDF(currDist));

    tableForm.SetSensitive(SampleDistributionBase::IsTableLookup(currDist));
    if (SampleDistributionBase::IsTableLookup(currDist))
        tableLookupUI.SetSensitive();
}

void DPO_TableSamplePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_TableSamplePS*>(inObj)->SetAllSensitive();
}

void DPO_TableSamplePS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    createNewTableUI.PreInitSetUp("Add to Existing", "Create New Table");
    createNewTableUI.InitFramedUI("Sample Table Type", URform.GetWidget(), args);
    createNewTableUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    nsamplesUI.InitFramedUI("Number of Samples", topRowForm.GetWidget(), args);

    args.StdSplitFinish();
    randomSeedUI.InitFramedUI("Random # Seed", topRowForm.GetWidget(), args);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    existingTableObjRefUI.InitFramedUI("Existing Sample Table", PPform.GetWidget(), args);

    args.StdTopObjectAttach(existingTableObjRefUI);
    thirdRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    stratifySampleUI.PreInitSetUp("Random", "Stratified");
    stratifySampleUI.InitFramedUI("Sampling Method", thirdRowForm.GetWidget(), args);

    args.StdSplitFinish();
    randomMixUI.PreInitSetUp("None (As Sampled)", "Randomize");
    randomMixUI.InitFramedUI("Post-Processing", thirdRowForm.GetWidget(), args);

    args.StdTopObjectAttach(thirdRowForm.GetWidget());
    sampleFrame.InitFramedUI("Sampling", PPform.GetWidget(), args);

    args.StdFormAttach();
    lrForm.InitWidget(sampleFrame.GetForm(), args);

    args.StdSplitSetup();
    leftForm.InitWidget(lrForm.GetWidget(), args);

    args.StdSplitFinish();
    rightForm.InitWidget(lrForm.GetWidget(), args);

    args.StdFormAttach();
    svDistributionUI.PreInitSetUp(SC_SampVar::GetDistStrings());
    svDistributionUI.InitLabeledUI("Distribution", leftForm.GetWidget(), args);
    svDistributionUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdTopObjectAttach(svDistributionUI);

    distMeanUI.SetFieldLen(12);
    distMeanUI.InitLabeledUI("Mean", leftForm.GetWidget(), args);

    args.StdTopObjectAttach(distMeanUI);
    distStdDevUI.SetFieldLen(12);
    distStdDevUI.InitLabeledUI("Std Dev", leftForm.GetWidget(), args);

    args.StdTopObjectAttach(distStdDevUI);
    distStdDevLogUI.SetFieldLen(12);
    distStdDevLogUI.InitLabeledUI("Log Std Dev", leftForm.GetWidget(), args);

    args.StdTopObjectAttach(distStdDevLogUI);
    distMinUI.SetFieldLen(12);
    distMinUI.InitLabeledUI("Minimum", leftForm.GetWidget(), args);

    args.StdTopObjectAttach(distMinUI);
    distPeakUI.SetFieldLen(12);
    distPeakUI.InitLabeledUI("Peak", leftForm.GetWidget(), args);

    args.StdTopObjectAttach(distPeakUI);
    distMaxUI.SetFieldLen(12);
    distMaxUI.InitLabeledUI("Maximum", leftForm.GetWidget(), args);

    args.StdFormAttach();
    poissonLambdaUI.SetFieldLen(12);
    poissonLambdaUI.InitLabeledUI("Poisson mean", rightForm.GetWidget(), args);

    args.StdTopObjectAttach(poissonLambdaUI);
    weibullShapeUI.SetFieldLen(12);
    weibullShapeUI.InitLabeledUI("Shape (alpha)", rightForm.GetWidget(), args);

    args.StdTopObjectAttach(weibullShapeUI);
    weibullScaleUI.SetFieldLen(12);
    weibullScaleUI.InitLabeledUI("Scale (beta)", rightForm.GetWidget(), args);

    args.StdTopObjectAttach(weibullScaleUI);
    betaPUI.SetFieldLen(12);
    betaPUI.InitLabeledUI("P exponent (alpha)", rightForm.GetWidget(), args);

    args.StdTopObjectAttach(betaPUI);
    betaQUI.SetFieldLen(12);
    betaQUI.InitLabeledUI("Q exponent (beta)", rightForm.GetWidget(), args);

    args.StdTopObjectAttach(lrForm.GetWidget());
    userCDFInputObjRefUI.AttachObjectToLabel();
    userCDFInputObjRefUI.InitLabeledUI("User CDF", sampleFrame.GetForm(), args);

    args.StdTopObjectAttach(userCDFInputObjRefUI);
    tableForm.InitWidget(sampleFrame.GetForm(), args);

    args.StdSplitSetup();
    tableLookupUI.ObjRefInit(tableForm.GetWidget(), args);
    args.StdSplitFinish();
    tableLookupUI.SelColInit(tableForm.GetWidget(), args);



}

void DPO_TableSamplePS::SetMenuData(DPO_TableSample&  inData)
{
    MenuBaseC::SetMenuData(inData);

    createNewTableUI.SetBoolChoice(inData.createNewTable);
    nsamplesUI.SetFieldValue(inData.nsamples);
    randomSeedUI.SetFieldValue(inData.randomSeed);
    existingTableObjRefUI.SetFuncObjRef(inData.existingTableObjRef);
    stratifySampleUI.SetBoolChoice(inData.stratifySample);
    randomMixUI.SetBoolChoice(inData.randomMix);

    SC_SampVar& currSamp = inData.sampVar;
    svDistributionUI.SetChoice(currSamp.svDistribution);

    distMeanUI.SetFieldValue(currSamp.distMean);
    distStdDevUI.SetFieldValue(currSamp.distStdDev);
    distStdDevLogUI.SetFieldValue(currSamp.distLogStdDev);
    poissonLambdaUI.SetFieldValue(int(currSamp.poissonLambda));

    weibullShapeUI.SetFieldValue(currSamp.weibullShape);
    weibullScaleUI.SetFieldValue(currSamp.weibullScale);
    betaPUI.SetFieldValue(currSamp.betaP);
    betaQUI.SetFieldValue(currSamp.betaQ);

    distMinUI.SetFieldValue(currSamp.distMin);
    distPeakUI.SetFieldValue(currSamp.distPeak);
    distMaxUI.SetFieldValue(currSamp.distMax);
    userCDFInputObjRefUI.SetFuncObjRef(currSamp.userCDFInputObjRef);

    DSC_TableColSel inTbl;
    inTbl.tableInputObjRef = currSamp.tableLookupObjRef;
    inTbl.selectedCol = currSamp.tableLookupColumn;
    tableLookupUI.SetTableColSel(inTbl);

    SetAllSensitive();
}


void DPO_TableSamplePS::MenuOpen(DPO_TableSample& inData)
{
    MenuBaseC::MenuOpen(false, false);
    existingTableObjRefUI.StdMenuInit(inData.existingTableObjRef);
    userCDFInputObjRefUI.StdMenuInit(inData.sampVar.userCDFInputObjRef);
    DSC_TableColSel inTbl;
    inTbl.tableInputObjRef = inData.sampVar.tableLookupObjRef;
    inTbl.selectedCol = inData.sampVar.tableLookupColumn;
    tableLookupUI.MenuOpen(inTbl);

}


bool DPO_TableSamplePS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    if (createNewTableUI.GetBoolChoice())
    {
        nsamplesUI.FieldValueOK(allOK, "# of samples", 5, 100000);
        randomSeedUI.FieldValueOK(allOK, "random seed", 0, 100000000);
    }
    else
        existingTableObjRefUI.FuncObjRefOK(allOK, "Input table");


    SampleDistributionBase::SVDistribution currDist =
        SampleDistributionBase::SVDistribution(svDistributionUI.GetChoice());

    if (SampleDistributionBase::UsesMean(currDist))
        if (SampleDistributionBase::IsLinear(currDist))
            distMeanUI.FieldValueOK(allOK, "distribution mean");
        else
            distMeanUI.FieldValueOK(allOK, "distribution mean", 1E-20, 1E20);

    if (SampleDistributionBase::UsesStdDev(currDist))
    {
        if (SampleDistributionBase::IsLinear(currDist))
            distStdDevUI.FieldValueOK(allOK, "std deviation");
        else
            distStdDevLogUI.FieldValueOK(allOK, "log std dev");
    }

    if (SampleDistributionBase::UsesMinMax(currDist))
    {
        distMinUI.FieldValueOK(allOK, "distribution min");
        distMaxUI.FieldValueOK(allOK, "distribution max");
    }

    if (SampleDistributionBase::IsTriangular(currDist))
        distPeakUI.FieldValueOK(allOK, "distribution min");

    if (SampleDistributionBase::IsPoisson(currDist))
        poissonLambdaUI.FieldValueOK(allOK, "poisson mean", 1, int(SC_SampVar::poissonMaxLambda));

    if (SampleDistributionBase::IsWeibull(currDist))
    {
        weibullShapeUI.FieldValueOK(allOK, "weibull shape");
        weibullScaleUI.FieldValueOK(allOK, "weibull scale");
    }

    if (SampleDistributionBase::IsBeta(currDist))
    {
        betaPUI.FieldValueOK(allOK, "Beta P", SC_SampVar::minBetaExp, SC_SampVar::maxBetaExp);
        betaQUI.FieldValueOK(allOK, "Beta Q", SC_SampVar::minBetaExp, SC_SampVar::maxBetaExp);
    }

    if (SampleDistributionBase::IsUserCDF(currDist))
        userCDFInputObjRefUI.FuncObjRefOK(allOK, "user CDF input object");

    if (SampleDistributionBase::IsTableLookup(currDist))
        tableLookupUI.UIdataOK(allOK);

    return allOK;
}

void DPO_TableSamplePS::GetMenuData(DPO_TableSample&  outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.createNewTable = createNewTableUI.GetBoolChoice();
    if (outData.createNewTable)
    {
        outData.nsamples = nsamplesUI.GetFieldValue();
        outData.randomSeed = randomSeedUI.GetFieldValue();
    }
    else
        outData.existingTableObjRef = existingTableObjRefUI.GetFuncObjRef();

    outData.stratifySample = stratifySampleUI.GetBoolChoice();
    outData.randomMix = randomMixUI.GetBoolChoice();


    SampleDistributionBase::SVDistribution currDist =
            SampleDistributionBase::SVDistribution(svDistributionUI.GetChoice());

    SC_SampVar& currSamp = outData.sampVar;
    currSamp.svDistribution = currDist;

    if (SampleDistributionBase::UsesMean(currDist))
        currSamp.distMean = distMeanUI.GetFieldValue();

    if (SampleDistributionBase::UsesStdDev(currDist))
    {
        if (SampleDistributionBase::IsLinear(currDist))
            currSamp.distStdDev = distStdDevUI.GetFieldValue();
        else
            currSamp.distLogStdDev = distStdDevLogUI.GetFieldValue();
    }

    if (SampleDistributionBase::UsesMinMax(currDist))
    {
        currSamp.distMin = distMinUI.GetFieldValue();
        currSamp.distMax = distMaxUI.GetFieldValue();
    }
    if (SampleDistributionBase::IsTriangular(currDist))
            currSamp.distPeak = distPeakUI.GetFieldValue();

    if (SampleDistributionBase::IsPoisson(currDist))
        currSamp.poissonLambda = double(poissonLambdaUI.GetFieldValue());

    if (SampleDistributionBase::IsWeibull(currDist))
    {
        currSamp.weibullShape = weibullShapeUI.GetFieldValue();
        currSamp.weibullScale = weibullScaleUI.GetFieldValue();
    }

    if (SampleDistributionBase::IsBeta(currDist))
    {
        currSamp.betaP = betaPUI.GetFieldValue();
        currSamp.betaQ = betaQUI.GetFieldValue();
    }
    if (SampleDistributionBase::IsUserCDF(currDist))
        currSamp.userCDFInputObjRef = userCDFInputObjRefUI.GetFuncObjRef();

    if (SampleDistributionBase::IsTableLookup(currDist))
    {
        DSC_TableColSel outTbl;
        tableLookupUI.GetTableColSel(outTbl);
        currSamp.tableLookupObjRef = outTbl.tableInputObjRef;
        currSamp.tableLookupColumn = outTbl.selectedCol;
    }


}

