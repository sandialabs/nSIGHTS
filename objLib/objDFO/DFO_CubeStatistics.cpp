///////////////////////////////////////////////////////////////////////////////////
//
// DFO_CubeStatistics.cpp
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
//      Univariate statistics of data in a cube.
//
///////////////////////////////////////////////////////////////////////////////////

#include <objDFO/DFO_CubeStatistics.h>

DFO_CubeStatistics :: DFO_CubeStatistics() :
    DFO_Statistics("Cube Statistics")
{
    InitCommon();
}

DFO_CubeStatistics::DFO_CubeStatistics(const DFO_CubeStatistics& a) :
    DFO_Statistics(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_CubeStatistics :: ~DFO_CubeStatistics ()
{
}

void DFO_CubeStatistics::InitCommon()
{
    DSC_CubeInput::AddPort(*this);
    DoStatusChk();
}



DFO_CubeStatistics& DFO_CubeStatistics::operator= (const DFO_CubeStatistics& a)
{
    if (&a != this)
    {
        DFO_Statistics::operator=(a);
        DSC_CubeInput::operator=(a);
    }
    return *this;
}


void  DFO_CubeStatistics::LocalCopy(const DFO_CubeStatistics& a)
{
}


void  DFO_CubeStatistics:: DoStatusChk()
{
    DFO_Statistics::DoStatusChk();
    DSC_CubeInput::DoCheck(*this);
}

void DFO_CubeStatistics:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    SC_DoubleArray cubeData;
    cubeInputDC->GetData(cubeData);
    DoStatistic(cubeData);
}

