///////////////////////////////////////////////////////////////////////////////////
//
// G_XYOutput.h
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

#ifndef G_XYOUTPUT_H
#define G_XYOUTPUT_H

#include <genClass/C_FuncObj.h>

#include <nsClass/DO_SequenceTimes.h>
#include <nsClass/IO_XYSimResults.h>
#include <Var/C_FileOutput.h>


//  namespace containing main output file variables
namespace nsOutputFiles  {


    //  ***************  std forward XY output
    //  reference to output packed in FO for correct updating on file read
    class XYForwardOutputFO : public FuncObjC, public FileOutputControl {

        public:
            // input
            FuncObjRef          xyDataArrayObjRef;      // data to output -- defaults to data capture XY Array output
            FuncObjRef          seqTimesObjRef;         // sequence times from G_Sequence
            FuncObjRef          staticPressureObjRef;   // static pressure

            bool                doingFinalUpdate;

            XYSimRunResults     runResultData;          // results to be output
        private:
            DC_SequenceTimes*   sequenceTimesDC;        // current sequence times
            DC_XYDataArray*     xyDataArrayDC;
            double              staticPressure;
            int                 nCase;
            int                 ncasePar;

            int                 currCaseIndex;
            int                 currFitIndex;   // nXY = nCase * nFit
            const char*         currFitID;


        public:
                                XYForwardOutputFO();
                                ~XYForwardOutputFO(){};

            virtual bool        OutputSetup(bool& addOK);           // prepare for simulation execution
            virtual void        SimulationComplete();   // writes file as planned, cleans up

            //  for optimizer pert forward support
            void                SetCaseIndex(int caseIndex) {currCaseIndex = caseIndex;}
            void                SetFitData(const char* fitID, int fitIndex);

            void                DoStatusChk();
            void                CalcOutput(FOcalcType  calcType);

        protected:
            virtual void        Cleanup();  // common to both exit routines

        private:
            void                CalcNCase();
    };

    // standard forward output
    extern IO_XYSimResults      xyForwardOutputFile;        // includes file name
    extern XYForwardOutputFO    xyForwardOutputFO;          // data to output -- defaults to data capture XY Array output

};


//  assuming that if it's included, we want to use it and no name clashes with other nSight
using namespace nsOutputFiles;


#endif // G_XYOUTPUT_H

