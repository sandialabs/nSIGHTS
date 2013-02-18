///////////////////////////////////////////////////////////////////////////////////
//
// PS_UIClass.cpp
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
// DESCRIPTION: Platform Specific but application independent code.
//              Adds labels or frames to individual PS_UIBase controls
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"
#include <genClass/U_Msg.h>

#include <math.h>

#include <genClassPS/PS_UIClass.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ************************************************************* UI Classes

namespace genClassPS {

    AttachUIC::AttachUIC()
    {
        topAttach       = false;
        bottomAttach    = true;

        topOffset       = 2;
        bottomOffset    = 2;
        rightOffset     = 5;
        leftOffset      = 5;
    }

    void AttachUIC::SetOffsets(int top,               //  if > -1 then sets attachments
                               int bottom,
                               int left,
                               int right)
    {
        if (top >= 0)
            topOffset = top;
        if (bottom >= 0)
            bottomOffset = bottom;
        if (left >= 0)
            leftOffset = left;
        if (right >= 0)
            rightOffset = right;
    }

    void AttachUIC::SetTopAttach()
    {
        topAttach = true;
        bottomAttach = false;
    }

    void  AttachUIC::CopyOffsetAttach(AttachUIC& srcUIC)
    {
        topAttach       = srcUIC.topAttach   ;
        bottomAttach    = srcUIC.bottomAttach;

        topOffset       = srcUIC.topOffset   ;
        bottomOffset    = srcUIC.bottomOffset;
        rightOffset     = srcUIC.rightOffset ;
        leftOffset      = srcUIC.leftOffset  ;
    }

    void  AttachUIC::SetTBattachArgs(ArgListC&  args)
    {
        if (topAttach)
            {
                args.AddArg(Arg::XmNtopAttachment,       Arg::XmATTACH_FORM);
                args.AddArg(Arg::XmNtopOffset,           topOffset);
            }

        if (bottomAttach)
            {
                args.AddArg(Arg::XmNbottomAttachment,    Arg::XmATTACH_FORM);
                args.AddArg(Arg::XmNbottomOffset,        bottomOffset);
            }
    }

    BaseUIC::BaseUIC()
    {
        interiorLeftAttach = ilaDefault;
    }

    void BaseUIC::InitFramedUI(const char       frameTitle[],
                               Widget           parent,
                               const ArgListC&  inArgs)      // attachment resources + others
    {
        ArgListC args = inArgs;

        args.LeftOffset(3);
        args.RightOffset(3);
        args.BottomOffset(2);

        frame.SetFrameText(frameTitle);
        frame.InitWidget(parent,  args);
    }

    void BaseUIC::InitLabeledUI(const char       labText[],
                                Widget     parent,
                                const ArgListC&  inArgs)      // attachment resources + others
    {
        form.InitWidget(parent, inArgs);

        ArgListC args;
        args.LeftFormAttach(leftOffset);
        if (topAttach)
            args.TopFormAttach(topOffset);
        if (bottomAttach)
            args.BottomFormAttach(bottomOffset + 4);
        /* TODO: implement!
           args.AddArg(XmNalignment,               (XtArgVal) XmALIGNMENT_BEGINNING);
        */
        //label = LeftAlign();

        label.StdLabelInit(labText, &form, args);
    }

    void BaseUIC::AddAttachedLabel(const char*       labText,
                                   BaseWindow& otherObj,
                                   int               bottOffset)
    {
        ArgListC args;
        if (topAttach)
            args.TopFormAttach(topOffset);
        if (bottomAttach)
            args.BottomFormAttach(bottomOffset + bottOffset);
        args.RightWidgetAttach(otherObj.GetWidget());
        args.RightOffset(5);
        label.StdLabelInit(labText, &form, args);
    }

    void BaseUIC::InitFormUI(Widget    parent,
                             const     ArgListC& inArgs)      // attachment resources + others
    {
        form.InitWidget(parent, inArgs);
    }

    void BaseUIC::ChangeLabel(const char* newLab)
    {
        if (frame.IsInitialized())
            {
                frame.SetFrameText(newLab);
            } else
            {
                label.SetLabelText(newLab);
            }
    }


