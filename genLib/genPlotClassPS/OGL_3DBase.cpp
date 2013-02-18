///////////////////////////////////////////////////////////////////////////////////
//
// OGL_3DBase.cpp
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

#include "GenLibAfx.h"

#include <genClass/C_Common.h>
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genClass/SC_StringArray.h>

#include <genPlotClassPS/OGL_StringProcess.h>

#include <genPlotClassPS/OGL_3DBase.h>


OGL3DBase::OGL3DBase(PD_3D& a) : OGLBase(a), plot3Dbase(a)
{
    xOrthoSpan = 1.0;
    yOrthoSpan = 1.0;
    zOrthoSpan = 1.0;
}


void OGL3DBase::DrawLine(const Coord3D& stPoint,
                         const Coord3D& endPoint) const
{
    if (stPoint.SameCoord(endPoint))
        return;

    glBegin(GL_LINES);
        glVertex3d(GLdouble(stPoint.cX), GLdouble(stPoint.cY), GLdouble(stPoint.cZ));
        glVertex3d(GLdouble(endPoint.cX), GLdouble(endPoint.cY), GLdouble(endPoint.cZ));
    glEnd();

}

void OGL3DBase::DrawLine(const Point2D& stPoint,
                         const Point2D& endPoint,
                         const double&  zVal,
                               Plane3D  linePlane) const
{
    Coord3D stCoord(stPoint, zVal);
    Coord3D endCoord(endPoint, zVal);
    stCoord.FromModelPlane(linePlane);
    endCoord.FromModelPlane(linePlane);
    DrawLine(stCoord, endCoord);
}


void OGL3DBase::InitView()
{
    viewIsStereo = false;
    ViewSetup();

    //  clear any clip planes
    ClearClipPlanes();

    int width, height;
    plotBase.CalcAvailablePixels(width, height);

    //  set projection
    if (plot3Dbase.ProjectionIsOrtho())
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        double aspect = double(width)/ double(height);

        double maxMap = sqrt(xOrthoSpan * xOrthoSpan + yOrthoSpan * yOrthoSpan + zOrthoSpan * zOrthoSpan) / 2.0;
        double maxZrange = -maxMap * currView.scale * 2.0;
        if (aspect > 1.0)
            glOrtho(-maxMap * aspect, maxMap * aspect, -maxMap, maxMap, maxZrange * 2.0, 0.0);
        else
            glOrtho(-maxMap, maxMap, -maxMap / aspect, maxMap / aspect, maxZrange * 2.0, 0.0);

            //  set initial translation/rotation
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        //  shift into z range
        glTranslated(0.0, 0.0, - maxZrange);

        //  attitude sliders
        //  scale
        glScaled(currView.scale, currView.scale, currView.scale);
        //  rotation around azimuth
        glRotated(currView.elevation -90.0, 1.0, 0.0, 0.0);
        //  azimuth around Z
        glRotated(currView.azimuth, 0.0, 0.0, 1.0);
        //  shift centre
        glTranslated(currView.translation.cX, currView.translation.cY, currView.translation.cZ);

        // scale for normalizations
        glScaled(xOrthoSpan / normSpan.cX, yOrthoSpan / normSpan.cY, zOrthoSpan / normSpan.cZ);

        // translate again for normalizations
        glTranslated(-normMid.cX, -normMid.cY, -normMid.cZ);

    }
    else
    {
        PerspectiveSetup(0.0);
    }

    glViewport(plotBase.plotUOffset + plotBase.leftMargin,
               plotBase.plotVOffset + plotBase.bottomMargin, width, height);
    //set scissors for clear
    ScissorSetup();
    // required for composite plots
    HardCopySetViewport();

    // clear viewport
    Clear();

    InitLighting();
    inAnnoView = false;

}

void OGL3DBase::PerspectiveSetup(double eyeSepMult)
{
    Coord3D zeroOffset(0.0, 0.0, 0.0);
    Coord3D minCoord = plotBase.GetNormalizedCoord(currView.viewLimits.minLim, zeroOffset);
    Coord3D maxCoord = plotBase.GetNormalizedCoord(currView.viewLimits.maxLim, zeroOffset);;

    double maxSceneWidth = minCoord.Distance(maxCoord);
    double sceneWidth = maxSceneWidth / currView.scale;
    double fovRadians = Radians(plot3Dbase.fieldOfView);
    double focalLength = sceneWidth / 2.0 / tan(fovRadians / 2.0);

    double nearDist = focalLength / 5.0;
    double farDist  = focalLength + maxSceneWidth * 4.0;
    double wd2 = nearDist * tan(fovRadians);
    double ndfl = nearDist / focalLength;
    double eyeSep = focalLength * eyeSepMult;

    int width, height;
    plotBase.CalcAvailablePixels(width, height);
    double aspect = double(width)/ double(height);

    double left  = -aspect * wd2 + eyeSep * ndfl;
    double right = aspect * wd2 + eyeSep * ndfl;
    double top    =   wd2;
    double bottom = - wd2;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(left, right, bottom, top, nearDist, farDist);

    Coord3D spanOffset((1.0 - xOrthoSpan)/ 2.0 , (1.0 - yOrthoSpan)/ 2.0 , (1.0 - zOrthoSpan)/ 2.0 );
    Coord3D translation = currView.translation + spanOffset;
    translation *= (maxCoord - minCoord);

    Coord3D lookAtPoint = (minCoord + maxCoord) / 2.0;
    lookAtPoint -= translation;

    double elevAngle = Radians(currView.elevation);
    double rotAngle = Radians(currView.azimuth);

    double dz = focalLength * sin(elevAngle);
    double xylen = focalLength * cos(elevAngle);
    double dx = - xylen * sin(rotAngle);
    double dy = - xylen * cos(rotAngle);

    Coord3D eyeCoord = lookAtPoint;
    eyeCoord.cX += dx;
    eyeCoord.cY += dy;
    eyeCoord.cZ += dz;

    Coord3D upVec(0.0, 0.0, 1.0);
    if (fabs(sin(elevAngle)) > 0.95)
    {
        upVec = Coord3D(sin(rotAngle), cos(rotAngle), 0.0);
        upVec.Normalize();
    }

    if (eyeSep > stdEps)
    {
        Coord3D rsep = CrossProduct(eyeCoord, upVec);
        rsep.Normalize();
        rsep *= eyeSep;
        eyeCoord += rsep;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeCoord.cX, eyeCoord.cY, eyeCoord.cZ,
              lookAtPoint.cX, lookAtPoint.cY, lookAtPoint.cZ,
              upVec.cX, upVec.cY, upVec.cZ);

    // denormalize
    // scale for normalizations
    glScaled(xOrthoSpan / normSpan.cX, yOrthoSpan / normSpan.cY, zOrthoSpan / normSpan.cZ);

    // translate again for normalizations
    glTranslated(-normMin.cX, -normMin.cY, -normMin.cZ);
}


