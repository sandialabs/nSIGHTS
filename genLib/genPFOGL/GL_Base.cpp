///////////////////////////////////////////////////////////////////////////////////
//
// GL_Base.cpp
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
//      Implements renderer dependent code to actually draw genPFO functional
// objects on a plot.  Is platform independent but
// relies on platform dependent code in genPlotClassPS.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include <iostream>

#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <genPlotClass/C_PlotDef.h>

#include <genPFOGL/GL_Base.h>


PlotDefC*   GL_Base::defAssocPlot = 0;  // associated plot definition - used mostly for pen extraction
OGLBase*    GL_Base::defAssocGL   = 0;  // OpenGL object - required for hardcopy
OGL2DBase*  GL_Base::assoc2DGL    = 0;  // 2D object used to get z val for 2D plotting
OGL3DBase*  GL_Base::assoc3DGL    = 0;  // 3D object used to get lighting info

const double GL_Base::polyOffsetEps = 1.0E-09;
double       GL_Base::polyOffsetBias = 1.0E-07;

Coord3D      GL_Base::offsetCoord;
Coord3D      GL_Base::normalCoords[3];
SC_ColorSpec GL_Base::currColor;

bool         GL_Base::axesReversed = false;


void GL_Base::SetDataLayer(int layer)
{
    assoc2DGL->SetDataLayer(layer);
}

void GL_Base::SetDataLayerOffset(int layer)
{
    assoc2DGL->SetDataLayerOffset(layer);
}

void GL_Base::SetOffsetCoord(const Coord3D& objOffsetCoord)
{
    if (assoc2DGL != 0)
        offsetCoord = Coord3D(0.0);
    else
        offsetCoord = objOffsetCoord;
}


bool  GL_Base::TransformAndOffset(Coord3D& untranCoord)
{
    bool retVal = defAssocGL->TransformForAxes(untranCoord);
    untranCoord += offsetCoord;
    return retVal;
}


void  GL_Base::SetColor(const SC_ColorSpec&  color)
{
    if (defAssocGL == 0)
        GenAppInternalError("GL_Base::SetColor-GL");

    defAssocGL->SetDrawColor(color);
    currColor = color;
}
void  GL_Base::SetDefaultObjectPlotDef(PlotDefC*    assPlot)
{
    defAssocPlot = assPlot;
}
void  GL_Base::SetDefaultObjectOGLBase(OGLBase*     assGL)
{
    defAssocGL = assGL;
    assoc2DGL = dynamic_cast<OGL2DBase*>(assGL);
    assoc3DGL = dynamic_cast<OGL3DBase*>(assGL);

    if (assoc3DGL)
    {
        const PC_View& currView = assoc3DGL->GetCurrentView();
        int nneg = 0;
        if (currView.viewLimits.Dx() < 0.0)
            nneg++;
        if (currView.viewLimits.Dy() < 0.0)
            nneg++;
        if (currView.viewLimits.Dz() < 0.0)
            nneg++;
        axesReversed = (nneg == 1) || (nneg == 3);
    }
}

void  GL_Base::SetColor(int penNumber)
{
    if (defAssocPlot == 0)
        GenAppInternalError("GL_Base::SetColor-PD");
    SetColor(defAssocPlot->defaultPenSet->GetColor(penNumber));
}

void  GL_Base::GetColor(SC_ColorSpec&  color)
{
    GLfloat getColor[4];
    glGetFloatv(GL_CURRENT_COLOR, getColor);
    color.RH = double(getColor[0]);
    color.GS = double(getColor[1]);
    color.BV = double(getColor[2]);
}

void GL_Base::Disable3Dlighting()
{
    if  (assoc3DGL && assoc3DGL->DoLightingCalcs())
        glDisable(GL_LIGHTING);
}

void  GL_Base::Restore3Dlighting()
{
    if  (assoc3DGL && assoc3DGL->DoLightingCalcs())
        glEnable(GL_LIGHTING);
}

void GL_Base::GetClearColor(SC_ColorSpec&  color)
{
    GLfloat clrColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clrColor);
    color.RH = double(clrColor[0]);
    color.GS = double(clrColor[1]);
    color.BV = double(clrColor[2]);
}

void GL_Base::SetPolyOffsetBias(const double& polyBias)
{
    polyOffsetBias = polyBias;

//  using namespace std;
//  cout << "GL poly bias: " << polyOffsetBias << endl;
}


void GL_Base::SetPolygonAsFilled(const double& polyOffset)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

// removed all GL version dependencies - only applicable to Unix platforms

/* #ifdef _GLVERSION10_
#ifdef _GLVERSION11_
    if (PS_OGLBase::IsVersion1_0())
    {
#endif


  * MS doesnt recognize any of EXT stuff
        //  assume extension for poly offset
        if (fabs(polyOffset) < polyOffsetEps)
            glDisable(GL_POLYGON_OFFSET_EXT);
        else
        {
            glEnable(GL_POLYGON_OFFSET_EXT);
            glPolygonOffsetEXT(polyOffset, polyOffsetBias);
        }

#ifdef _GLVERSION11_
    }
#endif
#endif

#ifdef _GLVERSION11_
    if (PS_OGLBase::IsVersion1_1())
    {
*/

    if (fabs(polyOffset) < polyOffsetEps)
        glDisable(GL_POLYGON_OFFSET_FILL);
    else
    {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset((GLfloat) polyOffset, 1.0);
    }


