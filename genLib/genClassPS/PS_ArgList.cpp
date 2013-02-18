///////////////////////////////////////////////////////////////////////////////////
//
// PS_ArgList.cpp
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

#include "GenLibAfx.h"

#include <genClass/U_Msg.h>
#include <genClassPS/PS_UIBase.h>
#include <genClassPS/PS_UIClass.h>

#include <genClassPS/PS_ArgList.h>

namespace genClassPS {


ArgListC::ArgListC() :
    ac(0), acPlaceHolder(0)
{
}

ArgListC::ArgListC(const ArgListC& a)
{
    ac = a.ac;
    for (int i = 0; i < ac; i++)
        args[i] = a.args[i];
}

ArgListC&   ArgListC::operator= (const ArgListC& a)
{
    if (&a != this)
    {
        ac = a.ac;
        for ( int i = 0; i < ac; i++)
            args[i] = a.args[i];
    }

    return *this;
}

void ArgListC::AddArg(Arg::ArgType argType, Arg::ArgVal argVal)
{
    if (ac == maxArg) {
        GenAppInternalError("ArgListC::AddArg XtArgVal");
    }
    args[ac].Set(argType, argVal);
    ac++;
}

void ArgListC::AddArg(Arg::ArgType argType, int value)
{
    if (ac == maxArg) {
        GenAppInternalError("ArgListC::AddArg XtArgVal");
    }
    args[ac].Set(argType, value);
    ac++;
}

void ArgListC::AddArg(Arg::ArgType argType, BaseWindow* baseWindow)
{
    if (ac == maxArg) {
        GenAppInternalError("ArgListC::AddArg XtArgVal");
    }
    args[ac].Set(argType, baseWindow);
    ac++;
}

//      basic attachments
void ArgListC::StdFormAttach(int tOff)
{
    Clear();
    LeftFormAttach();
    RightFormAttach();
    TopFormAttach(tOff);
}

void ArgListC::StdTopObjectAttach(BaseUIC& topObject,
                                  int      tOff)
{
    Clear();
    LeftFormAttach();
    RightFormAttach();
    AddArg(Arg::XmNtopAttachment,    Arg::XmATTACH_WIDGET);
    AddArg(Arg::XmNtopWidget,        topObject.GetBoundingWidget());
    if ( tOff > 0)
        TopOffset(tOff);
}

void ArgListC::StdTopObjectAttach(Widget  topObject,
                                  int     tOff)
{
    Clear();
    LeftFormAttach();
    RightFormAttach();
    AddArg(Arg::XmNtopAttachment,    Arg::XmATTACH_WIDGET);
    AddArg(Arg::XmNtopWidget,        topObject);
    if ( tOff > 0)
        TopOffset(tOff);
}

void ArgListC::TopLeftAttach(int tOff, int lOff)
{
    TopFormAttach(tOff);
    LeftFormAttach();
    if ( lOff > 0)
        LeftOffset(lOff);
}

void ArgListC::BottomFormAttach(int bOff)
{
    AddArg(Arg::XmNbottomAttachment,    Arg::XmATTACH_FORM);
    if ( bOff > -1)
        BottomOffset(bOff);
}

void  ArgListC::TopFormAttach(int tOff)
{
    AddArg(Arg::XmNtopAttachment,    Arg::XmATTACH_FORM);
    if ( tOff > -1)
        TopOffset(tOff);
}

void ArgListC::LeftFormAttach(int lOff)
{
    AddArg(Arg::XmNleftAttachment,   Arg::XmATTACH_FORM);
    if ( lOff > -1)
        LeftOffset(lOff);
}
void ArgListC::RightFormAttach(int rOff)
{
    AddArg(Arg::XmNrightAttachment,  Arg::XmATTACH_FORM);
    if ( rOff > -1)
        RightOffset(rOff);
}

void ArgListC::AllFormAttach()
{
    Clear();
    LeftFormAttach();
    RightFormAttach();
    TopFormAttach();
    BottomFormAttach();
}

void ArgListC::StdSplitSetup(int splitPos)
{
    Clear();
    TopFormAttach();
    BottomFormAttach();
    SetPlaceHolder();
    LeftFormAttach();
    RightPosAttach(splitPos);
}

void ArgListC::StdSplitFinish(int splitPos)
{
    Clear();
    TopFormAttach();
    BottomFormAttach();
    LeftPosAttach(100 - splitPos);
    RightFormAttach();
}

void ArgListC::StdSplitMiddle(int lPos, int rPos)
{
    Clear();
    TopFormAttach();
    BottomFormAttach();
    LeftPosAttach(lPos);
    RightPosAttach(rPos);
}

void ArgListC::StdEvenSplitSetup(int splitIndex, int nSplit)
{
    Clear();
    TopFormAttach();
    BottomFormAttach();

    double splitInterval = 100.0 / nSplit;

    int stSplit = int(double(splitIndex) * splitInterval);
    int endSplit = int(double(splitIndex + 1) * splitInterval);

    LeftPosAttach(stSplit);
    RightPosAttach(endSplit);
}

void ArgListC::StdToggleSetup(int togglePos)
{
    Clear();
    TopFormAttach(5);
    BottomFormAttach();
    if (togglePos >= 0)
    {
        LeftPosAttach(togglePos);
    }
    else
    {
        LeftFormAttach();
        LeftOffset(-togglePos);
    }
}


void ArgListC::LeftPosAttach(int lPos)
{
    AddArg(Arg::XmNleftAttachment,         Arg::XmATTACH_POSITION);
    AddArg(Arg::XmNleftPosition,           lPos);
}

void ArgListC::RightPosAttach(int rPos)
{
    AddArg(Arg::XmNrightAttachment,         Arg::XmATTACH_POSITION);
    AddArg(Arg::XmNrightPosition,           rPos);
}

void ArgListC::LeftUICAttach(BaseUIC& leftObject)
{
    AddArg(Arg::XmNleftAttachment,          Arg::XmATTACH_WIDGET);
    AddArg(Arg::XmNleftWidget,              leftObject.GetBoundingWidget());
}

void ArgListC::RightUICAttach(BaseUIC& rightObject)
{
    AddArg(Arg::XmNrightAttachment,          Arg::XmATTACH_WIDGET);
    AddArg(Arg::XmNrightWidget,              rightObject.GetBoundingWidget());
}

void ArgListC::LeftWidgetAttach(Widget leftWidget)
{
    AddArg(Arg::XmNleftAttachment,          Arg::XmATTACH_WIDGET);
    AddArg(Arg::XmNleftWidget,              leftWidget);
}
void ArgListC::RightWidgetAttach(Widget rightWidget)
{
    AddArg(Arg::XmNrightAttachment,          Arg::XmATTACH_WIDGET);
    AddArg(Arg::XmNrightWidget,              rightWidget);
}

void ArgListC::LeftBaseAttach(BaseWindow& leftBase)
{
    LeftWidgetAttach(&leftBase);
}
void ArgListC::RightBaseAttach(BaseWindow& rightBase)
{
    RightWidgetAttach(&rightBase);
}

void ArgListC::SpecifiedWidthAttach(int lPos, int rPos, int fullWidth)
{
    LeftPosAttach(lPos);
    RightPosAttach(rPos);

    double dwidth = double(fullWidth) / 100.0 * double(rPos - lPos);

    AddArg(Arg::XmNwidth, int(dwidth));
}

void ArgListC::TopUICAttach(BaseUIC& topObject)
{
    AddArg(Arg::XmNtopAttachment,          Arg::XmATTACH_WIDGET);
    AddArg(Arg::XmNtopWidget,              topObject.GetBoundingWidget());
}

void ArgListC::BottomUICAttach(BaseUIC& bottomObject)
{
    AddArg(Arg::XmNbottomAttachment,          Arg::XmATTACH_WIDGET);
    AddArg(Arg::XmNbottomWidget,              bottomObject.GetBoundingWidget());
}

void ArgListC::TopWidgetAttach(Widget topWidget)
{
    AddArg(Arg::XmNtopAttachment,          Arg::XmATTACH_WIDGET);
    AddArg(Arg::XmNtopWidget,              topWidget);
}
void ArgListC::BottomWidgetAttach(Widget bottomWidget)
{
    AddArg(Arg::XmNbottomAttachment,          Arg::XmATTACH_WIDGET);
    AddArg(Arg::XmNbottomWidget,              bottomWidget);
}

void ArgListC::TopBaseAttach(BaseWindow& topBase)
{
    TopWidgetAttach(&topBase);
}
void ArgListC::BottomBaseAttach(BaseWindow& bottomBase)
{
    BottomWidgetAttach(&bottomBase);
}

void ArgListC::TopPosAttach(int tPos)
{
    AddArg(Arg::XmNtopAttachment,         Arg::XmATTACH_POSITION);
    AddArg(Arg::XmNtopPosition,           tPos);
}

void ArgListC::BottomPosAttach(int bPos)
{
    AddArg(Arg::XmNbottomAttachment,         Arg::XmATTACH_POSITION);
    AddArg(Arg::XmNbottomPosition,           bPos);
}

void ArgListC::TopOffset(int tOff)
{
    AddArg(Arg::XmNtopOffset,    tOff);
}
void ArgListC::BottomOffset(int bOff)
{
    AddArg(Arg::XmNbottomOffset,    bOff);
}
void ArgListC::RightOffset(int rOff)
{
    AddArg(Arg::XmNrightOffset,    rOff);
}
void ArgListC::LeftOffset(int lOff)
{
    AddArg(Arg::XmNleftOffset,    lOff);
}

void ArgListC::AllOffsets(int tOff, int bOff, int lOff, int rOff)
{
    TopOffset(tOff);
    BottomOffset(bOff);
    LeftOffset(lOff);
    RightOffset(rOff);
}

void ArgListC::AddWidthHeight(int width,
                              int height)
{
    int dWidth  = width;
    int dHeight = height;

    if (width > 0)
        AddArg(Arg::XmNwidth,           dWidth);

    if (height > 0)
        AddArg(Arg::XmNheight,          dHeight);
}

void ArgListC::GetAttachment(BaseWindow& baseWindow) const
{
    WindowAttachment& topAttach = baseWindow.topAttach;
    WindowAttachment& bottomAttach = baseWindow.bottomAttach;
    WindowAttachment& leftAttach = baseWindow.leftAttach;
    WindowAttachment& rightAttach = baseWindow.rightAttach;
    for (int i = 0; i < ac; i++) {
        switch (args[i].argType) {
        case Arg::XmNwidth:
            baseWindow.windowWidth = args[i].value;
            break;
        case Arg::XmNheight:
            baseWindow.windowHeight = args[i].value;
            break;
        case Arg::XmNtopOffset:
            topAttach.offset = args[i].value;
            break;
        case Arg::XmNbottomOffset:
            bottomAttach.offset = args[i].value;
            break;
        case Arg::XmNrightOffset:
            rightAttach.offset = args[i].value;
            break;
        case Arg::XmNleftOffset:
            leftAttach.offset = args[i].value;
            break;
        case Arg::XmNtopAttachment:
            if (args[i].argVal == Arg::XmATTACH_FORM) {
                topAttach.attachType = WindowAttachment::atParent;
            } else if (args[i].argVal == Arg::XmATTACH_POSITION) {
                topAttach.attachType = WindowAttachment::atPosition;
            } else if (args[i].argVal == Arg::XmATTACH_WIDGET) {
                topAttach.attachType = WindowAttachment::atChild;
            } else
                ASSERT(0);
            break;
        case Arg::XmNbottomAttachment:
            if (args[i].argVal == Arg::XmATTACH_FORM) {
                bottomAttach.attachType = WindowAttachment::atParent;
            } else if (args[i].argVal == Arg::XmATTACH_POSITION) {
                bottomAttach.attachType = WindowAttachment::atPosition;
            } else if (args[i].argVal == Arg::XmATTACH_WIDGET) {
                bottomAttach.attachType = WindowAttachment::atChild;
            } else
                ASSERT(0);
            break;
        case Arg::XmNleftAttachment:
            if (args[i].argVal == Arg::XmATTACH_FORM) {
                leftAttach.attachType = WindowAttachment::atParent;
            } else if (args[i].argVal == Arg::XmATTACH_POSITION) {
                leftAttach.attachType = WindowAttachment::atPosition;
            } else if (args[i].argVal == Arg::XmATTACH_WIDGET) {
                leftAttach.attachType = WindowAttachment::atChild;
            } else
                ASSERT(0);
            break;
        case Arg::XmNrightAttachment:
            if (args[i].argVal == Arg::XmATTACH_FORM) {
                rightAttach.attachType = WindowAttachment::atParent;
            } else if (args[i].argVal == Arg::XmATTACH_POSITION) {
                rightAttach.attachType = WindowAttachment::atPosition;
            } else if (args[i].argVal == Arg::XmATTACH_WIDGET) {
                rightAttach.attachType = WindowAttachment::atChild;
            } else
                ASSERT(0);
            break;
        case Arg::XmNtopWidget:
            ASSERT(topAttach.attachType == WindowAttachment::atChild);
            topAttach.attachType = WindowAttachment::atChild;
            topAttach.attachChild = args[i].baseWindow;
            break;
        case Arg::XmNbottomWidget:
            ASSERT(bottomAttach.attachType == WindowAttachment::atChild);
            bottomAttach.attachType = WindowAttachment::atChild;
            bottomAttach.attachChild = args[i].baseWindow;
            break;
        case Arg::XmNleftWidget:
            ASSERT(leftAttach.attachType == WindowAttachment::atChild);
            leftAttach.attachType = WindowAttachment::atChild;
            leftAttach.attachChild = args[i].baseWindow;
            break;
        case Arg::XmNrightWidget:
            ASSERT(rightAttach.attachType == WindowAttachment::atChild);
            rightAttach.attachType = WindowAttachment::atChild;
            rightAttach.attachChild = args[i].baseWindow;
            break;
        case Arg::XmNtopPosition:
            ASSERT(topAttach.attachType == WindowAttachment::atPosition);
            topAttach.attachType = WindowAttachment::atPosition;
            topAttach.attachPos = args[i].value;
            break;
        case Arg::XmNbottomPosition:
            ASSERT(bottomAttach.attachType == WindowAttachment::atPosition);
            bottomAttach.attachType = WindowAttachment::atPosition;
            bottomAttach.attachPos = args[i].value;
            break;
        case Arg::XmNleftPosition:
            ASSERT(leftAttach.attachType == WindowAttachment::atPosition);
            leftAttach.attachType = WindowAttachment::atPosition;
            leftAttach.attachPos = args[i].value;
            break;
        case Arg::XmNrightPosition:
            ASSERT(rightAttach.attachType == WindowAttachment::atPosition);
            rightAttach.attachType = WindowAttachment::atPosition;
            rightAttach.attachPos = args[i].value;
            break;
        }
    }
}

}; // namespace

