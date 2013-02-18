///////////////////////////////////////////////////////////////////////////////////
//
// UPO_WriteCurveArrayPS.cpp
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
//      File I/O and user-interface for objUFO.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genClass/U_Str.h>

#include "UPO_WriteCurveArray.h"
#include "UPO_WriteCurveArrayPS.h"


UPO_WriteCurveArrayPS::CurveDataUIC::CurveDataUIC() : curveObjRefUI(typeid(DO_Curve))
{
}

void UPO_WriteCurveArrayPS::CurveDataUIC::SetAllSensitiveCB(void* currObj)
{
 static_cast<UPO_WriteCurveArrayPS::CurveDataUIC*>(currObj)->SetAllSensitive();
}

void UPO_WriteCurveArrayPS::CurveDataUIC::ObjChange()
{
    if (curveObjRefUI.CheckDataObj())
    {
        char tempStr[DC_CurveWithID::curveIDlen];
//curveIDUI.GetFieldString(tempStr, DC_CurveWithID::curveIDlen);
//if (StringLength(tempStr) == 0)
        {
            CopyString(tempStr, curveObjRefUI.GetFuncObjRef().objRef->GetID(), DC_CurveWithID::curveIDlen);
            curveIDUI.SetFieldString(tempStr);
        }
    }
}

void UPO_WriteCurveArrayPS::CurveDataUIC::ObjChangeCB(void* inObj)
{
     static_cast<UPO_WriteCurveArrayPS::CurveDataUIC*>(inObj)->ObjChange();
}


void UPO_WriteCurveArrayPS::CurveDataUIC::InitCurveData()
{
    static const int specFormwidth = 600;

    ArgListC args;

    args.TopFormAttach();
    args.LeftFormAttach();
    doCurveUI.StdToggleInit(" ", outsideForm.GetWidget(), args);
    doCurveUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.TopFormAttach();
    args.RightPosAttach(50);
    curveObjRefUI.InitFormUI(outsideForm.GetWidget(), args);
    curveObjRefUI.AddOtherObjectCallback(ObjChangeCB, this);

    args.Clear();
    args.TopFormAttach();
    args.SpecifiedWidthAttach(55, 95, specFormwidth);
    curveIDUI.SetFieldLen(DC_CurveWithID::curveIDlen);
    curveIDUI.InitWidget(outsideForm.GetWidget(), args);

}

void UPO_WriteCurveArrayPS::CurveDataUIC::SetCurveData(const UFO_WriteCurveArray::CurveData& curveData)
{
    doCurveUI.SetValue(curveData.doCurve);
    curveObjRefUI.SetFuncObjRef(curveData.curveObjRef);
    curveIDUI.SetFieldString(curveData.curveID);
}

void UPO_WriteCurveArrayPS::CurveDataUIC::GetCurveData(UFO_WriteCurveArray::CurveData& curveData)
{
    curveData.curveObjRef = curveObjRefUI.GetFuncObjRef();
    curveData.doCurve = doCurveUI.GetValue();
    curveIDUI.GetFieldString(curveData.curveID, DC_CurveWithID::curveIDlen);
}

bool UPO_WriteCurveArrayPS::CurveDataUIC::CurveDataOK(bool& prevOK)
{
    if (doCurveUI.GetValue())
    {
        curveObjRefUI.FuncObjRefOK(prevOK, "Curve input");
    }
    return prevOK;
}


void UPO_WriteCurveArrayPS::CurveDataUIC::SetAllSensitive()
{
    doCurveUI.SetSensitive(true);
    bool doCurve = doCurveUI.GetValue();
    curveObjRefUI.SetSensitive(doCurve);
    curveIDUI.SetSensitive(doCurve);
}

CSize UPO_WriteCurveArrayPS::CurveScrolledArea::SetCalculateFormSize()
{
    CSize szForm;
    szForm.cx = SetWindowPosX(0, 600);
    szForm.cy = SetWindowPosY(0, 1000);
    return szForm;
}

/////////////////////////////////////////////////////////////////
// UPO_WriteCurveArrayPS

