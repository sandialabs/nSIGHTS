///////////////////////////////////////////////////////////////////////////////////
//
// OGL_Base.cpp
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

#include "GenLibAfx.h"

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_AnnoObj.h>

#include <genPlotClassPS/PSC_FontPS.h>
#include <genPlotClassPS/OGL_Font.h>
#include <genPlotClassPS/OGL_StringProcess.h>

#include <genPlotClassPS/OGL_Base.h>

const double  OGLBase::logAxesMin = 1.0E-99;


#ifdef _GLPRHARDCOPY_

    #ifdef _DEBUG
        #define _GLPR_LIBRARY_FILE  "glprmd.lib"
    #else
        #define _GLPR_LIBRARY_FILE  "glprmd.lib"
    #endif

    #ifndef _GLPRNOMSG
        #pragma message("Project will be automatically linked with GLpr library " _GLPR_LIBRARY_FILE )
    #endif //_GLPRNOMSG

    #pragma comment(lib, _GLPR_LIBRARY_FILE)

#endif // _GLPRHARDCOPY_


ExtendedLabelData::ExtendedLabelData()
{
    Clear();
}

void ExtendedLabelData::GetMax(const ExtendedLabelData& a)
{
    if (a.width > width)
        width = a.width;
    if (a.height > height)
        height = a.height;
    if (a.nextYStart > nextYStart)
        nextYStart = a.nextYStart;
    if (a.aboveTop > aboveTop)
        aboveTop = a.aboveTop;
    if (a.belowBase > belowBase)
        belowBase = a.belowBase;
}

void  ExtendedLabelData::Clear()
{
   width        = 0.0;
   height       = 0.0;
   nextYStart   = 0.0;
   aboveTop     = 0.0;
   belowBase    = 0.0;
}


OGLBase::OGLBase(PlotDefC& a)
    : plotBase(a), currOGLBase(0)
{
#ifdef _GLPRHARDCOPY_
    glpInitComplete     = false;
    glpDoingHardCopy    = false;
    glpFontSizeScale    = 1.0;
#endif

    vrmlDoingOutput = false;
    vrmlOut = 0;

    doTransparency = false;
    currTransFactor = 0.0;


    textLengthMultiplier = 1.0;
    xPerPixel = 1.0;
    yPerPixel = 1.0;
    rasterZpos = 0.0;

    viewIsStereo = false;
}

OGLBase::~OGLBase()
{
    if (currOGLBase)
    {
        if ((currOGLBase->oglDisplay != 0) && (currOGLBase->oglContext !=0))
            OGLFont::UnloadFontsForContext(currOGLBase->oglContext);
    }
}

void OGLBase::SetOGLBase(PS_OGLBase* oglBase)
{
    currOGLBase = oglBase;
}
void OGLBase::Flush()
{
    if (currOGLBase)
    {
        currOGLBase->Flush();
        glFlush();
        glFinish();
    }
}

void OGLBase::Clear() const
{
    if (viewIsStereo)
        glDrawBuffer(GL_BACK_LEFT);
    else
        glDrawBuffer(GL_BACK);

    ClearBuffer();

    if (viewIsStereo)
    {
        glDrawBuffer(GL_BACK_RIGHT);
        ClearBuffer();
    }
}

void OGLBase::ClearBuffer() const
{
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
}


void OGLBase::SetClearColor(const SC_ColorSpec& inSpec) const
{
    glClearColor((GLfloat) inSpec.RH, (GLfloat) inSpec.GS, (GLfloat) inSpec.BV, 1.0f);
/*
    if (DoingVRMLOutput())
    {
        vrmlOut->SetRealConvFormat(SC_RealFormat(ncf_Decimal, 3));
        vrmlOut->WriteText("Background {");
        vrmlOut->WriteSolidText("   groundColor [ ");
        vrmlOut->WriteDouble(inSpec.RH);
        vrmlOut->WriteDouble(inSpec.GS);
        vrmlOut->WriteDouble(inSpec.BV);
        vrmlOut->WriteText("]");

        vrmlOut->WriteSolidText("   skyColor [ ");
        vrmlOut->WriteDouble(inSpec.RH);
        vrmlOut->WriteDouble(inSpec.GS);
        vrmlOut->WriteDouble(inSpec.BV);
        vrmlOut->WriteText("]");
        vrmlOut->WriteText("}");
    }

*/
}

void  OGLBase::SetLine(int lineWidth) const
{
    glLineWidth(GLfloat(lineWidth));
}

