///////////////////////////////////////////////////////////////////////////////////
//
// PFO_ColorCubeIsovolumeGL.h
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
//    Implements code to draw objPFO functional objects on a plot.
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PFO_ColorCubeIsovolumeGL_H
#define PFO_ColorCubeIsovolumeGL_H

#include <objPFO/PFO_ColorCubeIsovolume.h>
#include <genPFOGL/GL_Polygon.h>

#include <genPFOGL/OGLBaseObj.h>

class PFO_ColorCubeIsovolumeGL : public PFO_ColorCubeIsovolume, protected OGLBaseObj {
  private:
      int             vertexCount;
      GL_Polygon      polygonObj;

  public:
                      PFO_ColorCubeIsovolumeGL(PlotDefC& assPlt);
                      PFO_ColorCubeIsovolumeGL(const PFO_ColorCubeIsovolumeGL& a, PlotDefC& assPlt);
                      ~PFO_ColorCubeIsovolumeGL();

      PFO_ColorCubeIsovolumeGL& operator= (const PFO_ColorCubeIsovolumeGL& a);

      //  do draw
      void            DrawPlotObject();

      // transparency draw
      virtual void    DrawTransObject(int objIndex);


  private:
      void            DoPoly(int objIndex);
      void            SetColors();

};

#endif // !PFO_ColorCubeIsovolumeGL_H

