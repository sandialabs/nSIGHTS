///////////////////////////////////////////////////////////////////////////////////
//
// C_PlotDef.h
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
//      basic definition of a plot window.
//  Maintains lists of connected plot and annotation objects.
//  Controls overall plot display.
//  Handles default mouse right click menus.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_PLOTDEF_H
#define C_PLOTDEF_H

#include <genClass/C_Common.h>
#include <genClass/C_Graphic.h>
#include <genClass/DO_PenSet.h>

#include <genClass/T_SC_BasicPtrArray.h>

#include <genPlotClass/PC_View.h>
#include <genPlotClass/PC_Platform.h>
#include <genPlotClass/PC_DumpTGA.h>
#include <genPlotClass/PC_PSOutData.h>
#include <genPlotClass/PC_CallbackMenu.h>

#include <genClass/C_InteractiveObj.h>

#ifdef _WIN32
#pragma warning(disable : 4355)     // disable message for *this used in constructor
#endif


class PlotObjC;                     // forward decl
class AnnoObjC;
class OGLBase;                      // kluge required for dragging objects to different plots


//  This is the base class for all mView plot definitions
class PlotDefC : public InteractiveObj  {

    friend class PD_Composite;

    public:
        enum    SelectionKeys  {pdc_SelectObject,           //  keystrokes translation - LClick,
                                pdc_AddConsecutiveObject,   //  Shift LClick
                                pdc_AddDiscreteObject,      //  Ctrl LClick
                                pdc_Drag};                  //  LDown, drag

    protected:
        // **************** view stuff
        PC_ViewStack        plotViews;              //  view stack


        // **************** anno dragging support
        AnnoObjC*           selectedAnno;
        int                 dragStartX;
        int                 dragStartY;
        PC_ObjLimitRect     draggedObjectRect;

        // **************** rubber band drag for zooming
        bool                rubberbandDrag;
        PC_ObjLimitRect     rubberbandOKrect;

        PC_CallbackMenu     callbackMenu;

    private:
        bool                autoUpdateRequired;     //  last op was calc - may need to reset axes before
                                                    //  next draw
    public:
        PC_Platform*        platformData;           //  recast to type derived from PC_PlatformPS* in use


        // output objects
        DC_PenSet*          defaultPenSet;          //  default PenSet used for plot
        DO_PenSet           defaultPenSetDO;        //  output to other objects on same page

        // parameters
        FuncObjRef          plotPenSetObjRef;       //  plot backgound pen set
        int                 plotBackgroundPen;      //  plot backgound pen

        int                 plotUOffset;            // added for composite support
        int                 plotVOffset;
        int                 plotUwidth;             //  dimensions in pixels
        int                 plotVheight;

        int                 rightMargin;            //  margins surrounding plot in pixels
        int                 leftMargin;             //  margins surrounding plot in pixels
        int                 topMargin;              //  margins surrounding plot in pixels
        int                 bottomMargin;           //  margins surrounding plot in pixels

        //  pointer use mode
        bool                plotDefIsActive;        // if true, pointer is zoom mode
                                                    // else is object select
        bool                axesHaveBeenSet;        // if true then preferred axes limits/types
                                                    // have been set and can be restored

        //  postscript output data
        PC_PSOutData        psOutput;

        //  TGA output data
        PC_DumpTGA          tgaOutput;

        //  tracking lists
        static T_SC_BasicPtrArray<PlotDefC> plotList;   // all plots

        T_SC_BasicPtrArray<PlotObjC> plotObjList;     //  objects assovciated with def
        T_SC_BasicPtrArray<AnnoObjC> annoObjList;     //  anno objects assovciated with def

        // main window ID qualifier
        const char*         preID;

    protected:
        // for composite plot support
        PlotDefC*           masterPlot;   // defer to master for rendering start/end

    public:
                            PlotDefC(const char* dType);
                            PlotDefC(const PlotDefC& a);
                            ~PlotDefC();

        PlotDefC&           operator= (const PlotDefC& a);


        virtual void        SetDefault();
        //  FuncObjC virtuals
        void                DoStatusChk();
        void                CalcOutput(FOcalcType  calcType);
        virtual bool        IsInUse() const;


        Limit3D             CalcPlotLimits();       // gets extents of all PlotObjects
        int                 GetNPlotObject();  // returns number of plot objects attached

        //  view related
        virtual void        ResetView() = 0;        //  base class does processing of
                                                    // all plotobjects with recalcOnViewChange set
        // implemented in derived class
        // gets view off top of stack, reset axes limits to view reset major & minor incs

        virtual             PC_ViewOpRec GetPlotViewOps() = 0;
        virtual Coord3D     GetNewViewCenter()  = 0;
        //  used for view changes

        //  three basic methods for changing the view
        virtual void        ChangeView(const PC_View&  newView);  // virtual to allow PD_3D to set output vars with
                                                                  // elev/azimuth/scale data.  PD_2D uses base class

        void                ChangeView(const Coord3D&  scaleFactor);
                            //  gets current view off top stack, sets new view based on
                            //  cursorCoord and scale factor
                            //  pushes on stack
                            //  calls ResetView(false)

        void                ChangeView(double  scaleFactor);
                            //  same as above -- all scale fields to scaleFactor

        //  std view  procs
        virtual void        ViewTop();               // virtual for composite only - others use base class
        virtual void        ViewPrev();
        void                ViewPropagate();         // propagates view to all plots of the same type
        PC_View             GetCurrentView();        // current view
        Limit3D             GetCurrentViewLimits();  // current view limits
        virtual void        SetInitialView(const PC_View& initView);  // for file routines