void OGLBase::SetLineType(PlotLineType lineType) const
{
    if (lineType == plt_Solid)
    {
        glDisable(GL_LINE_STIPPLE);
    }
    else
    {
        glEnable(GL_LINE_STIPPLE);
        const LineTypeSpec& currType = lineTypeData.GetLineTypeSpec(lineType);
        glLineStipple(currType.lineFactor, currType.linePattern);
    }
}

void OGLBase::SetLineSolid() const
{
    glDisable(GL_LINE_STIPPLE);
}

void  OGLBase::SetCurrentView()
{
    currView = plotBase.GetCurrentView();
}

void  OGLBase::InitAnnoView()
{
    //  no lighting or depth testing
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LINE_SMOOTH);

    //  clear any clip planes
    ClearClipPlanes();

    //  change projection to XY for annotation
    glMatrixMode(GL_PROJECTION);        //  set projection  mode
    glLoadIdentity();                   //  clear existing

    glOrtho(0.0, 100.0, 0.0, 100.0, -0.01, 100.0);
    // znear and zfar set to keep 0.0 near to viewer so that anno always is closer than data in 3D Ortho

    int uOff = plotBase.plotUOffset;
    int vOff = plotBase.plotVOffset;
    glViewport(uOff, vOff, plotBase.plotUwidth, plotBase.plotVheight);

    // set hardcopy viewport but don't fill
    HardCopySetViewport(false);

    //  clear modelview
 // set initial translation/rotation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // set text sizing data
    xPerPixel = AnnoObjC::maxAnnoCoord / double(plotBase.plotUwidth);
    yPerPixel = AnnoObjC::maxAnnoCoord / double(plotBase.plotVheight);
    rasterZpos = 0.0;

    inAnnoView = true;
    doTransparency = false;
}


void OGLBase::SetClipPlane(int planeIndx,
                           const double*  planeEquation)
{
    glClipPlane(GLenum(GL_CLIP_PLANE0 + planeIndx), planeEquation);
    glEnable(GLenum(GL_CLIP_PLANE0 + planeIndx));
}

//  clip planes uses all 6
void OGLBase::ClearClipPlanes()
{
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);
    glDisable(GL_CLIP_PLANE4);
    glDisable(GL_CLIP_PLANE5);
}

void OGLBase::DisableLineAntiAlias()
{
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);

}
// transparency support

void OGLBase::SetDrawColor(const SC_ColorSpec& inSpec) const
{
    if (doTransparency)
        glColor4d(inSpec.RH, inSpec.GS, inSpec.BV, currTransFactor);
    else
        glColor3d(inSpec.RH, inSpec.GS, inSpec.BV);
}


double  OGLBase::GetDefaultLineMultiplier() const
{
    return 1.30;
}


bool OGLBase::GetSpaceIncrement(const PSC_Font&  inFont,
                                      double&    spaceWidth) const
{
    OGL2DFont* currFont = OGL2DFont::SetupFont(inFont);
    if (!currFont)
        return false;

    int wid, widInc, asc, desc;
    currFont->GetCharSizeInfo('L', wid, widInc, asc, desc);
    if (inFont.IsHorizontal())
    {
        spaceWidth = double(wid) * xPerPixel;
    }
    else
    {
        spaceWidth = double(wid) * yPerPixel;
    }
    return true;
}

bool OGLBase::GetFrameIncrement(const PSC_Font&    inFont,
                                const double&      aboveTop,
                                const double&      belowBase,
                                      double&      xInc,    // half space for horiz
                                      double&      yInc) const
{
    OGL2DFont* currFont = OGL2DFont::SetupFont(inFont);
    if (!currFont)
        return false;

    int intls = currFont->GetLineSize();
    int wid, widInc, asc, desc;
    currFont->GetCharSizeInfo('L', wid, widInc, asc, desc);
    if (inFont.IsHorizontal())
    {
        xInc = double(wid) * xPerPixel / 2.0;
        double dY = double(intls) * yPerPixel / 2.0;
        yInc = dY;
        while ((fabs(yInc) < fabs(aboveTop)) || (fabs(yInc) < fabs(belowBase)))
            yInc += dY;
    }
    else
    {
        double dX = double(intls) * xPerPixel / 2.0;
        xInc = dX;
        while ((fabs(xInc) < fabs(aboveTop)) || (fabs(xInc) < fabs(belowBase)))
            xInc += dX;
        yInc = double(wid) * yPerPixel / 2.0;
    }

    return true;
}