void OGL3DBase::InitStereoView()
{
    viewIsStereo = true;
    if (!currOGLBase->GLisStereo())
        GenAppInternalError("OGL3DBase::InitStereoView");
    Clear();

    ViewSetup();
}


void OGL3DBase::SetStereoView(bool leftSide)
{
    //  clear any clip planes
    ClearClipPlanes();

    double eyeMult = 1.0 / (plot3Dbase.eyeSeparationDivisor * 2.0);
    if (leftSide)
    {
        glDrawBuffer(GL_BACK_LEFT);
        PerspectiveSetup(eyeMult);
    }
    else
    {
        glDrawBuffer(GL_BACK_RIGHT);
        PerspectiveSetup(-eyeMult);
    }
    ClearBuffer();

    InitLighting();
    inAnnoView = false;
}

void  OGL3DBase::InitLighting()
{
    nextLight = 0;
    PC_Lighting& currLights = plot3Dbase.plotLighting;


    if (!currLights.useLighting)
    {
        //  no lighting
        glDisable(GL_LIGHTING);
        glDisable(GL_NORMALIZE);
        glDisable(GL_LIGHT0);
        return;
    }

    SC_ColorSpec penColor = plot3Dbase.defaultPenSet->GetColor(currLights.ambientPen);

    //  set basic ambient
    GLfloat ambientLight[4];
    ambientLight[0] = (GLfloat) (penColor.RH * currLights.ambientIntensity);
    ambientLight[1] = (GLfloat) (penColor.GS * currLights.ambientIntensity);
    ambientLight[2] = (GLfloat) (penColor.BV * currLights.ambientIntensity);
    ambientLight[3] = (GLfloat) currLights.ambientLightAlpha;

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    if (!currLights.useDefaultLight)
    {
        glDisable(GL_LIGHT0);
        return;
    }

    // set up default light
    penColor = plot3Dbase.defaultPenSet->GetColor(currLights.defaultDiffuseLightPen);
    GLfloat diffuseLight[4];
    diffuseLight[0] = (GLfloat) (penColor.RH * currLights.defaultDiffuseLightIntensity);
    diffuseLight[1] = (GLfloat) (penColor.GS * currLights.defaultDiffuseLightIntensity);
    diffuseLight[2] = (GLfloat) (penColor.BV * currLights.defaultDiffuseLightIntensity);
    diffuseLight[3] = (GLfloat) currLights.defaultDiffuseLightAlpha;
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

    penColor = plot3Dbase.defaultPenSet->GetColor(currLights.defaultSpecularLightPen);
    GLfloat specularLight[4];
    specularLight[0] = (GLfloat) (penColor.RH * currLights.defaultSpecularLightIntensity);
    specularLight[1] = (GLfloat) (penColor.GS * currLights.defaultSpecularLightIntensity);
    specularLight[2] = (GLfloat) (penColor.BV * currLights.defaultSpecularLightIntensity);
    specularLight[3] = (GLfloat) currLights.defaultSpecularLightAlpha;
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    double elev, azimuth;

    if (currLights.defaultLightIsFixed)
    {
        elev = currLights.fixedElevation;
        azimuth = currLights.fixedAzimuth;
    }
    else
    {
        elev = currView.elevation + currLights.relativeElevation;
        azimuth = currView.azimuth + currLights.relativeAzimuth;
    }
    elev = Radians(elev);
    azimuth = Radians(azimuth - 180.0);

    GLfloat defaultPosition[4];
    defaultPosition[0] = (GLfloat) (sin(azimuth) * cos(elev));
    defaultPosition[1] = (GLfloat) (cos(azimuth) * cos(elev));
    defaultPosition[2] = (GLfloat) sin(elev);
    defaultPosition[3] = (GLfloat) currLights.defaultLightW;
    glLightfv(GL_LIGHT0, GL_POSITION, defaultPosition);


    if (currLights.showDefaultLight)
    {
        double defX = defaultPosition[0];
        double defY = defaultPosition[1];
        double defZ = defaultPosition[2];
        if (currLights.defaultLightW > 0.01)
        {
            defX /= currLights.defaultLightW;
            defY /= currLights.defaultLightW;
            defZ /= currLights.defaultLightW;
        }

        static const double dl = 0.025;

        glColor3d(1.0, 0.0, 0.0);
        glLineWidth(3);
        glBegin(GL_LINES);
            glVertex4d(0.0, 0.0, 0.0, 1.0);
            glVertex4d(GLdouble(defX ), GLdouble(defY), GLdouble(defZ), currLights.defaultLightW);
        glEnd();

        glColor3d(0.0, 0.0, 0.0);
        glBegin(GL_LINE_LOOP);
            glVertex3d(defX - dl, defY- dl, defZ- dl);
            glVertex3d(defX - dl, defY+ dl, defZ- dl);
            glVertex3d(defX + dl, defY+ dl, defZ- dl);
            glVertex3d(defX + dl, defY- dl, defZ- dl);
        glEnd();

        glBegin(GL_LINE_LOOP);
            glVertex3d(defX - dl, defY- dl, defZ+ dl);
            glVertex3d(defX - dl, defY+ dl, defZ+ dl);
            glVertex3d(defX + dl, defY+ dl, defZ+ dl);
            glVertex3d(defX + dl, defY- dl, defZ+ dl);
        glEnd();

        glBegin(GL_LINES);
            glVertex3d(defX - dl, defY- dl, defZ- dl);
            glVertex3d(defX - dl, defY- dl, defZ+ dl);

            glVertex3d(defX - dl, defY+ dl, defZ- dl);
            glVertex3d(defX - dl, defY+ dl, defZ+ dl);

            glVertex3d(defX + dl, defY- dl, defZ- dl);
            glVertex3d(defX + dl, defY- dl, defZ+ dl);

            glVertex3d(defX + dl, defY+ dl, defZ- dl);
            glVertex3d(defX + dl, defY+ dl, defZ+ dl);
        glEnd();

    }

    glEnable(GL_LIGHT0);

    GLfloat mat_specular[]  = {0.2F, 0.2F, 0.2F, 1.0F};
    GLfloat mat_shininess[] = {50.0F};

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

    nextLight++;
}

