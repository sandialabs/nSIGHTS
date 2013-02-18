///////////////////////////////////////////////////////////////////////////////////
//
// U_MenuDPO.cpp
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
// DESCRIPTION: Code which defines and implements the basic framework user-interface (UI)
//              and provides OS platform specific implementation of common facilities
//              such as copy/paste and configuration file I/O.
//              basic UI support classes used primarily by DPO_XXXPS functional objects
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "U_Menu.h"

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClassPS/PS_Color.h>

#include "U_MenuDPO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace genAppMenuUtilities {


    extern void PropSetColorLabels(BaseUIC* labeledUIC[],  // assumed 3
                                   bool     useHSV)
    {
        static char* HSVlabs[3] = {"Hue ", "Sat.  ", "Value "};
        static char* RGBlabs[3] = {"Red ", "Green ", "Blue  "};

        char** currLabs;
        if (useHSV)
            currLabs = HSVlabs;
        else
            currLabs = RGBlabs;

        for (int i = 0; i < 3; i++)
            labeledUIC[i]->ChangeLabel(currLabs[i]);
    }

    // ************************************************************* widgets for specific data types

    ColorMapDisplay::ColorMapDisplay()
    {
    }

    void ColorMapDisplay::DrawColorMap(const DC_ColorMap& newMap)
    {
        colorMap = newMap;
        Invalidate();
        UpdateWindow();
    }

    void ColorMapDisplay::OnDraw(CDC* pDC)
    {
        CRect rect;
        GetClientRect(rect);
        int width = rect.Width();
        int height = rect.Height();

        int y = 0;
        int x = 0;
        double delta =  width / double(colorMap.ncolorMap);
        int dx = int(delta) + 1;

        for (int i = 0; i < colorMap.ncolorMap; i++)
            {
                //  convert to color and draw
                COLORREF currColor  = ConvertSC_ColorSpec(colorMap.colorMapColors[i]);
                x = int(delta * i);
                pDC->FillSolidRect(x, y, dx, height, currColor);
            }
    }


    void TimeBaseUIC::StdInit(      Widget     parent,
                                    const ArgListC&  inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);

        ArgListC args;

        static const char*   tbuLabels[]  = {"Second", "Minute", "Hour", "Day", "Year"};
        inputTBU.PreInitSetUp(tbuLabels, 5);
        outputTBU.PreInitSetUp(tbuLabels, 5);

        args.Clear();
        args.TopFormAttach();
        args.LeftFormAttach();
        args.RightPosAttach(50);
        inputTBU.InitFramedUI("Input Time Units", GetForm(), args);

        args.Clear();
        args.TopFormAttach();
        args.LeftUICAttach(inputTBU);
        args.RightFormAttach();
        outputTBU.InitFramedUI("Output Time Units", GetForm(), args);
    }

    void TimeBaseUIC::SetTimeBase(const DSC_TimeBase& inTB)
    {
        inputTBU.SetChoice(int(inTB.inputTimeBase));
        outputTBU.SetChoice(int(inTB.outputTimeBase));
    }

    void TimeBaseUIC::GetTimeBase(DSC_TimeBase& outTB)
    {
        outTB.inputTimeBase  = DSC_TimeBase::TimeBaseUnit(inputTBU.GetChoice());
        outTB.outputTimeBase = DSC_TimeBase::TimeBaseUnit(outputTBU.GetChoice());
    }


    void ThresholdUIC::StdInit(      Widget     parent,
                                     const  ArgListC&  inArgs,
                                     bool       horizontal)
    {
        BaseUIC::InitFormUI(parent, inArgs);

        ArgListC args;

        args.Clear();
        if (horizontal)
            {
                args.TopFormAttach();
                args.LeftFormAttach();
                args.RightPosAttach(50);
            }
        else
            args.StdFormAttach();
        minThresholdFrame.InitFramedUI("Minimum Thresholding", GetForm(), args);

        args.Clear();
        args.TopFormAttach();
        args.LeftFormAttach();
        args.RightPosAttach(35);
        static const char*  thresholdLabels[]  = {"None", "Null", "Clamp"};
        minThresholdUI.PreInitSetUp(thresholdLabels, 3);
        minThresholdUI.InitWidget(minThresholdFrame.GetForm(), args);

        args.Clear();
        args.TopFormAttach();
        args.LeftPosAttach(50);
        args.RightFormAttach();
        minValUI.InitWidget(minThresholdFrame.GetForm(), args);

        args.Clear();
        if (horizontal)
            {
                args.TopFormAttach();
                args.RightFormAttach();
                args.LeftPosAttach(50);
            }
        else
            args.StdTopObjectAttach(minThresholdFrame);

        maxThresholdFrame.InitFramedUI("Maximum Thresholding", GetForm(), args);

        args.Clear();
        args.TopFormAttach();
        args.LeftFormAttach();
        args.RightPosAttach(35);

        maxThresholdUI.PreInitSetUp(thresholdLabels, 3);
        maxThresholdUI.InitWidget(maxThresholdFrame.GetForm(), args);

        args.Clear();
        args.TopFormAttach();
        args.LeftPosAttach(50);
        args.RightFormAttach();
        maxValUI.InitWidget(maxThresholdFrame.GetForm(), args);
    }

    void ThresholdUIC::SetThreshold(const DSC_Threshold& inTh)
    {
        minThresholdUI.SetChoice(int(inTh.minThreshold));
        minValUI.SetFieldValue(inTh.minVal);

        maxThresholdUI.SetChoice(int(inTh.maxThreshold));
        maxValUI.SetFieldValue(inTh.maxVal);
    }

    bool ThresholdUIC::UIdataOK(bool& allOK)
    {
        if (minThresholdUI.GetChoice() > 0)
            minValUI.FieldValueOK(allOK, "min threshold");

        if (maxThresholdUI.GetChoice() > 0)
            maxValUI.FieldValueOK(allOK, "max threshold");

        return allOK;
    }


    void ThresholdUIC::GetThreshold(DSC_Threshold& outTh)
    {
        outTh.minThreshold = DSC_Threshold::ThresholdOp(minThresholdUI.GetChoice());
        outTh.maxThreshold = DSC_Threshold::ThresholdOp(maxThresholdUI.GetChoice());
        outTh.minVal       = minValUI.GetFieldValue();
        outTh.maxVal       = maxValUI.GetFieldValue();
    }


    void ThresholdUIC::SetInternalSensitive()
    {
        minValUI.SetSensitive(minThresholdUI.GetChoice() > 0);
        maxValUI.SetSensitive(maxThresholdUI.GetChoice() > 0);
    }


    RealMSSpecUI::RealMSSpecUI(const type_info& inType) : realValueMS(inType, true)
    {
    }

    RealMSSpecUI::RealMSSpecUI(const type_info& inType, bool expOK) : realValueMS(inType, expOK)
    {
    }

    void RealMSSpecUI::SetSensitive(bool inSens)
    {
        BaseUIC::SetSensitive(inSens);  // for some reason this isn't enough ..
        realValueMS.SetSensitive(inSens);
        if (inSens)
            SetAllSensitive();
        else
            realValueUI.SetSensitive(false);
    }

    void RealMSSpecUI::SetAllSensitive()
    {
        if (realValueMS.exposeOK)
            realValueUI.SetSensitive(realValueMS.isMaster.GetValue() && (!realValueMS.isExposed.GetValue()));
        else
            realValueUI.SetSensitive(realValueMS.isMaster.GetValue());
    }


    void RealMSSpecUI::MenuInit(DSC_RealMSSpec& iSpec)
    {
        realValueMS.masterObjectList.StdMenuInit(iSpec.valueMasterObjRef);
    }

    void RealMSSpecUI::SetValue(const DSC_RealMSSpec&    iSpec,
                                ExposedReal*          exposedSpec)
    {
        realValueMS.SetMasterSlaveSettings(iSpec.valueIsMaster, exposedSpec, iSpec.valueMasterObjRef);
        realValueUI.SetFieldValue(iSpec.enteredRealValue);
        SetAllSensitive();
    }

    void RealMSSpecUI::SetValue(const DSC_RealMSSpec&    iSpec,
                                ExposedRealWrapper& exposedSpec)
    {
        realValueMS.SetMasterSlaveSettings(iSpec.valueIsMaster, exposedSpec.exposedObj, iSpec.valueMasterObjRef);
        realValueUI.SetFieldValue(iSpec.GetCurrentValue());
        SetAllSensitive();
    }


    void RealMSSpecUI::SetValue(const DSC_RealMSSpec& iSpec)
    {
        realValueMS.SetMasterSlaveSettings(iSpec.valueIsMaster, iSpec.valueMasterObjRef);
        realValueUI.SetFieldValue(iSpec.GetCurrentValue());
        SetAllSensitive();
    }


    void RealMSSpecUI::GetValue(DSC_RealMSSpec& oSpec)
    {
        realValueMS.GetMasterSlaveSettings(oSpec.valueIsMaster, oSpec.valueMasterObjRef);
        if (oSpec.valueIsMaster)
            oSpec.enteredRealValue  = realValueUI.GetFieldValue();
    }

    bool RealMSSpecUI::UIdataOK(bool& allOK)
    {
        if (realValueMS.isMaster.GetValue())
            realValueUI.FieldValueOK(allOK, "Master value");
        return allOK;
    }

    bool RealMSSpecUI::UIdataOK(bool& allOK, const double& minVal, const double& maxVal)
    {
        if (realValueMS.isMaster.GetValue())
            realValueUI.FieldValueOK(allOK, "Master value", minVal, maxVal);
        return allOK;
    }



    void RealMSSpecUI::InitUI(char*       specID,
                              ArgListC&   topAttachments,
                              Widget      parent,
                              bool        horizontal,
                              bool        sameRow)
    {
        BaseUIC::InitFormUI(parent, topAttachments);

        ArgListC  args;
        if (sameRow)
            args.StdSplitSetup(65);
        else
            args.StdFormAttach();

        char tempStr[80];
        CopyString(tempStr, specID, 80);
        ConcatString(tempStr, " Value Source", 80);

        realValueMS.horizontal = horizontal;
        realValueMS.compressed = horizontal;
        realValueMS.InitFramedUI(tempStr, GetForm(), args);

        args.Clear();
        if (sameRow)
            args.StdSplitFinish(35);
        else
            args.StdTopObjectAttach(realValueMS);


        CopyString(tempStr, specID, 80);
        ConcatString(tempStr, " Value", 80);
        realValueUI.AttachObjectToFrame();
        realValueUI.InitFramedUI(tempStr, GetForm(), args);
    }


    void RealMSSpecUI::InitUI(char*      specID,
                              ArgListC&  topAttachments,
                              Widget     parent,
                              bool       horizontal,
                              bool       sameRow,
                              void       (*ToggleCB)(void* inOb),
                              void*      otherObj)
    {
        InitUI(specID, topAttachments, parent, horizontal, sameRow);
        realValueMS.isMaster.AddOtherObjectCallback(ToggleCB, otherObj);
        if (realValueMS.exposeOK)
            realValueMS.isExposed.AddOtherObjectCallback(ToggleCB, otherObj);
    }


    IndexMSSpecUI::IndexMSSpecUI(const type_info& inType) :
        indexValueMS(inType, true),
        snglmultOK(false), currMS(0)
    {
    }

    IndexMSSpecUI::IndexMSSpecUI(const type_info& inType, bool expOK) :
        indexValueMS(inType, expOK),
        snglmultOK(false), currMS(0)
    {
    }

    void IndexMSSpecUI::SetSensitive(bool inSens)
    {
        BaseUIC::SetSensitive(inSens);
        indexValueMS.SetSensitive(inSens);
        if (inSens)
            SetAllSensitive();
        else
            indexListUI.SetSensitive(false);
    }


    void IndexMSSpecUI::SetAllSensitive()
    {
        bool uiOK  = indexValueMS.isMaster.GetValue();
        if (indexValueMS.exposeOK)
            uiOK  = uiOK && (!indexValueMS.isExposed.GetValue());

        indexListUI.SetSensitive(uiOK);
        if (snglmultOK)
            isMultiple.SetSensitive(uiOK);
    }


    void IndexMSSpecUI::MenuInit(DSC_IndexMSSpec& iSpec)
    {
        indexValueMS.masterObjectList.StdMenuInit(iSpec.indexMasterObjRef);
    }

    void IndexMSSpecUI::SetSnglMultValue(const DSC_IndexMSSpec& iSpec)
    {
        indexListUI.SetSelectionPolicy(iSpec.CurrentIndexIsSingle());

        if (iSpec.indexDO.SingleOK())
            indexListUI.SetSingleSelection(iSpec.GetCurrentIndex());

        if (iSpec.indexDO.MultipleOK())
            indexListUI.SetMultipleSelection(iSpec.GetCurrentIndexArray());

        if (snglmultOK)
            isMultiple.SetValue(!iSpec.CurrentIndexIsSingle());

        SetAllSensitive();
    }


    void IndexMSSpecUI::SetValue(const DSC_IndexMSSpec&    iSpec,
                                 ExposedListWrapper&       exposedSpec)
    {
        currMS = &iSpec;
        indexValueMS.SetMasterSlaveSettings(iSpec.indexIsMaster, exposedSpec.exposedObj, iSpec.indexMasterObjRef);
        SetSnglMultValue(iSpec);
    }


    void IndexMSSpecUI::SetValue(const DSC_IndexMSSpec&  iSpec)
    {
        indexValueMS.SetMasterSlaveSettings(iSpec.indexIsMaster, iSpec.indexMasterObjRef);
        SetSnglMultValue(iSpec);
    }

    void IndexMSSpecUI::UpdateList(const DSC_IndexMSSpec&  iSpec)
    {
        if (iSpec.CurrentIndexIsSingle())
            indexListUI.SetSingleSelection(iSpec.GetCurrentIndex());
        else
            indexListUI.SetMultipleSelection(iSpec.GetCurrentIndexArray());
    }


    void IndexMSSpecUI::GetValue(DSC_IndexMSSpec& oSpec)
    {
        indexValueMS.GetMasterSlaveSettings(oSpec.indexIsMaster, oSpec.indexMasterObjRef);
        if (oSpec.indexIsMaster)
            {
                if (oSpec.indexDO.SingleOK())
                    oSpec.enteredIndexValue  = indexListUI.GetSingleSelection();

                if (oSpec.indexDO.MultipleOK())
                    oSpec.enteredIndexArray = indexListUI.currMultipleSelection;

                if (snglmultOK)
                    oSpec.enteredIndexIsSingle = indexListUI.isSingleSel;

            }
    }

    bool IndexMSSpecUI::UIdataOK(bool& allOK)
    {
        return allOK;
    }


    void IndexMSSpecUI::SetIndexSensitive()
    {
        SetAllSensitive();
    }
    void IndexMSSpecUI::SetIndexSensitiveCB(void* inObj)
    {
        static_cast<IndexMSSpecUI*>(inObj)->SetIndexSensitive();
    }

    void  IndexMSSpecUI::SetIndexSelection()
    {
        CWaitCursor w; // this can be time consuming
        indexListUI.SetSelectionPolicy(!isMultiple.GetValue());
    }

    void IndexMSSpecUI::SetIndexSelectionCB(void* inObj)
    {
        static_cast<IndexMSSpecUI*>(inObj)->SetIndexSelection();
    }

    void IndexMSSpecUI::SetExposeSinglMult()
    {
        // can you say kluge ??
        if (snglmultOK && (currMS != 0))
            const_cast<DSC_IndexMSSpec*>(currMS)->enteredIndexIsSingle = ! isMultiple.GetValue();
    }

    void IndexMSSpecUI::SetExposeSinglMultCB(void* inObj)
    {
        static_cast<IndexMSSpecUI*>(inObj)->SetExposeSinglMult();
    }


    void IndexMSSpecUI::InitUI(const char*  specID,
                               ArgListC&    topAttachments,
                               Widget       parent,
                               bool         snglOK,
                               bool         multOK)

    {
        BaseUIC::InitFormUI(parent, topAttachments);

        ArgListC  args;
        args.StdFormAttach();

        char tempStr[80];
        CopyString(tempStr, specID, 80);
        ConcatString(tempStr, " Value Source", 80);

        int expPos = 50;
        if (snglOK && multOK)
            expPos = 35;

        indexValueMS.InitFramedUI(tempStr, GetForm(), args, expPos);


        if (snglOK && multOK)
            {
                snglmultOK = true;
                FormWidget* toggleParent = indexValueMS.GetToggleForm();

                args.Clear();
                args.BottomFormAttach();
                args.BottomOffset(5);
                args.LeftOffset(5);
                args.LeftPosAttach(70);

                if (indexValueMS.compressed)
                    isMultiple.StdToggleInit("Mult", toggleParent, args);
                else
                    isMultiple.StdToggleInit("Multiple", toggleParent, args);
                isMultiple.AddOtherObjectCallback(SetIndexSelectionCB, this);

                indexValueMS.isMaster.AddOtherObjectCallback(SetIndexSensitiveCB, this);
                if (indexValueMS.exposeOK)
                    indexValueMS.isExposed.AddOtherObjectCallback(SetIndexSensitiveCB, this);
            }

        args.Clear();
        args.StdTopObjectAttach(indexValueMS);
        args.BottomFormAttach();

        if (!snglOK)
            indexListUI.isSingleSel = false;

        indexListUI.InitWidget(GetForm(), args);
    }


    void IndexMSSpecUI::InitUI(const char* specID,
                               ArgListC&   topAttachments,
                               Widget      parent,
                               void        (*ToggleCB)(void* inOb),
                               void*       otherObj)
    {
        InitUI(specID, topAttachments, parent, true, false);
        indexValueMS.isMaster.AddOtherObjectCallback(ToggleCB, otherObj);
        if (indexValueMS.exposeOK)
            indexValueMS.isExposed.AddOtherObjectCallback(ToggleCB, otherObj);
    }


    void IndexMSSpecUI::InitUI(const char* specID,
                               ArgListC&   topAttachments,
                               Widget      parent,
                               void        (*ToggleCB)(void* inOb),
                               void*       otherObj,
                               bool        snglOK,
                               bool        multOK)
    {
        InitUI(specID, topAttachments, parent, snglOK, multOK);
        indexValueMS.isMaster.AddOtherObjectCallback(ToggleCB, otherObj);
        if (indexValueMS.exposeOK)
            {
                if (snglmultOK)  // kluge CB
                    indexValueMS.isExposed.AddOtherObjectCallback(SetExposeSinglMultCB, this);
                indexValueMS.isExposed.AddOtherObjectCallback(ToggleCB, otherObj);
            }
    }


    StatusGroupUIC::StatusGroupUIC()
    {
        statusID.Alloc(maxStatus);
    }

    void StatusGroupUIC::StdInit(const char*       title,
                                 Widget            parent,
                                 const  ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(title, parent, inArgs);

        ninit = statusID.Size();
        if ((ninit < 1) || (ninit > maxStatus))
            GenAppInternalError("StatusGroupUIC::StdInit");


        ArgListC args;
        for (int i = 0; i < ninit; i++)
            {
                args.StdEvenSplitSetup(i, statusID.Size());
                args.RightOffset(5);
                args.LeftOffset(5);
                statusUI[i].InitLabeledUI(statusID[i], GetForm(), args);
            }

    }

    void  StatusGroupUIC::SetStatus(const FuncObjC&   inObj,
                                    int               stat1)
    {
        BaseUIC::SetSensitive(inObj.StatusOK());
        if (inObj.StatusNotOK())
            {
                for (int i = 0; i < ninit; i++)
                    statusUI[i].SetStatusNA();
            }
        else
            statusUI[0].SetFieldValue(stat1);

    }

    void  StatusGroupUIC::SetStatus(const FuncObjC& inObj,
                                    int              stat1,
                                    int              stat2)
    {
        SetStatus(inObj, stat1);
        if (inObj.StatusOK())
            statusUI[1].SetFieldValue(stat2);
    }

    void  StatusGroupUIC::SetStatus(const FuncObjC& inObj,
                                    int              stat1,
                                    int              stat2,
                                    int              stat3)
    {
        SetStatus(inObj, stat1, stat2);
        if (inObj.StatusOK())
            statusUI[2].SetFieldValue(stat3);
    }

    void  StatusGroupUIC::SetStatus(const FuncObjC& inObj,
                                    int              stat1,
                                    int              stat2,
                                    int              stat3,
                                    int              stat4)
    {
        SetStatus(inObj, stat1, stat2, stat3);
        if (inObj.StatusOK())
            statusUI[3].SetFieldValue(stat4);
    }


    NodeFileArrayUIC::NodeFileArrayUIC()
    {
        nodeFileID.Alloc(maxNodeFile);
    }

    void NodeFileArrayUIC::StdInit(Widget            parent,
                                   const  ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI("MPI Directory Support", parent, inArgs);

        ninit = nodeFileID.Size();
        if ((ninit < 1) || (ninit > maxNodeFile))
            GenAppInternalError("NodeFileArrayUIC::StdInit");

        ArgListC args;
        for (int i = 0; i < ninit; i++)
            {
                args.StdToggleSetup(5 + i * 100 / nodeFileID.Size());
                nodeFileUI[i].StdToggleInit(nodeFileID[i], GetForm(), args);
            }
    }

    void  NodeFileArrayUIC::SetNodeFileArray(const NodeFileArray& inData)
    {
        if (inData.Size() != ninit)
            GenAppInternalError("NodeFileArrayUIC::SetNodeFileArray");
        for (int i = 0; i < ninit; i++)
            nodeFileUI[i].SetValue(inData[i].nodeBasedFile);
    }

    void  NodeFileArrayUIC::GetNodeFileArray(NodeFileArray& inData)
    {
        if (inData.Size() != ninit)
            GenAppInternalError("NodeFileArrayUIC::SetNodeFileArray");
        for (int i = 0; i < ninit; i++)
            inData[i].nodeBasedFile = nodeFileUI[i].GetValue();
    }


};  // end namespace

