///////////////////////////////////////////////////////////////////////////////////
//
// C_GenUnits.cpp
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
//      unit conversion and basic units.  Additional units can be added by application.
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <genUnits/C_GenUnits.h>

//  statics
bool            UnitsBase::unitsInitialized = false;

double          UnitsBase::conversionFactors[maxUnits][maxConvFactor];
int             UnitsBase::maxConversionFactor[maxUnits];
char*           UnitsBase::unitString[maxUnits][maxConvFactor];
char*           UnitsBase::unitTypeString[maxUnits];
SC_RealFormat   UnitsBase::unitRealFormat[maxUnits][maxConvFactor];
int             UnitsBase::defaultUnitIndex[maxUnits];


void UnitsBase::InitializeUnits()
{
    unitTypeString[uNone]        = "none";
    maxConversionFactor[uNone]   = 0;
    conversionFactors[uNone][0]  = 1.0;                 unitString[uNone][0]    = "";
    unitRealFormat[uNone][0]  = SC_RealFormat(ncf_General, 5);

    unitTypeString[uTime]        = "time";
    maxConversionFactor[uTime]   = 4;
    conversionFactors[uTime][0]  = 1.0;                 unitString[uTime][0]    = "sec";
    conversionFactors[uTime][1]  = 60.0;                unitString[uTime][1]    = "min";
    conversionFactors[uTime][2]  = 3600.0;              unitString[uTime][2]    = "hour";
    conversionFactors[uTime][3]  = 86400.0;             unitString[uTime][3]    = "day";
    conversionFactors[uTime][4]  = 31557600.0;          unitString[uTime][4]    = "year";
    unitRealFormat[uTime][0]  = SC_DecFormat(2);
    unitRealFormat[uTime][1]  = SC_DecFormat(3);
    unitRealFormat[uTime][2]  = SC_DecFormat(5);
    unitRealFormat[uTime][3]  = SC_DecFormat(6);
    unitRealFormat[uTime][4]  = SC_DecFormat(5);


    unitTypeString[uPress]       = "pressure";
    maxConversionFactor[uPress]   = 4;
    conversionFactors[uPress][0]  = 1.0E-03;            unitString[uPress][0]    = "Pa";
    conversionFactors[uPress][1]  = 1.0;                unitString[uPress][1]    = "kPa";
    conversionFactors[uPress][2]  = 1.0E+03;            unitString[uPress][2]    = "MPa";
    conversionFactors[uPress][3]  = 6.8947572;          unitString[uPress][3]    = "psi";
    conversionFactors[uPress][4]  = 1.0E+02;            unitString[uPress][4]    = "bar";
    unitRealFormat[uPress][0]  = SC_DecFormat(2);
    unitRealFormat[uPress][1]  = SC_DecFormat(3);
    unitRealFormat[uPress][2]  = SC_DecFormat(4);
    unitRealFormat[uPress][3]  = SC_DecFormat(3);
    unitRealFormat[uPress][4]  = SC_DecFormat(3);

    unitTypeString[uFlow]        = "flow rate";
    maxConversionFactor[uFlow]   = 12;
    conversionFactors[uFlow][0]  = 1.0;                 unitString[uFlow][0]    = "m^3/sec";
    conversionFactors[uFlow][1]  = 1.1574074074E-05;    unitString[uFlow][1]    = "m^3/day";
    conversionFactors[uFlow][2]  = 1.0E-03;             unitString[uFlow][2]    = "L/sec";
    conversionFactors[uFlow][3]  = 1.1574074E-08;       unitString[uFlow][3]    = "L/day";
    conversionFactors[uFlow][4]  = 2.8316846592E-02;    unitString[uFlow][4]    = "ft^3/sec";
    conversionFactors[uFlow][5]  = 3.2774128E-07;       unitString[uFlow][5]    = "ft^3/day";
    conversionFactors[uFlow][6]  = 6.3090196667E-05;    unitString[uFlow][6]    = "USgpm";
    conversionFactors[uFlow][7]  = 7.5768116667E-05;    unitString[uFlow][7]    = "UKgpm";
    conversionFactors[uFlow][8]  = 1.840130787E-06;     unitString[uFlow][8]    = "STB/day";
    conversionFactors[uFlow][9]  = 1.66666666667E-08;   unitString[uFlow][9]    = "cm^3/min";
    conversionFactors[uFlow][10] = 1.666666666E-05;     unitString[uFlow][10]   = "L/min";
    conversionFactors[uFlow][11] = 3.168808781E-08;     unitString[uFlow][11]   = "m^3/yr";
    conversionFactors[uFlow][12] = 2.7777777778E-04;    unitString[uFlow][12]   = "m^3/hr";

    unitRealFormat[uFlow][0]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uFlow][1]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uFlow][2]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uFlow][3]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uFlow][4]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uFlow][5]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uFlow][6]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uFlow][7]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uFlow][8]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uFlow][9]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uFlow][10] = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uFlow][11] = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uFlow][12] = SC_RealFormat(ncf_General, 5);

    unitTypeString[uDist]        = "length";
    maxConversionFactor[uDist]   = 4;
    conversionFactors[uDist][0]  = 0.001;               unitString[uDist][0]    = "mm";
    conversionFactors[uDist][1]  = 0.01;                unitString[uDist][1]    = "cm";
    conversionFactors[uDist][2]  = 1.0;                 unitString[uDist][2]    = "m";
    conversionFactors[uDist][3]  = 0.0254;              unitString[uDist][3]    = "in";
    conversionFactors[uDist][4]  = 0.3048;              unitString[uDist][4]    = "ft";
    unitRealFormat[uDist][0]  = SC_DecFormat(3);
    unitRealFormat[uDist][1]  = SC_DecFormat(5);
    unitRealFormat[uDist][2]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uDist][3]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uDist][4]  = SC_DecFormat(3);

    unitTypeString[uDens]        = "density";
    maxConversionFactor[uDens]   = 3;
    conversionFactors[uDens][0]  = 1.0;                 unitString[uDens][0]    = "kg/m^3";
    conversionFactors[uDens][1]  = 1000.0;              unitString[uDens][1]    = "g/cm^3";
    conversionFactors[uDens][2]  = 1.6018463E+01;       unitString[uDens][2]    = "lb/ft^3";
    conversionFactors[uDens][3]  = 2.7679904064E+04;    unitString[uDens][3]    = "lb/in^3";
    unitRealFormat[uDens][0]  = SC_DecFormat(2);
    unitRealFormat[uDens][1]  = SC_DecFormat(5);
    unitRealFormat[uDens][2]  = SC_RealFormat(ncf_General, 2);
    unitRealFormat[uDens][3]  = SC_RealFormat(ncf_General, 5);

    unitTypeString[uVisc]        = "viscosity";
    maxConversionFactor[uVisc]   = 1;
    conversionFactors[uVisc][0]  = 0.001;            unitString[uVisc][0]  = "cP";
    conversionFactors[uVisc][1]  = 1.0;              unitString[uVisc][1]    = "Pa.s";
    unitRealFormat[uVisc][0]  = SC_DecFormat(3);
    unitRealFormat[uVisc][1]  = SC_RealFormat(ncf_General, 5);

    unitTypeString[uVol]         = "volume";
    maxConversionFactor[uVol]    = 7;
    conversionFactors[uVol][0]   = 1.0;                 unitString[uVol][0]     = "m^3";
    conversionFactors[uVol][1]   = 1.0E-06;             unitString[uVol][1]     = "cm^3";
    conversionFactors[uVol][2]   = 1.0E-03;             unitString[uVol][2]     = "L";
    conversionFactors[uVol][3]   = 1.6387064E-05;       unitString[uVol][3]     = "in^3";
    conversionFactors[uVol][4]   = 2.8316846592E-02;    unitString[uVol][4]     = "ft^3";
    conversionFactors[uVol][5]   = 3.785411784E-03;     unitString[uVol][5]     = "USgal";
    conversionFactors[uVol][6]   = 4.546087E-03;        unitString[uVol][6]     = "UKgal";
    conversionFactors[uVol][7]   = 1.589872956E-01;     unitString[uVol][7]     = "STB";
    unitRealFormat[uVol][0]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uVol][1]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uVol][2]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uVol][3]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uVol][4]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uVol][5]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uVol][6]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uVol][7]  = SC_RealFormat(ncf_General, 5);

    //  special case for temperature
    unitTypeString[uTemp]        = "temperature";
    maxConversionFactor[uTemp]   = 1;
    conversionFactors[uTemp][0]  = 0.0;                 unitString[uTemp][0]    = "C";
    conversionFactors[uTemp][1]  = 0.0;                 unitString[uTemp][1]    = "F";
    unitRealFormat[uTemp][0]  = SC_DecFormat(2);
    unitRealFormat[uTemp][1]  = SC_DecFormat(5);

    unitTypeString[uMflow]       = "mass flux";
    maxConversionFactor[uMflow]  = 8;
    conversionFactors[uMflow][0] = 1.0;                 unitString[uMflow][0]   = "g/sec";
    conversionFactors[uMflow][1] = 0.0166666666666666;  unitString[uMflow][1]   = "g/min";
    conversionFactors[uMflow][2] = 1.1574074074E-05;    unitString[uMflow][2]   = "g/day";
    conversionFactors[uMflow][3] = 1000.0;              unitString[uMflow][3]   = "kg/sec";
    conversionFactors[uMflow][4] = 16.66666666;         unitString[uMflow][4]   = "kg/min";
    conversionFactors[uMflow][5] = 1.1574074074E-02;    unitString[uMflow][5]   = "kg/day";
    conversionFactors[uMflow][6] = 453.59237;           unitString[uMflow][6]   = "lb/sec";
    conversionFactors[uMflow][7] = 7.559872833;         unitString[uMflow][7]   = "lb/min";
    conversionFactors[uMflow][8] = 5.249911690E-03;     unitString[uMflow][8]   = "lb/day";
    unitRealFormat[uMflow][0]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uMflow][1]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uMflow][2]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uMflow][3]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uMflow][4]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uMflow][5]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uMflow][6]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uMflow][7]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uMflow][8]  = SC_RealFormat(ncf_General, 5);

    unitTypeString[uMass]        = "mass";
    maxConversionFactor[uMass]   = 2;
    conversionFactors[uMass][0]  = 1.0;                 unitString[uMass][0]    = "g";
    conversionFactors[uMass][1]  = 1000.0;              unitString[uMass][1]    = "kg";
    conversionFactors[uMass][2]  = 453.59237;           unitString[uMass][2]    = "lb";
    unitRealFormat[uMass][0]  = SC_SciFormat(5);
    unitRealFormat[uMass][1]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uMass][2]  = SC_RealFormat(ncf_General, 5);

    unitTypeString[uRatio]       = "ratio";
    maxConversionFactor[uRatio]   = 1;
    conversionFactors[uRatio][0]  = 1.0;                 unitString[uRatio][0]    = "decimal";
    conversionFactors[uRatio][1]  = 0.01;                unitString[uRatio][1]    = "%";
    unitRealFormat[uRatio][0]  = SC_RealFormat(ncf_General, 5);
    unitRealFormat[uRatio][1]  = SC_DecFormat(4);

    // and unit indexes
    for ( int i = 0; i < maxUnits; i++)
        defaultUnitIndex[i] = 0;

    defaultUnitIndex[uFlow] = 6;   // psi are default
    defaultUnitIndex[uPress] = 3;   // psi are default
    defaultUnitIndex[uDist] = 2;    // m are default

    unitsInitialized = true;
}


