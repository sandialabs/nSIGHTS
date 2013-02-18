///////////////////////////////////////////////////////////////////////////////////
//
// DC_TimeProcess.h
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
//      Derived from C_TimeProcess and adds calculation.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DC_TIMEPROCESS_H
#define DC_TIMEPROCESS_H

#include <nsClass/C_TimeProcess.h>
#include <genClass/SC_SetupErr.h>
#include <genClass/DC_XYData.h>

class DC_TimeProcess : public TimeProcessDescription {
    public:
        DC_XYData               tqi;

    protected:
        bool                    timeProcessIsCreated;

    public:
                                DC_TimeProcess();
                                DC_TimeProcess(const DC_TimeProcess& a);
                                DC_TimeProcess(const TimeProcessDescription& a);  // for init from base
        virtual                 ~DC_TimeProcess();
        DC_TimeProcess&         operator= (const DC_TimeProcess& a);
        DC_TimeProcess&         operator= (const TimeProcessDescription& a);  // for copying from base

                                // returns true if basic setup is OK
        bool                    CreateTimeProcess(SC_SetupErr& errData);      // arg and horner
        bool                    CreateTimeProcess(const DC_XYData&   tqInput,  // for all
                                                        SC_SetupErr& errData);

        bool                    IsCreated() {return timeProcessIsCreated;}

        //  various sigs to get data
        bool                    GetProcessedTime(const double& rawTime,
                                                       double& processedTime) const;

        bool                    GetProcessedTime(const DC_XYData&      inputXY,
                                                       DC_XYData&      outputXY) const;

    private:
        void                    LocalCopy(const DC_TimeProcess& a);  // implementation of copy

};


#endif // DC_TIMEPROCESS_H