void OGL3DBase::StartTransparency()
{
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    doTransparency = true;
}

Coord3D OGL3DBase::GetNormalizedCoord(const Coord3D& worldCoord)
{
    return (worldCoord - normMin) / normSpan;
}
void OGL3DBase::SetTransFactor(const double& transFactor)
{
    currTransFactor = transFactor;
}

void OGL3DBase::EndTransparency()
{
    glDisable (GL_BLEND);
    doTransparency = false;
}


void OGL3DBase::SetLineAntiAlias()
{
    // line anti alias
    if (PS_OGLBase::Get3DAntiAliasLines() && (!DoingHardCopy()))
    {
        glEnable(GL_LINE_SMOOTH);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }

}

void OGL3DBase::NormalizeSpans()
{
    //  normalize to min length = 1.0
    double minSpan = xOrthoSpan;
    if (yOrthoSpan < minSpan)
        minSpan = yOrthoSpan;
    if (zOrthoSpan < minSpan)
        minSpan = zOrthoSpan;

    //  normalize
    xOrthoSpan /= minSpan;
    yOrthoSpan /= minSpan;
    zOrthoSpan /= minSpan;
}


void  OGL3DBase::GetTransformedLimits(Coord3D& minLim,
                                     Coord3D& maxLim) const
{
    maxLim = currView.viewLimits.maxLim;
    minLim = currView.viewLimits.minLim;
    TransformForAxes(maxLim);
    TransformForAxes(minLim);
}


Coord3D OGL3DBase::GetPixelComponents(int nPixels) const
{
    double maxMap = sqrt(xOrthoSpan * xOrthoSpan + yOrthoSpan * yOrthoSpan + zOrthoSpan * zOrthoSpan);

    // max diag of window
    int uVal = plotBase.plotUwidth;
    int vVal = plotBase.plotVheight;
    double windowDiag = sqrt(double(uVal * uVal) + double(vVal * vVal));
    double pixelsPerAxes =  maxMap / windowDiag * double(nPixels);

    //  set output
    Coord3D outCoord(pixelsPerAxes);

    // adjust for axes scaling distortions
    Coord3D minCoord, maxCoord;
    GetTransformedLimits(minCoord, maxCoord);

    outCoord *= (maxCoord - minCoord);

    // adjust for axes length differences
    outCoord.cX /= xOrthoSpan;
    outCoord.cY /= yOrthoSpan;
    outCoord.cZ /= zOrthoSpan;

    return outCoord;
}



// scaling for axes independent data like symbols and stroke fonts
Coord3D OGL3DBase::GetIndependentScale(const double& inSize) const
{
    if (inAnnoView)
    {
        return Coord3D(xPerPixel * inSize, yPerPixel * inSize, 1.0);
    }

    // compensate for window size - base is 640x480
    int ixAvail, iyAvail;
    plotBase.CalcAvailablePixels(ixAvail, iyAvail);

    double xAvail = double(ixAvail);
    double yAvail = double(iyAvail);
    double winDiag = 800.0 / sqrt(xAvail * xAvail  +  yAvail * yAvail);

    double maxSpan = sqrt(xOrthoSpan * xOrthoSpan + yOrthoSpan * yOrthoSpan + zOrthoSpan * zOrthoSpan);

    Coord3D scale(winDiag * maxSpan * inSize);

    Limit3D lim = currView.viewLimits;
    TransformForAxes(lim.minLim);
    TransformForAxes(lim.maxLim);

    scale *= (lim.maxLim - lim.minLim);

    double currScale = currView.scale;

    if (!plot3Dbase.ProjectionIsOrtho())
//        currScale = 90.0 / plot3Dbase.fieldOfView;
            currScale = 1.0;

    scale /= currScale;

    scale.cX /= xOrthoSpan;
    scale.cY /= yOrthoSpan;
    scale.cZ /= zOrthoSpan;

    return scale;
}



