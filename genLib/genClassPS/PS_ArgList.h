///////////////////////////////////////////////////////////////////////////////////
//
// PS_ArgList.h
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
//              Encapsulates arguments to UI creation routines primarily used for
//              defining UI component layout. Based on original mView Unix implementation.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PS_ARGLIST_H
#define PS_ARGLIST_H


namespace genClassPS {

    class BaseWindow;                   //  BaseWindow is defined in PS_UIBase.h
    class BaseUIC;                      //  BaseUIC is defined in PS_UIClass.h
    typedef BaseWindow* Widget;

    struct Arg {
        enum ArgType {  XmNone,
                        XmNwidth,
                        XmNheight,
                        XmNtopOffset,
                        XmNbottomOffset,
                        XmNrightOffset,
                        XmNleftOffset,
                        XmNtopAttachment,       //XmATTACH_FORM,XmATTACH_POSITION,XmATTACH_WIDGET
                        XmNbottomAttachment,    //XmATTACH_FORM,XmATTACH_POSITION,XmATTACH_WIDGET
                        XmNleftAttachment,      //XmATTACH_FORM,XmATTACH_POSITION,XmATTACH_WIDGET
                        XmNrightAttachment,     //XmATTACH_FORM,XmATTACH_POSITION,XmATTACH_WIDGET
                        XmNtopWidget,
                        XmNbottomWidget,
                        XmNleftWidget,
                        XmNrightWidget,
                        XmNtopPosition,
                        XmNbottomPosition,
                        XmNleftPosition,
                        XmNrightPosition };
        enum ArgVal {   XmNONE,
                        XmATTACH_FORM,
                        XmATTACH_POSITION,
                        XmATTACH_WIDGET };

        ArgType argType;
        ArgVal argVal;
        int value;
        BaseWindow* baseWindow;
        Arg () :
            argType(XmNone), argVal(XmNONE), value(-1), baseWindow(NULL)
        {}
        void Set(ArgType argType, ArgVal argVal);
        void Set(ArgType argType, int value);
        void Set(ArgType argType, BaseWindow* baseWindow);
    };
    inline void Arg::Set(ArgType argType, ArgVal argVal)
    {
        Arg::argType = argType;
        Arg::argVal = argVal;
    }
    inline void Arg::Set(ArgType argType, int value)
    {
        Arg::argType = argType;
        Arg::value = value;
    }
    inline void Arg::Set(ArgType argType, BaseWindow* baseWindow)
    {
        Arg::argType = argType;
        Arg::baseWindow = baseWindow;
    }

    class ArgListC {
        enum        {maxArg = 50};

    public:
        Arg         args[maxArg];
        int         ac;
        int         acPlaceHolder;

    public:

        //      for re-using args in subsequent calls
        void        SetPlaceHolder()    {acPlaceHolder = ac;}
        void        ResetPlace()        {ac = acPlaceHolder;}
        void        Clear()             {ac = 0;}

        ArgListC();
        ArgListC(const ArgListC& a);

        ArgListC&   operator= (const ArgListC& a);  // copy operator

        void GetAttachment(BaseWindow& baseWindow) const;

        void        AddArg(Arg::ArgType argType, Arg::ArgVal argVal);
        void        AddArg(Arg::ArgType argType, int value);
        void        AddArg(Arg::ArgType argType, BaseWindow* baseWindow);

        //      basic attachments used primarily by menu building routines in
        //      all StdXXX include Clear() at start
        void        StdFormAttach(int tOff = 0);
        // R,L,T AttachForm.  if tOff > 0 then topOffset is added
        void        StdTopObjectAttach(BaseUIC& topObject, int tOff = 0);
        void        StdTopObjectAttach(Widget topObject, int tOff = 0);


        // R,L AttachForm, T attach outside widget. tOff as above
        void        TopLeftAttach(int tOff, int lOff);
        //  T,L Attach Form, tOff, lOff as above
        // T attach form

        void        AllFormAttach();
        // includes Clear() at start
        // RLTB Attach Form

        // used as a pair for splitting a form
        void        StdSplitSetup(int splitPos = 50);   // left half -- must be called first
        void        StdSplitFinish(int splitPos = 50);  // right half of split
        void        StdSplitMiddle(int lPos, int rPos); // for middle splits

        // for nSplit even splits
        void        StdEvenSplitSetup(int splitIndex, int nSplit);

        //  std toggle setups
        void        StdToggleSetup(int togglePos);

        //  useful LR attachments
        void        LeftFormAttach(int lOff = -1);  // if lOff > -1 then left offset set as well
        void        RightFormAttach(int rOff = -1);
        void        LeftPosAttach(int lPos);
        void        RightPosAttach(int rPos);
        void        LeftUICAttach(BaseUIC& leftObject);
        void        RightUICAttach(BaseUIC& rightObject);
        void        LeftWidgetAttach(Widget leftWidget);
        void        RightWidgetAttach(Widget rightWidget);
        void        LeftBaseAttach(BaseWindow& leftBase);
        void        RightBaseAttach(BaseWindow& rightBase);
        void        SpecifiedWidthAttach(int lPos, int rPos, int fullWidth);

        //  useful TB attachments
        void        TopFormAttach(int tOff = -1);
        void        BottomFormAttach(int bOff = -1);
        void        TopUICAttach(BaseUIC& topObject);
        void        BottomUICAttach(BaseUIC& bottomObject);
        void        TopWidgetAttach(Widget topWidget);
        void        BottomWidgetAttach(Widget bottomWidget);
        void        TopBaseAttach(BaseWindow& topBase);
        void        BottomBaseAttach(BaseWindow& bottomBase);
        void        TopPosAttach(int tPos);
        void        BottomPosAttach(int bPos);

        //  offsets
        void        TopOffset(int tOff);
        void        BottomOffset(int bOff);
        void        RightOffset(int rOff);
        void        LeftOffset(int lOff);
        void        AllOffsets(int tOff, int bOff, int lOff, int rOff);

        //  miscellania
        void        AddWidthHeight(int width,       //  XmNwidth if > 0
                                   int height);     //  XmNheight if > 0
        //  0, NULL, false arg --
        //void        AddZeroArg(char* argName);


    };

}; // namespace

#endif // PS_ARGLIST_H

