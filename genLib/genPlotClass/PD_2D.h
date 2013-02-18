///////////////////////////////////////////////////////////////////////////////////
//
// PD_2D.h
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
//      defines basic 2D plot
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PD_2D_H
#define PD_2D_H

#include <genClass/SC_RealFormat.h>

#include <genPlotClass/PC_2DPlotAnno.h>
#include <genPlotClass/PC_Report.h>

#include <genPlotClass/C_PlotDef.h>

//  This is the base class for all 2D plot definitions
class PD_2D : public PlotDefC  {
    protected:
        PC_Report*          cursorReport;           //  cursor reporting area
        SC_RealFormat       xFormat;
        SC_RealFormat       yFormat;

    public:
        enum                {maxPlotLayers = 8};        // number of layers in plot layer control


        bool                showReportArea;         //  controls display of report area

        //  cursor stuff
        Coord3D             cursorCoord;            //  cursor coord on last locate CB
                                                    //  used in view changes
        bool                cursorIsInsidePlot;     //  within axis

        //  plot anno parameter page
        PC_2DPlotAnno       plotAnnotation;

    private:
        bool                layersToPlot[maxPlotLayers];

    public:
                            PD_2D(const char* dType);
                            PD_2D(const PD_2D& a);
                            ~PD_2D();

        PD_2D&              operator= (const PD_2D& a);

        void                SetDefault();

        Coord3D             GetNewViewCenter()  {return cursorCoord;}

        //  FuncObjC virtual -- sets up cursor report if not done
        void                CalcOutput(FOcalcType  calcType);

        //  PlotDefC virtual
        void                DrawPlotObjects();

        //  user to pixel conversion
        virtual bool        UserXYtoPixelXY(const Point2D& userPt,
                                            int&     userPixX,
                                            int&     userPixY) = 0;

        virtual bool        PixelXYtoUserXY(int      userPixX,
                                            int      userPixY,
                                            Point2D& userPt)   = 0;

        //  keypress
        void                ProcessSelectionKeys(SelectionKeys inKey);
                                                // passes keypress and location to objects

        virtual void        TransformCursor(Coord3D& currCursor) {};  // done prior to call to plot objects

        const SC_RealFormat&    GetXFormat() const {return xFormat;}
        const SC_RealFormat&    GetYFormat() const {return yFormat;}

    protected:
        void                FullCopy(const PD_2D& a);  // called by copy in derived class

        //  PlotDefC virtuals
        void                ProcessCursorMove(int xpos,
                                              int ypos);

        bool                ProcessRubberbandRegion(const PC_ObjLimitRect&     rubberbandRegion);

    private:
        void                InitCommon();               // common to normal and copy constructor
        void                LocalCopy(const PD_2D& a);  // common to copy constructor and = operator

};


#endif // !PD_2D_H