void  OGL3DBase::Axes3DLabel(const PSC_Font&               inFont,
                                 const Coord3D&                xyz,
                                 const double&                 xOffsetChars,
                                 const double&                 yOffsetChars,
                                 const double&                 rotation,
                                       HorizontalTextAlignment halign,
                                       VerticalTextAlignment   valign,
                                       Plane3D                 labelPlane,
                                       bool                    mirrorPlane,
                                const  char                    str[])
{

    glPushMatrix();
    glTranslated(xyz.cX, xyz.cY, xyz.cZ);

    SetFontScale(inFont);

    switch (labelPlane)
    {
        case p3D_XY : {
            break;
        }
        case p3D_XZ : {
            glRotated(90, 1.0, 0.0, 0.0);
            break;
        }
        case p3D_YZ : {
            glRotated(90, 0.0, 1.0, 0.0);
            glRotated(90, 0.0, 0.0, 1.0);
            break;
        }
    }
    if (mirrorPlane)
        glRotated(180, 0.0, 1.0, 0.0);
    if (fabs(rotation) > 0.01)
        glRotated(rotation, 0.0, 0.0, 1.0);

    Do3DLabel(inFont, xOffsetChars, yOffsetChars, halign, valign, str);

    glPopMatrix();
}


                            // for data labelling
void  OGL3DBase::Data3DLabel(const PSC_Font&               inFont,
                                 const Coord3D&                xyz,
                                 const double&                 xyRotation,
                                       HorizontalTextAlignment halign,
                                       VerticalTextAlignment   valign,
                                       bool                    isVertical,
                                 const char                    str[])
{
    glPushMatrix();

        glTranslated(xyz.cX, xyz.cY, xyz.cZ);

        SetFontScale(inFont);

        if (fabs(xyRotation) > 0.01)
            glRotated(xyRotation, 0.0, 0.0, 1.0);

        if (isVertical)
            glRotated(90, 1.0, 0.0, 0.0);

        Do3DLabel(inFont, 0.0, 0.0, halign, valign, str);


    glPopMatrix();
}

void OGL3DBase::SetFontScale(const PSC_Font& inFont)
{
    static const double fontSc = 250.0;  // magic number -- seems to work OK

    Coord3D scale =  GetIndependentScale(inFont.fontSize);

    scale.cX /= fontSc;
    scale.cY /= fontSc;
    scale.cZ /= fontSc;

    glScaled(scale.cX, scale.cY, scale.cZ);
}

void  OGL3DBase::GetLabelSpecs(const PSC_Font&    inFont,
                             const char*        inLab,
                                       double&      overTop,
                                       double&      width)
{
    double lineMult = GetDefaultLineMultiplier();
    ProcessedString procStr(lineMult);
    if ((!procStr.Process3DString(inFont, inLab)) || (!procStr.IsPrintable()))
    {
        overTop = 0.0;
        width = 0.0;
        return;
    }

    procStr.SetBasicXY(1.0, 1.0, 1.0, true);

    double ySize;
    procStr.GetSizeInfo(width, ySize);

    double belowBase;
    procStr.GetSizeDeviation(overTop, belowBase);
}



void OGL3DBase::Do3DLabel(const PSC_Font&               inFont,
                          const double&                 xOffsetChars,
                          const double&                 yOffsetChars,
                                HorizontalTextAlignment halign,
                                VerticalTextAlignment   valign,
                                const char              str[])
{

    double lineMult = GetDefaultLineMultiplier();
    ProcessedString procStr(lineMult);
    if ((!procStr.Process3DString(inFont, str)) || (!procStr.IsPrintable()))
        return;

    procStr.SetBasicXY(1.0, 1.0, 1.0, true);

    double xSize, ySize;
    procStr.GetSizeInfo(xSize, ySize);

    glTranslated(xOffsetChars, yOffsetChars, 0.0);

    if (halign != hta_Left)
    {
        double strX = xSize;
        if (halign == hta_Center)
            strX /= 2;
        glTranslated(double(-strX), 0.0, 0.0);
    }

    if (valign != vta_Top)
    {
        double strY = ySize;
        if (valign == vta_Center)
            strY /= 2.0;
        glTranslated(0.0, strY, 0.0);
    }

    procStr.SetFinalXY(0.0, 0.0, halign);

    procStr.Print3D(*this, plot3Dbase.plotLighting.useLighting);

}

// axes drawing support

void  OGL3DBase::TransformIncrements(bool      axisIsLog,
               const SC_DoubleArray& inIncs,
          SC_DoubleArray&    tranIncs)
{
    tranIncs = inIncs;
    if (axisIsLog)
        for (int i = 0; i < tranIncs.Size(); i++)
            tranIncs[i] = log10(inIncs[i]);
}


