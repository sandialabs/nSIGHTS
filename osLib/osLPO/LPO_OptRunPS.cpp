///////////////////////////////////////////////////////////////////////////////////
//
// LPO_OptRunPS.cpp
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
//      File I/O and user interface for all osLFO functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genClass/U_Str.h>

#include "LPO_OptRun.h"
#include "LPO_OptRunPS.h"


LPO_OptRunPS::LPO_OptRunPS() :
    optSimResultsObjRefUI(typeid(DO_OptSimResults))
{
    runDataOK = false;
}

void LPO_OptRunPS::ChangeListType()
{
    bool listRuns = listRunsUI.GetBoolChoice();
    if (listRuns)
    // last setting was cases
        selectedDataUI.GetMultipleSelection(currCases);
    else
        selectedDataUI.GetMultipleSelection(currRuns);

    SetSelectedData();
}



void LPO_OptRunPS::ChangeListTypeCB(void* inObj)
{
    static_cast<LPO_OptRunPS*>(inObj)->ChangeListType();
}




void LPO_OptRunPS::SetAllSensitive()
{
    selectedDataFrame.SetSensitive(runDataOK);
    csFrame.SetSensitive(runDataOK && runDataDC->containsCovariance);
    mosFrame.SetSensitive(runDataOK && (runDataDC->runType != OptSimResultsHeader::rtSingle));
    listFrame.SetSensitive(runDataOK);
    if (!runDataOK)
        return;

    listEstimatedDataVariancesUI.SetSensitive(runDataDC->containsCovariance);
    listParameterCorrelationsUI.SetSensitive(runDataDC->containsCovariance);
    listNormalizedCovarianceMatricesUI.SetSensitive(runDataDC->containsCovariance);
    listDenormalizedCovarianceMatricesUI.SetSensitive(runDataDC->containsCovariance);
    list95ConfidenceLimitsUI.SetSensitive(runDataDC->containsCovariance);
    listCovarianceMatrixCorrelationsUI.SetSensitive(runDataDC->containsCovariance);

    listJacobianUI.SetSensitive(runDataDC->containsJacobian);
//    listModelStructureUI.SetSensitive(runDataDC->containsJacobian);
    listModelStructureUI.SetSensitive(false); // not implemented
}


void LPO_OptRunPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<LPO_OptRunPS*>(inObj)->SetAllSensitive();
}


