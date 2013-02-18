///////////////////////////////////////////////////////////////////////////////////
//
// PS_UIClass.h
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

#ifndef PS_UICLASS_H
#define PS_UICLASS_H

#include <genClassPS/PS_UIBase.h>

// ************************************************************* basic attachments

namespace genClassPS {

    class AttachUIC  {
        //  base object for most mView UI components which consist
        //  sets attachments

    public:
        bool        topAttach;          //  most objects are attached at bottom
        //  - default false
        bool        bottomAttach;       //  most objects are attached at bottom
        //  - default true

        // offsets for interior objects - default 5 on LR, 3 on TB
        int         topOffset;          //  applied to all interior objects if topAttach
        int         bottomOffset;       //  applied to all interior objects if bottomAttach
        int         rightOffset;        //  applied to fields in derived objects
        int         leftOffset;         //  applied to label if InitLabeledUI
        //  otherwise to derived objects

    public:
        AttachUIC();

        void        SetOffsets(int top,                   //  if > -1 then sets attachments
                               int bottom,
                               int left,
                               int right);

        void        SetTopAttach();                      // sets top true and bottom false

        void        CopyOffsetAttach(AttachUIC& srcUIC); // copies data from

        void        SetTBattachArgs(ArgListC&  args);
    };


    class BaseUIC : public AttachUIC {
        //  base object for most mView UI components which consist
        //  of a labeled frame with one or more objects inside or
        //  a form with a label attached on the left side
        //  attachments are applied to interior objects
    public:
        FrameWidget frame;          //  outside frame if InitFrame used
        FormWidget  form;           //  form - inside frame if InitFramedUI
        LabelWidget label;          //  in frame if InitFrameUI
        //  attached to left side of form if InitLabeledUI
    protected:
        enum InteriorLeftAttach     {ilaDefault,         // no left attachment on widget
                                                         // label is left attached to inside form
                                     ilaLabelToWidget,   // widget must have fixed width
                                                         // label is attached to left side of widget
                                     ilaWidgetToLeft};   // label is attached to form
        // widget to label (if label)
        // widget to left side form if framed
        InteriorLeftAttach      interiorLeftAttach;


    public:
        BaseUIC();

        //  base class virtuals

        virtual void    InitFramedUI(const char       frameTitle[],
                                     Widget           parent,
                                     const ArgListC&  inArgs);      // attachment resources + others
        //  framed form with title

        virtual void    InitLabeledUI(const char      labText[],
                                      Widget          parent,
                                      const ArgListC& inArgs);      // attachment resources + others
        //  form with label on right

        virtual void    InitFormUI(      Widget          parent,
                                         const ArgListC& inArgs);      // attachment resources + others
        //  form only


        void            ChangeLabel(const char* newLab);
        void            SetSensitive(bool isSensitive);

        void            Manage();
        void            Unmanage();
        inline void         AttachLabelToObject() {interiorLeftAttach = ilaLabelToWidget;}
        inline void         AttachObjectToLabel() {interiorLeftAttach = ilaWidgetToLeft;}
        inline void         AttachObjectToFrame() {interiorLeftAttach = ilaWidgetToLeft;}

        //  interior widget format control
        LayoutManager*  GetBoundingWidget();               // returns frame or form
        LayoutManager*  GetForm();

    protected:
        void            AddAttachedLabel(const char*       labText,
                                         BaseWindow& otherObj,
                                         int           bottOffset);

    };

    class SplitFormUIC : public BaseUIC {
    public:
        FormWidget  leftForm;
        FormWidget  rightForm;
        int         leftStart;      // positions for start and end of left form
        int         leftEnd;
        // Note: constructor resets attachments and offsets as follows
        // top T, bot T, all offsets = 0

    public:
        SplitFormUIC();

        void        PreInitSetUp(int lstart, int lend) {leftStart = lstart; leftEnd = lend;}

        void        InitFramedUI(const char       frameTitle[],
                                 Widget           parent,
                                 const ArgListC&  inArgs);         // attachment resources + others

        void        InitFormUI(      Widget          parent,
                                     const ArgListC& inArgs);      // attachment resources + others

        FormWidget*  GetLeft() {return &leftForm;}
        FormWidget*  GetRight(){return &rightForm;}

    private:
        void        SetSplitForms(LayoutManager& form);

    };

    class TriSplitFormUIC : public SplitFormUIC {
    public:
        FormWidget  midForm;
        int         midEnd;

    public:
        TriSplitFormUIC();

        void        PreInitSetUp(int lstart, int mstart, int mend)
        {leftStart = lstart; leftEnd = mstart;
            midEnd = mend;}

        void        InitFramedUI(const  char      frameTitle[],
                                 Widget           parent,
                                 const ArgListC&  inArgs);         // attachment resources + others

        void        InitFormUI(      Widget          parent,
                                     const ArgListC& inArgs);      // attachment resources + others

        FormWidget* GetMiddle() {return &midForm;}

    private:
        void        SetTriSplitForms(LayoutManager& form);

    };


