///////////////////////////////////////////////////////////////////////////////////
//
// C_Units.cpp
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

#include "StdAfx.h"
#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <nsClass/nSightConst.h>

#include <Var/C_Units.h>

//  statics
bool            NSUnitsBase::headFlag        = false;        //  if true, uPress values are converted to head
UnitReal        NSUnitsBase::headDensity(uDens, 1000.0);     //  density used in pressure head conversions

void UnitsBase::AppUnitsInitialize()
{
  InitializeUnits();   // base class init

  maxConversionFactor[uNone]   = 0;
    conversionFactors[uNone][0]  = 1.0;                 unitString[uNone][0]    = "";
  unitRealFormat[uNone][0]  = SC_RealFormat(ncf_General, 5);


  maxConversionFactor[uK]   = 5;
    conversionFactors[uK][0]  = 1.0;                    unitString[uK][0]       = "m/sec";
    conversionFactors[uK][1]  = 0.01;                   unitString[uK][1]       = "cm/sec";
    conversionFactors[uK][2]  = 0.3048;                 unitString[uK][2]       = "ft/sec";
    conversionFactors[uK][3]  = 3.52777777E-06;         unitString[uK][3]       = "ft/day";
    conversionFactors[uK][4]  = 4.7159532012E-07;       unitString[uK][4]       = "USG/d/ft^2";
    conversionFactors[uK][5]  = 1.0 / 86400.0;          unitString[uK][5]       = "m/day";
  unitRealFormat[uK][0]  = SC_RealFormat(ncf_Scientific, 5);
  unitRealFormat[uK][1]  = SC_RealFormat(ncf_Scientific, 5);
  unitRealFormat[uK][2]  = SC_RealFormat(ncf_Scientific, 5);
  unitRealFormat[uK][3]  = SC_RealFormat(ncf_General, 5);
  unitRealFormat[uK][4]  = SC_RealFormat(ncf_General, 5);
  unitRealFormat[uK][5]  = SC_RealFormat(ncf_Scientific, 5);

  maxConversionFactor[uPerm]   = 3;
    conversionFactors[uPerm][0]  = 1.0;                 unitString[uPerm][0]    = "m^2";
    conversionFactors[uPerm][1]  = 0.0001;              unitString[uPerm][1]    = "cm^2";
    conversionFactors[uPerm][2]  = 9.86923E-13;         unitString[uPerm][2]    = "D";
    conversionFactors[uPerm][3]  = 9.86923E-16;         unitString[uPerm][3]    = "mD";
  unitRealFormat[uPerm][0]  = SC_RealFormat(ncf_Scientific, 5);
  unitRealFormat[uPerm][1]  = SC_RealFormat(ncf_Scientific, 5);
  unitRealFormat[uPerm][2]  = SC_RealFormat(ncf_General, 5);
  unitRealFormat[uPerm][3]  = SC_RealFormat(ncf_General, 5);

  maxConversionFactor[uComp]   = 2;
    conversionFactors[uComp][0]  = 1.0;                 unitString[uComp][0]    = "1/Pa";
    conversionFactors[uComp][1]  = 0.001;               unitString[uComp][1]    = "1/kPa";
    conversionFactors[uComp][2]  = 1.4503773969E-04;    unitString[uComp][2]    = "1/psi";
  unitRealFormat[uComp][0]  = SC_RealFormat(ncf_Scientific, 5);
  unitRealFormat[uComp][1]  = SC_RealFormat(ncf_Scientific, 5);
  unitRealFormat[uComp][2]  = SC_RealFormat(ncf_Scientific, 5);


  maxConversionFactor[uTexp]   = 1;
    conversionFactors[uTexp][0]  = 1.0;                 unitString[uTexp][0]    = "1/C";
//    conversionFactors[uTexp][1]  = 0.5555555555556;   unitString[uTexp][1]    = "1/F"; bug fixed in 2.30
    conversionFactors[uTexp][1]  = 1.8;                 unitString[uTexp][1]    = "1/F";
  unitRealFormat[uTexp][0]  = SC_RealFormat(ncf_Scientific, 5);
  unitRealFormat[uTexp][1]  = SC_RealFormat(ncf_Scientific, 5);

  maxConversionFactor[uTran]   = 5;
    conversionFactors[uTran][0]  = 1.0;                 unitString[uTran][0]    = "m^2/sec";
    conversionFactors[uTran][1]  = 1.0E-4;              unitString[uTran][1]    = "cm^2/sec";
    conversionFactors[uTran][2]  = 9.290304E-02;        unitString[uTran][2]    = "ft^2/sec";
    conversionFactors[uTran][3]  = 1.0752667E-06;       unitString[uTran][3]    = "ft^2/day";
    conversionFactors[uTran][4]  = 1.4374225357E-07;    unitString[uTran][4]    = "USg/d/ft";
    conversionFactors[uTran][5]  = 1.0 / 86400.0;       unitString[uTran][5]    = "m^2/day";
  unitRealFormat[uTran][0]  = SC_RealFormat(ncf_Scientific, 5);
  unitRealFormat[uTran][1]  = SC_RealFormat(ncf_Scientific, 5);
  unitRealFormat[uTran][2]  = SC_RealFormat(ncf_Scientific, 5);
  unitRealFormat[uTran][3]  = SC_RealFormat(ncf_Scientific, 5);
  unitRealFormat[uTran][4]  = SC_RealFormat(ncf_Scientific, 5);
  unitRealFormat[uTran][5]  = SC_RealFormat(ncf_Scientific, 5);

  maxConversionFactor[uStor]   = 1;
    conversionFactors[uStor][0]  = 1.0;                 unitString[uStor][0]    = "1/m";
    conversionFactors[uStor][1]  = 3.280839895;         unitString[uStor][1]    = "1/ft";
  unitRealFormat[uStor][0]  = SC_RealFormat(ncf_Scientific, 5);
  unitRealFormat[uStor][1]  = SC_RealFormat(ncf_Scientific, 5);

  maxConversionFactor[uGeom]   = 1;
    conversionFactors[uGeom][0]  = 1.0;                 unitString[uGeom][0]    = "1/m^2";
    conversionFactors[uGeom][1]  = 1.0763910417E+01;    unitString[uGeom][1]    = "1/ft^2";
  unitRealFormat[uGeom][0]  = SC_RealFormat(ncf_General, 5);
  unitRealFormat[uGeom][1]  = SC_RealFormat(ncf_General, 5);


  maxConversionFactor[uWbor]   = 3;
    conversionFactors[uWbor][0]  = 1.0;                 unitString[uWbor][0]    = "m^3/Pa";
    conversionFactors[uWbor][1]  = 1.0E-03;             unitString[uWbor][1]    = "m^3/kPa";
    conversionFactors[uWbor][2]  = 4.107011484E-06;     unitString[uWbor][2]    = "ft^3/psi";
    conversionFactors[uWbor][3]  = 2.3059158631E-05;    unitString[uWbor][3]    = "STB/psi";
  unitRealFormat[uWbor][0]  = SC_RealFormat(ncf_General, 5);
  unitRealFormat[uWbor][1]  = SC_RealFormat(ncf_General, 5);
  unitRealFormat[uWbor][2]  = SC_RealFormat(ncf_General, 5);
  unitRealFormat[uWbor][3]  = SC_RealFormat(ncf_General, 5);

  maxConversionFactor[uTinv]   = 3;
    conversionFactors[uTinv][0]  = 1.0;                 unitString[uTinv][0]    = "1/sec";
    conversionFactors[uTinv][1]  = 1.666666666667E-02;  unitString[uTinv][1]    = "1/min";
    conversionFactors[uTinv][2]  = 2.77777777778E-04;   unitString[uTinv][2]    = "1/hour";
    conversionFactors[uTinv][3]  = 1.1574074E-05;       unitString[uTinv][3]    = "1/day";
  unitRealFormat[uTinv][0]  = SC_RealFormat(ncf_Scientific, 6);
  unitRealFormat[uTinv][1]  = SC_RealFormat(ncf_Scientific, 6);
  unitRealFormat[uTinv][2]  = SC_RealFormat(ncf_Scientific, 6);
  unitRealFormat[uTinv][3]  = SC_RealFormat(ncf_Scientific, 6);


  NSUnitsBase::ResetHeadPressureConversions();
}