void  OGL3DBase::DrawOneAxes( const double&         axMin,
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
                              const double&         zValue,
                                    Plane3D         axPlane,
                                    bool            axisIsLog)
{

    Point2D* endPoint;
    double*  endComponent;

    PC_3DAxesFormat&  axesFormat = plot3Dbase.axesFormat;
    DC_PenSet&        penSet   = *plot3Dbase.defaultPenSet;          //  default PenSet used for plot

    if (format.axesLinePos == PC_AxesFormat::alpBoth)
    {
        SetDrawColor(penSet.GetColor(axesFormat.axesLinePen));
        SetLine(axesFormat.axesLineWidth);
        SetLineSolid();

        HardCopyBlockStart(6);

        stComponent = axMin;
        Point2D axSt = stPoint;
        stComponent = axMax;
        Point2D axEnd = stPoint;
        DrawLine(axSt, axEnd, zValue, axPlane);

        //  needed at axes end to make offsets look clean
        endPoint = &offsetEndPoint;
        endComponent = &offsetComponent;

        stComponent = axMin;
        *endComponent = stComponent;
        DrawLine(stPoint, *endPoint, zValue, axPlane);

        stComponent = axMax;
        *endComponent = stComponent;
        DrawLine(stPoint, *endPoint, zValue, axPlane);

        HardCopyBlockEnd();
    }


    if ((format.axesMajorInc == PC_Axes::aitGrid) ||
        ((format.axesMajorInc == PC_Axes::aitTic) && (format.axesTicPos == PC_AxesFormat::atpBoth)))
    {
        SetDrawColor(penSet.GetColor(axesFormat.majorPen));
        if (format.axesMajorInc == PC_Axes::aitTic)
        {
            SetLine(axesFormat.majorTicWidth);
            endPoint = &majTicEndPoint;
            endComponent = &majTicComponent;
        }
        else
        {
            SetLine(axesFormat.majorGridWidth);
            SetLineType(axesFormat.majorGridLineType);
            endPoint = &gridEndPoint;
            endComponent = &gridComponent;
        }
        HardCopyBlockStart(majorIncs.Size() * 2);
        for (int i = 0; i < majorIncs.Size(); i++)
        {
            stComponent = majorIncs[i];
            if (Limit::WithinOneLimit(axMin, axMax, stComponent))
            {
                *endComponent = stComponent;
                DrawLine(stPoint, *endPoint, zValue, axPlane);
            }
        }
        HardCopyBlockEnd();
    }



    if ((format.axesMinorInc == PC_Axes::aitGrid) ||
        ((format.axesMinorInc == PC_Axes::aitTic) && (format.axesTicPos == PC_AxesFormat::atpBoth)))
    {
        SetDrawColor(penSet.GetColor(axesFormat.minorPen));
        if (format.axesMinorInc == PC_Axes::aitTic)
        {
            SetLine(axesFormat.minorTicWidth);
            endPoint = &minTicEndPoint;
            endComponent = &minTicComponent;
        }
        else
        {
            SetLine(axesFormat.minorGridWidth);
            SetLineType(axesFormat.minorGridLineType);

            endPoint = &gridEndPoint;
            endComponent = &gridComponent;
        }


        HardCopyBlockStart((majorIncs.Size() + 1) * minorIncs.Size()  * 2);

        if (!axisIsLog)
        {
            double multVal = 1.0;
            if (axMin > axMax )
                multVal = -1.0;

            for (int i = -1; i < majorIncs.Size(); i++)
                for (int j = 0; j < minorIncs.Size(); j++)
                {
                    if (i < 0)
                        stComponent = majorIncs[0] - (minorIncs[j] * multVal);
                    else
                        stComponent = majorIncs[i] + (minorIncs[j] * multVal);

                    if (Limit::WithinOneLimit(axMin, axMax, stComponent))
                    {
                        *endComponent = stComponent;
                        DrawLine(stPoint, *endPoint, zValue, axPlane);
                    }
                }
        }
        else
        {
            //
            static const double logIncVals[] = {0.30103, 0.47712, 0.60206, 0.69897,
                                                0.77815, 0.84510, 0.90309, 0.95424};
            if (axMin < axMax)
            {
                for (int i = -1; i < majorIncs.Size(); i++)
                    for (int j = 0; j < 8; j++)
                    {
                        if (i < 0)
                            stComponent = majorIncs[0] - 1.0 + logIncVals[j];
                        else
                            stComponent = majorIncs[i] + logIncVals[j];

                        if (Limit::WithinOneLimit(axMin, axMax, stComponent))
                        {
                            *endComponent = stComponent;
                            DrawLine(stPoint, *endPoint, zValue, axPlane);
                        }
                    }
            }
            else
            {
                for (int i = -1; i < majorIncs.Size(); i++)
                    for (int j = 0; j < 8; j++)
                    {
                        if (i < 0)
                            stComponent = majorIncs[0] + logIncVals[j];
                        else
                            stComponent = majorIncs[i] - 1.0 + logIncVals[j];

                        if (Limit::WithinOneLimit(axMin, axMax, stComponent))
                        {
                            *endComponent = stComponent;
                            DrawLine(stPoint, *endPoint, zValue, axPlane);
                        }
                    }
            }

        }
        HardCopyBlockEnd();

    }
}

