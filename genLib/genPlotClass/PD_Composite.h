///////////////////////////////////////////////////////////////////////////////////
//
// PD_Composite.h
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
//      defines basic composite plot - includes list of sub-plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PD_COMPOSITE_H
#define PD_COMPOSITE_H

#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_BoolArray.h>

#include <genPlotClass/C_PlotDef.h>

class PD_Composite;

class PSC_PlotLayout {

    friend class PD_Composite;

    public:
        //  all within 0 to 100.0 anno coord space
        double      xOffset;
        double      yOffset;
        double      xWidth;
        double      yWidth;

    private:
        int         uMin, uMax, vMin, vMax;

    public:
                    PSC_PlotLayout();
                    PSC_PlotLayout(const PSC_PlotLayout& a);
                    ~PSC_PlotLayout();

        PSC_PlotLayout& operator=(const PSC_PlotLayout& a);

        bool        LayoutOK(); // all coords OK

        void        SetSubplotLocation(const PD_Composite& masterPlot,
                                             PlotDefC&     subPlot);

        bool        WithinSubPlot(int uChk,
                                  int vChk);

        bool        GetSublotUV(int& uChk,
                                int& vChk);

    private:
        void        LocalCopy(const PSC_PlotLayout& a);

};

typedef T_SC_Array<PSC_PlotLayout> PlotLayoutArray;



//  base class for all composite plots
class PD_Composite : public PlotDefC  {

    public:
        T_SC_BasicPtrArray<PlotDefC> subPlots;

        bool            autoLayout;

        int             nlayoutRows;
        int             nlayoutCols;
        enum            {maxLayoutRowCol = 10};
        SC_DoubleArray  rowSpacing;
        SC_DoubleArray  colSpacing;


        enum            {maxCustomLayout = 20};
        SC_BoolArray    useCustomLayout;
        PlotLayoutArray customLayout;

        bool            showLayout;
        int             layoutPen;

    protected:
        int             nplotsToDisplay;
        int             lastCursorPlot;
        int             lastPlotSelected;
        PlotLayoutArray pageLayout;
        bool            annoSelected;

    public:
                            PD_Composite();
                            PD_Composite(const PD_Composite& a);
                            ~PD_Composite();

        PD_Composite&               operator= (const PD_Composite& a);

        void                SetDefault();

        //  FuncObjC virtuals
        virtual void        DoStatusChk();
        virtual void        CalcOutput(FOcalcType  calcType);
        virtual void        UpdateFuncObjRefs();
        virtual bool        UsesAsInput(const FuncObjC* inputObj) const;  // overriden to check subplots

        //  PlotDefC virtual
        virtual void        ViewTop();               // virtual for composite only - others use base class
        virtual void        ViewPrev();
        virtual void        DrawPlotObjects();
        virtual void        ResetView();
        virtual PC_ViewOpRec    GetPlotViewOps();
        virtual Coord3D     GetNewViewCenter();
        virtual void        SetViewFromAxes();

        virtual void        SetAxesAutoLimits();
        virtual bool        SetAxesFromExtents();
        virtual void        SetAxesFromView();
        virtual void        RestoreSetAxes();

        virtual PC_CallbackMenu*    GetCallbackMenu();


        //  add/remove plots
        void                AddSubplot(PlotDefC* newSubPlot);
        void                RemoveSubplot(PlotDefC* currentSubPlot);


        //  keypress
        void                ProcessSelectionKeys(SelectionKeys inKey);
                                                // passes keypress and location to objects

        virtual void        TransformCursor(Coord3D& currCursor) {};  // done prior to call to plot objects



    protected:
        //  PlotDefC virtuals
        void                ProcessCursorMove(int xpos,
                                              int ypos);
        virtual void        PointerSelect(int   xpos,
                                          int   ypos,
                                          bool  shifted);
        virtual void        PointerDrag(int     xpos,
                                        int     ypos,
                                        bool    shifted);
        virtual void        PointerRelease(int  xpos,
                                           int  ypos,
                                           bool shifted);

        bool                ProcessRubberbandRegion(const PC_ObjLimitRect&     rubberbandRegion);

    private:
        void                InitCommon();               // common to normal and copy constructor
        void                LocalCopy(const PD_Composite& a);  // common to copy constructor and = operator

        bool                GetSubplotUV(int& xpos, int& ypos);

};


#endif // !PD_COMPOSITE_H

