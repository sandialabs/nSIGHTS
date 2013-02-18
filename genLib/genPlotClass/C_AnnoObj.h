///////////////////////////////////////////////////////////////////////////////////
//
// C_AnnoObj.h
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
//      PlotObjC derived object that is plotted in 2D annotation coordinate system.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef ANNOOBJC_H
#define ANNOOBJC_H

#include <genClass/C_Graphic.h>
#include <genPlotClass/C_PlotObjBase.h>
#include <genPlotClass/PSC_AnnoSpec.h>
#include <genPlotClass/PSC_TransparentObject.h>
#include <genPlotClass/PC_ObjLimitRect.h>

class AnnoObjC : public PlotObjBaseC, public PSC_AnnoSpec, public PSC_TransparentObject {
    public:
        static const double        maxAnnoCoord;  // 100.0

        //  for hit testing & mouse dragging
        PC_ObjLimitRect rectLimits;
        Point2D         ulRectOffset;

    public:
                        AnnoObjC(const char* legendType, PlotDefC& assPlt);
                        AnnoObjC(const AnnoObjC& a, PlotDefC& assPlt);
                        ~AnnoObjC();

        AnnoObjC& operator= (const AnnoObjC& a);

        void            SetDefault();

                        //  FuncObjC virtuals
        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

        static void     ForceToAnnoLimits(Point2D&  inPoint);

                        //  for cursor grab/move anno
        void            UpdateULlocation(const Point2D& newrectUL);
        virtual void    UpdateULUI() {};  // called by UpdateULlocation, implemented in UI derived classes

        // transparency virtuals - all anno can be transparent
        virtual bool    SupportsTransparency() const;
        virtual int     GetnTransObjects() const;
        virtual Coord3D GetTransObjectCoord(int objIndex);

    protected:
        void            SetRectOffset(const Point2D& rectUL);


    private:
        void            InitCommon();  // common to normal and copy constructor
        void            LocalCopy(const AnnoObjC& a);  // common to copy constructor and = operator
};

#endif // !ANNOOBJC_H