bool OGLBase::GetStringSizeInfo(const PSC_Font& inFont,  // metrics in user units
                                const char      inStr[],
                                      double&   xSize,
                                      double&   ySize) const
{
    double lineMult = GetDefaultLineMultiplier();
    ProcessedString procStr(lineMult);
    if ((!procStr.Process2DString(inFont, inStr)) || (!procStr.IsPrintable()))
        return false;

    procStr.SetBasicXY(xPerPixel, yPerPixel, textLengthMultiplier, false);

    procStr.GetSizeInfo(xSize, ySize);

    return true;
}

bool OGLBase::GetExtendedStringSizeInfo(const PSC_Font&           inFont,
                                        const char                inStr[],
                                              double&             lineMult,
                                              ExtendedLabelData&  textInfo) const
{
    ProcessedString procStr(lineMult);
    if (!procStr.Process2DString(inFont, inStr))
        return false;

    if (!procStr.IsPrintable())
    {
        textInfo.Clear();
        return true;
    }

    procStr.SetBasicXY(xPerPixel, yPerPixel, textLengthMultiplier, false);

    procStr.GetSizeInfo(textInfo.width, textInfo.height);

    procStr.GetSizeDeviation(textInfo.aboveTop, textInfo.belowBase);

    textInfo.nextYStart = textInfo.height + procStr.GetLineSize() * (procStr.lineSpacingMultiplier - 1.0);

    return true;
}

bool OGLBase::GetExtendedStringSizeInfo(const PSC_Font&           inFont,
                                        const char                inStr[],
                                              ExtendedLabelData&  textInfo) const
{
    double lineMult = GetDefaultLineMultiplier();
    return GetExtendedStringSizeInfo(inFont, inStr, lineMult, textInfo);
}


void   OGLBase::PrintString(const PSC_Font& inFont,          // prints string starting at xPos, yPos
                            const char      inStr[],
                                  double&   lineMult,   // carries multiplier
                            const double&   xPos,
                            const double&   yPos,
                                  HorizontalTextAlignment   halign) const

{
    ProcessedString procStr(lineMult);
    if ((!procStr.Process2DString(inFont, inStr)) || (!procStr.IsPrintable()))
        return;

    procStr.SetBasicXY(xPerPixel, yPerPixel, textLengthMultiplier, false);

    procStr.SetFinalXY(xPos, yPos, halign);

    procStr.Print2D(rasterZpos);

#ifdef _GLPRHARDCOPY_

    if (DoingHardCopy())
        procStr.HardCopy2D(plotBase.GetMasterPlot().GetOGLBase()->glpFontSizeScale, rasterZpos);

#endif

}

void   OGLBase::PrintString(const PSC_Font& inFont,          // prints string starting at xPos, yPos
                            const char      inStr[],
                            const double&   xPos,
                            const double&   yPos) const
{
    double lineMult = GetDefaultLineMultiplier();
    PrintString(inFont, inStr, lineMult, xPos, yPos);
}

void OGLBase::PrintString(const PSC_Font&            inFont,          // prints string starting at xPos, yPos
                            const char                inStr[],
                            HorizontalTextAlignment   halign,
                            VerticalTextAlignment     valign,
                            const double&             xPos,
                            const double&             yPos) const
{
    double lineMult = GetDefaultLineMultiplier();
    ProcessedString procStr(lineMult);
    if ((!procStr.Process2DString(inFont, inStr)) || (!procStr.IsPrintable()))
        return;

    procStr.SetBasicXY(xPerPixel, yPerPixel, textLengthMultiplier, false);

    double xSize, ySize;
    procStr.GetSizeInfo(xSize, ySize);

    double labXpos = xPos;
    double labYpos = yPos;
    if (inFont.IsHorizontal())
    {
        switch (halign)
        {
            case hta_Left   : {break;}
            case hta_Center : {labXpos -= xSize / 2.0; break;}
            case hta_Right  : {labXpos -= xSize; break;}
        }

        switch (valign)
        {
            case vta_Top    : {break;}
            case vta_Center : {labYpos += ySize / 2.0 ; break;}
            case vta_Bottom : {labYpos += ySize; break;}
        }
    }
    else
    {
        if (inFont.fontRotation == PSC_Font::fr_VertLeft)
        {
            switch (halign)
            {
                case hta_Left   : {break;}
                case hta_Center : {labYpos -= xSize / 2.0; break;}
                case hta_Right  : {labYpos -= xSize; break;}
            }

            switch (valign)
            {
                case vta_Top    : {break;}
                case vta_Center : {labXpos -= ySize / 2.0 ; break;}
                case vta_Bottom : {labXpos -= ySize; break;}
            }
        }
        else
        {
            switch (halign)
            {
                case hta_Left   : {break;}
                case hta_Center : {labYpos += xSize/ 2.0; break;}
                case hta_Right  : {labYpos += xSize; break;;}
            }

            switch (valign)
            {
                case vta_Top    : {break;}
                case vta_Center : {labXpos += ySize / 2.0 ; break;}
                case vta_Bottom : {labXpos += ySize; break;}
            }
        }
    }

    procStr.SetFinalXY(labXpos, labYpos, halign);

    procStr.Print2D(rasterZpos);

#ifdef _GLPRHARDCOPY_

    if (DoingHardCopy())
        procStr.HardCopy2D(plotBase.GetMasterPlot().GetOGLBase()->glpFontSizeScale, rasterZpos);

#endif

}


