///////////////////////////////////////////////////////////////////////////////////
//
// PD_CompositePS.h
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
//      merges OGL_Base with PD_Composite
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PD_COMPOSITEPS_H
#define PD_COMPOSITEPS_H

#include <genPlotClass/PD_Composite.h>
#include <genPlotClassPS/OGL_Base.h>


class PD_CompositePS : public PD_Composite, public OGLBase {
    private:

    public:
                        PD_CompositePS();
                        PD_CompositePS(const PD_CompositePS& a);

                        ~PD_CompositePS();



        PD_CompositePS&      operator= (const PD_CompositePS& a);

                        // PlotDefC virtuals
        void            DrawPlotAxes();
        void            DrawPlotObjects();
        void            ResetSize();
        OGLBase*        GetOGLBase();

        void            InitRenderer();        // binds context, clears screen
        void            SetupDataView();       // sets axes limits and view rotations/translations
        void            StartLineSection();    // all poly objects drawn, start line only objects
        void            EndLineSection();
        void            SetupAnnoView();       // changes to 0-100 system for annotation
        void            CloseRenderer();       // flushes

        virtual void    SetClipPlanes();        // OGL Base virtual
        virtual void    InitView();
        virtual void    DrawLine(const Coord3D& stPoint,
                                 const Coord3D& endPoint) const;
        virtual Coord3D GetIndependentScale(const double& inSize) const;  //  returns scale factor



    private:
        void            InitCommon();  // common to normal and copy constructor
        void            LocalCopy(const PD_CompositePS& a);

};


#endif // !PD_COMPOSITEPS_H