void NSUnitsBase::ResetHeadPressureConversions()
{
  if (headFlag)
  {
    double mulFactor = headDensity.GetMetricVal() * nSightConst::gravConst * 1.0E-3;
        conversionFactors[uPress][0]  = 0.001 * mulFactor;  unitString[uPress][0]    = "mm";
        conversionFactors[uPress][1]  = 0.01 * mulFactor;   unitString[uPress][1]    = "cm";
        conversionFactors[uPress][2]  = mulFactor;          unitString[uPress][2]    = "m";
        conversionFactors[uPress][3]  = 0.3048 * mulFactor; unitString[uPress][3]    = "ft";
        conversionFactors[uPress][4]  = 0.0254 * mulFactor; unitString[uPress][4]    = "in";
    unitRealFormat[uPress][0]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uPress][1]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uPress][2]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uPress][3]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uPress][4]  = SC_RealFormat(ncf_General, 5);
  }
  else
  {
        conversionFactors[uPress][0]  = 1.0E-03;            unitString[uPress][0]    = "Pa";
        conversionFactors[uPress][1]  = 1.0;                unitString[uPress][1]    = "kPa";
        conversionFactors[uPress][2]  = 1.0E+03;            unitString[uPress][2]    = "MPa";
        conversionFactors[uPress][3]  = 6.8947572;          unitString[uPress][3]    = "psi";
        conversionFactors[uPress][4]  = 1.0E+02;            unitString[uPress][4]    = "bar";
    unitRealFormat[uPress][0]  = SC_RealFormat(ncf_Decimal, 2);
    unitRealFormat[uPress][1]  = SC_RealFormat(ncf_Decimal, 3);
    unitRealFormat[uPress][2]  = SC_RealFormat(ncf_Decimal, 4);
    unitRealFormat[uPress][3]  = SC_RealFormat(ncf_Decimal, 3);
    unitRealFormat[uPress][4]  = SC_RealFormat(ncf_Decimal, 3);
  }
}


int UnitsBase::GetAppMaxUnits()
{
  return uTinv + 1;
}



