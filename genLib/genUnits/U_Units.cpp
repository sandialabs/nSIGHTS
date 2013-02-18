///////////////////////////////////////////////////////////////////////////////////
//
// U_Units.cpp
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
//      unit controls (unit selection combined real/units) for use in form managed UI
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include "U_Units.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace genUnitsMenuUtilities {



UnitsTypeOptionWidget::UnitsTypeOptionWidget()
{
    PreInitSetUp();
}

void UnitsTypeOptionWidget::PreInitSetUp()
{
    nchoice = UnitsBase::GetAppMaxUnits();
    choiceLabels.SetEmpty();
    for (int i = 0; i < nchoice; i++)
        choiceLabels += unitTypeString[i];
}

void UnitsTypeOptionWidget::SetUnitType(int  uType)
{
    SetChoice(uType);
}

int UnitsTypeOptionWidget::GetUnitType()
{
    return GetChoice();
};


UnitsConvOptionWidget::UnitsConvOptionWidget()
{
    initType = 0;
    for (int i = 0; i < 10; i++)
        choiceLabels += "uuuuuuuu";
    nchoice = 10;
}

UnitsConvOptionWidget::UnitsConvOptionWidget(UnitType uType)
{
    ChangeUnitType(uType);
}

void UnitsConvOptionWidget::SetSensitive(bool inSens)
{
    OptionChooserWidget::SetSensitive(inSens && (initType > 0));
}

void UnitsConvOptionWidget::ChangeUnitType(UnitType uType)
{
    initType = uType;
    nchoice = GetMaxConversionIndex(uType) + 1;
    choiceLabels.SetEmpty();

    if (IsCreated())
        CComboBox::ResetContent();

    for (int i = 0; i < nchoice; i++)
    {
        choiceLabels += unitString[uType][i];
        CComboBox::AddString(choiceLabels[i]);
    }


    currChoice = 0;
    if (IsCreated()) {
        CComboBox::SetCurSel(0);
    }
}

void UnitsConvOptionWidget::SetUnitConv(int  uConv)
{
    SetChoice(uConv);
}

void  UnitsConvOptionWidget::SetUnitConv(const UnitIndex& inConv)
{
    if (inConv.unitType != initType)
        ChangeUnitType(inConv.unitType);

    SetChoice(inConv.GetConvIndex());
}

int UnitsConvOptionWidget::GetUnitConv()
{
    return GetChoice();
}

void UnitsConvOptionWidget::GetUnitConv(UnitIndex& outConv)
{
    outConv.SetConvIndex(GetChoice());
}

void UnitsConvUIC::SetSensitive(bool isSensitive)
{
    OptionBaseUIC::SetSensitive(isSensitive);
    UnitsConvOptionWidget::SetSensitive(isSensitive);
}



void UnitRealUIC::UnitConvChange(const UnitIndex&   newConv)
{
    bool ok = true;
    if (realValueUI.FieldValueOK(ok, "Unit value"))
    {
        double userVal = realValueUI.GetFieldValue();
        double metricVal = currConv.UserToMetric(userVal);
        userVal = newConv.MetricToUser(metricVal);
        currConv = newConv;
        realValueUI.SetFieldFormat(currConv.GetUnitRealFormat());
        realValueUI.SetFieldValue(userVal);
    }
    else
    {
        currConv = newConv;
        realValueUI.SetFieldFormat(currConv.GetUnitRealFormat());
    }
}



void UnitRealUIC::InitFramedUI(      char       frameTitle[],
                                      Widget     parent,
                                const ArgListC&  inArgs)
{
    BaseUIC::InitFramedUI(frameTitle, parent, inArgs);
    InitUI();
}

void UnitRealUIC::InitLabeledUI(      char       labText[],
                                      Widget     parent,
                                const ArgListC&  inArgs)
{
    BaseUIC::InitLabeledUI(labText, parent, inArgs);
    InitUI();
}

void UnitRealUIC::InitUI()
{
    ArgListC args;
    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.RightFormAttach(rightOffset);
    realValueUI.SetFieldLen(12);
    realValueUI.InitWidget(GetForm(), args);
}


void UnitRealUIC::SetUnitReal(const UnitReal& inReal)
{
    currConv = inReal;
    realValueUI.SetFieldFormat(currConv.GetUnitRealFormat());
    realValueUI.SetFieldValue(inReal.GetUserVal());
}

void UnitRealUIC::SetUnitReal(const double&     inMetricVal,
                              const UnitIndex&  inConv)
{
    UnitReal tmp(inConv, inMetricVal);
    SetUnitReal(tmp);
}

void UnitRealUIC::ChangeUnitType(int  uType)
{
    bool ok = true;
    double metricVal = 0.0;
    if (realValueUI.FieldValueOK(ok, "Unit value"))
        metricVal = currConv.UserToMetric(realValueUI.GetFieldValue());

    currConv = UnitIndex(uType);
    realValueUI.SetFieldFormat(currConv.GetUnitRealFormat());
    realValueUI.SetFieldValue(metricVal);
}

void UnitRealUIC::ChangeConvIndex(const UnitIndex& newConv)
{
    UnitConvChange(newConv);
}


bool UnitRealUIC::UIdataOK(bool& allOK)
{
    return realValueUI.FieldValueOK(allOK, "Unit value");
}

