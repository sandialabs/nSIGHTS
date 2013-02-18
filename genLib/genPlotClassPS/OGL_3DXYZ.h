///////////////////////////////////////////////////////////////////////////////////
//
// OGL_3DXYZ.h
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
//      adds coordinate transforms for log axes.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef OGL_3DXYZ_H
#define OGL_3DXYZ_H

#include <genPlotClassPS/OGL_3DBase.h>

// OGL 3D grid widget - defines 3D coordinate space

class PD_3DXYZ;

class OGL3DXYZ : public OGL3DBase {

    protected:
        bool            doLogX;
        bool            doLogY;
        bool            doLogZ;

        //  for VRML coord mods
        double          zMid;
        double          zMult;
        double          yMid;
        double          yMult;


    private:
        PD_3DXYZ&       plot3DXYZDef;

    public:
                        OGL3DXYZ(PD_3DXYZ& inPlotDef);

        // OGL3DBase virtuals
        virtual void    ViewSetup();


        void            VRMLTransformCoord(Coord3D& inCoord);

        //  OGLBase virtual
        virtual bool    TransformForAxes(Coord3D& inCoord) const;

    protected:
        double          TransformX(const double& xVal);
        double          TransformY(const double& yVal);
        double          TransformZ(const double& zVal);

};


#endif // !OGL_3DXYZ_H