void  OGL3DBase::DrawAxesPlane(const SC_DoubleArray& xMajorIncs,
                               const SC_DoubleArray& xMinorIncs,
                                    const SC_DoubleArray& yMajorIncs,
                                    const SC_DoubleArray& yMinorIncs,
                                    const Limit2D&        xyLimits,
                                    const double&         zValue,
                                          PC_3DAxesFormat::Axes3DPlanes axPlane,
                                          bool             xIsLog,
                                          bool             yIsLog)
{
    Plane3D drawPlane = PC_3DAxesFormat::GetPlane(axPlane);

    PC_3DAxesFormat&  axesFormat = plot3Dbase.axesFormat;

    Coord3D offComp = GetPixelComponents(axesFormat.axesOffset);
    offComp.ToModelPlane(drawPlane);
    Coord3D majTicComp = GetPixelComponents(axesFormat.majorTicLength);
    majTicComp.ToModelPlane(drawPlane);
    Coord3D minTicComp = GetPixelComponents(axesFormat.minorTicLength);
    minTicComp.ToModelPlane(drawPlane);


    //  bottom x
    Point2D stPoint, majTicEnd, minTicEnd,  gridEnd, offsetEnd;

    stPoint.pY = xyLimits.minLim.pY - offComp.cY;
    majTicEnd.pY = stPoint.pY + majTicComp.cY;
    minTicEnd.pY = stPoint.pY + minTicComp.cY;
    gridEnd.pY = xyLimits.maxLim.pY + offComp.cY;
    offsetEnd.pY = xyLimits.minLim.pY;

    PC_AxesFormat wCopy;
    axesFormat.axesUFormat[axPlane].SetWorkingCopy(wCopy, true);
    DrawOneAxes(xyLimits.minLim.pX, xyLimits.maxLim.pX,
                xMajorIncs, xMinorIncs, wCopy,
                stPoint, majTicEnd, minTicEnd, gridEnd, offsetEnd,
                stPoint.pX,majTicEnd.pX, minTicEnd.pX, gridEnd.pX, offsetEnd.pX, zValue, drawPlane, xIsLog);

    // top X
    stPoint.pY   = xyLimits.maxLim.pY + offComp.cY;
    majTicEnd.pY = stPoint.pY - majTicComp.cY;
    minTicEnd.pY = stPoint.pY - minTicComp.cY;
    offsetEnd.pY = xyLimits.maxLim.pY;

    axesFormat.axesUFormat[axPlane].SetWorkingCopy(wCopy, false);
    DrawOneAxes(xyLimits.minLim.pX, xyLimits.maxLim.pX,
                xMajorIncs, xMinorIncs, wCopy,
                stPoint, majTicEnd, minTicEnd, gridEnd, offsetEnd,
                stPoint.pX, majTicEnd.pX, minTicEnd.pX, gridEnd.pX, offsetEnd.pX, zValue, drawPlane, xIsLog);

    //  left y
    stPoint.pX = xyLimits.minLim.pX - offComp.cX;
    majTicEnd.pX = stPoint.pX + majTicComp.cX;
    minTicEnd.pX = stPoint.pX + minTicComp.cX;
    gridEnd.pX = xyLimits.maxLim.pX + offComp.cX;
    offsetEnd.pX = xyLimits.minLim.pX;

    axesFormat.axesVFormat[axPlane].SetWorkingCopy(wCopy, true);
    DrawOneAxes(xyLimits.minLim.pY, xyLimits.maxLim.pY,
                yMajorIncs, yMinorIncs, wCopy,
                stPoint, majTicEnd, minTicEnd, gridEnd, offsetEnd,
                stPoint.pY, majTicEnd.pY, minTicEnd.pY, gridEnd.pY, offsetEnd.pY, zValue, drawPlane, yIsLog);

    // right Y
    stPoint.pX = xyLimits.maxLim.pX + offComp.cX;
    majTicEnd.pX = stPoint.pX - majTicComp.cX;
    minTicEnd.pX = stPoint.pX - minTicComp.cX;
    offsetEnd.pX = xyLimits.maxLim.pX;

    axesFormat.axesVFormat[axPlane].SetWorkingCopy(wCopy, false);
    DrawOneAxes(xyLimits.minLim.pY, xyLimits.maxLim.pY,
                yMajorIncs, yMinorIncs, wCopy,
                stPoint, majTicEnd, minTicEnd, gridEnd, offsetEnd,
                stPoint.pY, majTicEnd.pY, minTicEnd.pY, gridEnd.pY, offsetEnd.pY ,zValue, drawPlane, yIsLog);
}


