///////////////////////////////////////////////////////////////////////////////////
//
// DC_DataCoord.h
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_DATACOORD_H
#define DC_DATACOORD_H

#include <genClass/SC_DoubleArray.h>


class DC_DataCoord : public SC_DoubleArray {
        public:
            bool        dataIsLog;
            enum        {dataIDLen = 40};
            char        dataID[dataIDLen];

        public:
                        DC_DataCoord();
                        DC_DataCoord(const DC_DataCoord& a);
                        ~DC_DataCoord();

            DC_DataCoord&  operator=(const DC_DataCoord& a);
            DC_DataCoord&  operator=(const SC_DoubleArray& a);

            bool        Alloc(int nData);
            bool        CreateFrom(DC_DataCoord& a);  // assigns with alloc checks

            bool        IsCompatible(const DC_DataCoord& a)      const;

            void        SetID(const char id[]);
            void        GetID(char id[], int  maxIDlen) const;
            const char* GetID()                         const {return dataID;}

            //  operations
            double      GetMid(int indx) const;
            bool        GetIndex(double inVal,
                                 int&   indx) const;
            bool        GetExactIndex(double inVal,
                                      int&   indx) const;


            bool        GetInterpT(const double& inVal,
                                         int&    stIndx,
                                         double& tVal) const;

            bool        TransformVal(double& inVal) const;
            void        UnTransformVal(double& inVal) const;

            void        GetStartEnd(int indx, 
                                    double& sVal,
                                    double& eVal) const;

            void        Decimate(int decModulus);

        private:
            void        LocalCopy(const DC_DataCoord& a);

};

#endif // DC_DATACOORD_H