//  }
//#endif

}
void GL_Base::SetPolygonAsLine(      int     lineWidth,
                               const double& polyOffset)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (defAssocGL == 0)
        GenAppInternalError("GL_Polygon::SetPolygonLine");
    defAssocGL->SetLine(lineWidth);

//#ifdef _GLVERSION11_
//  if (PS_OGLBase::IsVersion1_1())
//  {
        if (fabs(polyOffset) < polyOffsetEps)
            glDisable(GL_POLYGON_OFFSET_LINE);
        else
        {
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset((GLfloat) polyOffset, 1.0);
        }
//  }
//#endif
}

void  GL_Base::GetAnnoPixelXY(const Point2D&    annoXY,
                                    int&        pixelX,
                                    int&        pixelY)
{
    defAssocPlot->AnnoXYtoPixelXY(annoXY, pixelX, pixelY);
}


#ifdef _GLPRHARDCOPY_
void GL_Base::StdHardCopyBlockStart(int nvertex)
{
    defAssocGL->HardCopyBlockStart(nvertex);
}

void  GL_Base::Std2DHardCopyBlockStart(int nvertex)
{
    if (defAssocGL->DoingHardCopy())
    {
        glpBegin(GLP_2D, nvertex);
        defAssocGL->SetHardCopyParameters();
    }
}
void GL_Base::HardCopyBlockEnd()
{
    defAssocGL->HardCopyBlockEnd();
}
#endif

void GL_Base::CalcNormals()
{
    Coord3D normVector;
    CalcNormals(normVector);
    glNormal3d(GLdouble(normVector.cX), GLdouble(normVector.cY), GLdouble(normVector.cZ));
}

void GL_Base::CalcNormals(Coord3D& normVector)
{
    // calc normal -- assume points are not collinear
    //commented out approach was approach previous to Mar06
    //this approach resulted in negative normals for YZ and XZ cases
    //current approach provides positive normals for all three planes
    //not big difference btwn approaches as far as I can see - NJC
    /*Coord3D v21 = normalCoords[0] - normalCoords[1];
    Coord3D v23 = normalCoords[2] - normalCoords[1];*/
    Coord3D v12 = normalCoords[0] - normalCoords[1];
    Coord3D v23 = normalCoords[1] - normalCoords[2];

    if (axesReversed)
    {
        double mMatrix[16];
        glGetDoublev(GL_MODELVIEW_MATRIX, mMatrix);

        int nneg = 0;
        if (mMatrix[0] < 0.0)
            nneg++;
        if (mMatrix[5] < 0.0)
            nneg++;
        if (mMatrix[10] < 0.0)
            nneg++;

        if ((nneg == 1) || (nneg == 3))
        {
            /*Coord3D temp = v21;
            v21 = v23;
            v23 = temp;*/
            Coord3D temp = v12;
            v12 = v23;
            v23 = temp;
        }
    }

    //normVector = CrossProduct(v23, v21);
    normVector = CrossProduct(v12, v23);
    normVector.Normalize();
}


void GL_Base::VMRLwriteShapePolyHeader(bool polyIsFilled)
{
    TxtFileC& currOut = defAssocGL->GetVRMLOutputFile();
    currOut.WriteText("Shape {");
    currOut.WriteSolidText("    geometry ");
    if (polyIsFilled)
        currOut.WriteText("IndexedFaceSet {");
    else
        currOut.WriteText("IndexedLineSet {");
    currOut.WriteText("        coord Coordinate {");
    currOut.WriteText("           point [");
    currOut.SetRealConvFormat(SC_RealFormat(ncf_Decimal, 5));
}


void GL_Base::VMRLwritePolyCoords(const SC_CoordArray& polyPoints)
{
    TxtFileC& currOut = defAssocGL->GetVRMLOutputFile();

    Coord3D tmpCoord;
    for(int i = 0; i <  polyPoints.Size(); i++)
    {
        tmpCoord = polyPoints[i];
        if (TransformAndOffset(tmpCoord))
        {
            defAssocGL->VRMLTransformCoord(tmpCoord);
            currOut.WriteSolidText("                 ");
            currOut.WriteDouble(tmpCoord.cX);
            currOut.WriteDouble(tmpCoord.cY);
            currOut.WriteDouble(tmpCoord.cZ);
            if (i <  (polyPoints.Size() - 1))
                currOut.WriteText(",");
        }
    }
}


void GL_Base::VMRLwriteColor()
{
    TxtFileC& currOut = defAssocGL->GetVRMLOutputFile();
    currOut.WriteText("     color Color {");
    currOut.WriteSolidText("            color [ ");
    currOut.SetRealConvFormat(SC_RealFormat(ncf_Decimal, 3));
    currOut.WriteDouble(currColor.RH);
    currOut.WriteDouble(currColor.GS);
    currOut.WriteDouble(currColor.BV);
    currOut.WriteText("         ]");
    currOut.WriteText("     }");
}