void UnitsBase::ResetConversionIndex(UnitType   unitType,
                                     int&       conversionIndex)
{
    if ((conversionIndex < 0) || (conversionIndex > maxConversionFactor[unitType]))
        if (unitsInitialized)
        {
            conversionIndex = defaultUnitIndex[unitType];
        }
        else
        {
            conversionIndex = 0;
        }
}


double UnitsBase::MetricToUser(UnitType     unitType,
                               int          conversionIndex,
                               const double& metricVal)
{
    if (RealIsNull(metricVal))
        return metricVal;


    ResetConversionIndex(unitType, conversionIndex);
    if (unitType != uTemp)
    {
        return metricVal / conversionFactors[unitType][conversionIndex];
    }

    if (conversionIndex > 0)
        return metricVal * 1.8 + 32.0;

    return metricVal;
}

double  UnitsBase::UserToMetric(UnitType      unitType,
                                int           conversionIndex,
                                const double& userVal)
{
    if (RealIsNull(userVal))
        return userVal;

    ResetConversionIndex(unitType, conversionIndex);
    if (unitType != uTemp)
    {
        return userVal * conversionFactors[unitType][conversionIndex];
    }

    if (conversionIndex > 0)
        return (userVal - 32.0) / 1.8;

    return userVal;
}


