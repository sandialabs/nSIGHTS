///////////////////////////////////////////////////////////////////////////////////
//
// DO_XYResponseFunction.h
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
//      Wrapper for DC_XYResponseFunction.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DO_XYRESPONSEFUNCTION_H
#define DO_XYRESPONSEFUNCTION_H

#include <genClass/DO_XYData.h>
#include <nsClass/DC_XYResponseFunction.h>

    class DO_XYResponseFunction : public DO_XYData {
        private:
            static char*    typeDesc;

        public:
                            DO_XYResponseFunction(){}
                            DO_XYResponseFunction(const DO_XYResponseFunction& a) : DO_XYData(a) {}
                            ~DO_XYResponseFunction(){};

            DO_XYResponseFunction&      operator= (const DO_XYResponseFunction& a);

            void            SetResponseFunction(DC_XYResponseFunction& inData);
            const DC_XYResponseFunction*    GetResponseFunction() const;

            bool            DataOK();
            char*           GetType() const {return typeDesc;}

    };

#endif // DO_XYRESPONSEFUNCTION_H

