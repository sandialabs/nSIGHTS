///////////////////////////////////////////////////////////////////////////////////
//
// PopupCmdDefines.h
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract
// DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
// retains certain rights in this software.
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

#ifndef POPUPCMDDEFINES_H
#define POPUPCMDDEFINES_H

//  sequence main grid popup
#define WM_SEQM_INS_BEFORE  WM_APP+1000
#define WM_SEQM_INS_AFTER   WM_APP+1001
#define WM_SEQM_DUPLICATE WM_APP+1002
#define WM_SEQM_DELETE    WM_APP+1003

//  test zone curve main grid popup
#define WM_TZCM_INS_BEFORE  WM_APP+1010
#define WM_TZCM_INS_AFTER   WM_APP+1011
#define WM_TZCM_DUPLICATE WM_APP+1012
#define WM_TZCM_DELETE    WM_APP+1013

//  data capture main grid popup
#define WM_DCAPM_INS_BEFORE WM_APP+1020
#define WM_DCAPM_INS_AFTER  WM_APP+1021
#define WM_DCAPM_DUPLICATE  WM_APP+1022
#define WM_DCAPM_DELETE   WM_APP+1023

//  data capture superposition popup
#define WM_DCAPSUP_INS_BEFORE WM_APP+1030
#define WM_DCAPSUP_INS_AFTER  WM_APP+1031
#define WM_DCAPSUP_DUPLICATE  WM_APP+1032
#define WM_DCAPSUP_DELETE     WM_APP+1033

//  point parameter popup
#define WM_POINTPAR_INS_BEFORE WM_APP+1040
#define WM_POINTPAR_INS_AFTER  WM_APP+1041
#define WM_POINTPAR_DELETE     WM_APP+1042

//  layer geology popup
#define WM_LAYERGEOLOGY_INS_BEFORE WM_APP+1050
#define WM_LAYERGEOLOGY_INS_AFTER  WM_APP+1051
#define WM_LAYERGEOLOGY_DELETE     WM_APP+1052

//  layer wellbore zones popup
#define WM_LAYERWELLBOREZONE_INS_BEFORE WM_APP+1060
#define WM_LAYERWELLBOREZONE_INS_AFTER  WM_APP+1061
#define WM_LAYERWELLBOREZONE_DELETE     WM_APP+1062

#endif // POPUPCMDDEFINES_H

