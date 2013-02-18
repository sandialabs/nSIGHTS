///////////////////////////////////////////////////////////////////////////////////
//
// OGL_3DBase.h
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
//      plotting space for 3D plots
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef OGL_3DBASE_H
#define OGL_3DBASE_H

#include <genPlotClass/PD_3D.h>

#include <genPlotClassPS/OGL_Base.h>


// Open GL base class - implemented as DrawingArea
class OGL3DBase : public OGLBase {
    private:
        int                 nextLight;

    protected:
        PD_3D&              plot3Dbase;

        //  normalized rel lengths of axes
        double              xOrthoSpan;
        double              yOrthoSpan;
        double              zOrthoSpan;

        //  for transparency normalizations and projection scaling - set in derived ViewSetup()
        Coord3D                 normMin;
        Coord3D                 normMax;
        Coord3D                 normMid;
        Coord3D                 normSpan;


    public:
                        OGL3DBase(PD_3D& a);
                        ~OGL3DBase() {};

        //      view & lighting initialization
        virtual void    InitView();

        // stereo support
        virtual void    InitStereoView();
        virtual void    SetStereoView(bool leftSide);

        virtual void    ViewSetup() = 0;

        void            InitLighting();


        bool            DoLightingCalcs() {return plot3Dbase.plotLighting.useLighting;}
        int             GetNextLight() {return nextLight++;}


        // transparency support
        void            StartTransparency();
        Coord3D         GetNormalizedCoord(const Coord3D& worldCoord); // for Z calcs
        void            SetTransFactor(const double& transFactor);
        inline bool     InTransparencySection() {return doTransparency;}
        void            EndTransparency();


        //  miscellaneous 3D OGL drawing routines used mostly for axes lines
        virtual void    DrawLine(const Coord3D& stPoint,  // OGL_base virtual
                                 const Coord3D& endPoint) const;

        void            DrawLine(const Point2D& stPoint,
                                 const Point2D& endPoint,
                                 const double&  zVal,
                                       Plane3D  linePlane) const;

        // plot space limits in linear space
        void            GetTransformedLimits(Coord3D& minLim,
                                             Coord3D& maxLim) const;

        //  returns XYZ components for given pixel size - used primarily for axis increments
        Coord3D         GetPixelComponents(int nPixels) const;

        // similar to above -- scaling for axes independent data like symbols and stroke fonts
        Coord3D         GetIndependentScale(const double& inSize) const;  //  returns scale factor


                        // for axes labelling
        void            Axes3DLabel(const PSC_Font&               inFont,
                                        const Coord3D&                xyz,
                                        const double&                 xOffsetChars,
                                        const double&                 yOffsetChars,
                                        const double&                 rotation,
                                              HorizontalTextAlignment halign,
                                              VerticalTextAlignment   valign,
                                              Plane3D                 labelPlane,
                                              bool                    mirrorPlane,
                                        const char                    str[]);

                        // for data labelling
        void            Data3DLabel(const PSC_Font&               inFont,
                                        const Coord3D&                xyz,
                                        const double&                 xyRotation,
                                              HorizontalTextAlignment halign,
                                              VerticalTextAlignment   valign,
                                              bool                    isVertical,
                                        const char                    str[]);


        virtual Coord3D GetExtrusionScale(bool pixelSizing) const;
                        // used by GL_Extrusion for circular tubes
                        // base class is good for pixelSizing
                        // real implemented in derived class

    protected:

        void            SetLineAntiAlias();


        void            NormalizeSpans();   // takes spans and normalizes to smallest length

        // axes drawing support

        static void     TransformIncrements(bool                    axisIsLog,
                                            const SC_DoubleArray&   inIncs,
                                            SC_DoubleArray&         tranIncs);

        void            DrawOneAxes(const double&         axMin,
                                    const double&         axMax,
                                    const SC_DoubleArray& majorIncs,
                                    const SC_DoubleArray& minorIncs,
                                    const PC_AxesFormat&  format,
                                    Point2D&        stPoint,
                                    Point2D&        majTicEndPoint,
                                    Point2D&        minTicEndPoint,
                                    Point2D&        gridEndPoint,
                                    Point2D&        offsetEndPoint,
                                    double&         stComponent,
                                    double&         majTicComponent,
                                    double&         minTicComponent,
                                    double&         gridComponent,
                                    double&         offsetComponent,
                                    const double&   zValue,
                                          Plane3D   axPlane,
                                          bool      axisIsLog);

        void            DrawAxesPlane(const SC_DoubleArray& xMajorIncs,
                                      const SC_DoubleArray& xMinorIncs,
                                      const SC_DoubleArray& yMajorIncs,
                                      const SC_DoubleArray& yMinorIncs,
                                      const Limit2D&        xyLimits,
                                      const double&         zValue,
                                            PC_3DAxesFormat::Axes3DPlanes axPlane,
                                            bool               xIsLog,
                                            bool               yIsLog);

        void            DrawAxesLabels(const SC_DoubleArray& xMajorIncs,
                                       const SC_DoubleArray& xtranMajorIncs,
                                       const SC_DoubleArray& yMajorIncs,
                                       const SC_DoubleArray& ytranMajorIncs,
                                       const SC_DoubleArray& zMajorIncs,
                                       const SC_DoubleArray& ztranMajorIncs,
                                       const SC_RealFormat&  xFormat,
                                       const SC_RealFormat&  yFormat,
                                       const SC_RealFormat&  zFormat);

        void            SetAlignmentAdjust(const SC_DoubleArray& majorIncs,
                                           const double&         axMin,
                                           const double&         axMax,
                                           bool&           adjMin,
                                           bool&           adjMax);

        void            SetAutoView(PC_AxesFormat& xynUorig,
                                    PC_AxesFormat& xynVorig);


    private:

        void            SetFontScale(const PSC_Font&            inFont);

        void            GetLabelSpecs(const PSC_Font&   inFont,
                                      const char*       inLab,
                                            double&     overTop,
                                            double&     width);

        void            Do3DLabel(const PSC_Font&               inFont,
                                  const double&                 xOffsetChars,
                                  const double&                 yOffsetChars,
                                        HorizontalTextAlignment halign,
                                        VerticalTextAlignment   valign,
                                        const char              str[]);

        void            PerspectiveSetup(double eyeSepMult);

};


#endif // !OGL_3DBASE_H

