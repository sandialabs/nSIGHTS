///////////////////////////////////////////////////////////////////////////////////
//
// osConfigFile.h
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
//      Main optimizer/sampler code and global variables and other support classes used by optimizer and application.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef OSCONFIGFILE_H
#define OSCONFIGFILE_H

#include <genApp/AppConfigFile.h>

#include <osClass/C_OptVar.h>
#include <osClass/C_SampVar.h>
#include <osClass/C_VaryVar.h>

namespace osConfigFile {

        extern bool         osUseVersionControl;   // major kluge to overcome short-sighted not using
                                                 // versions on opt, samp, and vary vars.

        extern void        WriteOptVar(const OptVar& inOpt);
        extern void        ReadOptVar(OptVar& outOpt);

        extern void        SampVarPreReadSetup();    // must be called before any sampled vars are read
        extern void        SampVarPostReadFixup();   // must be called after all sampled vars are read

        extern void        WriteSampVar(SampVar& inSamp);
        extern void        ReadSampVar(SampVar& outSamp);

        extern void        WriteVaryVar(VaryVar& inVary);
        extern void        ReadVaryVar(VaryVar& outVary);

};

using namespace osConfigFile;

#endif // !OSCONFIGFILE_H

