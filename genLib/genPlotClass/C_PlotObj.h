///////////////////////////////////////////////////////////////////////////////////
//
// C_PlotObj.h
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
//      derived from PlotObjBaseC.  Adds plot layering and
//  reporting support for 2D objects and polygon offset
//  support for 3D objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef C_PLOTOBJ_H
#define C_PLOTOBJ_H

#include <genClass/C_Common.h>

#include <genPlotClass/PC_Report.h>
#include <genPlotClass/PSC_TransparentObject.h>
#include <genPlotClass/C_PlotDef.h>
#include <genPlotClass/C_PlotObjBase.h>

//  This is the base class for all mView objects which can produce plottable stuff

class PC_Report;

class PlotObjC : public PlotObjBaseC, public PSC_TransparentObject  {
    public:
        bool            reportOK;           // object can report
                                            // should be set at derived object construction
                                            // default true
        bool            activeOK;           // is interactive obj
        bool            isActive;           // is active
        bool            is3DObject;         //  for drawing in 3D, set automatically by constructor
                                            //  based on PlotDef (recast to PD_3D)

                        // properties
        int             plotLayer;          //  layer for layer control - 2D only
        double          polyOffset;         //  polyOffset for 3D decaling & edgeing
        bool            usePolyOffset;      //  used by UI to set access to control - set false by default
                                            //  true by 3D objects using polygons
        bool            recalcOnViewChange;  //  requires calc if view changes
        PC_Report*      cursorReport;       //  reporting area
                                            //  platform specific type must be created
                                            //  on derived object construction

                        //  following is for hard copy implementation with GLpr library
                        //  klug: GLpr does not implement polygon offset so it has to be done manually
        Coord3D         offsetCoord;        //  added to each vertex when plotted, only used in 3D data obj


    public:
                        PlotObjC(const char* pltType, PlotDefC& assPlt);
                        PlotObjC(const PlotObjC& a, PlotDefC& assPlt);

        virtual         ~PlotObjC();

        PlotObjC&       operator= (const PlotObjC& a);


                        //  FuncObjC virtuals
        void            DoStatusChk();

        //  new object virtuals
        //  gets limits of object for axis sizing
        virtual Limit3D GetPlotObjLimits();
        // recalc
        virtual void    ViewChangeRecalc() {};  // default does nothing
        //  cursor reporting
        virtual void    ReportCursorData(const Coord3D& cursorPos);
        //  keystroke processing
        virtual void    ProcessSelection(const Coord3D&                cursorPos,
                                               PlotDefC::SelectionKeys inKey);

        // implemented by isActive objects
        virtual PC_CallbackMenu* GetCallbackMenu();  // default returns null


        // support for 2/3D line antialiasing
        // objects that are lines only are drawn last
        virtual bool    ContainsLinesOnly();  // default is false, will complete
                                              // as time permits

    private:
        void                InitCommon();  // common to normal and copy constructor
        void                LocalCopy(const PlotObjC& a);  // common to copy constructor and = operator
};

#endif // !C_PLOTOBJ_H

