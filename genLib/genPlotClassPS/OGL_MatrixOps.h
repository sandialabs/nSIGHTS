///////////////////////////////////////////////////////////////////////////////////
//
// OGL_MatrixOps.h
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
//      Platform specific implementation of genPlotClass codes
//  adds Windows and OpenGL specific code.  Includes
//  operating system independent implementation of
//  2D and 3D OpenGL functionality (platform specific
//  in this case refers to the OpenGL renderer).
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef OGL_MATRIXOPS_H
#define OGL_MATRIXOPS_H

#include <GL/GL.h>
#include <genClass/C_Common.h>

class Matrix4 {
    public:
        GLdouble    m[4][4];

    public:
                    Matrix4() {};
                    Matrix4(const Matrix4& a);
        Matrix4&    operator= (const Matrix4& a);
        Matrix4&    operator= (const double& a);

        Matrix4&    operator+= (const Matrix4& a);
        Matrix4&    operator-= (const Matrix4& a);
        Matrix4&    operator*= (const Matrix4& a);  // element for element
        Matrix4&    operator/= (const Matrix4& a);  // element for element

        Matrix4&    operator*= (const double& a);
        Matrix4&    operator/= (const double& a);

        friend Matrix4  operator+  (const Matrix4& a, const Matrix4& b);
        friend Matrix4  operator-  (const Matrix4& a, const Matrix4& b);

        void         Identity();
        void         MatrixProduct(const Matrix4& a, const Matrix4& b);
        Coord3D      VectorDotProduct(const Coord3D& vec) const;

        // OPenGL matrix ops
        void        GLMultMatrix(); // glMultMatrix with m

        //  rotation matrix
        void        RotateY(const double& cosine,
                            const double& sine);
                    // rotation about the y-axis

        void        RotateZ(const double& cosine,
                            const double& sine);
                    // rotation about the z-axis


/*
        void rot_axis_d (double omega, double axis[3]);
        void rot_about_axis_d (double angle, double axis[3]);
        void rot_omega_d (double axis[3]);
*/
        void        UrotAxis(const double&     omega,
                              const Coord3D&    axis);
       /*
        * Computes a rotation matrix.
        * The rotation is around the the direction specified (omega, radians) by the argument
        * argument axis
        * It is assumed that axis is a vector of unit length.
        * If it is not of unit length, the returned matrix will not be correct.
        */

        void UrotAboutAxis(const double& angle, const Coord3D& axis);
        /*
         * Computes a rotation matrix.
         * The rotation is around the the direction specified by the argument
         * argument axis[3].  User may specify vector which is not of unit
         * length.  The angle of rotation is specified in degrees, and is in the
         * right-handed direction.
        */

        void UrotOmega(const Coord3D& axis);

        void UviewDirection(const Coord3D& v21,
                            const Coord3D& up);

        void Uviewpoint(const Coord3D& v1,
                        const Coord3D& v2,
                        const Coord3D& up);
        /*
         * The uviewpoint subroutine computes and returns a 4x4 matrix that
         * translates the origen to the point v1, puts the negative z axis
         * along the direction v21==v2-v1, and puts the y axis along the up
         * vector.
         */

};


#endif // !OGL_MATRIXOPS_H

