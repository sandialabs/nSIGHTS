///////////////////////////////////////////////////////////////////////////////////
//
// osDPOHelp.h
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
//      I/O and user interface for all osDFO functional objects.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef OSDPOHELP_H
#define OSDPOHELP_H

#include <appHelp.h>

#define IDH_DPO_CompositeFit                IDH_OS_DPO + 0
#define IDH_DPO_ReadRangeSimResults         IDH_OS_DPO + 1
#define IDH_DPO_ReadOptSimResults           IDH_OS_DPO + 2
#define IDH_DPO_SelectRangeCube             IDH_OS_DPO + 3
#define IDH_DPO_SelectRangeGrid             IDH_OS_DPO + 4
#define IDH_DPO_SelectRTRangeCube           IDH_OS_DPO + 5
#define IDH_DPO_SelectRTRangeGrid           IDH_OS_DPO + 6
#define IDH_DPO_SelectOptCovar              IDH_OS_DPO + 7
#define IDH_DPO_SelectOptJacob              IDH_OS_DPO + 8
#define IDH_DPO_SelectOptResid              IDH_OS_DPO + 9
#define IDH_DPO_SelectOptResults            IDH_OS_DPO + 10
#define IDH_DPO_SingleFit                   IDH_OS_DPO + 11

#endif //! OSDPOHELP_H

