///////////////////////////////////////////////////////////////////////////////////
//
// PD_3DXYZPS.h
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
//      merges OGL_3DXYZ with PD_3DXYZ.  Implements axes drawing routines.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PD_3DXYZPS_H
#define PD_3DXYZPS_H

#include <genPlotClass/PD_3DXYZ.h>
#include <genPlotClassPS/OGL_3DXYZ.h>


class PD_3DXYZPS : public PD_3DXYZ, public OGL3DXYZ {
    public:
                        PD_3DXYZPS();
                        PD_3DXYZPS(const PD_3DXYZPS& a);
                        ~PD_3DXYZPS();

        PD_3DXYZPS& operator= (const PD_3DXYZPS& a);

                        // PlotDefC virtuals
        void            DrawPlotAxes();
        void            ResetSize();
        void            ResetView();
        OGLBase*        GetOGLBase();

        void            InitRenderer();        // binds context, clears screen
        void            SetupDataView();       // sets axes limits and view rotations/translations
        void            StartLineSection();    // all poly objects drawn, start line only objects
        void            EndLineSection();
        void            SetupAnnoView();       // changes to 0-100 system for annotation
        void            CloseRenderer();       // flushes

        virtual void    SetClipPlanes();        // OGL Base virtual

        virtual Coord3D GetNormalizedCoord(const Coord3D& inCoord,
                                           const Coord3D& offsetCoord);

        virtual void    SetTransparencyFactor(const double& tranFactor);
        virtual void    StartTransparentSection();
        virtual void    EndTransparentSection();



    protected:
        void            FullCopy(const PD_3DXYZPS& a);
        virtual void    BasicFullDraw();   // PlotDefC overriden for 3D stereo

    private:
        void            InitCommon();  // common to normal and copy constructor
        void            LocalCopy(const PD_3DXYZPS& a);


};


#endif // !PD_3DXYZPS_H