    LayoutManager* BaseUIC::GetForm()
    {
        if (frame.IsInitialized()) {
            return &frame;
        } else {
            return &form;
        }
    }

    LayoutManager* BaseUIC::GetBoundingWidget()
    {
        if (frame.IsInitialized())
            return &frame;
        return &form;
    }

    void  BaseUIC::SetSensitive(bool isSensitive)
    {
        if (frame.IsCreated())
            frame.SetSensitive(isSensitive);
        if (form.IsCreated())
            form.SetSensitive(isSensitive);
        if (label.IsCreated())
            label.SetSensitive(isSensitive);
    }

    void  BaseUIC::Manage()
    {
        if (frame.IsCreated())
            frame.Manage();
        if (form.IsCreated())
            form.Manage();
        if (label.IsCreated())
            label.Manage();
    }

    void  BaseUIC::Unmanage()
    {
        if (frame.IsCreated())
            frame.Unmanage();
        if (form.IsCreated())
            form.Unmanage();
        if (label.IsCreated())
            label.Unmanage();
    }

    // ************************************************************* split form

    SplitFormUIC::SplitFormUIC()
    {
        leftStart = 0;
        leftEnd   = 50;

        topAttach       = true;
        bottomAttach    = true;

        topOffset       = 0;
        bottomOffset    = 0;
        rightOffset     = 0;
        leftOffset      = 0;
    }

    void SplitFormUIC::SetSplitForms(LayoutManager& form)
    {
        ArgListC args;

        if ( topAttach)
            args.TopFormAttach(topOffset);
        if ( bottomAttach)
            args.BottomFormAttach(bottomOffset);

        args.SetPlaceHolder();

        if ( leftStart == 0)
            args.LeftFormAttach();
        else
            args.LeftPosAttach(leftStart);
        args.LeftOffset(leftOffset);

        int midOffset = (leftOffset + rightOffset) / 2;

        args.RightPosAttach(leftEnd);
        args.RightOffset(midOffset);
        leftForm.InitWidget(form, args);

        args.ResetPlace();

        args.LeftBaseAttach(leftForm);
        args.LeftOffset(midOffset);
        args.RightFormAttach(rightOffset);

        rightForm.InitWidget(form, args);
    }

