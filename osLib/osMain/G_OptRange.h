///////////////////////////////////////////////////////////////////////////////////
//
// G_OptRange.h
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
//      support for combined multiple value/optimization runs.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef G_OPTRANGE_H
#define G_OPTRANGE_H

#include <genClass/C_GlobalFunc.h>
#include <genClass/DO_Real.h>

#include <osDFO/DFO_FitBase.h>

#include <osClass/DO_GridArray.h>
#include <osClass/DO_CubeArray.h>

#include <osMain/G_Range.h>

class Optimizer;    // foward ref

//  namespace containing main range runtime
namespace osRange {

    //  reference to output packed in FO for correct updating on file read
    class OptRangeOutputFO : public RangeOutputFO {

        private:
            T_SC_Array<DFO_FitBase*>    currFits;
            T_SC_Array<DO_Real*>        currFitOutput;

        public:
                                OptRangeOutputFO();
                                ~OptRangeOutputFO(){};


                                // allocates output arrays with IDs only
            bool                AllocOutputIDs(bool optRun);

                                // adds actual allocations
            bool                PreCalcSetup(bool optRun);

            void                DoStatusChk();

            void                CalcOutput(FOcalcType  calcType);

            void                AddForwardRun();

            void                FirstOptRunSetup();
            void                AddOptRun(const Optimizer& opt, bool runOK);

    };

    //  range
    extern OptRangeOutputFO        optRangeOutputFO;          // data to output -- defaults to data capture XY Array output

};

//  assuming that if it's included, we want to use it and no name clashes with other nSight
using namespace osRange;


#endif // !G_OPTRANGE_H

