///////////////////////////////////////////////////////////////////////////////////
//
// PC_TransparencySpec.h
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

#ifndef PC_TRANSPARENCYSPEC_H
#define PC_TRANSPARENCYSPEC_H

#include <genClass/T_SC_BasicPtrArray.h>
#include <genClass/SC_BoolArray.h>

class PD_3D;

class PSC_TransparentObject;

class PC_TransparencySpec {

    public:

        class TransparencyGroup {
            public:
                bool    groupIsTransparent;
                bool    transparencyIsConstant;
                double  constantTransFactor;
                double  startTransFactor;               // for animations
                double  endTransFactor;
            public:
                                    TransparencyGroup();
                                    TransparencyGroup(const TransparencyGroup& a);
                TransparencyGroup&  operator= (const TransparencyGroup& a);

                double              GetTransFactor(int index, int maxIndex) const;

            private:
                void                LocalCopy(const TransparencyGroup& a);
        };


        //  data
        enum                {maxTransparencyGroup = 8};
        TransparencyGroup   transparencyGroups[maxTransparencyGroup];

        //  control
        bool                useTransparency;        //  overall control
        int                 ntransSteps;            // for animation
        int                 currTransStep;

        SC_BoolArray        objectIsTransparent;
        SC_BoolArray        annoIsTransparent;

    private:

        class   ObjectTransDesc {
            public:
                double                  objDist;
                PSC_TransparentObject*  objRef;
                int                     objIndex;
                int                     objGroup;

            public:
                            ObjectTransDesc() {};
        };

        T_SC_Array<ObjectTransDesc> tranObjectList;

        typedef T_SC_BasicPtrArray<ObjectTransDesc> ObjPtrList;
        T_SC_Array<ObjPtrList>      objectSortArray;

        PD_3D&              plotDef;

        Coord3D             eyeCoord;

        bool                annoOnly;

        SC_DoubleArray      transFactor;

    public:
                            PC_TransparencySpec(PD_3D& inDef);
            PC_TransparencySpec&    operator= (const PC_TransparencySpec& a);


            bool            TransparencySetup();

            void            DrawTransparentObjects();
            void            DrawTransparentAnno(); // assumes DrawTransparentObjects called already

            void            SetDefault();

            const Coord3D&  GetEyeCoord() {return eyeCoord;}

    private:
            void            LocalCopy(const PC_TransparencySpec& a);
                            PC_TransparencySpec(const PC_TransparencySpec& a); // verboten


            static void     QBSort(ObjPtrList& a, int lo, int hi);
            static void     QSort(ObjPtrList& a, int lo, int hi);
};


#endif // !PC_TRANSPARENCYSPEC_H