UPO_WriteCurveArrayPS::UPO_WriteCurveArrayPS()
{
}


void UPO_WriteCurveArrayPS::SetAllSensitive()
{
    for (int i = 0; i < UFO_WriteCurveArray::maxCurveData; i++)
        curveDataUI[i].SetAllSensitive();
}


void UPO_WriteCurveArrayPS::InitMenu()

{
    MenuBaseC::InitMenu();

    ArgListC args;
    args.StdFormAttach();
    outField.InitFramedUI("Curve Output File", PPform.GetWidget(), args);
    outField.SetPromptAndMask("Curve file to create",
        "Curve files (*.nCRV)|*.nCRV|All Files (*.*)|*.*|||");

    args.Clear();
    args.StdTopObjectAttach(outField);
    args.BottomFormAttach();
    args.LeftOffset(3);
    args.RightOffset(3);

    //scrollArea.needHorizontal = false;
    scrollArea.InitWidget(PPform.GetWidget(), args);


    args.Clear();
    args.StdFormAttach();
    titleForm.InitWidget(scrollArea, args);

    args.Clear();
    args.TopFormAttach();
    args.LeftPosAttach(15);
    title1.StdLabelInit("Curve Object", titleForm.GetWidget(), args);

    args.Clear();
    args.TopFormAttach();
    args.LeftPosAttach(55);
    title2.StdLabelInit("Curve Description (mandatory)", titleForm.GetWidget(), args);

    args.Clear();
    args.TopOffset(5);
    args.StdTopObjectAttach(titleForm.GetWidget());
    hSep.InitWidget(scrollArea, args);

    args.Clear();
    args.TopOffset(5);
    args.StdTopObjectAttach(hSep.GetWidget());

    for (int i = 0; i < UFO_WriteCurveArray::maxCurveData; i++)
    {
        curveDataUI[i].outsideForm.InitWidget(scrollArea, args);
        curveDataUI[i].InitCurveData();

        args.Clear();
        args.StdTopObjectAttach(curveDataUI[i].outsideForm.GetWidget());
        curveDataUI[i].hSep.InitWidget(scrollArea, args);

        args.Clear();
        args.StdTopObjectAttach(curveDataUI[i].hSep.GetWidget());
    }
}


void UPO_WriteCurveArrayPS::MenuOpen(UPO_WriteCurveArray&   inData)
{
    MenuBaseC::MenuOpen(false, true);
    for (int i = 0; i < UFO_WriteCurveArray::maxCurveData; i++)
    {
        curveDataUI[i].curveObjRefUI.StdMenuInit(inData.curveData[i].curveObjRef);
    }
}

void UPO_WriteCurveArrayPS::MenuClose()
{
    outField.CloseDialog();
    MenuBaseC::MenuClose();
}


void UPO_WriteCurveArrayPS::SetMenuData(UPO_WriteCurveArray&    inData)
{
    for (int i = 0; i < UFO_WriteCurveArray::maxCurveData; i++)
        curveDataUI[i].SetCurveData(inData.curveData[i]);
    outField.SetFieldString(inData.outFname);
    MenuBaseC::SetMenuData(inData);
    SetAllSensitive();
}

bool UPO_WriteCurveArrayPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    outField.FieldFileOK(allOK, "Curve output file", false, true);
    for (int i = 0; i < UFO_WriteCurveArray::maxCurveData; i++)
        if (!curveDataUI[i].CurveDataOK(allOK))
            return false;
    return allOK;
}

void UPO_WriteCurveArrayPS::GetMenuData(UPO_WriteCurveArray&    outData)
{
    MenuBaseC::GetMenuData(outData);

    outField.GetFieldFile(outData.outFname);
    for (int i = 0; i < UFO_WriteCurveArray::maxCurveData; i++)
        curveDataUI[i].GetCurveData(outData.curveData[i]);
}

void UPO_WriteCurveArrayPS::MenuClear()
{
    for (int i = 0; i < UFO_WriteCurveArray::maxCurveData; i++)
    {
        curveDataUI[i].doCurveUI.SetValue(false);
        curveDataUI[i].SetAllSensitive();
    }
}