const char*  UnitsBase::GetUnitString(UnitType  unitType,
                                      int       conversionIndex)
{
    if (conversionIndex == nullUnitConvIndex)
        return nullStr;

    ResetConversionIndex(unitType, conversionIndex);
    return unitString[unitType][conversionIndex];
}

void  UnitsBase::GetUnitStrings(UnitType        unitType,
                                bool            withBrackets,
                                SC_StringArray& unitStrs)
{
    int maxStr = maxConversionFactor[unitType] + 1;
    unitStrs.Alloc(maxStr);
    char tmpStr[80];
    for (int i = 0; i < maxStr; i++)
    {
        SetToNull(tmpStr);
        if (withBrackets)
            ConcatString(tmpStr, "[", 80);
        ConcatString(tmpStr, GetUnitString(unitType, i), 80);
        if (withBrackets)
            ConcatString(tmpStr, "]", 80);

        unitStrs += tmpStr;
    }

}


void UnitIndex::MetricToUser(SC_DoubleArray& inMetOutUser) const
{
    for (int i = 0; i < inMetOutUser.Size(); i++)
    {
        inMetOutUser[i] = MetricToUser(inMetOutUser[i]);
    }
}

void UnitIndex::UserToMetric(SC_DoubleArray& inUserOutMet) const
{
    for (int i = 0; i < inUserOutMet.Size(); i++)
    {
        inUserOutMet[i] = UserToMetric(inUserOutMet[i]);
    }
}

