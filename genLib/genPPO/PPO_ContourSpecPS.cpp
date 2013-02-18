///////////////////////////////////////////////////////////////////////////////////
//
// PPO_ContourSpecPS.cpp
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

#include <genClass/DO_ContourSpec.h>
#include "PPO_ContourSpecPS.h"

PPO_ContourSpecPS::PPO_ContourSpecPS(const type_info& dataObjType) :
    dataObjRefUI(dataObjType),
    specMS(typeid(DO_ContourSpec), false)
{
    overrideMaster = false;
}

void PPO_ContourSpecPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<PPO_ContourSpecPS*>(inObj)->SetAllSensitive();
}

void PPO_ContourSpecPS::SetAllSensitive()
{
    bool isMaster = specMS.isMaster.GetValue();
    calcIncValUI.SetSensitive(isMaster);
    doLogIncUI.SetSensitive(isMaster);
    incFrame.SetSensitive(isMaster);
    legendGCUI.SetSensitive(true);

    formatFrame.SetSensitive(isMaster || overrideMaster);
    if (isMaster || overrideMaster)
    {
        contourLineTypeUI.SetLineColorAndThick(contourPenUI.GetSelectedColorValue(), contourLineThkUI.GetLineThick());
    }

    if (isMaster)
    {
        bool calcIncs = calcIncValUI.GetBoolChoice();
        endValUI.SetSensitive(calcIncs);
        incValUI.SetSensitive(!calcIncs);
    }
}

// ****************************************

void PPO_ContourSpecPS::InitMenu(const PPO_Base& inData,
                                 bool        splitTopRow)
{
    PPO_BasePS::InitMenu(inData);

    ArgListC    args;
    args.StdFormAttach();
    topRowForm.InitWidget(PPform.GetWidget(), args);

    args.TopFormAttach();
    args.LeftFormAttach();
    if (splitTopRow)
        args.RightPosAttach(50);
    else
        args.RightFormAttach(10);
    dataObjRefUI.InitFramedUI("Data To Plot", topRowForm.GetWidget(), args);

    args.Clear();
    args.StdTopObjectAttach(topRowForm.GetWidget());

    InitMenu(inData, args);
 }

void PPO_ContourSpecPS::InitMenu(const PPO_Base&   inData,
                                     ArgListC&  attachments)
{
    specMS.horizontal = true;
    specMS.InitFramedUI("Contour Specification", PPform.GetWidget(), attachments);
    specMS.isMaster.AddOtherObjectCallback(SetAllSensitiveCB, this);

    ArgListC args;
    args.StdTopObjectAttach(specMS);
    contourOptions.PreInitSetUp(0, 30, 70);
    contourOptions.InitFormUI(PPform.GetWidget(), args);

    // range widget
    args.StdFormAttach();
    args.RightOffset(10);
    calcIncValUI.PreInitSetUp("Specified", "Calculated",  false);
    calcIncValUI.InitFramedUI("Increment Size", contourOptions.GetLeft(), args);
    calcIncValUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    // log/lin widget
    args.StdTopObjectAttach(calcIncValUI);
    args.RightOffset(10);
//    args.BottomFormAttach();   // Radim -- if this is uncommented, the frame disappears
    doLogIncUI.PreInitSetUp("Linear", "Logarithmic", false);
    doLogIncUI.InitFramedUI("Increment Type", contourOptions.GetLeft(), args);

    // st end incval
    args.AllFormAttach();
    incFrame.InitFramedUI("Increment Values", contourOptions.GetMiddle(), args);

    args.StdFormAttach();
    startValUI.SetFieldLen(8);
    startValUI.InitLabeledUI("Start:", incFrame.GetForm(), args);

    args.StdTopObjectAttach(startValUI);
    endValUI.SetFieldLen(8);
    endValUI.InitLabeledUI("End:", incFrame.GetForm(), args);

    args.StdTopObjectAttach(endValUI);
    incValUI.SetFieldLen(8);
    incValUI.InitLabeledUI("Inc size:", incFrame.GetForm(), args);

    args.StdTopObjectAttach(incValUI);
    nIncsUI.SetFieldLen(4);
    nIncsUI.InitLabeledUI("# of inc", incFrame.GetForm(), args);

    // line format
    args.AllFormAttach();
    formatFrame.InitFramedUI("Line Format", contourOptions.GetRight(), args);

    args.Clear();
    args.TopFormAttach();
    args.RightFormAttach();
    contourPenUI.InitWidget(formatFrame.GetForm(), args);
    contourPenUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.TopWidgetAttach(contourPenUI.GetWidget());
    args.TopOffset(5);
    args.RightFormAttach();
    contourLineThkUI.InitWidget(formatFrame.GetForm(), args);
    contourLineThkUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.Clear();
    args.TopWidgetAttach(contourLineThkUI.GetWidget());
    args.TopOffset(5);
    args.RightFormAttach();
    contourLineTypeUI.InitWidget(formatFrame.GetForm(), args);

    args.Clear();
    args.TopWidgetAttach(contourLineTypeUI.GetWidget());
    args.TopOffset(5);
    args.RightOffset(10);
    args.RightFormAttach();
    mitreLinesUI.StdToggleInit("mitre", formatFrame.GetForm(), args);

    // legend
    args.StdTopObjectAttach(contourOptions);
    legendGCUI.InitFramedUI("Legend Label", PPform.GetWidget(), args);
    legendGCUI.overrideDefaultLabelUI.AddOtherObjectCallback(SetAllSensitiveCB, this);
}

