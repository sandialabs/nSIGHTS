///////////////////////////////////////////////////////////////////////////////////
//
// PSC_TransparentObject.h
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
//      Platform independent base classes for plot definitions and plot functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef PSC_TRANSPARENTOBJECT_H
#define PSC_TRANSPARENTOBJECT_H

#include <genClass/C_Common.h>
#include <genClass/SC_CoordArray.h>


class PlotObjBaseC;

class PSC_TransparentObject {

    friend class PC_TransparencySpec;

    public:
        //  control
        int                 transGroup;

    protected:
        bool                transPlotted; // cleared at start of transparency
                                          // plotting sequence - not used yet
        Limit3D             transLimits;

    private:
        PlotObjBaseC&       baseObj;

    public:
                            PSC_TransparentObject(PlotObjBaseC& inObj);
                            ~PSC_TransparentObject();

            PSC_TransparentObject&    operator= (const PSC_TransparentObject& a);

            virtual bool    SupportsTransparency() const;    // default false, set to true in each object

            virtual int     GetnTransObjects() const;                    // number of transparent poly/objects

            virtual void    SetupForGetCoord();

            virtual Coord3D GetTransObjectCoord(int objIndex);

            virtual void    DrawTransObject(int objIndex);

            inline  int     GetTransGroup() const {return transGroup;}

    protected:
            Coord3D         GetTransPolyCentroid(const SC_CoordArray& inPoly);
                            // checks agains limits, returns null if at least
                            // one point in poly is not not in limits
                            // otherwise, returns centroid
    private:
            void            LocalCopy(const PSC_TransparentObject& a);
                            PSC_TransparentObject(const PSC_TransparentObject& a);

};


#endif // !PSC_TRANSPARENTOBJECT_H

