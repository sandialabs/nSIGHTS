///////////////////////////////////////////////////////////////////////////////////
//
// PFO_EnterXYOnPlotGL.h
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

#ifndef PFO_ENTERXYONPLOTGL_H
#define PFO_ENTERXYONPLOTGL_H

#include <genPFOGL/OGLBaseObj.h>
#include <genPFOGL/GL_PixelCursorTrack.h>

#include <objPFO/PFO_EnterXYOnPlot.h>


class PFO_EnterXYOnPlotGL : public PFO_EnterXYOnPlot, protected OGLBaseObj {
  private:
      GL_PixelCursorTrack cursorTrackData;
      bool                deleteMode;
      bool                changesMade;
      bool                dragOK;
      int                 enterStartPoint;

  public:
                      PFO_EnterXYOnPlotGL(PlotDefC& assPlt);
                      PFO_EnterXYOnPlotGL(const PFO_EnterXYOnPlotGL& a, PlotDefC& assPlt);
                      ~PFO_EnterXYOnPlotGL();

      PFO_EnterXYOnPlotGL& operator= (const PFO_EnterXYOnPlotGL& a);

      void            ProcessSelection(const Coord3D&                cursorPos,
                                             PlotDefC::SelectionKeys inKey);



      //  do draw
      void            DrawPlotObject();

  private:
      void            InitCommon();
      void            AddEnteredPoint(const double& pX, const double& pY);
      void            DrawSymbols(const PSC_SymbolSpec& inSpec, PointStatusType statToDraw, int& ndrawn);
      void            BuildCallbackMenu();        // for locality
      void            SetCallbackSensitive();
      virtual void    DoCallback(int reason);     // interactiveobj virtual
      void            DoCancel();
      void            DoDeleteAll();
      void            DoReplot();
      bool            EnterStartOK();


};

#endif // !PFO_ENTERXYONPLOTGL_H

