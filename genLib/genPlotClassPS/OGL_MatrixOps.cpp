///////////////////////////////////////////////////////////////////////////////////
//
// OGL_MatrixOps.cpp
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



/*
 * MODULE: urotate.c
 *
 * FUNCTION:
 * This module contains three different routines that compute rotation
 * matricies and return these to user.
 * Detailed description is provided below.
 *
 * HISTORY:
 * Developed & written, Linas Vepstas, Septmeber 1991
 * double precision port, March 1993
 *
 * DETAILED DESCRIPTION:
 * This module contains three routines:
 * --------------------------------------------------------------------
 *
 *
 * void rot_about_axis (float angle,        --- input
 *                      float axis[3])      --- input
 * Same as above routine, except that the matrix is multiplied into the
 * GL matrix stack.
 *
 * --------------------------------------------------------------------
 *
 * void urot_axis (float m[4][4],      --- returned
 *                 float omega,        --- input
 *                 float axis[3])      --- input
 * Same as urot_about_axis(), but angle specified in radians.
 * It is assumed that the argument axis[3] is a vector of unit length.
 * If it is not of unit length, the returned matrix will not be correct.
 *
 * void rot_axis (float omega,        --- input
 *                float axis[3])      --- input
 * Same as above routine, except that the matrix is multiplied into the
 * GL matrix stack.
 *
 * --------------------------------------------------------------------
 *
 * void urot_omega (float m[4][4],       --- returned
 *                  float omega[3])      --- input
 * same as urot_axis(), but the angle is taken as the length of the
 * vector omega[3]
 *
 * void rot_omega (float omega[3])      --- input
 * Same as above routine, except that the matrix is multiplied into the
 * GL matrix stack.
 *
 * --------------------------------------------------------------------
 */

#include "GenLibAfx.h"

#include <genClass/U_Real.h>

#include <genPlotClassPS/OGL_MatrixOps.h>


Matrix4::Matrix4(const Matrix4& a)
{
    *this = a;
}

Matrix4& Matrix4::operator=(const Matrix4& a)
{
    if (&a != this)
    {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = a.m[i][j];
    }
    return *this;
}

Matrix4& Matrix4::operator=(const double& a)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] = a;
    return *this;
}

Matrix4&  Matrix4::operator+= (const Matrix4& a)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] += a.m[i][j];
    return *this;
}

Matrix4&  Matrix4::operator-= (const Matrix4& a)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] -= a.m[i][j];
    return *this;
}

Matrix4&  Matrix4::operator *= (const Matrix4& a)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] *= a.m[i][j];
    return *this;
}

Matrix4&  Matrix4::operator /= (const Matrix4& a)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] /= a.m[i][j];
    return *this;
}

Matrix4&  Matrix4::operator *= (const double& a)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] *= a;
    return *this;
}

Matrix4&  Matrix4::operator /= (const double& a)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] /= a;
    return *this;
}

Matrix4  operator+  (const Matrix4& a, const Matrix4& b)
{
    Matrix4 temp;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            temp.m[i][j] = a.m[i][j] + b.m[i][j];
    return temp;
}

Matrix4  operator-  (const Matrix4& a, const Matrix4& b)
{
    Matrix4 temp;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            temp.m[i][j] = a.m[i][j] - b.m[i][j];
    return temp;
}

void Matrix4::Identity()
{
    *this = 0.0;
    for (int i = 0; i < 4; i++)
        m[i][i] = 1.0;
}