void UnitIndex::ResetConversionIndex()
{
    UnitsBase::ResetConversionIndex(unitType, convIndex);
}

int UnitIndex::GetConvIndex() const
{
    int tempConv = convIndex;
    UnitsBase::ResetConversionIndex(unitType, tempConv);
    return tempConv;
}

void UnitIndex::MakeUnitString(char*    inStr,
                               int      maxLen,
                               bool     addBrackets) const
{
    if (addBrackets)
    {
        CopyString(inStr,"[", maxLen);
    }
    else
    {
        SetToNull(inStr);
    }

    ConcatString(inStr, GetUnitString(), maxLen);
    if (addBrackets)
        ConcatString(inStr,"]", maxLen);
}



void  UnitIndex::GetUserString(const double& metricVal,
                                     char    str[],
                                     int     maxLen) const
{
    if (RealIsNull(metricVal))
        CopyString(str, nullStr, maxLen);
    else
        unitRealFormat[unitType][GetConvIndex()].RealToString(MetricToUser(metricVal), str, maxLen);
}

void UnitIndex::GetUserString(const double&         metricVal,
                                    char            str[],
                                    int             maxLen,
                             const SC_RealFormat&  overrideFormat) const
{
    if (RealIsNull(metricVal))
        CopyString(str, nullStr, maxLen);
    else
        overrideFormat.RealToString(MetricToUser(metricVal), str, maxLen);
}