void PPO_ContourSpecPS::MenuOpen(PSC_ContourSpec&  inContourSpec)
{
    MenuBaseC::MenuOpen(true, false);
    dataObjRefUI.StdMenuInit(inContourSpec.dataObjRef);
    specMS.masterObjectList.StdMenuInit(inContourSpec.specMasterObjRef);

}

void PPO_ContourSpecPS::SetMenuData(PPO_Base& inObj,
                                    PSC_ContourSpec&  inContourSpec)

{
    PPO_BasePS::SetMenuData(inObj);

    dataObjRefUI.SetFuncObjRef(inContourSpec.dataObjRef);

    specMS.SetMasterSlaveSettings(inContourSpec.specIsMaster, inContourSpec.specMasterObjRef);

    calcIncValUI.SetBoolChoice(inContourSpec.calcIncVal);
    doLogIncUI.SetBoolChoice(inContourSpec.doLogInc);

    startValUI.SetFieldValue(inContourSpec.startVal);
    endValUI.SetFieldValue(inContourSpec.endVal);
    incValUI.SetFieldValue(inContourSpec.incVal);
    nIncsUI.SetFieldValue(inContourSpec.nIncs);

    contourLineThkUI.SetLineThick(inContourSpec.contourLineSpec.lineThickness);
    contourPenUI.SetPenSelection(inContourSpec.contourLineSpec.linePen);
    contourLineTypeUI.SetLineType(inContourSpec.contourLineSpec.lineType);
    mitreLinesUI.SetValue(inContourSpec.contourLineSpec.mitreLines);

    legendGCUI.SetLegendOverride(inContourSpec.legendGC);
    SetAllSensitive();
}

bool PPO_ContourSpecPS::UIdataOK()
{
    PPO_BasePS::UIdataOK();
    dataObjRefUI.FuncObjRefOK(allOK, "Contour input source");
    if (specMS.isMaster.GetValue())
    {
        startValUI.FieldValueOK(allOK, "Start val");
        nIncsUI.FieldValueOK(allOK, "# incs", 0, PSC_ContourSpec::maxContourIncs);

        if (calcIncValUI.GetBoolChoice())
            endValUI.FieldValueOK(allOK, "End val");
        else
            incValUI.FieldValueOK(allOK, "Inc val");
    }
 return allOK;
}

void PPO_ContourSpecPS::GetMenuData(PPO_Base&    outObj,
                                    PSC_ContourSpec&  outContourSpec)
{
    PPO_BasePS::GetMenuData(outObj);

    outContourSpec.dataObjRef = dataObjRefUI.GetFuncObjRef();

    specMS.GetMasterSlaveSettings(outContourSpec.specIsMaster, outContourSpec.specMasterObjRef);
    if (outContourSpec.specIsMaster)
    {
        outContourSpec.calcIncVal = calcIncValUI.GetBoolChoice();
        outContourSpec.doLogInc = doLogIncUI.GetBoolChoice();

        outContourSpec.startVal = startValUI.GetFieldValue();
        if (outContourSpec.calcIncVal)
            outContourSpec.endVal = endValUI.GetFieldValue();
        else
            outContourSpec.incVal = incValUI.GetFieldValue();
        outContourSpec.nIncs = nIncsUI.GetFieldValue();
    }

    outContourSpec.overrideMasterLineSpec = overrideMaster;
    if (outContourSpec.specIsMaster || overrideMaster)
    {
        outContourSpec.contourLineSpec.lineThickness  = contourLineThkUI.GetLineThick();
        outContourSpec.contourLineSpec.linePen  = contourPenUI.GetPenSelection();
        outContourSpec.contourLineSpec.lineType = contourLineTypeUI.GetLineType();
        outContourSpec.contourLineSpec.mitreLines = mitreLinesUI.GetValue();
    }

    legendGCUI.GetLegendOverride(outContourSpec.legendGC);
}