void OGL3DBase::DrawAxesLabels(const SC_DoubleArray& xMajorIncs,
                                const SC_DoubleArray& xtranMajorIncs,
                                const SC_DoubleArray& yMajorIncs,
                                const SC_DoubleArray& ytranMajorIncs,
                                const SC_DoubleArray& zMajorIncs,
                                const SC_DoubleArray& ztranMajorIncs,
                                const SC_RealFormat&     xFormat,
                                const SC_RealFormat&     yFormat,
                                const SC_RealFormat&     zFormat)
{
    PC_3DAxesLabel&      axesLabel  = plot3Dbase.axesLabel;
    if (!(axesLabel.plotIncrementLabel || axesLabel.plotAxesLabel))
        return;

    PC_3DAxesFormat&     axesFormat = plot3Dbase.axesFormat;


    if (axesLabel.autoPositionLabels)
    {
        axesLabel.xyLabIsVert = (fabs(currView.elevation) < 45.0);
        axesLabel.xLabYPosIsMax = (fabs(currView.azimuth) >= 90.0);
        axesLabel.yLabXPosIsMax = (currView.azimuth < 0.0);
        axesLabel.zLabYPosIsMax = !axesLabel.yLabXPosIsMax;
        axesLabel.zLabXPosIsMax = axesLabel.xLabYPosIsMax;

        if (fabs(currView.azimuth) >= 135.0)
            axesLabel.zLabOrientation = PC_3DAxesLabel::zloXZr;
        else if (currView.azimuth < -45.0)
            axesLabel.zLabOrientation = PC_3DAxesLabel::zloYZn;
        else if (currView.azimuth < 45.0)
            axesLabel.zLabOrientation = PC_3DAxesLabel::zloXZn;
        else
            axesLabel.zLabOrientation = PC_3DAxesLabel::zloYZr;
    }


    SetDrawColor(plot3Dbase.defaultPenSet->GetColor(axesFormat.majorPen));
    double fontRatio = double(axesLabel.incrementFont.fontSize) / double(axesLabel.labelFont.fontSize);

    char labStr[80];
    Coord3D labLoc;
    int i;
    bool adjSt, adjEnd;

    Coord3D minLim, maxLim;
    GetTransformedLimits(minLim, maxLim);
    Coord3D offComp = GetPixelComponents(axesFormat.axesOffset);

    //  x axis first
    SetAlignmentAdjust(xtranMajorIncs, minLim.cX, maxLim.cX, adjSt, adjEnd);
    labLoc = Coord3D(0.0, minLim.cY - offComp.cY, minLim.cZ - offComp.cZ);
    if (axesLabel.xLabYPosIsMax)
        labLoc.cY = maxLim.cY + offComp.cY;

    double rotVal = 0.0;
    if ((!axesLabel.xyLabIsVert) && axesLabel.xLabYPosIsMax)
        rotVal = 180.0;
    bool mirror = (axesLabel.xyLabIsVert) && axesLabel.xLabYPosIsMax;
    Plane3D txtPlane = p3D_XY;
    if (axesLabel.xyLabIsVert)
        txtPlane = p3D_XZ;
    HorizontalTextAlignment halign = hta_Center;
    if (adjSt)
        if (axesLabel.xLabYPosIsMax)
            halign = hta_Right;
        else
            halign = hta_Left;

    bool xyExponents = (xFormat.format > ncf_Scientific) || (yFormat.format > ncf_Scientific);
    double xyIncOffset = -0.25;
    if (xyExponents)
        xyIncOffset = -0.40;

    if (axesLabel.plotIncrementLabel)
    {
        for (i = 0; i < xMajorIncs.Size(); i++)
        {
            labLoc.cX = xtranMajorIncs[i];
            if (Limit::WithinOneLimit(minLim.cX, maxLim.cX, labLoc.cX))
            {
                xFormat.RealToString(xMajorIncs[i], labStr, 80);
                if (adjEnd && (i == (xMajorIncs.Size() - 1)))
                    if (axesLabel.xLabYPosIsMax)
                        halign = hta_Left;
                    else
                        halign = hta_Right;

                Axes3DLabel(axesLabel.incrementFont, labLoc,
                                    0.0, xyIncOffset, rotVal, halign, vta_Top, txtPlane, mirror, labStr);
            }
            halign = hta_Center;
        }
    }

//  need to check both X&Y here
    double xyAxOffset = xyIncOffset - 1.15;

    if (axesLabel.plotAxesLabel)
    {
        if (!axesLabel.plotIncrementLabel)
            xyAxOffset = -.25;
        xyAxOffset *= fontRatio;

        double labWidth, xoverTop, yoverTop;
        GetLabelSpecs(axesLabel.labelFont, axesLabel.xaxesLabel, xoverTop, labWidth);
        GetLabelSpecs(axesLabel.labelFont, axesLabel.yaxesLabel, yoverTop, labWidth);

        if (xoverTop > yoverTop)
            xyAxOffset -= xoverTop;
        else
            xyAxOffset -= yoverTop;
    }


    if (axesLabel.plotAxesLabel && (StringLength(axesLabel.xaxesLabel) > 0))
    {
        labLoc.cX = (minLim.cX + maxLim.cX) / 2.0;
        Axes3DLabel(axesLabel.labelFont, labLoc, 0.0, xyAxOffset,
                            rotVal, hta_Center, vta_Top, txtPlane, mirror, axesLabel.xaxesLabel);
    }

    //  y axes
    SetAlignmentAdjust(ytranMajorIncs, minLim.cY, maxLim.cY, adjSt, adjEnd);
    labLoc = Coord3D(minLim.cX - offComp.cX, 0.0, minLim.cZ - offComp.cZ);

    if (axesLabel.yLabXPosIsMax)
        labLoc.cX = maxLim.cX + offComp.cX;

    rotVal = 0.0;
    if (axesLabel.xyLabIsVert)
        txtPlane = p3D_YZ;
    else
        if (axesLabel.yLabXPosIsMax)
            rotVal = 90.0;
        else
           rotVal = -90.0;

    mirror = (axesLabel.xyLabIsVert) && (!axesLabel.yLabXPosIsMax);
    halign = hta_Center;
    if (adjSt)
        if (axesLabel.yLabXPosIsMax)
            halign = hta_Left;
        else
            halign = hta_Right;

    if (axesLabel.plotIncrementLabel)
    {
        for (i = 0; i < yMajorIncs.Size(); i++)
        {
            labLoc.cY = ytranMajorIncs[i];
            if (Limit::WithinOneLimit(minLim.cY, maxLim.cY, labLoc.cY))
            {
                yFormat.RealToString(yMajorIncs[i], labStr, 80);
                if (adjEnd && (i == (yMajorIncs.Size() - 1)))
                    if (axesLabel.yLabXPosIsMax)
                        halign = hta_Right;
                    else
                        halign = hta_Left;

                Axes3DLabel(axesLabel.incrementFont, labLoc,
                                    0.0, xyIncOffset, rotVal, halign, vta_Top, txtPlane, mirror, labStr);
            }
            halign = hta_Center;
        }
    }

    if (axesLabel.plotAxesLabel && (StringLength(axesLabel.yaxesLabel) > 0))
    {
        labLoc.cY = (minLim.cY + maxLim.cY) / 2.0;
        double fontOffset = -1.50;
        if (!axesLabel.plotIncrementLabel)
            fontOffset = -.25;

        Axes3DLabel(axesLabel.labelFont, labLoc,0.0, xyAxOffset,
                    rotVal, hta_Center, vta_Top, txtPlane, mirror, axesLabel.yaxesLabel);
    }

    //  zaxes
    bool doLeft = false;
    switch (axesLabel.zLabOrientation)
    {
        case PC_3DAxesLabel::zloXZn : {
            txtPlane = p3D_XZ;
            mirror = false;
            doLeft = axesLabel.zLabXPosIsMax;
            break;
        }
        case PC_3DAxesLabel::zloXZr : {
            txtPlane = p3D_XZ;
            mirror = true;
            doLeft = !axesLabel.zLabXPosIsMax;
            break;
        }
        case PC_3DAxesLabel::zloYZn : {
            txtPlane = p3D_YZ;
            mirror = false;
            doLeft = axesLabel.zLabYPosIsMax;
            break;
        }
        case PC_3DAxesLabel::zloYZr : {
            txtPlane = p3D_YZ;
            mirror = true;
            doLeft = !axesLabel.zLabYPosIsMax;
            break;
        }
    }

    double offsetMult = 1.0;
    if (doLeft)
        halign = hta_Left;
    else
    {
        halign = hta_Right;
        offsetMult = -1.0;
    }

    if (axesLabel.zLabYPosIsMax)
        labLoc.cY = maxLim.cY + offComp.cY;
    else
        labLoc.cY = minLim.cY - offComp.cY;

    if (axesLabel.zLabXPosIsMax)
        labLoc.cX = maxLim.cX  + offComp.cX;
    else
        labLoc.cX = minLim.cX  - offComp.cX;


    double zAxOffset = 0.0;
    if (axesLabel.plotIncrementLabel)
    {
        for (i = 0; i < zMajorIncs.Size(); i++)
        {
            labLoc.cZ = ztranMajorIncs[i];
            if (Limit::WithinOneLimit(minLim.cZ, maxLim.cZ, labLoc.cZ))
            {
                zFormat.RealToString(zMajorIncs[i], labStr, 80);
                double labLen, overTop;
                GetLabelSpecs(axesLabel.incrementFont, labStr, overTop, labLen);
                if (labLen > zAxOffset)
                    zAxOffset = labLen;

                Axes3DLabel(axesLabel.incrementFont, labLoc,
                                    0.5 * offsetMult, 0.0, 0.0, halign, vta_Center, txtPlane, mirror, labStr);
            }
        }
    }

    if (axesLabel.plotAxesLabel && (StringLength(axesLabel.zaxesLabel) > 0))
    {
        labLoc.cZ = (minLim.cZ + maxLim.cZ) / 2.0;
        double fontOffset = - (zAxOffset + 0.75) * fontRatio * offsetMult;

        VerticalTextAlignment valign = vta_Bottom;
        if (fontOffset < 0.0)
            valign = vta_Top;

        Axes3DLabel(axesLabel.labelFont, labLoc, 0.0, fontOffset,
                             90.0, hta_Center, valign, txtPlane, mirror, axesLabel.zaxesLabel);
    }
}