void LPO_OptRunPS::SetSelectedData()
{
    runDataOK = optSimResultsObjRefUI.CheckDataObj();
    if (runDataOK)
    {
        runDataDC = static_cast<DO_OptSimResults*>(optSimResultsObjRefUI.GetDataObj())->optResults;


        SC_StringArray runDesc;
        runDesc.SetStringLen(80);
        runDesc.SetResizable(100);

        bool listRuns = listRunsUI.GetBoolChoice();
        int nruns = runDataDC->Size();

        for (int i = 0; i < nruns; i++)
        {
            OptSimRunResults& currRun = (*runDataDC)[i];

            char caseStr[80];
            int ncase = currRun.runResults.Size();

            if (listRuns)
            {
                IntToString(ncase, caseStr, 80);
                switch (runDataDC->runType)
                {
                    case  OptSimResultsHeader::rtSingle : {
                        CopyString(caseStr, "Single Run", 80);
                        break;
                    }
                    case OptSimResultsHeader::rtPerturb : {
                        ConcatString(caseStr, " Perturbations", 80);
                        break;
                    }
                    case OptSimResultsHeader::rtMultiple : {
                        ConcatString(caseStr, " Cases", 80);
                        break;
                    }
                    case OptSimResultsHeader::rtSample : {
                        ConcatString(caseStr, " Samples", 80);
                        break;
                    }
                }

                for (int j = 0; j < currRun.nFits; j++)
                {
                    OptFitDescription& currFit = currRun.fitDescriptions[j];

                    char fitStr[80];
                    if (currFit.fitComponentDescription.Size() > 1)
                        CopyString(fitStr, " \tComposite", 80);
                    else
                        CopyString(fitStr, " \tSingle", 80);

                    char tempStr[80];
                    MakeString(tempStr, currRun.runID, " \t ", currRun.fitDescriptions[j].fitID, fitStr, " \t ", caseStr, 80);

                    runDesc += tempStr;
                }
            }
            else
            {
                for (int j = 0; j < currRun.nFits; j++)
                {
                    OptFitDescription& currFit = currRun.fitDescriptions[j];

                    char tempStr[80];
                    if (currFit.fitComponentDescription.Size() > 1)
                        CopyString(tempStr, " \tComposite", 80);
                    else
                        CopyString(tempStr, " \tSingle", 80);

                    char fitStr[80];
                    MakeString(fitStr, currRun.runID, " \t ", currRun.fitDescriptions[j].fitID, tempStr, " \t ", 80);

                    for (int k = 0; k < ncase; k++)
                    {
                        switch (runDataDC->runType)
                        {
                            case  OptSimResultsHeader::rtSingle : {
                                CopyString(caseStr, "Single Run", 80);
                                break;
                            }
                            case OptSimResultsHeader::rtPerturb : {
                                CopyString(caseStr, "Pert#", 80);
                                break;
                            }
                            case OptSimResultsHeader::rtMultiple : {
                                CopyString(caseStr, "Case#", 80);
                                break;
                            }
                            case OptSimResultsHeader::rtSample : {
                                CopyString(caseStr, "Sample#", 80);
                                break;
                            }
                        }
                        if (!runDataDC->IsSingle())
                            ConcatInt(k + 1, caseStr, 80);

                        OptSingleFit& currFit = currRun.runResults[k].caseResults[j];

                        if (currFit.fitCompletedOK)
                            ConcatString(caseStr, "\t OK \t", 80);
                        else
                            ConcatString(caseStr, "\t Bad \t", 80);

                        char realStr[40];
                        SC_SciFormat rForm(4);
                        rForm.RealToString(currFit.fitValue, realStr, 40);

                        char tempStr[80];
                        MakeString(tempStr, fitStr, caseStr, realStr , 80);
                        runDesc += tempStr;
                    }
                }

            }
        }
        selectedDataUI.SetListText(runDesc);

        if (listRuns)
        {
            selectedDataUI.SetMultipleSelection(currRuns);
        }
        else
        {
            selectedDataUI.SetMultipleSelection(currCases);
        }

    }
    else
    {
        selectedDataUI.SetListNA();
    }


    SetAllSensitive();
}


void LPO_OptRunPS::SetSelectedDataCB(void* inObj)
{
    static_cast<LPO_OptRunPS*>(inObj)->SetSelectedData();
}

void LPO_OptRunPS::ClearAll()
{
    selectedDataUI.ClearAll();
}

void LPO_OptRunPS::ClearAllCB(void* inObj)
{
    static_cast<LPO_OptRunPS*>(inObj)->ClearAll();
}

void LPO_OptRunPS::SelectAll()
{
    selectedDataUI.SelectAll();
}

void LPO_OptRunPS::SelectAllCB(void* inObj)
{
    static_cast<LPO_OptRunPS*>(inObj)->SelectAll();
}

void LPO_OptRunPS::InitMenu()

