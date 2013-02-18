///////////////////////////////////////////////////////////////////////////////////
//
// DPO_CalcConfidenceGridPS.cpp
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
//      I/O and user interface for all osDFO functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "OsLibAfx.h"

#include <genClass/U_Str.h>
#include <genClassPS/PS_UIBase.h>

#include "DPO_CalcConfidenceGrid.h"
#include "DPO_CalcConfidenceGridPS.h"

DPO_CalcConfidenceGridPS::DPO_CalcConfidenceGridPS() :
    inputGridResultsObjRefUI(typeid(DO_GridData))
{
}

void DPO_CalcConfidenceGridPS::SetIndexList()
{
    SC_StringArray caseIDs;
    bool autoBF = autoBestCaseUI.GetValue();
    if (inputGridResultsObjRefUI.CheckDataObj())
    {

        const DC_GridData* dataDC = static_cast<DO_GridData*>(inputGridResultsObjRefUI.GetDataObj())->gridData;

        ny = dataDC->yData.Size();
        int nx = dataDC->yData.Size();

        const int strLen = 40;
        char istr[strLen], jstr[strLen], datastr[strLen];
        char xStr[strLen], yStr[strLen], valStr[strLen];
        SC_RealFormat rformat;
        caseIDs.AllocAndSetSize(nx * ny);

        for (int i = 0; i < nx; i++)
        {
            IntToString(i, istr, strLen);
            MakeString(xStr, "x = ", istr, "  ", strLen);
            for (int j = 0; j < ny; j++)
            {
                IntToString(j, jstr, strLen);
                MakeString(yStr, "y = ", jstr, "  ", strLen);
                RealToString(rformat, dataDC->gData[i][j], datastr, strLen);
                MakeString(valStr, "value = ", datastr, strLen);
                int indx = i * ny + j;
                MakeString(caseIDs[indx], xStr, yStr, valStr, strLen);
            }
        }
    }

    listDataOK = !caseIDs.IsEmpty();
    if (listDataOK)
        bestCaseIndexUI.SetListText(caseIDs);
    else
        bestCaseIndexUI.SetListNA();

    SetAllSensitive();
}

void DPO_CalcConfidenceGridPS::SetIndexListCB(void* inObj)
{
    static_cast<DPO_CalcConfidenceGridPS*>(inObj)->SetIndexList();
}

void DPO_CalcConfidenceGridPS::SetAllSensitive()
{
    bool autoBF = autoBestCaseUI.GetValue();
    bestCaseFrame.SetSensitive(!autoBF);
    bestCaseIndexUI.SetSensitive(!autoBF);
}


void DPO_CalcConfidenceGridPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_CalcConfidenceGridPS*>(inObj)->SetAllSensitive();
}

void DPO_CalcConfidenceGridPS::InitMenu()
{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.AllFormAttach();
    inputGridResultsObjRefUI.InitFramedUI("nSIGHTS Grid Results To Select From", URform.GetWidget(), args);
    inputGridResultsObjRefUI.AddOtherObjectCallback(SetIndexListCB, this);

    args.StdFormAttach();
    npForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    nDataPtsUI.InitFramedUI("# of Data Points", npForm.GetWidget(), args);

    args.StdSplitFinish();
    nParamUI.InitFramedUI("# of Parameters", npForm.GetWidget(), args);

    args.StdTopObjectAttach(npForm.GetWidget());
    optionFrame.InitFramedUI("Options", PPform.GetWidget(), args);

    args.StdSplitSetup();
    args.StdToggleSetup(5);
    autoBestCaseUI.StdToggleInit("automatically determine best fit case", optionFrame.GetForm(), args);
    autoBestCaseUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    static const char* distrStrings[] = {"F-test Method", "Log-Likelihood Method"};
    distributionUI.PreInitSetUp(distrStrings, 2);
    distributionUI.InitWidget(optionFrame.GetForm(), args);

    args.StdTopObjectAttach(optionFrame);
    bestCaseFrame.InitFramedUI("Select Best Fit Case", PPform.GetWidget(), args);

    args.AllFormAttach();
    args.TopOffset(2);
    bestCaseIndexUI.InitWidget(bestCaseFrame.GetForm(), args);

}

void DPO_CalcConfidenceGridPS::MenuOpen(DPO_CalcConfidenceGrid& inData)
{
    MenuBaseC::MenuOpen(false, false);

    inputGridResultsObjRefUI.StdMenuInit(inData.inputGridResultsObjRef);
}


void DPO_CalcConfidenceGridPS::SetMenuData(DPO_CalcConfidenceGrid&  inData)
{
    MenuBaseC::SetMenuData(inData);

    inputGridResultsObjRefUI.SetFuncObjRef(inData.inputGridResultsObjRef);
    distributionUI.SetChoice(int(inData.distribution));
    nDataPtsUI.SetFieldValue(inData.nDataPts);
    nParamUI.SetFieldValue(inData.nParam);

    autoBestCaseUI.SetValue(inData.autoBestCase);

    SetIndexList();
    selection = inData.bestCaseXIndex * ny + inData.bestCaseYIndex;
    bestCaseIndexUI.SetSingleSelection(selection);
}

bool DPO_CalcConfidenceGridPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    inputGridResultsObjRefUI.FuncObjRefOK(allOK, "Opt input");
    nDataPtsUI.FieldValueOK(allOK, "# data pts", 1, 1000000);
    nParamUI.FieldValueOK(allOK, "# parameters", 1, 1000000);
    return allOK;
}

void DPO_CalcConfidenceGridPS::GetMenuData(DPO_CalcConfidenceGrid&  outData)
{
    MenuBaseC::GetMenuData(outData);
    outData.inputGridResultsObjRef = inputGridResultsObjRefUI.GetFuncObjRef();

    outData.autoBestCase = autoBestCaseUI.GetValue();
    outData.distribution = DFO_CalcConfidenceGrid::Distribution(distributionUI.GetChoice());
    outData.nDataPts = nDataPtsUI.GetFieldValue();
    outData.nParam = nParamUI.GetFieldValue();

    if (listDataOK && (!outData.autoBestCase))
    {
        int sel = bestCaseIndexUI.GetSingleSelection();
        outData.bestCaseXIndex = sel / ny;
        outData.bestCaseYIndex = sel % ny;
    }
}