bool UnitRealUIC::UIdataOK(bool&         allOK,
                           const double& metricMin,
                           const double& metricMax)
{
    return realValueUI.FieldValueOK(allOK, "Unit value",
            currConv.MetricToUser(metricMin),
            currConv.MetricToUser(metricMax));
}


void UnitRealUIC::GetUnitReal(UnitReal& outReal)
{
    outReal = currConv;
    GetUnitReal(outReal.metricVal);
}

void UnitRealUIC::GetUnitReal(double&       outMetricVal,
                              UnitIndex&    outConv)
{
    outConv = currConv;
    GetUnitReal(outMetricVal);
}

void UnitRealUIC::GetUnitReal(double&       outMetricVal)
{
    outMetricVal = currConv.UserToMetric(realValueUI.GetFieldValue());
}

void UnitRealUIC::SetSensitive(bool isSensitive)
{
    BaseUIC::SetSensitive(isSensitive);
    realValueUI.SetSensitive(isSensitive);
}


void UnitRealAndConvUIC::UnitConvChange()
{
    UnitIndex newConv(currConv);
    convIndexUI.GetUnitConv(newConv);

    UnitRealUIC::UnitConvChange(newConv);
}

void UnitRealAndConvUIC::UnitConvChangeCB(void* inObj)
{
    static_cast<UnitRealAndConvUIC*>(inObj)->UnitConvChange();
}



void UnitRealAndConvUIC::InitFramedUI(      char       frameTitle[],
                                      Widget     parent,
                                const ArgListC&  inArgs,
                                      UnitType    inUnits)
{
    BaseUIC::InitFramedUI(frameTitle, parent, inArgs);
    InitUI(65, inUnits);
}

void UnitRealAndConvUIC::InitLabeledUI(      char       labText[],
                                          Widget     parent,
                                    const ArgListC&  inArgs,
                                          UnitType    inUnits)
{
    BaseUIC::InitLabeledUI(labText, parent, inArgs);
    InitUI(75, inUnits);
}

void UnitRealAndConvUIC::InitUI(int     valPos,
                         UnitType   inUnits)
{
    currConv = UnitIndex(inUnits);

    ArgListC args;

    args.TopFormAttach();
    args.BottomFormAttach();
    args.RightFormAttach(rightOffset);

    convIndexUI.InitWidget(GetForm(), args);
    convIndexUI.AddOtherObjectCallback(UnitConvChangeCB, this);

    args.Clear();
    args.TopFormAttach();
    args.BottomFormAttach();
    args.RightPosAttach(valPos);
    realValueUI.SetFieldLen(12);
    realValueUI.InitWidget(GetForm(), args);
}


void UnitRealAndConvUIC::SetUnitReal(const UnitReal& inReal)
{
    UnitRealUIC::SetUnitReal(inReal);
    convIndexUI.ChangeUnitType(currConv.unitType);
    convIndexUI.SetUnitConv(currConv);
}

void UnitRealAndConvUIC::SetUnitReal(const double&      inMetricVal,
                              const UnitIndex&  inConv)
{
    UnitReal tmp(inConv, inMetricVal);
    SetUnitReal(tmp);
}

void UnitRealAndConvUIC::ChangeUnitType(int  uType)
{
    UnitRealUIC::ChangeUnitType(uType);
    convIndexUI.ChangeUnitType(uType);
}

void UnitRealAndConvUIC::ChangeConvIndex(const UnitIndex& newConv)
{
    convIndexUI.SetUnitConv(newConv);
    UnitConvChange();
}

void UnitRealAndConvUIC::SetSensitive(bool isSensitive)
{
    UnitRealUIC::SetSensitive(isSensitive);
    convIndexUI.SetSensitive(isSensitive);
}


void UnitRealStatusUIC::SetUnitReal(const double&       inMetricVal,
                                    const UnitIndex&    inConv)
{
    SetFieldFormat(inConv.GetUnitRealFormat());
    SetFieldValue(inConv.MetricToUser(inMetricVal));
}


void UnitTableColUIC::InitUI(const char* frameTitle,
                               Widget parent, const ArgListC& inArgs)
{
    frame.InitFramedUI(frameTitle, parent, inArgs);

    ArgListC args;
    args.StdFormAttach();
    tableUnitsUI.AttachObjectToLabel();
    tableUnitsUI.InitLabeledUI("Units:", frame.GetForm(), args);

    args.StdTopObjectAttach(tableUnitsUI.GetWidget());
    tableColumnUI.AttachObjectToLabel();
    tableColumnUI.InitLabeledUI("Col: ", frame.GetForm(), args);
}

void UnitTableColUIC::SetTableColumns(const DC_TableData& inTbl)
{
    tableColumnUI.SetListText(inTbl.columnDesc);
    tableColumnUI.SetSensitive(true);
}

void UnitTableColUIC::SetTableNA()
{
    tableColumnUI.SetListNA();
    tableColumnUI.SetSensitive(false);
}

void UnitTableColUIC::SetSensitive(bool inSens)
{
    frame.SetSensitive(inSens);
}

void UnitTableColUIC::SetData(const UnitTableCol& inData)
{
    tableUnitsUI.SetUnitConv(inData);
    tableColumnUI.SetSelection(inData.selectedTableCol);
}
void UnitTableColUIC::GetData(UnitTableCol& outData)
{
    tableUnitsUI.GetUnitConv(outData);
    outData.selectedTableCol = tableColumnUI.GetSelection();
}





};                                      // end namespace

