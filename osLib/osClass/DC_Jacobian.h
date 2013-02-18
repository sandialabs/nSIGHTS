///////////////////////////////////////////////////////////////////////////////////
//
// DC_Jacobian.h
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
//      optimizer results (Jacobian matrices) for a single fit.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_JACOBIAN_H
#define DC_JACOBIAN_H

#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_StringArray.h>

#include <osClass/DC_OptSimResults.h>

// jacobian results for a single fit
class DC_Jacobian {
    public:
        enum                    {idLen = 80};
        char                    runID[idLen];
        char                    caseID[idLen];
        char                    fitID[idLen];

        SC_DoubleCube           jacobData;       // nFit x noptVar x nfitPoints (may vary from slice to slice)
                                                 // better to view as array of matrices.  cube used only because the type exists
        double                  derivSpan;       // final derivative span

        OptFitDescription       fitDesc;
        OptVarDescriptionArray  parDesc;

    public:
                                DC_Jacobian();
                                ~DC_Jacobian() {}
                                DC_Jacobian(const DC_Jacobian& a);

        DC_Jacobian&            operator=(const DC_Jacobian& a);

        bool                    ExtractFrom(const OptSimRunResults&  optRes,  // returns false only if jacob data no good
                                                  int                caseIndex,
                                                  int                fitIndex);

    private:
        void                    LocalCopy(const DC_Jacobian& a);
};


#endif // !DC_JACOBIAN_H

