///////////////////////////////////////////////////////////////////////////////////
//
// DO_XYZArray.h
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
//      Wrapper for DC_XXX.
//
//
///////////////////////////////////////////////////////////////////////////////////


//
//
// Programmer   : John Avis,   INTERA Inc.
// Purpose      :
//
//
// Last Modification Date:
//
// Revision History:
//

#ifndef DO_XYZARRAY_H
#define DO_XYZARRAY_H

#include <genClass/SC_CoordArray.h>
#include <genClass/C_Common.h>
#include <genClass/DO_Label.h>

class DO_XYZArray : public DO_Label {
    private:
        static char* typeDesc;
    public:
        SC_CoordArray       xyzCoords;
    public:
                            DO_XYZArray();
                            //  array is set resizable and allocd to 10
                            DO_XYZArray(const DO_XYZArray& a);
        virtual             ~DO_XYZArray(){};
        DO_XYZArray&        operator= (const DO_XYZArray& a);

        void                Reset(const char* typeLabel);   // clears all data and sets type label
        void                Clear();                        //
        int                 Size() {return xyzCoords.Size();}
        bool                CoordsAvailable() {return !xyzCoords.IsEmpty();}
        bool                DataOK();

    protected:
        // ******************************* standard methods
        void                LocalCopy(const DO_XYZArray& a);
        void                LocalInitialize();
        void                FullCopy(const DO_XYZArray& a);

};

#endif // DO_XYZARRAY_H

