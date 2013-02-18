///////////////////////////////////////////////////////////////////////////////////
//
// DFO_FitBase.h
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
//      For calculating fits, and for reading, selecting, and operating on optimizer results.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_FITBASE_H
#define DFO_FITBASE_H

#include <genClass/C_FuncObj.h>

#include <osClass/DO_FitResults.h>
#include <osClass/C_OptVar.h>


class DFO_FitBase : public FuncObjC, public OptimizationControlStatics {
    public:
        //  suppresses derivative cals & allocs when fits used for
        //  forward range calcs
        static  bool        useFitsForOptimization;

    protected:
        //  fit output
        DO_FitResults       fitResultsDO;

    public:
                        DFO_FitBase(const char* fID);
                        DFO_FitBase(const DFO_FitBase& a);
                        ~DFO_FitBase();

        DFO_FitBase& operator= (const DFO_FitBase& a);

        //  to simplify matters in the output file routines
        DO_Real&        GetFitReal() {return fitResultsDO;}

        FitResultsPtrArray&  GetFitResults() {return  fitResultsDO.fitResults;}
        virtual void      GetFitScaling(SC_DoubleArray& scaleData);  // default returns 1 entry of 1.0
                                                                     // overriden in multiple fits

        //  required to ensure alloc checks are done on multiple fits incorporating multiple fits
        virtual void    DoAllocCheck() = 0;

    private:
        void            InitCommon();                           // common to normal and copy constructor
        void            LocalCopy(const DFO_FitBase& a);     // common to copy constructor and = operator
};

#endif // !DFO_FITBASE_H