{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    optSimResultsObjRefUI.InitFramedUI("Optimization Results To List", URform.GetWidget(), args);
    optSimResultsObjRefUI.AddOtherObjectCallback(SetSelectedDataCB, this);

    // top radio box

    args.Clear();
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(40);
    listRunsUI.PreInitSetUp("  Cases  ", "  Runs ");
    listRunsUI.InitFramedUI("Listing Type", topRowForm.GetWidget(), args);
    listRunsUI.AddOtherObjectCallback(ChangeListTypeCB, this);

    args.StdSplitFinish(60);
    buttonFrame.InitFramedUI("Control", topRowForm.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.AddArg(Arg::XmNwidth,  55);
    args.SetPlaceHolder();
    args.LeftPosAttach(10);
    clearAllUI.StdButtonInit("None", buttonFrame.GetForm(), args);
    clearAllUI.AddOtherObjectCallback(ClearAllCB, this);

    args.ResetPlace();
    args.RightPosAttach(90);
    selectAllUI.StdButtonInit("All", buttonFrame.GetForm(), args);
    selectAllUI.AddOtherObjectCallback(SelectAllCB, this);


    // from bottom up
    args.Clear();
    args.LeftFormAttach();
    args.RightFormAttach();
    args.BottomFormAttach();
    listFrame.InitFramedUI("Listing Selections", PPform.GetWidget(), args);

    args.Clear();
    args.StdFormAttach();
    row1Form.InitWidget(listFrame.GetForm(), args);

    args.StdToggleSetup(5);
    listRunSummaryUI.StdToggleInit("Summary", row1Form.GetWidget(), args);
    args.StdToggleSetup(35);
    listFitValuesUI.StdToggleInit("Fit Values", row1Form.GetWidget(), args);
    args.StdToggleSetup(70);
    listParameterEstimatesUI.StdToggleInit("Parameter Estimates", row1Form.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(row1Form.GetWidget());
    row2Form.InitWidget(listFrame.GetForm(), args);
    args.StdToggleSetup(5);
    listEstimatedDataVariancesUI.StdToggleInit("Estimated data variance", row2Form.GetWidget(), args);
    args.StdToggleSetup(35);
    listParameterCorrelationsUI.StdToggleInit("Parameter correlations", row2Form.GetWidget(), args);
    args.StdToggleSetup(70);
    listNormalizedCovarianceMatricesUI.StdToggleInit("Norm. Covar. Matrix", row2Form.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(row2Form.GetWidget());
    row3Form.InitWidget(listFrame.GetForm(), args);
    args.StdToggleSetup(5);
    listDenormalizedCovarianceMatricesUI.StdToggleInit("Denorm. Covar. Matrix", row3Form.GetWidget(), args);
    args.StdToggleSetup(35);
    list95ConfidenceLimitsUI.StdToggleInit("95% Limits", row3Form.GetWidget(), args);
    args.StdToggleSetup(70);
    listJacobianUI.StdToggleInit("Jacobian", row3Form.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(row3Form.GetWidget());
    row4Form.InitWidget(listFrame.GetForm(), args);
    args.StdToggleSetup(5);
    listModelStructureUI.StdToggleInit("Model Structure", row4Form.GetWidget(), args);
    args.StdToggleSetup(35);
    listCovarianceMatrixCorrelationsUI.StdToggleInit("Covar. Matrix Correlations", row4Form.GetWidget(), args);

    args.Clear();
    args.LeftFormAttach();
    args.RightFormAttach();
    args.BottomUICAttach(listFrame);
    covarMosForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    csFrame.InitFramedUI("Covariance Matrix", covarMosForm.GetWidget(), args);

    args.StdSplitFinish();
    mosFrame.InitFramedUI("Multiple List Output", covarMosForm.GetWidget(), args);

    args.StdToggleSetup(5);
    csActualUI.StdToggleInit("Actual", csFrame.GetForm(), args);
    args.StdToggleSetup(35);
    csEstimatedUI.StdToggleInit("Estimated", csFrame.GetForm(), args);
    args.StdToggleSetup(70);
    csFitComponentUI.StdToggleInit("Sub-fits", csFrame.GetForm(), args);

    args.StdToggleSetup(5);
    mosListAllUI.StdToggleInit("All cases", mosFrame.GetForm(), args);
    args.StdToggleSetup(35);
    mosListBestUI.StdToggleInit("Best fit", mosFrame.GetForm(), args);
    args.StdToggleSetup(70);
    mosListStatisticsUI.StdToggleInit("Statistics", mosFrame.GetForm(), args);

    args.Clear();
    args.StdTopObjectAttach(topRowForm.GetWidget());
    args.BottomWidgetAttach(covarMosForm.GetWidget());
    args.BottomOffset(5);

    selectedDataFrame.InitFramedUI("Selected Runs to List", PPform.GetWidget(), args);

    args.Clear();
    args.AllFormAttach();
    args.BottomOffset(20);  // kluge required for layout bug
    selectedDataUI.isSingleSel = false;
    selectedDataUI.InitWidget(selectedDataFrame.GetForm(), args);
}


void LPO_OptRunPS::MenuOpen(LPO_OptRun&   inData)
{
    MenuBaseC::MenuOpen(true, true);
    optSimResultsObjRefUI.StdMenuInit(inData.optSimResultsObjRef);
}


void LPO_OptRunPS::SetMenuData(LPO_OptRun&    inData)
{
    MenuBaseC::SetMenuData(inData);

    optSimResultsObjRefUI.SetFuncObjRef(inData.optSimResultsObjRef);

    listRunsUI.SetBoolChoice(inData.listRuns);
    currCases = inData.casesToList;
    currRuns = inData.runsToList;

    csActualUI.SetValue(inData.csActual);
    csEstimatedUI.SetValue(inData.csEstimated);
    csFitComponentUI.SetValue(inData.csFitComponent);

    mosListAllUI.SetValue(inData.mosListAll);
    mosListBestUI.SetValue(inData.mosListBest);
    mosListStatisticsUI.SetValue(inData.mosListStatistics);

    listRunSummaryUI.SetValue(inData.listRunSummary);
    listFitValuesUI.SetValue(inData.listFitValues);
    listParameterEstimatesUI.SetValue(inData.listParameterEstimates);
    listEstimatedDataVariancesUI.SetValue(inData.listEstimatedDataVariances);
    listParameterCorrelationsUI.SetValue(inData.listParameterCorrelations);
    listNormalizedCovarianceMatricesUI.SetValue(inData.listNormalizedCovarianceMatrices);
    listDenormalizedCovarianceMatricesUI.SetValue(inData.listDenormalizedCovarianceMatrices);
    list95ConfidenceLimitsUI.SetValue(inData.list95ConfidenceLimits);
    listJacobianUI.SetValue(inData.listJacobian);
    listModelStructureUI.SetValue(inData.listModelStructure);
    listCovarianceMatrixCorrelationsUI.SetValue(inData.listCovarianceMatrixCorrelations);

    SetSelectedData();
}

bool LPO_OptRunPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    optSimResultsObjRefUI.FuncObjRefOK(allOK, "OptRun data source");

    return allOK;
}

void LPO_OptRunPS::GetMenuData(LPO_OptRun&    outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.optSimResultsObjRef = optSimResultsObjRefUI.GetFuncObjRef();

    outData.listRuns = listRunsUI.GetBoolChoice();
    if (outData.listRuns)
    {
        selectedDataUI.GetMultipleSelection(outData.runsToList);
        outData.casesToList = currCases;
    }
    else
    {
        selectedDataUI.GetMultipleSelection(outData.casesToList);
        outData.runsToList = currRuns;
    }

    outData.csActual = csActualUI.GetValue();
    outData.csEstimated = csEstimatedUI.GetValue();
    outData.csFitComponent = csFitComponentUI.GetValue();

    outData.mosListAll = mosListAllUI.GetValue();
    outData.mosListBest = mosListBestUI.GetValue();
    outData.mosListStatistics = mosListStatisticsUI.GetValue();

    outData.listRunSummary = listRunSummaryUI.GetValue();
    outData.listFitValues = listFitValuesUI.GetValue();
    outData.listParameterEstimates = listParameterEstimatesUI.GetValue();
    outData.listEstimatedDataVariances = listEstimatedDataVariancesUI.GetValue();
    outData.listParameterCorrelations = listParameterCorrelationsUI.GetValue();
    outData.listNormalizedCovarianceMatrices = listNormalizedCovarianceMatricesUI.GetValue();
    outData.listDenormalizedCovarianceMatrices = listDenormalizedCovarianceMatricesUI.GetValue();
    outData.list95ConfidenceLimits = list95ConfidenceLimitsUI.GetValue();
    outData.listJacobian = listJacobianUI.GetValue();
    outData.listModelStructure = listModelStructureUI.GetValue();
    outData.listCovarianceMatrixCorrelations = listCovarianceMatrixCorrelationsUI.GetValue();

}


void LPO_OptRunPS::SetAllUI(bool flag)
{
    listRunSummaryUI.SetValue(flag);
    listFitValuesUI.SetValue(flag);
    listParameterEstimatesUI.SetValue(flag);
    listEstimatedDataVariancesUI.SetValue(flag);
    listParameterCorrelationsUI.SetValue(flag);
    listNormalizedCovarianceMatricesUI.SetValue(flag);
    listDenormalizedCovarianceMatricesUI.SetValue(flag);
    list95ConfidenceLimitsUI.SetValue(flag);
    listJacobianUI.SetValue(flag);
//    listModelStructureUI.SetValue(flag);
    listCovarianceMatrixCorrelationsUI.SetValue(flag);
}

void LPO_OptRunPS::MenuClear()
{
    SetAllUI(false);
}

void LPO_OptRunPS::MenuDefault()
{
    SetAllUI(true);
}




