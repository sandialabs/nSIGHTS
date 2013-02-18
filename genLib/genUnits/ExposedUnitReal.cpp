///////////////////////////////////////////////////////////////////////////////////
//
// ExposedUnitReal.cpp
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
//      adds unit conversion to ExposedRealC
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "GenLibAfx.h"

#include "ExposedUnitReal.h"


ExposedUnitReal::ExposedUnitReal(FuncObjC& FOref,
                                 const char* mainT,
                                 double&        inMetricVal,
                                 UnitIndex&  inUnitIndex) :
                    realProxy(0.0),
                    ExposedReal(FOref, mainT, realProxy),
                    metricVal(inMetricVal),
                    unitIndex(inUnitIndex)
{
    realProxy = unitIndex.MetricToUser(metricVal);
#ifndef ADCONSOLEAPP
    Update();
    startValUI.SetFieldValue(realProxy);
    endValUI.SetFieldValue(realProxy);
#endif

}

ExposedUnitReal :: ExposedUnitReal(FuncObjC& FOref,
                            const char* mainT,
                            UnitReal&   unitRealRef) :
                    realProxy(0.0),
                    ExposedReal(FOref, mainT, realProxy),
                    metricVal(unitRealRef.metricVal),
                    unitIndex(unitRealRef)
{
    realProxy = unitIndex.MetricToUser(metricVal);
#ifndef ADCONSOLEAPP
    Update();
#endif
}


ExposedUnitReal::~ExposedUnitReal()
{
}

#ifndef ADCONSOLEAPP

void ExposedUnitReal::Execute()
{
    metricVal = unitIndex.UserToMetric(realProxy);
    ExposedObjC::Execute();
}

void  ExposedUnitReal::UpdateConversion(const UnitIndex&  currConv)
{
    unitIndex = currConv;
    realProxy = unitIndex.MetricToUser(metricVal);
    Update();
}

void ExposedUnitRealWrapper::UpdateConversion(const UnitIndex&  currConv)
{
    if (exposedObj != 0)
        static_cast<ExposedUnitReal*>(exposedObj)->UpdateConversion(currConv);
}

#endif //! ADCONSOLEAPP

void ExposedUnitRealWrapper::CreateObject()
{
    if (exposedObj == 0)
        exposedObj = new ExposedUnitReal(FOref, mainTitle, metricVal, unitIndex);
}


