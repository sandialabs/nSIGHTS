///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ScatterplotPS.cpp
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

#include "PPO_Scatterplot.h"
#include "PPO_ScatterplotPS.h"


PPO_ScatterplotPS::PPO_ScatterplotPS() :
    scatterDataObjRefUI(typeid(DO_SAPlotTable)),
    xgridLineObjRefUI(typeid(DO_SAPlotTable)),
    ygridLineObjRefUI(typeid(DO_SAPlotTable)),
    tableDataOK(false)
{
}

void PPO_ScatterplotPS::SetAllSensitive()
{
    xgridLineObjRefUI.SetSensitive(plotXGridLinesUI.GetValue());
    ygridLineObjRefUI.SetSensitive(plotYGridLinesUI.GetValue());

    plotDVFrame.SetSensitive(tableDataOK);
    plotIVFrame.SetSensitive(tableDataOK);
}

void PPO_ScatterplotPS::SetAllSensitiveCB(void* currObj)
{
    static_cast<PPO_ScatterplotPS*>(currObj)->SetAllSensitive();
}

void PPO_ScatterplotPS::SetTableColumnsCB(void* inObj)
{
    static_cast<PPO_ScatterplotPS*>(inObj)->SetTableColumns();
}

void PPO_ScatterplotPS::SetTableColumns()
{
    tableDataOK = scatterDataObjRefUI.CheckDataObj();
    if (tableDataOK)
    {
        const DO_SAPlotTable* scatterDO = static_cast<DO_SAPlotTable*>(scatterDataObjRefUI.GetDataObj());
        int nplotDV = scatterDO->nDV;
        int nplotIV = scatterDO->nIV;

        if ((nplotDV > 0) && (nplotIV > 0))
        {
            const SC_StringArray& colDesc = scatterDO->tableData->columnDesc;
            SC_StringArray vStrs(colDesc.GetStringLen());
            vStrs.Alloc(nplotDV + nplotIV);
            for (int i = 0; i < nplotDV; i++)
                vStrs += colDesc[i + nplotIV];
            plotDVIndxUI.SetListText(vStrs);

            vStrs.SetEmpty();
            for (int i = 0; i < nplotIV; i++)
                vStrs += colDesc[i];
            plotIVIndxUI.SetListText(vStrs);
        }
        else
            tableDataOK = false;
    }
    if (!tableDataOK)
    {
        plotDVIndxUI.SetListNA();
        plotIVIndxUI.SetListNA();
    }

    SetAllSensitive();
}

void PPO_ScatterplotPS::InitMenu(const PPO_Scatterplot& inData)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(65);
    scatterDataObjRefUI.topOffset = 0;
    scatterDataObjRefUI.InitFramedUI("Scatterplot Data", topRowForm.GetWidget(), args);
    scatterDataObjRefUI.AddOtherObjectCallback(SetTableColumnsCB, this);

    args.StdSplitFinish(35);
    optionsFrame.InitFramedUI("Options", topRowForm.GetWidget(), args);

    args.StdToggleSetup(5);
    plotXGridLinesUI.StdToggleInit("X grid", optionsFrame.GetForm(), args);
    plotXGridLinesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(40);
    plotYGridLinesUI.StdToggleInit("Y grid", optionsFrame.GetForm(), args);
    plotYGridLinesUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(65);
    plotAxesLabelsUI.StdToggleInit("Labels", optionsFrame.GetForm(), args);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    gridRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    xgridLineObjRefUI.InitFramedUI("X Grid Line Data", gridRowForm.GetWidget(), args);

    args.StdSplitFinish();
    ygridLineObjRefUI.InitFramedUI("Y Grid Line Data", gridRowForm.GetWidget(), args);

    args.StdTopObjectAttach(gridRowForm.GetWidget());
    scatterSpecUI.InitFramedUI("Scatterplot Symbol", PPform.GetWidget(), args);

    args.StdTopObjectAttach(scatterSpecUI);
    args.BottomFormAttach();
    varRowForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup();
    plotDVFrame.InitFramedUI("Dependent (Y) Variable", varRowForm.GetWidget(), args);

    args.StdSplitFinish();
    plotIVFrame.InitFramedUI("Independent (X) Variable", varRowForm.GetWidget(), args);

    args.AllFormAttach();
    plotDVIndxUI.InitWidget(plotDVFrame.GetForm(), args);
    plotIVIndxUI.InitWidget(plotIVFrame.GetForm(), args);

}

void PPO_ScatterplotPS::MenuOpen(PPO_Scatterplot&   inData)
{
    MenuBaseC::MenuOpen(true, false);
    scatterDataObjRefUI.StdMenuInit(inData.scatterDataObjRef);
    xgridLineObjRefUI.StdMenuInit(inData.xgridLineObjRef);
    ygridLineObjRefUI.StdMenuInit(inData.ygridLineObjRef);
    scatterSpecUI.ResetPens(inData.objPenSet);
}

void PPO_ScatterplotPS::SetMenuData(PPO_Scatterplot&    inData)
{
    PPO_BasePS::SetMenuData(inData);

    scatterDataObjRefUI.SetFuncObjRef(inData.scatterDataObjRef);
    xgridLineObjRefUI.SetFuncObjRef(inData.xgridLineObjRef);
    ygridLineObjRefUI.SetFuncObjRef(inData.ygridLineObjRef);

    plotXGridLinesUI.SetValue(inData.plotXGridLines);
    plotYGridLinesUI.SetValue(inData.plotYGridLines);
    plotAxesLabelsUI.SetValue(inData.plotAxesLabels);

    plotDVIndxUI.SetSingleSelection(inData.plotDVIndx);
    plotIVIndxUI.SetSingleSelection(inData.plotIVIndx);

    scatterSpecUI.SetSymbolSpec(inData.scatterSpec);

    SetTableColumns();
}

bool PPO_ScatterplotPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    scatterDataObjRefUI.FuncObjRefOK(allOK, "Scatter plot data");
    if (plotXGridLinesUI.GetValue())
        xgridLineObjRefUI.FuncObjRefOK(allOK, "X grid lines source");
    if (plotYGridLinesUI.GetValue())
        ygridLineObjRefUI.FuncObjRefOK(allOK, "Y grid lines source");
    return allOK;
}

void PPO_ScatterplotPS::GetMenuData(PPO_Scatterplot&    outData)
{
    PPO_BasePS::GetMenuData(outData);

    outData.scatterDataObjRef = scatterDataObjRefUI.GetFuncObjRef();

    outData.plotXGridLines = plotXGridLinesUI.GetValue();
    if (outData.plotXGridLines)
        outData.xgridLineObjRef = xgridLineObjRefUI.GetFuncObjRef();
    outData.plotYGridLines = plotYGridLinesUI.GetValue();
    if (outData.plotYGridLines)
        outData.ygridLineObjRef = ygridLineObjRefUI.GetFuncObjRef();

    outData.plotAxesLabels = plotAxesLabelsUI.GetValue();

    outData.plotDVIndx = plotDVIndxUI.GetSingleSelection();
    outData.plotIVIndx = plotIVIndxUI.GetSingleSelection();

    scatterSpecUI.GetSymbolSpec(outData.scatterSpec);

}