void UnitIndex::GetUserString(const double&         metricVal,
                                    char            userStr[],
                                    int             maxLen,
                             const SC_RealFormat*  poverrideFormat) const
{
    if (poverrideFormat == NULL)
        GetUserString(metricVal, userStr, maxLen);
    else
        GetUserString(metricVal, userStr, maxLen, *poverrideFormat);
}

void  UnitReal::GetUserString(char    str[],
                              int     maxLen) const
{
    UnitIndex::GetUserString(metricVal, str, maxLen);
}

void UnitReal::GetUserString(       char            userStr[],
                                    int             maxLen,
                             const SC_RealFormat&  overrideFormat) const
{
    UnitIndex::GetUserString(metricVal, userStr, maxLen, overrideFormat);
}

void  UnitReal::GetUserString(      char            userStr[],
                                    int             maxLen,
                             const SC_RealFormat*  poverrideFormat) const
{
    UnitIndex::GetUserString(metricVal, userStr, maxLen, poverrideFormat);
}


UnitReal& UnitReal::operator= (const UnitIndex& a)
{
    if (this != &a)
    {
        UnitIndex::operator = (a);
    }
    return *this;
}

UnitReal& UnitReal::operator= (const UnitReal& a)
{
    if (this != &a)
    {
        UnitIndex::operator = (a);
        metricVal = a.metricVal;
    }
    return *this;
}

void UnitReal::InitVal(const UnitIndex& inUnitIndex, const double& inMetricVal)
{
    UnitIndex::operator=(inUnitIndex);
    metricVal = inMetricVal;
}




#ifdef _WIN32
void   UnitIndex::GetUserString(const double&  metricVal,
                                      CString& str) const
{
    if (RealIsNull(metricVal))
        str = nullStr;
    else
        unitRealFormat[unitType][GetConvIndex()].RealToString(MetricToUser(metricVal), str);
}

void UnitIndex::GetUserString(const double&         metricVal,
                                    CString&        userStr,
                              const SC_RealFormat*  poverrideFormat) const
{
    if (RealIsNull(metricVal))
    {
        userStr = nullStr;
        return;
    }
    if (poverrideFormat == NULL)
        GetUserString(metricVal, userStr);
    else
        poverrideFormat->RealToString(MetricToUser(metricVal), userStr);

}
void UnitIndex::SetRangeMsg(const double& minMetric,
                            const double& maxMetric) const
{
    CString loLim, upLim;
    GetUserString(minMetric, loLim);
    GetUserString(maxMetric, upLim);
    GenAppParameterRangeMsg(loLim, upLim);
}



void UnitReal::GetUserString(CString& str) const
{
    UnitIndex::GetUserString(metricVal, str);
}

void UnitReal::GetUserString(CString& userStr,
                            const SC_RealFormat&  overrideFormat) const
{
    UnitIndex::GetUserString(metricVal, userStr, &overrideFormat);
}

bool UnitReal::IsWithinRange(const CString& userStr,
                              const double& minMetric,
                              const double& maxMetric)
{
    double tVal;
    if (IsValidReal(userStr, tVal))
    {
        double mVal = UnitIndex::UserToMetric(tVal);
        if ((mVal >= minMetric) && (mVal <= maxMetric))
        {
            metricVal = mVal;
            GenAppClearMsgs();
            return true;
        }
        else
        {
            GenAppErrorMsg("RealEntry","value out of range");
            SetRangeMsg(minMetric, maxMetric);
        }
    }
    else
        if (userStr == nullStr)
            GenAppClearMsgs();
        else
            GenAppErrorMsg("RealEntry","not a valid real number");

    return false;
}

#endif //_WIN32



