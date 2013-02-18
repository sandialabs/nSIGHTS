///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ColorBasePS.cpp
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
//      File I/O and user interface for all genPFOGL functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <genClass/DO_ColorMap.h>
#include <genClass/DO_ColorLimit.h>

#include "PPO_ColorBasePS.h"

/////////////////////////////////////////////////////////////////
// PPO_ColorBasePS

PPO_ColorBasePS::PPO_ColorBasePS(const type_info& dataObjType) :
    inputDataObjRefUI(dataObjType),
    colorMapObjRefUI(typeid(DO_ColorMap)),
    limitSelectionMS(typeid(DO_ColorLimit), false)
{
}

void PPO_ColorBasePS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_ColorBasePS*>(inObj)->SetAllSensitive();
}

void PPO_ColorBasePS::SetAllSensitive()
{
    bool isMaster = limitSelectionMS.isMaster.GetValue();

    limitFrame.SetSensitive(isMaster);
    colorMapObjRefUI.SetSensitive(isMaster);
    clipToRangeBox.SetSensitive(isMaster); ;

    if (isMaster)
    {
        minLimitField.SetSensitive(! autoLimitToggle.GetValue());
        maxLimitField.SetSensitive(! autoLimitToggle.GetValue());
        baseAutoDataSourceUI.SetSensitive(autoLimitToggle.GetValue());
    }
}

// ****************************************

void PPO_ColorBasePS::InitMenu(const PPO_Base& inData,
                                     bool        splitTopRow,
                               const char**     autoLimitStrings,
                                     int         nautoLimit)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC args;

    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.TopFormAttach();
    args.LeftFormAttach();
    if (splitTopRow)
        args.RightPosAttach(50);
    else
        args.RightFormAttach(10);
    inputDataObjRefUI.InitFramedUI("Data To Plot", topRowForm.GetWidget(), args);

    args.StdTopObjectAttach(topRowForm.GetWidget());
    InitMenu(inData, args, autoLimitStrings, nautoLimit);
 }

void PPO_ColorBasePS::InitMenu(const PPO_Base&   inData,
                                     ArgListC&  attachments,
                               const char**      autoLimitStrings,
                                     int         nautoLimit)
{
    limitAndMapForm.InitWidget(PPform.GetWidget(), attachments);

    ArgListC    args;

    args.StdSplitSetup();
    limitSelectionMS.horizontal = false;
    limitSelectionMS.InitFramedUI("Limit Selection", limitAndMapForm.GetWidget(), args);
    limitSelectionMS.isMaster.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish();
    colorMapObjRefUI.InitFramedUI("Color Map", limitAndMapForm.GetWidget(), args);

    args.StdTopObjectAttach(limitAndMapForm.GetWidget());
    outsideForm.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(80);
    limitFrame.InitFramedUI("Data Limits", outsideForm.GetWidget(), args);

    //  split - left for toggles and auto basis, right for min max
    limitSplit.leftEnd = 50;
    args.Clear();
    args.AllFormAttach();
    limitSplit.InitFormUI(limitFrame.GetForm(), args);

    //  toggles
    args.Clear();
    args.TopLeftAttach(5, 10);
    autoLimitToggle.StdToggleInit("Auto", limitSplit.GetLeft(), args);
    autoLimitToggle.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.TopFormAttach(5);
    args.RightFormAttach(10);
    logRangeToggle.StdToggleInit("Log", limitSplit.GetLeft(), args);

    //  toggles
    args.StdTopObjectAttach(autoLimitToggle.GetWidget(), 10);
    baseAutoDataSourceUI.PreInitSetUp(autoLimitStrings, nautoLimit);
    baseAutoDataSourceUI.InitLabeledUI("Auto source", limitSplit.GetLeft(), args);

    // min and max in right
    args.StdFormAttach();
    args.TopOffset(10);
    minLimitField.InitLabeledUI("Min:", limitSplit.GetRight(), args);

    args.StdTopObjectAttach(minLimitField);
    maxLimitField.InitLabeledUI("Max:", limitSplit.GetRight(), args);

    //  Out of Range
    args.StdSplitFinish(20);
    clipToRangeBox.PreInitSetUp("Extend", "Clip", false);
    clipToRangeBox.InitFramedUI("Out-of-Range", outsideForm.GetWidget(), args);
}

void PPO_ColorBasePS::MenuOpen(PSC_ColorBase&  inColorBase)
{
    MenuBaseC::MenuOpen(true, false);

    inputDataObjRefUI.StdMenuInit(inColorBase.inputDataObjRef);
    limitSelectionMS.masterObjectList.StdMenuInit(inColorBase.limitMasterObjRef);
    colorMapObjRefUI.StdMenuInit(inColorBase.colorMapObjRef);
}

void PPO_ColorBasePS::SetMenuData(PPO_Base& inObj,
                                  PSC_ColorBase&  inColorBase)

{
    PPO_BasePS::SetMenuData(inObj);

    inputDataObjRefUI.SetFuncObjRef(inColorBase.inputDataObjRef);
    limitSelectionMS.SetMasterSlaveSettings(inColorBase.limitIsMaster, inColorBase.limitMasterObjRef);
    autoLimitToggle.SetValue(inColorBase.autoLimits);
    logRangeToggle.SetValue(inColorBase.rangeLimit.logRange);
    minLimitField.SetFieldValue(inColorBase.rangeLimit.minLimit);
    maxLimitField.SetFieldValue(inColorBase.rangeLimit.maxLimit);
    baseAutoDataSourceUI.SetChoice(inColorBase.baseAutoDataSource);
    clipToRangeBox.SetBoolChoice(inColorBase.clipToRange);
    colorMapObjRefUI.SetFuncObjRef(inColorBase.colorMapObjRef);
    SetAllSensitive();
}

bool PPO_ColorBasePS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    inputDataObjRefUI.FuncObjRefOK(allOK, "Data source");
    colorMapObjRefUI.FuncObjRefOK(allOK, "Color Map");
    limitSelectionMS.MasterSlaveOK(allOK, "Limit");
    if (!autoLimitToggle.GetValue())
    {
        minLimitField.FieldValueOK(allOK, "Limit minimum");
        maxLimitField.FieldValueOK(allOK, "Limit maximum");
        if (allOK && (minLimitField.GetFieldValue() > maxLimitField.GetFieldValue()))
        {
            PropErrorMsg("Max must be > than min");
            allOK = false;
        }
    }
    return allOK;
}

void PPO_ColorBasePS::GetMenuData(PPO_Base&     outObj,
                                  PSC_ColorBase& outColorBase)
{
    PPO_BasePS::GetMenuData(outObj);

    outColorBase.inputDataObjRef = inputDataObjRefUI.GetFuncObjRef();
    limitSelectionMS.GetMasterSlaveSettings(outColorBase.limitIsMaster, outColorBase.limitMasterObjRef);
    outColorBase.autoLimits          = autoLimitToggle.GetValue();
    outColorBase.rangeLimit.logRange = logRangeToggle.GetValue();

    if (!outColorBase.autoLimits)
    {
        outColorBase.rangeLimit.minLimit = minLimitField.GetFieldValue();
        outColorBase.rangeLimit.maxLimit = maxLimitField.GetFieldValue();
    }
    outColorBase.baseAutoDataSource = baseAutoDataSourceUI.GetChoice();
    outColorBase.clipToRange = clipToRangeBox.GetBoolChoice();
    outColorBase.colorMapObjRef = colorMapObjRefUI.GetFuncObjRef();
}