    class TextBaseFieldUIC : public BaseUIC {
    private:
        TextEntryWidget*    fieldObj;
    public:
        TextBaseFieldUIC(TextEntryWidget* inObj)
        : fieldObj(inObj) {};


        void            InitFramedUI(const  char       frameTitle[],
                                     Widget            parent,
                                     const  ArgListC&  inArgs);

        void            InitLabeledUI(const char       labText[],
                                      Widget           parent,
                                      const ArgListC&  inArgs);
        // supports attach label to object

        void            InitFormUI(         Widget           parent,
                                            const ArgListC&  inArgs);

        void            SetSensitive(bool isSensitive);

    private:
        void            SetWidgetArgs(ArgListC& args);
    };

#pragma warning( push )
#pragma warning( disable : 4355 ) // 'this' : used in base member initializer list

    class TextEntryUIC : public TextBaseFieldUIC, public TextEntryWidget  {

    public:
        TextEntryUIC() : TextBaseFieldUIC(this) {};

        void        SetSensitive(bool isSensitive) {TextBaseFieldUIC::SetSensitive(isSensitive);};

    };


    class RealEntryUIC : public TextBaseFieldUIC, public RealEntryWidget{

    public:
        RealEntryUIC() : TextBaseFieldUIC(this) {};

        void        SetSensitive(bool isSensitive) {TextBaseFieldUIC::SetSensitive(isSensitive);};
    };

    class IntEntryUIC : public TextBaseFieldUIC, public IntEntryWidget{
    public:
        IntEntryUIC() : TextBaseFieldUIC(this) {};

        void        SetSensitive(bool isSensitive) {TextBaseFieldUIC::SetSensitive(isSensitive);};
    };


    class FileEntryUIC : public BaseUIC, public FileEntryWidget {
    public:
        FileEntryUIC();


        void        InitFramedUI(const  char       frameTitle[],
                                 Widget            parent,
                                 const  ArgListC&  inArgs);      // attachment resources + others

        void        InitLabeledUI(const char       labText[],
                                  Widget           parent,
                                  const ArgListC&  inArgs);      // attachment resources + others

        void        SetSensitive(bool isSensitive);

    };


    class ChooserBaseUIC : public BaseUIC {
    protected:
        ChooserWidget*  fieldObj;

    public:
        ChooserBaseUIC(ChooserWidget* inObj) : fieldObj(inObj) {};

        virtual             ~ChooserBaseUIC();

        void            InitFramedUI(const  char       frameTitle[],
                                     Widget            parent,
                                     const  ArgListC&  inArgs);      // attachment resources + others

        void            InitLabeledUI(const char       labText[],
                                      Widget           parent,
                                      const ArgListC&  inArgs);      // attachment resources + others

        void            InitFormUI(Widget           parent,
                                   const ArgListC&  inArgs);         // attachment resources + others

        void            SetSensitive(bool isSensitive);

    protected:
        void            DoWidgetInit(LayoutManager& form);

    private:
        // no default ctor
        ChooserBaseUIC() {};

    };

    class RadioBoxUIC : public ChooserBaseUIC, public RadioBoxChooserWidget  {

    public:
        RadioBoxUIC(): ChooserBaseUIC(this) {}

        void    SetSensitive(bool isSensitive) {ChooserBaseUIC::SetSensitive(isSensitive);}

    };

    class BoolRadioBoxUIC : public ChooserBaseUIC, public BoolRadioBoxChooserWidget  {

    public:
        BoolRadioBoxUIC(): ChooserBaseUIC(this) {}

        void    SetSensitive(bool isSensitive) {ChooserBaseUIC::SetSensitive(isSensitive);}
    };

    class ButtonBoxUIC : public ChooserBaseUIC, public ButtonBoxChooserWidget  {

    public:
        ButtonBoxUIC(): ChooserBaseUIC(this) {}

        void    SetSensitive(bool isSensitive) {ChooserBaseUIC::SetSensitive(isSensitive);}

    };


    class OptionBaseUIC : public ChooserBaseUIC {

    public:
        OptionBaseUIC(ChooserWidget* inObj) :
        ChooserBaseUIC(inObj) {}

        //  special case for option chooser
        //  adjusts label up to appear in centre of chooser
        void    InitLabeledUI(const char       labText[],
                              Widget           parent,
                              const ArgListC&  inArgs);      // attachment resources + others

    };

    class OptionUIC : public OptionBaseUIC, public OptionChooserWidget  {

    public:
        OptionUIC(): OptionBaseUIC(this) {}

        void    SetSensitive(bool isSensitive) {ChooserBaseUIC::SetSensitive(isSensitive);}
    };

    class BoolOptionUIC : public OptionBaseUIC, public BoolOptionChooserWidget{

    public:
        BoolOptionUIC(): OptionBaseUIC(this) {}

        void    SetSensitive(bool isSensitive) {ChooserBaseUIC::SetSensitive(isSensitive);}
    };

    class ToggleBoxUIC : public BaseUIC, public ToggleBoxWidget {

