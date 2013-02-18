///////////////////////////////////////////////////////////////////////////////////
//
// G_Range.h
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
//      support for multiple value runs.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef G_RANGE_H
#define G_RANGE_H

#include <genClass/C_GlobalFunc.h>
#include <genClass/DO_Real.h>

#include <osClass/DO_GridArray.h>
#include <osClass/DO_CubeArray.h>


//  namespace containing main range runtime
namespace osRange {

    //  reference to output packed in FO for correct updating on file read
    class RangeOutputFO : public GlobalFunc {

            // for access in app file writing
        public:
            DC_GridArray        gridOutputDC;
            DC_CubeArray        cubeOutputDC;
            bool                doingCubes;

        protected:
            DO_GridArray        gridOutputDO;
            DO_CubeArray        cubeOutputDO;
            DO_Real             rangeXDO;
            DO_Real             rangeYDO;
            DO_Real             rangeZDO;
            int                 xIndex, yIndex, zIndex;
            int                 rangeOutputIndex;

        public:
                                RangeOutputFO(const char* id);
                                ~RangeOutputFO(){};


            void                SetToInitial(); // clears

                                // allocations of range vars
            bool                PreCalcSetup();

            void                DoStatusChk();


        protected:
            void                DeAllocDC();
            bool                AllocSetup();
            void                SetNextRangeValue(const double& nextVal);

            void                SetIndexes();

            void                SetDCID(const char* id,
                                        int&        nextID);

    };


};

//  assuming that if it's included, we want to use it and no name clashes with other nSight
using namespace osRange;


#endif // !G_RANGE_H