void Matrix4::MatrixProduct(const Matrix4& a, const Matrix4& b)
{
    m[0][0] = a.m[0][0]*b.m[0][0]+a.m[0][1]*b.m[1][0]+a.m[0][2]*b.m[2][0]+a.m[0][3]*b.m[3][0];
    m[0][1] = a.m[0][0]*b.m[0][1]+a.m[0][1]*b.m[1][1]+a.m[0][2]*b.m[2][1]+a.m[0][3]*b.m[3][1];
    m[0][2] = a.m[0][0]*b.m[0][2]+a.m[0][1]*b.m[1][2]+a.m[0][2]*b.m[2][2]+a.m[0][3]*b.m[3][2];
    m[0][3] = a.m[0][0]*b.m[0][3]+a.m[0][1]*b.m[1][3]+a.m[0][2]*b.m[2][3]+a.m[0][3]*b.m[3][3];

    m[1][0] = a.m[1][0]*b.m[0][0]+a.m[1][1]*b.m[1][0]+a.m[1][2]*b.m[2][0]+a.m[1][3]*b.m[3][0];
    m[1][1] = a.m[1][0]*b.m[0][1]+a.m[1][1]*b.m[1][1]+a.m[1][2]*b.m[2][1]+a.m[1][3]*b.m[3][1];
    m[1][2] = a.m[1][0]*b.m[0][2]+a.m[1][1]*b.m[1][2]+a.m[1][2]*b.m[2][2]+a.m[1][3]*b.m[3][2];
    m[1][3] = a.m[1][0]*b.m[0][3]+a.m[1][1]*b.m[1][3]+a.m[1][2]*b.m[2][3]+a.m[1][3]*b.m[3][3];

    m[2][0] = a.m[2][0]*b.m[0][0]+a.m[2][1]*b.m[1][0]+a.m[2][2]*b.m[2][0]+a.m[2][3]*b.m[3][0];
    m[2][1] = a.m[2][0]*b.m[0][1]+a.m[2][1]*b.m[1][1]+a.m[2][2]*b.m[2][1]+a.m[2][3]*b.m[3][1];
    m[2][2] = a.m[2][0]*b.m[0][2]+a.m[2][1]*b.m[1][2]+a.m[2][2]*b.m[2][2]+a.m[2][3]*b.m[3][2];
    m[2][3] = a.m[2][0]*b.m[0][3]+a.m[2][1]*b.m[1][3]+a.m[2][2]*b.m[2][3]+a.m[2][3]*b.m[3][3];

    m[3][0] = a.m[3][0]*b.m[0][0]+a.m[3][1]*b.m[1][0]+a.m[3][2]*b.m[2][0]+a.m[3][3]*b.m[3][0];
    m[3][1] = a.m[3][0]*b.m[0][1]+a.m[3][1]*b.m[1][1]+a.m[3][2]*b.m[2][1]+a.m[3][3]*b.m[3][1];
    m[3][2] = a.m[3][0]*b.m[0][2]+a.m[3][1]*b.m[1][2]+a.m[3][2]*b.m[2][2]+a.m[3][3]*b.m[3][2];
    m[3][3] = a.m[3][0]*b.m[0][3]+a.m[3][1]*b.m[1][3]+a.m[3][2]*b.m[2][3]+a.m[3][3]*b.m[3][3];
}

Coord3D  Matrix4::VectorDotProduct(const Coord3D& vec) const
{
    Coord3D temp;
    temp.cX = m[0][0]*vec.cX + m[0][1]*vec.cY + m[0][2]*vec.cZ;
    temp.cY = m[1][0]*vec.cX + m[1][1]*vec.cY + m[1][2]*vec.cZ;
    temp.cZ = m[2][0]*vec.cX + m[2][1]*vec.cY + m[2][2]*vec.cZ;
    return temp;
}

void  Matrix4::GLMultMatrix()
{
    glMultMatrixd((GLdouble*) m);
}


void Matrix4::RotateY(const double& cosine,
                      const double& sine)
{
    Identity();

    m[0][0] = cosine;
    m[0][2] = -sine;

    m[2][0] = sine;
    m[2][2] = cosine;
}

void Matrix4::RotateZ(const double& cosine,
                      const double& sine)
{
    Identity();

    m[0][0] = cosine;
    m[0][1] = sine;

    m[1][0] = -sine;
    m[1][1] = cosine;
}


/* ========================================================== */

void Matrix4::UrotAxis (const double& omega,
                        const Coord3D& axis)
{
   /*
    * The formula coded up below can be derived by using the
    * homomorphism between SU(2) and O(3), namely, that the
    * 3x3 rotation matrix R is given by
    *      t.R.v = S(-1) t.v S
    * where
    * t are the Pauli matrices (similar to Quaternions, easier to use)
    * v is an arbitrary 3-vector
    * and S is a 2x2 hermitian matrix:
    *     S = exp ( i omega t.axis / 2 )
    *
    * (Also, remember that computer graphics uses the transpose of R).
    *
    * The Pauli matrices are:
    *
    *  tx = (0 1)          ty = (0 -i)            tz = (1  0)
    *       (1 0)               (i  0)                 (0 -1)
    *
    * Note that no error checking is done -- if the axis vector is not
    * of unit length, you'll get strange results.
    */

   double tmp = omega / 2.0;
   double s = sin (tmp);
   double c = cos (tmp);

   double ssq = s*s;
   double csq = c*c;

   m[0][0] = m[1][1] = m[2][2] = csq - ssq;

   ssq *= 2.0;

   // on-diagonal entries
   m[0][0] += ssq * axis.cX * axis.cX;
   m[1][1] += ssq * axis.cY * axis.cY;
   m[2][2] += ssq * axis.cZ * axis.cZ;

   /* off-diagonal entries */
   m[0][1] = m[1][0] = axis.cX * axis.cY * ssq;
   m[1][2] = m[2][1] = axis.cY * axis.cZ * ssq;
   m[2][0] = m[0][2] = axis.cZ * axis.cX * ssq;

   double cts = 2.0 * c * s;

   tmp = cts * axis.cZ;
   m[0][1] += tmp;
   m[1][0] -= tmp;

   tmp = cts * axis.cX;
   m[1][2] += tmp;
   m[2][1] -= tmp;

   tmp = cts * axis.cY;
   m[2][0] += tmp;
   m[0][2] -= tmp;

   /* homogeneous entries */
   m[0][3] = m[1][3] = m[2][3] = m[3][2] = m[3][1] = m[3][0] = 0.0;
   m[3][3] = 1.0;
}