    void SplitFormUIC::InitFramedUI(const char   frameTitle[],
                                    Widget  parent,
                                    const  ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);
        SetSplitForms(frame);
    }


    void SplitFormUIC::InitFormUI(       Widget  parent,
                                         const  ArgListC&  inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);
        SetSplitForms(form);
    }

    // ************************************************************* tri split form

    TriSplitFormUIC::TriSplitFormUIC()
    {
        leftStart = 0;
        leftEnd   = 33;
        midEnd    = 66;

        topAttach       = true;
        bottomAttach    = true;

        topOffset       = 0;
        bottomOffset    = 0;
        rightOffset     = 0;
        leftOffset      = 0;
    }

    void TriSplitFormUIC::SetTriSplitForms(LayoutManager& form)
    {
        ArgListC args;

        if ( topAttach)
            args.TopFormAttach(topOffset);
        if ( bottomAttach)
            args.BottomFormAttach(bottomOffset);

        args.SetPlaceHolder();

        if (leftStart == 0)
            args.LeftFormAttach();
        else
            args.LeftPosAttach(leftStart);
        args.LeftOffset(leftOffset);

        int midOffset = (leftOffset + rightOffset) / 2;

        args.RightPosAttach(leftEnd);
        args.RightOffset(midOffset);
        leftForm.InitWidget(form, args);

        args.ResetPlace();

        args.LeftBaseAttach(leftForm);
        args.LeftOffset(midOffset);
        args.RightPosAttach(midEnd);
        args.RightOffset(midOffset);
        midForm.InitWidget(form, args);

        args.LeftBaseAttach(midForm);
        args.LeftOffset(midOffset);
        args.RightFormAttach(rightOffset);
        rightForm.InitWidget(form, args);
    }

    void TriSplitFormUIC::InitFramedUI(const char   frameTitle[],
                                       Widget  parent,
                                       const  ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);
        SetTriSplitForms(frame);
    }


    void TriSplitFormUIC::InitFormUI(       Widget  parent,
                                            const  ArgListC&  inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);
        SetTriSplitForms(form);
    }


    // ************************************************************* text field


    void TextBaseFieldUIC::InitFramedUI(const  char      frameTitle[],
                                        Widget    parent,
                                        const  ArgListC& inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);

        //  set field widget
        ArgListC args;
        SetWidgetArgs(args);

        if (fieldObj->textFieldLen == 0 || (interiorLeftAttach == ilaWidgetToLeft))
            args.LeftFormAttach(leftOffset);

        fieldObj->InitWidget(frame, args);
    }


    void TextBaseFieldUIC::InitLabeledUI(const  char      labText[],
                                         Widget    parent,
                                         const  ArgListC& inArgs)
    {
        if (interiorLeftAttach == ilaLabelToWidget)
            {
                if (fieldObj->textFieldLen == 0)
                    GenAppInternalError("TextBaseFieldUIC::InitLabeledUI");
                form.InitWidget(parent, inArgs);
            }
        else
            {
                BaseUIC::InitLabeledUI(labText, parent, inArgs);
            }


        //  set field widget
        ArgListC args;
        SetWidgetArgs(args);

        if ((fieldObj->textFieldLen == 0) || (interiorLeftAttach == ilaWidgetToLeft))
            {
                args.LeftBaseAttach(label);
                args.LeftOffset(leftOffset);
            }
        fieldObj->InitWidget(form, args);

        if (interiorLeftAttach == ilaLabelToWidget)
            AddAttachedLabel(labText, *fieldObj, 2);
    }

    void TextBaseFieldUIC::InitFormUI(      Widget    parent,
                                            const ArgListC& inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);

        //  set field widget
        ArgListC args;
        SetWidgetArgs(args);
        if (fieldObj->textFieldLen == 0)
            args.LeftFormAttach(leftOffset);

        fieldObj->InitWidget(form, args);
    }

    void  TextBaseFieldUIC::SetWidgetArgs(ArgListC&  args)
    {
        SetTBattachArgs(args);
        args.RightFormAttach(rightOffset);
    }

    void  TextBaseFieldUIC::SetSensitive(bool isSensitive)
    {
        BaseUIC::SetSensitive(isSensitive);
        fieldObj->DoSetSensitive(isSensitive);
    }


    // ************************************************************* file name
    FileEntryUIC::FileEntryUIC()
    {
    }

    void FileEntryUIC::InitFramedUI(const  char         frameTitle[],
                                    Widget              parent,
                                    const  ArgListC&    inArgs)      // attachment resources + others
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);

        SetButtonOffsets(rightOffset, bottomOffset);

        //  set field widget
        ArgListC args;
        //if (AttachUIC::topAttach)
        args.TopFormAttach(topOffset);
        args.LeftFormAttach(leftOffset);
        args.RightFormAttach(rightOffset);
        args.BottomOffset(bottomOffset);

        InitWidget(frame, args);
    }

    void FileEntryUIC::InitLabeledUI(const  char        labText[],
                                     Widget             parent,
                                     const  ArgListC&   inArgs)      // attachment resources + others
    {
        BaseUIC::InitLabeledUI(labText, parent, inArgs);

        SetButtonOffsets(rightOffset, bottomOffset);

        //  set field widget
        textFieldLen = 0;
        ArgListC args;
        if (AttachUIC::topAttach)
            args.TopFormAttach(topOffset);
        if (AttachUIC::bottomAttach)
            args.BottomFormAttach(bottomOffset);
        args.LeftBaseAttach(label);

        args.RightFormAttach(rightOffset);

        InitWidget(form, args);
    }

    void FileEntryUIC::SetSensitive(bool isSensitive)
    {
        BaseUIC::SetSensitive(isSensitive);
        FileEntryWidget::SetSensitive(isSensitive);
    }

    // ************************************************************* radio box

    ChooserBaseUIC::~ChooserBaseUIC()
    {
        //  interiorLeftAttach = ilaWidgetToLeft;
    }

    void ChooserBaseUIC::DoWidgetInit(LayoutManager& form)
    {
        ArgListC args;

        if (topAttach)
            args.TopFormAttach(topOffset);
        if (bottomAttach)
            args.BottomFormAttach(bottomOffset);

        args.RightFormAttach(rightOffset);
        //  if (interiorLeftAttach == ilaWidgetToLeft)
        //      args.LeftFormAttach(leftOffset);

        fieldObj->InitWidget(form, args);
    }

    void ChooserBaseUIC::InitFramedUI(const  char      frameTitle[],
                                      Widget           parent,
                                      const  ArgListC& inArgs)      // attachment resources + others
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);
        DoWidgetInit(frame);
    }

    void ChooserBaseUIC::InitLabeledUI(const  char        labText[],
                                       Widget             parent,
                                       const  ArgListC&   inArgs)      // attachment resources + others
    {
        if (interiorLeftAttach == ilaLabelToWidget)
            form.InitWidget(parent, inArgs);
        else
            BaseUIC::InitLabeledUI(labText, parent, inArgs);
        DoWidgetInit(form);

        if (interiorLeftAttach == ilaLabelToWidget)
            AddAttachedLabel(labText, *fieldObj, 2);

    }

    void ChooserBaseUIC::InitFormUI(           Widget             parent,
                                               const  ArgListC&   inArgs)      // attachment resources + others
    {
        BaseUIC::InitFormUI(parent, inArgs);
        DoWidgetInit(form);
    }


    void ChooserBaseUIC::SetSensitive(bool isSensitive)
    {
        BaseUIC::SetSensitive(isSensitive);
        fieldObj->DoSetSensitive(isSensitive);
    }

    void OptionBaseUIC ::InitLabeledUI(const    char       labText[],
                                       Widget     parent,
                                       const   ArgListC&  inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);
        DoWidgetInit(form);

        ArgListC args;
        if (topAttach)
            args.TopFormAttach(topOffset);
        if (bottomAttach)
            args.BottomFormAttach(bottomOffset + 4); // there was +8
        if (interiorLeftAttach == ilaLabelToWidget)
            {
                args.RightWidgetAttach(fieldObj);
                args.RightOffset(leftOffset);
            }
        else
            args.LeftFormAttach(leftOffset);

        label.StdLabelInit(labText, &form, args);
    }


    // ************************************************************* toggle box

    void ToggleBoxUIC::InitFramedUI(const   char   frameTitle[],
                                    Widget  parent,
                                    const   ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);
        ArgListC args;

        if (AttachUIC::topAttach)
            args.TopFormAttach(topOffset);
        if (AttachUIC::bottomAttach)
            args.BottomFormAttach(bottomOffset);
        args.LeftFormAttach(leftOffset);

        InitWidget(frame, args);
    }


    // ************************************************************* sliders


    void SliderUIC::InitFramedUI(const char   frameTitle[],
                                 Widget  parent,
                                 const  ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);
        ArgListC args;
        if (attachPos == 0)
            args.LeftFormAttach();

        DoWidgetInit(frame, args);
    }


    void SliderUIC::InitLabeledUI(const char   labText[],
                                  Widget  parent,
                                  const  ArgListC&  inArgs)
    {
        BaseUIC::InitLabeledUI(labText, parent, inArgs);

        //  set field widget
        ArgListC args;
        if (attachPos == 0)
            args.LeftBaseAttach(label);
        DoWidgetInit(form, args);
    }

    void SliderUIC::InitFormUI (      Widget  parent,
                                      const ArgListC&  inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);

        ArgListC args;
        if (attachPos == 0)
            args.LeftFormAttach();
        DoWidgetInit(form, args);
    }

    void SliderUIC::SetSensitive(bool isSensitive)
    {
        BaseUIC::SetSensitive(isSensitive);
        SliderWidget::SetSensitive(isSensitive);
    }

    void SliderUIC::DoWidgetInit(LayoutManager& form, ArgListC& args)
    {
        if (AttachUIC::topAttach)
            args.TopFormAttach(topOffset);
        if (AttachUIC::bottomAttach)
            args.BottomFormAttach(bottomOffset);
        args.RightFormAttach(rightOffset);
        if (attachPos > 0)
            args.LeftPosAttach(attachPos);

        args.LeftOffset(leftOffset);

        InitWidget(form, args);
    }

    // ************************************************************* lists and combo boxes

    void ComboBaseUIC::InitFramedUI(const char   frameTitle[],
                                    Widget  parent,
                                    const  ArgListC&  inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);

        ArgListC args;
        if (comboObj->listWidth == 0)
            args.LeftFormAttach(leftOffset);
        DoWidgetInit(frame, args);
    }


    void ComboBaseUIC::InitLabeledUI(const char   labText[],
                                     Widget  parent,
                                     const  ArgListC&  inArgs)
    {
        if (interiorLeftAttach == ilaLabelToWidget)
            {
                if (comboObj->listWidth == 0)
                    GenAppInternalError("ComboBaseUIC::InitLabeledUI");
                form.InitWidget(parent, inArgs);
            }
        else
            {
                bottomOffset = 5;
                BaseUIC::InitLabeledUI(labText, parent, inArgs);
            }

        //  set field widget
        ArgListC args;
        if ((comboObj->listWidth == 0) || (interiorLeftAttach == ilaWidgetToLeft))
            {
                args.LeftBaseAttach(label);
                args.LeftOffset(leftOffset);
            }
        DoWidgetInit(form, args);

        if (interiorLeftAttach == ilaLabelToWidget)
            AddAttachedLabel(labText, *comboObj, 5);

    }

    void ComboBaseUIC::InitFormUI (      Widget  parent,
                                         const ArgListC&  inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);

        ArgListC args;
        if (comboObj->listWidth == 0)
            args.LeftFormAttach(leftOffset);
        DoWidgetInit(form, args);
    }

    void ComboBaseUIC::SetSensitive(bool isSensitive)
    {
        BaseUIC::SetSensitive(isSensitive);
        comboObj->DoSetSensitive(isSensitive);
    }

    void ComboBaseUIC::DoWidgetInit(LayoutManager& form, ArgListC& args)
    {
        if (topAttach)
            args.TopFormAttach(topOffset);
        if (bottomAttach)
            args.BottomFormAttach(bottomOffset);
        args.RightFormAttach(rightOffset);

        comboObj->InitWidget(form, args);
    }


    StatusBaseUIC::~StatusBaseUIC()
    {
    }

    void StatusBaseUIC::InitFramedUI(const  char      frameTitle[],
                                     Widget    parent,
                                     const  ArgListC& inArgs)
    {
        BaseUIC::InitFramedUI(frameTitle, parent, inArgs);

        //  set field widget
        ArgListC args;
        SetWidgetArgs(args);

        if (fieldObj->fieldLen == 0 ||      (interiorLeftAttach == ilaWidgetToLeft))
            args.LeftFormAttach(leftOffset);

        fieldObj->InitWidget(frame, args);
    }


    void StatusBaseUIC::InitLabeledUI(const char      labText[],
                                      Widget    parent,
                                      const  ArgListC& inArgs)
    {
        if (interiorLeftAttach == ilaLabelToWidget)
            {
                if (fieldObj->fieldLen == 0)
                    GenAppInternalError("StatusBaseUIC::InitLabeledUI");
                form.InitWidget(parent, inArgs);
            }
        else
            {
                BaseUIC::InitLabeledUI(labText, parent, inArgs);
            }


        //  set field widget
        ArgListC args;
        SetWidgetArgs(args);

        if ((fieldObj->fieldLen == 0) || (interiorLeftAttach == ilaWidgetToLeft))
            {
                args.LeftBaseAttach(label);
                args.LeftOffset(leftOffset);
            }
        fieldObj->InitWidget(form, args);

        if (interiorLeftAttach == ilaLabelToWidget)
            AddAttachedLabel(labText, *fieldObj, 2);

    }

    void StatusBaseUIC::InitFormUI(      Widget    parent,
                                         const ArgListC& inArgs)
    {
        BaseUIC::InitFormUI(parent, inArgs);

        //  set field widget
        ArgListC args;
        SetWidgetArgs(args);
        if (fieldObj->fieldLen == 0)
            args.LeftFormAttach(leftOffset);

        fieldObj->InitWidget(form, args);
    }

    void  StatusBaseUIC::SetWidgetArgs(ArgListC&  args)
    {
        SetTBattachArgs(args);
        args.RightFormAttach(rightOffset);
    }

    void  StatusBaseUIC::SetSensitive(bool isSensitive)
    {
        BaseUIC::SetSensitive(isSensitive);
        fieldObj->DoSetSensitive(isSensitive);
    }

    //////

    void  EntrySliderUIC::SetSliderVals(    const double& rMin,
                                            const double& rMax,
                                            int           iMin,
                                            int           iMax,
                                            int           incSize)
    {
        valSlider.realMin   = rMin;
        valSlider.realMax   = rMax;
        valSlider.showVal   = false;
        valSlider.minVal    = iMin;
        valSlider.maxVal    = iMax;
        valSlider.pageSize  = incSize;
    }

    void  EntrySliderUIC::DragCB(void* clientData)
    {
        ((EntrySliderUIC*) clientData)-> SliderValueChangedCB();
    }


    void EntrySliderUIC::FieldValueChangedCB()
    {
        bool ok = true;
        double valMin = valSlider.realMin;
        double valMax = valSlider.realMax;
        if (logSlider)
            {
                valMin = pow(10.0, valMin);
                valMax = pow(10.0, valMax);
            }

        if (!valDisplay.FieldValueOK(ok, "slider", valMin, valMax))
            return;

        double tmp = valDisplay.GetFieldValue();
        if (logSlider)
            valSlider.SetRealValue(log10(tmp));
        else
            valSlider.SetRealValue(tmp);

        // update
        valDisplay.SetFieldValue(tmp);
    }

    void EntrySliderUIC::FieldCB(void* clientData)
    {
        ((EntrySliderUIC*) clientData)-> FieldValueChangedCB();
    }

    void EntrySliderUIC :: InitLabeledUI(const char       inLabel[],
                                         Widget     parent,
                                         const ArgListC&  inArgs)
    {
        //  form
        BaseUIC::InitFormUI(parent, inArgs);

        //  label
        ArgListC args;
        args.Clear();
        args.BottomFormAttach();
        args.BottomOffset(5);
        args.SetPlaceHolder();
        args.LeftFormAttach();
        args.LeftOffset(5);
        label.InitWidget(GetForm(), args);
        label.SetLabelText(inLabel);

        //  slider
        args.ResetPlace();
        args.RightOffset(5);
        args.SetPlaceHolder();
        args.BottomOffset(3);

        if (attachPos == 0)
            args.LeftBaseAttach(label);
        else
            args.LeftPosAttach(attachPos);
        args.RightPosAttach(85);
        /* TODO: implement
           args.AddArg(XmNscaleMultiple, scaleMult);
        */
        args.AddWidthHeight(0, 20);
        valSlider.InitWidget(GetForm(), args);
        //valSlider.AddCallback(XmNvalueChangedCallback, DragCB, this);
        //valSlider.AddCallback(XmNdragCallback, DragCB, this);
        valSlider.AddOtherObjectCallback(DragCB, this);

        //  real field
        args.ResetPlace();
        args.LeftBaseAttach(valSlider);
        args.RightFormAttach();
        args.LeftOffset(5);
        /* TODO: implement
           args.AddArg(XmNmarginHeight,            2);
           args.AddArg(XmNmarginWidth,             2);
           args.AddArg(XmNshadowThickness,         1);
        */

        valDisplay.SetFieldFormat(SC_RealFormat(ncf_Decimal, 2));
        valDisplay.SetFieldLen(7);
        valDisplay.InitWidget(GetForm(), args);
        valDisplay.AddOtherObjectCallback(FieldCB, this);
    }

    void EntrySliderUIC :: InitFramedUI(const  char       inLabel[],
                                        Widget     parent,
                                        const ArgListC&  inArgs)
    {
        //  form
        BaseUIC::InitFramedUI(inLabel, parent, inArgs);

        ArgListC args;
        args.Clear();
        args.BottomFormAttach();
        args.SetPlaceHolder();
        args.RightOffset(5);
        args.SetPlaceHolder();
        args.BottomOffset(3);
        args.LeftFormAttach();
        //    args.LeftPosAttach(8);
        args.RightPosAttach(85);
        /* TODO: implement
           args.AddArg(XmNscaleMultiple, scaleMult);
        */
        args.AddWidthHeight(0, 20);
        valSlider.InitWidget(frame, args);
        //valSlider.AddCallback(XmNvalueChangedCallback, DragCB, this);
        //valSlider.AddCallback(XmNdragCallback, DragCB, this);
        valSlider.AddOtherObjectCallback(DragCB, this);

        //  real field
        args.ResetPlace();
        args.LeftBaseAttach(valSlider);
        args.RightFormAttach();
        args.LeftOffset(5);
        /* TODO: implement
           args.AddArg(XmNmarginHeight,            2);
           args.AddArg(XmNmarginWidth,             2);
           args.AddArg(XmNshadowThickness,         1);
        */

        valDisplay.SetFieldFormat(SC_RealFormat(ncf_Decimal, 2));
        valDisplay.SetFieldLen(7);
        valDisplay.InitWidget(frame, args);
        valDisplay.AddOtherObjectCallback(FieldCB, this);
    }


    void EntrySliderUIC :: InitFormUI(       Widget     parent,
                                             const ArgListC&  inArgs)
    {
        //  form
        BaseUIC::InitFormUI(parent, inArgs);

        //  slider
        ArgListC args;
        args.Clear();
        args.BottomFormAttach();
        args.BottomOffset(5);
        args.RightOffset(5);
        args.SetPlaceHolder();

        if (attachPos == 0)
            args.LeftFormAttach();
        else
            args.LeftPosAttach(attachPos);
        args.RightPosAttach(80);

        /* TODO: implement
           args.AddArg(XmNscaleMultiple, scaleMult);
        */

        args.AddWidthHeight(0, 20);
        valSlider.InitWidget(GetForm(), args);
        valSlider.AddOtherObjectCallback(DragCB, this);

        //  real field
        args.ResetPlace();
        args.LeftBaseAttach(valSlider);
        args.RightFormAttach();
        args.LeftOffset(5);

        /* TODO: implement
           args.AddArg(XmNmarginHeight,            2);
           args.AddArg(XmNmarginWidth,             2);
           args.AddArg(XmNshadowThickness,         1);
        */

        valDisplay.SetFieldFormat(SC_RealFormat(ncf_Decimal, 2));
        valDisplay.SetFieldLen(7);
        valDisplay.InitWidget(GetForm(), args);
        valDisplay.AddOtherObjectCallback(FieldCB, this);
    }

    void EntrySliderUIC::SetValues(const double& inVal)
    {
        if (logSlider)
            valSlider.SetRealValue(log10(inVal));
        else
            valSlider.SetRealValue(inVal);
        valDisplay.SetFieldValue(inVal);
    }


    void EntrySliderUIC::SliderValueChangedCB()
    {
        double tmp = valSlider.GetRealValue();
        if (logSlider)
            tmp = pow(10.0, tmp);
        valDisplay.SetFieldValue(tmp);
        valDisplay.UpdateWindow();
    }


    bool EntrySliderUIC::ValHasChanged(double& inVal)
    {
        bool ok = true;
        double valMin = valSlider.realMin;
        double valMax = valSlider.realMax;
        if (logSlider)
            {
                valMin = pow(10.0, valMin);
                valMax = pow(10.0, valMax);
            }

        if (!valDisplay.FieldValueOK(ok, "Entry", valMin, valMax))
            return false;

        double tmp = valDisplay.GetFieldValue();
        if (fabs(tmp - inVal) < 0.001)
            return false;

        inVal = tmp;
        return true;
    }

}; // namespace