    public:
        void            InitFramedUI(const  char       frameTitle[],
                                     Widget            parent,
                                     const  ArgListC&  inArgs);      // attachment resources + others

        void            SetSensitive(bool isSensitive) {BaseUIC::SetSensitive(isSensitive);}

    };

    class SliderUIC : public BaseUIC, public SliderWidget {
    public:
        int             attachPos;

    public:
        SliderUIC() {attachPos = 0;}

        void            InitFramedUI(const  char       frameTitle[],
                                     Widget            parent,
                                     const  ArgListC&  inArgs);      // attachment resources + others

        void            InitLabeledUI(const char       labText[],
                                      Widget           parent,
                                      const ArgListC&  inArgs);      // attachment resources + others

        void            InitFormUI(Widget           parent,
                                   const ArgListC&  inArgs);         // attachment resources + others

        void            SetSensitive(bool isSensitive);

    private:
        void            DoWidgetInit(LayoutManager& form, ArgListC& args);
    };


    class ComboBaseUIC : public BaseUIC  {
    private:
        ComboBoxSelectionWidget*  comboObj;

    public:
        ComboBaseUIC(ComboBoxSelectionWidget * inObj) : comboObj(inObj) {};

        void            InitFramedUI(const  char       frameTitle[],
                                     Widget            parent,
                                     const  ArgListC&  inArgs);      // attachment resources + others

        void            InitLabeledUI(const char       labText[],
                                      Widget           parent,
                                      const ArgListC&  inArgs);      // attachment resources + others

        void            InitFormUI(Widget           parent,
                                   const ArgListC&  inArgs);         // attachment resources + others

        void            SetSensitive(bool isSensitive);

    private:
        void            DoWidgetInit(LayoutManager& form, ArgListC& args);

    };

    class ComboBoxSelectionUIC : public ComboBaseUIC, public ComboBoxSelectionWidget {
    public:
        ComboBoxSelectionUIC(): ComboBaseUIC(this) {}

        void  SetSensitive(bool isSensitive) {ComboBaseUIC::SetSensitive(isSensitive);}
    };

    class StatusBaseUIC : public BaseUIC {
    private:
        StatusWidget*  fieldObj;

    public:
        StatusBaseUIC(StatusWidget* inObj) : fieldObj(inObj) {};
        virtual             ~StatusBaseUIC();

        void            InitFramedUI(const  char       frameTitle[],
                                     Widget            parent,
                                     const  ArgListC&  inArgs);

        void            InitLabeledUI(const char       labText[],
                                      Widget           parent,
                                      const ArgListC&  inArgs);

        void            InitFormUI(         Widget           parent,
                                            const ArgListC&  inArgs);

        void            SetSensitive(bool isSensitive);

    private:
        void            SetWidgetArgs(ArgListC& args);

        // no default ctor
        StatusBaseUIC() {}
    };

    class StatusUIC : public StatusBaseUIC, public StatusWidget  {

    public:
        StatusUIC() : StatusBaseUIC(this) {};

        void        SetSensitive(bool isSensitive) {StatusBaseUIC::SetSensitive(isSensitive);};

    };

    class RealStatusUIC : public StatusBaseUIC, public RealStatusWidget  {

    public:
        RealStatusUIC() : StatusBaseUIC(this) {};

        void        SetSensitive(bool isSensitive) {StatusBaseUIC::SetSensitive(isSensitive);};

    };

    class IntStatusUIC : public StatusBaseUIC, public IntStatusWidget  {

    public:
        IntStatusUIC() : StatusBaseUIC(this) {};

        void        SetSensitive(bool isSensitive) {StatusBaseUIC::SetSensitive(isSensitive);};

    };

#pragma warning( pop )

    class EntrySliderUIC : public BaseUIC {
    public:
        int                     attachPos;    // copied from SliderUIC
                                              // only implemented for InitFormUI
                                              // and InitLabeledUI
    public:
        SliderWidget        valSlider;
        RealEntryWidget     valDisplay;
        bool                logSlider;

    public:
        EntrySliderUIC() {attachPos = 0, logSlider = false;}


        void            SetSliderVals(const double& rMin,
                                      const double& rMax,
                                      int       iMin,
                                      int       iMax,
                                      int       incSize);

        void            InitLabeledUI(const char       label[],
                                      Widget           parent,
                                      const ArgListC&  inArgs);      // attachment resources + others

        void            InitFramedUI(const  char       label[],
                                     Widget            parent,
                                     const  ArgListC&  inArgs);      // attachment resources + others

        void            InitFormUI(             Widget           parent,
                                                const ArgListC&  inArgs);      // attachment resources + others

        void            SetValues(const double& inVal);


        bool            ValHasChanged(double& inVal);

        void            SliderValueChangedCB();

        void            FieldValueChangedCB();

        void            SetSensitive(bool isSensitive) {BaseUIC::SetSensitive(isSensitive);
            valSlider.SetSensitive(isSensitive);
            valDisplay.SetSensitive(isSensitive);
        }

    private:
        static void DragCB(void *);
        static void FieldCB(void *);

    };

}; // namespace

#endif // PS_UICLASS_H