#ifdef _GLPRHARDCOPY_

bool  OGLBase::DoingHardCopy() const
{
    if (plotBase.IsMasterPlot())
        return glpDoingHardCopy;

    return plotBase.GetMasterPlot().GetOGLBase()->DoingHardCopy();
}

void OGLBase::HardCopySetViewport(bool doFill) const
{
    if (!DoingHardCopy())
        return;

    if (doFill)
    {
        SC_ColorSpec background = plotBase.defaultPenSet->GetColor(plotBase.plotBackgroundPen);
        GLfloat fillColor[3];
        fillColor[0] = GLfloat(background.RH);
        fillColor[1] = GLfloat(background.GS);
        fillColor[2] = GLfloat(background.BV);
        glpSetViewport(GLP_VIEWPORT_FILL, fillColor, fillColor);
    }
    else
        glpSetViewport(GLP_VIEWPORT_NONE, NULL, NULL);
}

void OGLBase::HardCopyBlockStart(int nvertex) const
{
    if (DoingHardCopy())
    {
        glpBegin(GLP_3D_COLOR, nvertex);
        SetHardCopyParameters();
    }
}

void  OGLBase::HardCopyBlockEnd() const
{
    if (DoingHardCopy())
    {
        glpEnd();
        GlpCheckError();
    }
}

void OGLBase::SetHardCopyParameters() const
{
    if (DoingHardCopy())
    {
        GLboolean getLineStipple = glIsEnabled(GL_LINE_STIPPLE);
        glpSetStyle(true, true, true, GLfloat(plotBase.psOutput.lineWidthMultiplier), getLineStipple);
        GlpCheckError();
    }
}

void OGLBase::GlpCheckError() const
{
    if (!DoingHardCopy())
        return;

    GLPenum currErr = glpGetError();
    if (currErr == GLP_NONE)
        return;

    GenAppErrorMsg("GLP error: ", glpGetErrorMsg(currErr));
}

#endif