        //  size related
        virtual void        ResetSize() = 0;
        // implemented in derived class
        // sets plot size to current uv width
        void                SizePropagate();   // propagates size to all plots of the same type

        //  axes autoscaling support
        virtual void        SetViewFromAxes()   = 0;  // sets view from current axes limits
        virtual void        SetAxesAutoLimits() = 0;  // axes limits from plot objects if auto
        virtual bool        SetAxesFromExtents() = 0;  // axes limits from plot objects, returns true if not default
        virtual void        SetAxesFromView()   = 0;  // sets axes to manual, limits to current
        virtual void        RestoreSetAxes()   = 0;   // returns to last settings from setAxes

        //  main draw virtuals
        virtual void        DrawPlotAxes() = 0;
        virtual void        DrawPlotObjects() = 0;

        //  drivers
        void                DrawPlot();                 // draws plot -- triggers TGA dump
        void                RedrawPlot();               // for use by expose/anno move events -- does not trigger TGA dump

        //  glp hardcopy support
        void                DrawAllObjects();

        //  renderer setup & draw
        virtual void        InitRenderer() = 0;        // binds context, clears screen
        virtual void        SetupDataView() = 0;       // sets axes limits and view rotations/translations
        virtual void        StartLineSection() = 0;    // all poly objects drawn, start line only objects
                                                       // for anti-aliasing
        virtual void        EndLineSection() = 0;      // all line objects drawn disable AA
        virtual void        SetupAnnoView() = 0;       // changes to 0-100 system for annotation
        virtual void        CloseRenderer() = 0;       // flushes

        //  anno to pixel conversion
        void                AnnoXYtoPixelXY(const Point2D& annoPt,
                                                  int&     annoPixX,
                                                  int&     annoPixY);
        void                PixelXYtoAnnoXY(int      annoPixX,
                                            int      annoPixY,
                                            Point2D& annoPt);

        //  avail plot space
        virtual void        CalcAvailablePixels(int&    xAvail,  // base class OK for all except PD_2Dspatial
                                                int&    yAvail);  // tot size - margins
        virtual void        AdjustMargins();  // base class OK for all except PD_2Dspatial


        //  cursor processing -- all virtuals, base case is OK for all except PD_2DSpatial
        virtual void        PointerEnter(int xpos,
                                         int ypos);
        virtual void        PointerLeave();
        virtual void        PointerMove(int xpos,  // 2D routines  process xpos,ypos to current cursor
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

        bool                InViewOKRect(int  xpos,
                                         int  ypos) {return rubberbandOKrect.InRect(xpos, ypos);}

        //  keypress
        virtual void        ProcessSelectionKeys(SelectionKeys inKey);

        // callback menu selection
        // base class OK for most -- PD_Composite has special case
        virtual PC_CallbackMenu*  GetCallbackMenu();

        //  miscellaneous

        static void         ExecuteAndReplot(FuncObjC& inObj);  // executes inObj and replots
        static void         ReplotAfterCalc();  // replots where required
                                                // uses objWasCalculated in base class
        void                CheckTGADump();     // if TGA dump method is auto does dump

        virtual void        GetAxesTypes(bool& xIsLinear,
                                         bool& yIsLinear,
                                         bool& zIsLinear);      // default -- all linear

        void                GetAxesTypes(bool& xIsLinear,
                                         bool& yIsLinear);      // calls 3D version

        virtual Coord3D     GetNormalizedCoord(const Coord3D& inCoord,
                                               const Coord3D& offsetCoord);
                                // for transparency calcs
                                // gets coordinate transformed for view space for distance
                                // calculations
                                // default returns inCoord, override in PS

        static void         InitializeAllPlots();  // use on file read to create plots before calcs
                                                // avoids problems with some video drivers

        // composite plot support
        inline bool         IsMasterPlot()  const {return masterPlot == this;}
        inline bool         IsSubPlot()     const {return masterPlot != this;}
        inline PlotDefC&    GetMasterPlot() const {return *masterPlot;}
        void                ResetToMaster() {masterPlot = this;}
        PC_Platform*        GetPlatformData() const {return GetMasterPlot().platformData;}


        // dragging support
        virtual OGLBase*    GetOGLBase() = 0;

    protected:
        void                FullCopy(const PlotDefC& a);  // called by copy in derived class

                            // for zooming or moving, depends on derived class
                            // returns true if view changed
        virtual bool        ProcessRubberbandRegion(const PC_ObjLimitRect&     rubberbandRegion) {return false;}

                            //  virtual for plot type cursor processing
        virtual void        ProcessCursorMove(int xpos,
                                              int ypos) {};

        virtual void        DrawAnnoObjects(); // virtual to allow override for anno

        virtual void        BasicFullDraw();   // overriden for 3D stereo


    private:
        void                BuildCallbackMenu();        // for locality
        virtual void        DoCallback(int reason);     // interactiveobj virtual

        void                DoDrawPlot();               // called by Draw & Redraw
        void                SetRubberbandOKLimits(); // area inside margins

        bool                CheckViewChangeReplot();

        void                AdjustAnnoSelection(int& dx,
                                                int& dy,
                                                int& ulXpos,
                                                int& ulYpos);

        void                InitCommon();  // common to normal and copy constructor
        void                LocalCopy(const PlotDefC& a);  // common to copy constructor and = operator


        // for vectoring/adjusting rectangles on composite plots
        void                ClearPlatformRectangle(const PC_ObjLimitRect&     rectToClear);
        void                DrawPlatformRectangle(const PC_ObjLimitRect&     rectToDraw,
                                                        bool                 useSelectColor);

};


#endif // !C_PLOTDEF_H