void OGL3DBase::SetAlignmentAdjust(const SC_DoubleArray& majorIncs,
                                        const double&         axMin,
                                        const double&         axMax,
                                               bool&           adjMin,
                                               bool&           adjMax)
{
    double dax = fabs(axMin - axMax);
    //
    adjMin = false;
    adjMax = false;

//  ajustment is more pain than its worth -- will make option someday
//    adjMin = (fabs(axMin - majorIncs[0]) / dax < 0.1);
//    adjMax = (fabs(axMax - majorIncs.LastIndex()) / dax < 0.1);
}

void  OGL3DBase::SetAutoView(PC_AxesFormat& xynUorig,
                             PC_AxesFormat& xynVorig)
{
    PC_3DAxesFormat&     axesFormat = plot3Dbase.axesFormat;

    xynUorig = axesFormat.axesUFormat[PC_3DAxesFormat::a3pXYn];
    xynVorig = axesFormat.axesVFormat[PC_3DAxesFormat::a3pXYn];
    if (!axesFormat.autoFormat)
        return;

    axesFormat.Clear();
    PC_AxesFormat newU = xynUorig;
    PC_AxesFormat newV = xynVorig;
    newU.axesLinePos = PC_AxesFormat::alpBoth;
    newV.axesLinePos = PC_AxesFormat::alpBoth;
    newU.axesTicPos = PC_AxesFormat::atpBoth;
    newV.axesTicPos = PC_AxesFormat::atpBoth;

    // XY plane
    PC_3DAxesFormat::Axes3DPlanes autoPlane;
    if (currView.elevation < 0.0)
        autoPlane = PC_3DAxesFormat::a3pXYp;
    else
        autoPlane = PC_3DAxesFormat::a3pXYn;
    axesFormat.axesUFormat[autoPlane] = newU;
    axesFormat.axesVFormat[autoPlane] = newV;

    // YZ plane
    if (currView.azimuth < 0.0)
        autoPlane = PC_3DAxesFormat::a3pYZn;
    else
        autoPlane = PC_3DAxesFormat::a3pYZp;
    axesFormat.axesUFormat[autoPlane] = newU;
    axesFormat.axesVFormat[autoPlane] = newV;

    // XZ plane
    if (fabs(currView.azimuth)> 90.0)
        autoPlane = PC_3DAxesFormat::a3pXZn;
    else
        autoPlane = PC_3DAxesFormat::a3pXZp;
    axesFormat.axesUFormat[autoPlane] = newU;
    axesFormat.axesVFormat[autoPlane] = newV;
}

Coord3D OGL3DBase::GetExtrusionScale(bool pixelSizing) const
{
    // pixel sizing assumed
    Coord3D minLim, maxLim;
    GetTransformedLimits(minLim, maxLim);
    Coord3D deltaLim = maxLim - minLim;

    return Coord3D(xOrthoSpan / deltaLim.cX,yOrthoSpan / deltaLim.cY, zOrthoSpan / deltaLim.cZ);
}


