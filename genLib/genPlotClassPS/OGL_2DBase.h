///////////////////////////////////////////////////////////////////////////////////
//
// OGL_2DBase.h
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
//      plotting space for 2D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef OGL_2DBASE_H
#define OGL_2DBASE_H

#include <genPlotClass/PD_2D.h>
#include <genPlotClassPS/OGL_Base.h>

// Open GL base class - implemented as DrawingArea
class OGL2DBase : public OGLBase {
    public:
        static const double  zdataMin;
        static const double  zdataMax;
        static const double  zannoMin;
        static const double  zannoMax;

    protected:
        PD_2D&          plot2Dbase;
        double          currZval;   // for layer imp

    private:
        //  for pixel conversion
        double          tranXMin;
        double          pixelsPerXUnit;
        double          tranYMin;
        double          pixelsPerYUnit;

    public:
                        OGL2DBase(PD_2D& a);
                        ~OGL2DBase() {};

        //  miscellaneous 3D OGL drawing routines used mostly for axes lines
        virtual void    DrawLine(const Coord3D& stPoint,  // OGL_base virtual
                                 const Coord3D& endPoint) const;  // cZ is given currZval

        void            DrawLine(const Point2D& stPoint,
                                 const Point2D& endPoint) const;

        double          GetZVal() const {return currZval;}
        // scaling for axes independent data like symbols and stroke fonts
        virtual Coord3D GetIndependentScale(const double& inSize) const;  //  returns scale factor

        void            SetDataLayer(int layer);
        void            SetDataLayerOffset(int layer);    // a nudge above the data layer

        //  point 2D equiv of 3D
        virtual bool    TransformForXY(Point2D& inPoint) const {return true;}  // default does nothing

        //  these are used by data objects which add custom grid lines
        void            GetAxesOffsets(Point2D& axOffset,       // offset of axes lines in transformed coords
                                       Point2D& labOffset,
                                       bool&    axesIsFramed) const;    // offset of axes labels (beyond axes lines)
        Point2D         GetPixelComponents(int nPixels) const;  //  returns XY components for given pixel size


        // kluges to put in pixel scaling for hard copy line patterns and
        // line polygons
        //  origin is lower-left corner of plot window,
        void            SetPixelScaling();// pushes existing matrix
        bool            GetPixelXY(Point2D& inPoint) const;
        bool            GetPixelXY(Coord3D& inCoord) const;

        // for use by supplementary axes drawing routines
        void            AxesDrawSetup();
        void            MajorGridDrawSetup();
        void            MajorTicDrawSetup();
        void            MinorGridDrawSetup();
        void            MinorTicDrawSetup();

    protected:
        void            SetAnnoLow()  {currZval = zannoMin;}
        void            SetAnnoHigh() {currZval = zannoMax;}
        void            SetGridHigh() {currZval = zdataMax - 0.1;}

        void            SetLineAntiAlias();


};


#endif // !OGL_2DBASE_H

