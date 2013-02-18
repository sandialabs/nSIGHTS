///////////////////////////////////////////////////////////////////////////////////
//
// PC_View.h
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

#ifndef PC_VIEW_H
#define PC_VIEW_H

#include <genClass/C_Common.h>

//  all plot objects support interactive zoom/pan etc

class   PC_View {

    public:
        Limit3D     viewLimits;       //  limits of axes viewport in plot coord sys
        Coord3D     translation;    //  3D coord
        double      azimuth;        //  az, elev, and scale  of eye coord from translation
        double      elevation;
        double      scale;

    public:
                    PC_View();
                    PC_View(const PC_View& a);

};

class PC_ViewStackRecord : public PC_View {
    public:
        PC_ViewStackRecord*    nextView;
        PC_ViewStackRecord*    prevView;

    public:
        PC_ViewStackRecord() {nextView = 0; prevView = 0;}
        PC_ViewStackRecord(const PC_View& a);
};


class   PC_ViewStack {
    private:
        PC_ViewStackRecord    firstRec;
        PC_ViewStackRecord*   stackTop;

    public:
                PC_ViewStack();
                ~PC_ViewStack();

                //  3D signatures
        void    InitStack(const PC_View& firstView);
                // cleans up if necessary, initializes firstRec
        void    PushStack(const PC_View& newView);
                //  adds view to top of stack, no check for same yet

                //  2D signatures
        void    InitStack(const Limit3D& firstView);
                // cleans up if necessary, initializes firstRec
        void    PushStack(Limit3D& newView);
                //  adds view to top of stack,
                //  checks to see if view is the same -- does not add if same view

        PC_View PopStack();
                //  gets view off top of stack, removes top

        PC_View TopStack();
                //  gets current view off top of stack

        void    ClearStack();
                //  clears all views except init

        bool    AtStackTop();

    private:
        void    StackCleanup();
                // clears all newed items - used by destructor, init, and clear

};

enum ViewOp{vo_Lin, vo_Log, vo_na};  // axes types to perform op on

class PC_ViewOpRec {
    public:
        ViewOp  xOp;
        ViewOp  yOp;
        ViewOp  zOp;

    public:
                PC_ViewOpRec(){xOp = vo_Lin; yOp = vo_Lin, zOp = vo_Lin;};
                ~PC_ViewOpRec() {};
};



#endif // !PC_VIEW_H

