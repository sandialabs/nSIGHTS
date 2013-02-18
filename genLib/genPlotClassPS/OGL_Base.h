///////////////////////////////////////////////////////////////////////////////////
//
// OGL_Base.h
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
//      basic OpenGL plotting space
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef OGL_BASE_H
#define OGL_BASE_H

#include <GL/GL.h>
#include <GL/GLU.h>

#include <genClass/C_TxtFile.h>

#include <genClassPS/PS_OGLBase.h>

#include <genPlotClass/C_PlotDef.h>
#include <genPlotClass/PSC_Font.h>
#include <genPlotClass/PC_VRMLOutData.h>
#include <genPlotClassPS/OGL_MatrixOps.h>


class OGLFont;


class ExtendedLabelData {
    public:
        double    width;            // absolute width of label
        double    height;           // height from standard top of first line to baseline of next row
        double    nextYStart;       // height + current defined line spacing component
        double    aboveTop;         // above standard top of first line (super scripts if +ve, all small if -ve)
        double    belowBase;        // descenders or subscript distance below baseline of last line of label

    public:
                  ExtendedLabelData();

        void      GetMax(const ExtendedLabelData& a);
        void      Clear();          // sets to 0.0
};


// Open GL base class - derived from platform specific
class OGLBase {

    protected:
        static const double     logAxesMin;

        PlotDefC&               plotBase;
        PS_OGLBase*             currOGLBase;

#ifdef _GLPRHARDCOPY_
        //  hard copy
        bool                    glpInitComplete;
        bool                    glpDoingHardCopy;
        double                  glpFontSizeScale;
#endif

       //  vrml output
        bool                    vrmlDoingOutput;
        TxtFileC*               vrmlOut;
        double                  vrmlEdgeFactor;

        //  used for text sizing & pixel scaling in Anno, text size & pixel scaling in 2D
        double                  xPerPixel;
        double                  yPerPixel;
        double                  rasterZpos;  //  z pos for print string
        bool                    inAnnoView;  // kluge to determine what symbol scaling to use in 3D
                                             // set true by InitAnnoView
        double                  textLengthMultiplier;  // 1.0 normally, adjusted during hard copy

        PC_View                 currView;


        // stereo support
        bool                    viewIsStereo;

        // transparency support - just use by 3D now, 2D later maybe
        bool                    doTransparency;
        double                  currTransFactor;

    private:
        int                 nextLight;

    public:
                                OGLBase(PlotDefC& a);
                                ~OGLBase();

        // for composite plot support
        void                    SetOGLBase(PS_OGLBase* oglBase);

        //  basic usage
        void                    Clear() const;            // glClear
        virtual void            Flush();            //

        //  basic 2D bitmap font access
        double                  GetDefaultLineMultiplier() const;

        bool                    GetSpaceIncrement(const PSC_Font&  inFont,
                                                        double&    spaceWidth) const;

        bool                    GetFrameIncrement(const PSC_Font&  inFont,
                                                  const double&    aboveTop,
                                                  const double&    belowBase,
                                                        double&    xInc,    // half space for horiz
                                                        double&    yInc) const;   // half line height


        bool                    GetStringSizeInfo(const PSC_Font&  inFont,      // metrics in user units
                                                  const char      inStr[],      // defined by xPerPixel and yPerPixel
                                                        double&   xSize,        // x Size of string
                                                        double&   ySize) const;       // y Size of  string

        bool                    GetExtendedStringSizeInfo(const PSC_Font&           inFont,
                                                          const char                inStr[],
                                                                double&             lineMult, // for successive rows
                                                                ExtendedLabelData&  textInfo) const;

        bool                    GetExtendedStringSizeInfo(const PSC_Font&           inFont,
                                                          const char                inStr[],
                                                                ExtendedLabelData&  textInfo) const;


        //  2D bitmap font use
        void                    PrintString(const PSC_Font&                 inFont,    // prints string starting at xPos, yPos
                                            const char                      inStr[],  // in 2D user units
                                            const double&                   xPos,
                                            const double&                   yPos) const;

        void                    PrintString(const PSC_Font& inFont,    // prints string starting at xPos, yPos
                                            const char      inStr[],  // in 2D user units
                                                  double&   lineMult,   // carries multiplier
                                            const double&   xPos,
                                            const double&   yPos,
                                                  HorizontalTextAlignment   halign = hta_Left) const;       // internal to string


        void                    PrintString(const PSC_Font&                  inFont,    // prints string at xPos, yPos
                                            const char                      inStr[],    // in user units
                                                  HorizontalTextAlignment   halign,     // relative to text orientation
                                                  VerticalTextAlignment     valign,     // relative to text orientation
                                            const double&                   xPos,
                                            const double&                   yPos) const;



        //                  view initialization
        virtual void            InitView()  = 0;     // plot specific
        void                    InitAnnoView();      // sets up for annotation in Ortho2D window
        const PC_View&          GetCurrentView() const {return currView;}

        //  miscellaneous OGL drawing routines used mostly for axes lines
        //  set colors
        void                SetClearColor(const SC_ColorSpec& inSpec) const;
        void                SetDrawColor(const SC_ColorSpec& inSpec) const;
        void                SetLine(int lineWidth) const;
        void                SetLineType(PlotLineType lineType) const;
        void                SetLineSolid() const;

        //  basic line draw is virtual -- treated differently for 2D/3D
        virtual void        DrawLine(const Coord3D& stPoint,
                                     const Coord3D& endPoint) const = 0;

        // this does conversion for log axes if necessary -- returns false if log and <= zero
        virtual bool        TransformForAxes(Coord3D& inCoord) const {return true;}  // default does nothing

        // scaling for axes independent data like symbols and stroke fonts
        virtual Coord3D     GetIndependentScale(const double& inSize) const = 0;  //  returns scale factor

        //  clip planes
        virtual void        SetClipPlanes() = 0;
        void                ClearClipPlanes();  // clears all 6

        //  generic GLpr library hard copy support

        void                OutputPostscript();


#ifdef _GLPRHARDCOPY_
        void                GlpCheckError() const;
        bool                DoingHardCopy() const;
        void                HardCopySetViewport(bool doFill = true) const;

        void                HardCopyBlockStart(int nvertex) const;  // this starts a std single color 3D block
        void                HardCopyBlockEnd() const;

        void                SetHardCopyParameters() const;
#else
        void                SetHardCopyParameters() const {};
        void                HardCopySetViewport(bool doFill = true)const{};
        void                HardCopyBlockStart(int nvertex)const {};
        void                HardCopyBlockEnd()const {};
        bool                DoingHardCopy() const {return false;}
#endif



        // VRML output support
        bool                DoingVRMLOutput() {return vrmlDoingOutput;}
        TxtFileC&           GetVRMLOutputFile() {return *vrmlOut;}
        double              GetVRMLEdgeFactor() {return vrmlEdgeFactor;}


                            // does transforms before writing to VRML
                            // base case just swaps Z & Y, -ve X
                            // derived case in 3D spatial does vert exag
        virtual void        VRMLTransformCoord(Coord3D& inCoord);


    protected:
        void                DisableLineAntiAlias();
        void                SetCurrentView();    // set currView var -- call before any init view work
        void                SetClipPlane(int planeIndx,
                                         const double*  planeEquation); // array of 4

        void                ScissorSetup();  // sets up scissor/clear region

        void                ClearBuffer() const;

    private:
        void                VRMLOutputViewpoint(const Coord3D&          lookAtPoint,
                                                const PC_VRMLOutData&   vrmlData,
                                                const double&           rotDegrees,
                                                const char              viewpointName[]);



};


#endif // !OGL_BASE_H

