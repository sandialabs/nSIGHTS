///////////////////////////////////////////////////////////////////////////////////
//
// PC_View.cpp
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
//      defines axes limits
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include <genPlotClass/PC_View.h>

PC_View::PC_View(const PC_View& a)
{
    viewLimits   = a.viewLimits;
    translation  = a.translation;
    azimuth      = a.azimuth;
    elevation    = a.elevation;
    scale        = a.scale;
}

PC_View::PC_View() : translation(0.0)
{
    viewLimits.minLim = Coord3D(0.0);
    viewLimits.maxLim = Coord3D(100.0);
    azimuth      = 0.0;
    elevation    = 90.0;
    scale        = 1.0;
}

PC_ViewStackRecord::PC_ViewStackRecord(const PC_View& a) : PC_View(a)
{
    nextView = 0;
    prevView = 0;
}


PC_ViewStack::PC_ViewStack()
{
    stackTop = &firstRec;
}

void PC_ViewStack::StackCleanup()
{
    PC_ViewStackRecord* next = firstRec.nextView;
    while (next)
    {
        PC_ViewStackRecord* curr = next;
        next = next->nextView;
        delete curr;
    }
    firstRec.nextView = 0;
    stackTop = &firstRec;
}

bool PC_ViewStack::AtStackTop()
{
    return (stackTop == &firstRec);
}


PC_ViewStack::~PC_ViewStack()
{
    StackCleanup();
}

void PC_ViewStack::InitStack(const PC_View& firstView)
{
    StackCleanup();
    firstRec = firstView;
}

void PC_ViewStack::PushStack(const PC_View& newView)
{
    PC_ViewStackRecord* newData  = new PC_ViewStackRecord(newView);
    newData->prevView = stackTop;
    stackTop->nextView = newData;
    stackTop  = newData;
}


void PC_ViewStack::InitStack(const Limit3D& firstView)
{
    StackCleanup();
    firstRec.viewLimits = firstView;
}

void PC_ViewStack::PushStack(Limit3D& newView)
{
    if ( newView.minLim.SameCoord(stackTop->viewLimits.minLim) &&
         newView.maxLim.SameCoord(stackTop->viewLimits.maxLim))
         return;

    PC_ViewStackRecord* newData  = new PC_ViewStackRecord();
    newData->viewLimits = newView;
    newData->prevView = stackTop;
    stackTop->nextView = newData;
    stackTop  = newData;
}

PC_View PC_ViewStack::PopStack()
{
    PC_View lastView = *stackTop;
    if ( stackTop != &firstRec)
    {
        stackTop = stackTop->prevView;
        delete stackTop->nextView;
        stackTop->nextView = 0;
    }
    return lastView;
}
PC_View PC_ViewStack::TopStack()
{
    PC_View currView;
    if (stackTop != 0)
        currView = *stackTop;
    return currView;
}


void PC_ViewStack::ClearStack()
{
    StackCleanup();
}