void  OGLBase::OutputPostscript()
{
#ifdef _GLPRHARDCOPY_

    #pragma message("OutputPostscript is valid")

    const PC_PSOutData& currPSsetup = plotBase.psOutput;

    FILE* file = fopen(currPSsetup.outputFile, "w");
    if (!file)
    {
        GenAppErrorMsg("OpenPS", "opening file");
        return;
    }

    plotBase.InitRenderer();
    plotBase.SetupDataView();

    if (!glpInitComplete)
    {
        glpInit();
        glpInitComplete = true;
    }

    GLPenum opcode;
    switch (currPSsetup.outputFormat)
    {
        case PC_PSOutData::of_PS : {
            opcode = GLP_PS;
            break;
        }
        case PC_PSOutData::of_EPS : {
            opcode = GLP_EPS;
            break;
        }
        case PC_PSOutData::of_EPSWin : {
            opcode = GLP_EPS;
            break;
        }
        case PC_PSOutData::of_CGM : {
            opcode = GLP_CGM;
            break;
        }
        case PC_PSOutData::of_HPGL : {
            opcode = GLP_HPGL;
            break;
        }
    }

    char fName[80];
    ExtractFileName(fName, currPSsetup.outputFile, 80);

    // kluge for EPS preview bug
    if (currPSsetup.outputFormat == PC_PSOutData::of_EPSWin)
    {
        CopyString(fName, "%%Title: ", 80);
        char temp[80];
        ExtractFileName(temp, currPSsetup.outputFile, 80);
        ConcatString(fName, temp, 80);
    }

    // kluge for Adobe illustrator
    glpSetHelveticaFontname(currPSsetup.arialIsHelvetica ? 0 : 1);
    glpBeginPage(opcode, "AppDev", fName, "AppDevUser", file);
    GlpCheckError();

    double maxVerticalInches = currPSsetup.pageHeight - currPSsetup.topMargin - currPSsetup.bottomMargin;
    double maxHorizontalInches = currPSsetup.pageWidth - currPSsetup.rightMargin - currPSsetup.leftMargin;
    GLPenum orient = GLP_ORIENT_PORTRAIT;
    if (!currPSsetup.outputIsPortrait)
    {
        orient = GLP_ORIENT_LANDSCAPE;
        double temp =  maxVerticalInches;
        maxVerticalInches = maxHorizontalInches;
        maxHorizontalInches = temp;
    }
    double plotAspect  = double(plotBase.plotUwidth) / double(plotBase.plotVheight);
    double printAspect = maxHorizontalInches / maxVerticalInches;

    if (plotAspect > printAspect)
        maxVerticalInches = maxHorizontalInches / plotAspect;

    glpFontSizeScale = maxVerticalInches * 72.0 / double(plotBase.plotVheight); // points per pixel
    textLengthMultiplier =  currPSsetup.textLengthMultiplier;

    glpSetPage(GLfloat(currPSsetup.pageWidth), GLfloat(currPSsetup.pageHeight), orient, 1,
               GLfloat(currPSsetup.leftMargin), GLfloat(currPSsetup.rightMargin),
               GLfloat(currPSsetup.topMargin), GLfloat(currPSsetup.bottomMargin));
    GlpCheckError();

    glpSetDevice(GLP_DEVICE_TYPE_RGB, 600);
    GlpCheckError();

    glpDisable(GLP_OPTION_SUBDIVIDE);
    GlpCheckError();

    if (currPSsetup.zBufferMultiplier > 0.0)
    {
        glpEnable(GLP_OPTION_OCCLUSION_CULL);
        GlpCheckError();

        glpSetFloat(GLP_OPTION_ZBRESFACTOR, GLfloat(currPSsetup.zBufferMultiplier));
        GlpCheckError();
    }
    else
    {
        glpEnable(GLP_OPTION_SIMPLESORT);
        glpDisable(GLP_OPTION_OCCLUSION_CULL);
        GlpCheckError();
    }


    glpSetFloat(GLP_OPTION_GAMMA, GLfloat(currPSsetup.gammaCorrection));
    GlpCheckError();

    glpDoingHardCopy = true;
    HardCopySetViewport();


    plotBase.DrawAllObjects();

    plotBase.CloseRenderer();

    // add Win32 preview image
    if (currPSsetup.outputFormat == PC_PSOutData::of_EPSWin)
    {
        glpSetInteger(GLP_OPTION_PREVIEW_STRIDE, 1);
        glpEnable(GLP_OPTION_EPS_PREVIEW);
        glpDoingHardCopy = false;
        plotBase.InitRenderer();
        plotBase.SetupDataView();
        plotBase.DrawAllObjects();
        plotBase.CloseRenderer();
    }

    glpEndPage();
    GlpCheckError();

    if (glpGetPageStatus() == GLP_ZBRES_PGSTAT)
        GenAppWarningMsg("OutputPS", "Possible occlusion culling dropouts - check output");

    fclose(file);
    glpDoingHardCopy = false;
    textLengthMultiplier = 1.0;

#else
    GenAppErrorMsg("OutputPS", "no PostScript output in this platform release");
#endif
}


void OGLBase::VRMLTransformCoord(Coord3D& inCoord)
{
//  inCoord.cX = -inCoord.cX;
    double tmp = inCoord.cY;
    inCoord.cY = inCoord.cZ;
    inCoord.cZ = -tmp;
}

void OGLBase::ScissorSetup()
{
    //set scissors for clear
    glScissor(plotBase.plotUOffset, plotBase.plotVOffset, plotBase.plotUwidth, plotBase.plotVheight);
    glEnable(GL_SCISSOR_TEST);

    // and clear color
    SetClearColor(plotBase.defaultPenSet->GetColor(plotBase.plotBackgroundPen));
}