void Matrix4::UrotAboutAxis(const double&  angle,
                            const Coord3D& axis)
{
    Coord3D tmp(axis);
    tmp.Normalize();
    UrotAxis(Radians(angle), tmp);
}

void Matrix4::UrotOmega(const Coord3D& axis)
{
    Coord3D tmp(axis);
    double len = tmp.Magnitude();
    tmp.Normalize();

   // the amount of rotation is equal to the 1/length, in radians
   UrotAxis(1.0 / len, tmp);

}

void Matrix4::Uviewpoint(const Coord3D& v1,
                         const Coord3D& v2,
                         const Coord3D& up)
{
   // find the vector that points in the v21 direction
   Coord3D v_hat_21 = v2 - v1;

   // compute rotation matrix that takes -z axis to the v21 axis,
   // and y to the up direction
   Matrix4 rotMat;
   rotMat.UviewDirection(v_hat_21, up);

   // build matrix that translates the origin to v1
   Matrix4 transMat;
   transMat.Identity();
   transMat.m[3][0] = v1.cX;
   transMat.m[3][1] = v1.cY;
   transMat.m[3][2] = v1.cZ;

   // concatenate the matrices together
   MatrixProduct(rotMat, transMat);

}

void Matrix4::UviewDirection(const Coord3D& v21,
                             const Coord3D& up)
{
   double sine, cosine;

    // find the unit vector that points in the v21 direction
    Coord3D v_hat_21(v21);
    double len = v_hat_21.Magnitude();

    Matrix4 amat;
    if (fabs(len) > stdEps)
    {
        len = 1.0 / len;
        v_hat_21 *= len;

        // rotate z in the xz-plane until same latitude
        sine = sqrt (1.0 - v_hat_21.cZ * v_hat_21.cZ);

        amat.RotateY(-v_hat_21.cZ, -sine);

    }
    else
        // error condition: zero length vecotr passed in -- do nothing */
        amat.Identity();

    // project v21 onto the xy plane
    Coord3D v_xy(v21);
    v_xy.cZ = 0.0;
    len = v_xy.Magnitude();

    // rotate in the x-y plane until v21 lies on z axis ---
    // but of course, if its already there, do nothing
    Matrix4 bmat, cmat;
    if (fabs(len) > stdEps)
    {
      // want xy projection to be unit vector, so that sines/cosines pop out
      len = 1.0 / len;
      v_xy *= len;

      // rotate the projection of v21 in the xy-plane over to the x axis
      bmat.RotateZ(v_xy.cX, v_xy.cY);

      // concatenate these together
      cmat.MatrixProduct(amat, bmat);

    }
    else
        cmat = amat;


    /* up vector really should be perpendicular to the x-form direction --
     * Use up a couple of cycles, and make sure it is,
     * just in case the user blew it.
     */
    Coord3D up_proj;
    up_proj.Perpendicular(up, v_hat_21);
    len = up_proj.Magnitude();
    if (fabs(len) > stdEps)
    {
        // normalize the vector
        len = 1.0/len;
        up_proj *= len;

        // compare the up-vector to the  y-axis to get the cosine of the angle
        Coord3D tmp;
        tmp.cX = cmat.m[1][0];
        tmp.cY = cmat.m[1][1];
        tmp.cZ = cmat.m[1][2];
        cosine = tmp.Dot(up_proj);

        // compare the up-vector to the x-axis to get the sine of the angle
        tmp.cX = cmat.m[0][0];
        tmp.cY = cmat.m[0][1];
        tmp.cZ = cmat.m[0][2];
        sine = tmp.Dot(up_proj);

        // rotate to align the up vector with the y-axis
        amat.RotateZ(cosine, -sine);

        // This xform, although computed last, acts first
        MatrixProduct(amat, cmat);

    }
    else
    {

        // error condition: up vector is indeterminate (zero length)
        // -- do nothing
        *this = cmat;
    }
}


