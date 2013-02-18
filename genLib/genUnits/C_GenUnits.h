///////////////////////////////////////////////////////////////////////////////////
//
// C_GenUnits.h
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

#ifndef C_APPUNITS_H
#define C_APPUNITS_H

#include <genClass/C_Common.h>
#include <genClass/SC_RealFormat.h>
#include <genClass/SC_DoubleArray.h>
#include <genClass/SC_StringArray.h>

#ifdef _WIN32
#include <afx.h>            // strings for dialogs
#endif

namespace appUnits  {

    typedef int UnitType;

    const int uNone     = 0;    // 0    no units
    const int uTime     = 1;    // 1    t        time
    const int uPress    = 2;    // 2    P        pressure
    const int uFlow     = 3;    // 3    L^3/t    volumetric flow rate
    const int uDist     = 4;    // 4    L        distance
    const int uDens     = 5;    // 5    M/L^3    density
    const int uVisc     = 6;    // 6    P t      viscosity
    const int uVol      = 7;    // 7    L^3      volume
    const int uTemp     = 8;    // 8    T        temperature
    const int uMflow    = 9;    // 9   M/t      mass flow
    const int uMass     = 10;   // 10   M        mass
    const int uRatio    = 11;    // 11            ratio

    enum {nullUnitConvIndex = -99}; // kluge to return null string from GetUnitString

    // various enums for specific units
    const int utimeYears = 4;

    class UnitReal;        // forward ref for static

    //  the base class contains statics for conversions and basic conversion methods
    class UnitsBase  {
        private:
            enum                    {maxUnits = 50};
            enum                    {maxConvFactor = 16};

        public:
            static int              defaultUnitIndex[maxUnits];     // default unit conversions

        protected:

            // should generally all be treated as const after init

            static double           conversionFactors[maxUnits][maxConvFactor];
            static int              maxConversionFactor[maxUnits];
            static char*            unitString[maxUnits][maxConvFactor];
            static char*            unitTypeString[maxUnits];
            static SC_RealFormat    unitRealFormat[maxUnits][maxConvFactor];
            static bool             unitsInitialized;


        public:

            static int          GetMaxConversionIndex(UnitType unitType) {return maxConversionFactor[unitType];}

            static double       MetricToUser(UnitType      unitType,
                                             int           conversionIndex,
                                             const double& metricVal);      //conv index 0 = metric base

            static double       UserToMetric(UnitType      unitType,
                                             int           conversionIndex,
                                             const double& userVal);   //conv index 0 = metric base

            static const char*  GetUnitString(UnitType  unitType,  // gets unit descriptor for type and index
                                              int       conversionIndex);

            static const char*  GetUnitTypeString(UnitType  unitType) {return unitTypeString[unitType];}  // gets unit descriptor for type and index


            static void         GetUnitStrings(UnitType         unitType,       // gets all unit strings
                                               bool             withBrackets,   // if true then [] added
                                               SC_StringArray&  unitStrs);      //

            static void         ResetConversionIndex(UnitType   unitType,
                                                     int&       conversionIndex);

            static void         AppUnitsInitialize();
                                // must be implemented in app
                                // must be called in app startup to initialize units
            static int          GetAppMaxUnits();
                                // must be implemented in app

        protected:
            static void         InitializeUnits();  // to be called by AppUnitsInitialize
    };


    //  contains the unit type and a current conversion index
    class UnitIndex : public UnitsBase {
        public:
            int         unitType;

        protected:
            int         convIndex;

        public:
                            UnitIndex() : unitType(uNone), convIndex(-1) {}
                            UnitIndex(UnitType  uType) : unitType(uType), convIndex(-1)  {}
                            UnitIndex(const UnitIndex& a) : unitType(a.unitType), convIndex(a.convIndex) {}

            UnitIndex&      operator= (const UnitIndex& a){unitType = a.unitType; convIndex = a.convIndex; return *this;}

                            // conversion
            double          MetricToUser(const double& metricVal) const {return UnitsBase::MetricToUser(unitType, GetConvIndex(), metricVal);}
            double          UserToMetric(const double& userVal) const {return UnitsBase::UserToMetric(unitType, GetConvIndex(), userVal);}
            void            MetricToUser(SC_DoubleArray& inMetOutUser) const;
            void            UserToMetric(SC_DoubleArray& inUserOutMet) const;

                            // index access
            int             GetConvIndex()  const;              // gets effective conversion index (actual if > -1, default if -1)
            void            SetConvIndex(int newConv)   {convIndex = newConv;}  // sets conv index
            int             GetActualConvIndex() const  {return convIndex;}     // for file/temporary use only
            void            ResetConversionIndex();

            void            MakeUnitString(char*    inStr,
                                           int      maxLen,
                                           bool     addBrackets = true) const;


            void            GetUserString(const double& metricVal,
                                                char    str[],
                                                int     maxLen) const;

            void            GetUserString(const double&         metricVal,
                                                char            str[],
                                                int             maxLen,
                                          const SC_RealFormat&  overrideFormat) const;

            void            GetUserString(const double&         metricVal,
                                                char            userStr[],
                                                int             maxLen,
                                          const SC_RealFormat*  poverrideFormat) const;


#ifdef _WIN32
            void            GetUserString(const double&  metricVal,
                                                CString& str) const;

            void            GetUserString(const double&         metricVal,
                                                CString&        userStr,
                                          const SC_RealFormat*  poverrideFormat) const;


            void            SetRangeMsg(const double& minMetric,
                                        const double& maxMetric) const;

#endif

            const SC_RealFormat&    GetUnitRealFormat() const {return unitRealFormat[unitType][GetConvIndex()];}
            const char*             GetUnitString() const {return UnitsBase::GetUnitString(unitType, convIndex);}
            void                    GetUnitStrings(bool             withBrackets,
                                                    SC_StringArray&  unitStrs) const {UnitsBase::GetUnitStrings(unitType, withBrackets, unitStrs);}

    };


    //  contains the metric (SI) value of a real data element
    class UnitReal : public UnitIndex {
        public:
            double          metricVal;

        public:
                            UnitReal() {metricVal = nullReal;}
                            UnitReal(UnitType  uType) : UnitIndex(uType), metricVal(nullReal) {}
                            UnitReal(UnitType  uType, double initVal) : UnitIndex(uType), metricVal(initVal) {}
                            UnitReal(UnitIndex uIndex) : UnitIndex(uIndex), metricVal(nullReal) {}
                            UnitReal(UnitIndex uIndex, double initVal) : UnitIndex(uIndex), metricVal(initVal) {}

            UnitReal&       operator= (const UnitIndex& a);
            UnitReal&       operator= (const UnitReal& a);

            void            InitVal(const UnitIndex& inUnitIndex, const double& inMetricVal);

            bool            IsNull() {return RealIsNull(metricVal);}

            double          GetMetricVal() const {return metricVal;}
            double          GetUserVal() const {return UnitIndex::MetricToUser(metricVal);}

            void            SetMetricVal(const double& initmVal) {metricVal = initmVal;}
            void            SetUserVal(const double& inituVal) {metricVal = UnitIndex::UserToMetric(inituVal);}

            void            GetUserString(char  userStr[],
                                          int   maxLen) const;

            void            GetUserString(      char            userStr[],
                                                int             maxLen,
                                          const SC_RealFormat&  overrideFormat) const;

            void            GetUserString(      char            userStr[],
                                                int             maxLen,
                                          const SC_RealFormat*  poverrideFormat) const;


#ifdef _WIN32
            void            GetUserString(CString& userStr) const;
            void            GetUserString(CString& userStr,
                                          const SC_RealFormat&  overrideFormat) const;
                            // UI chores
            bool            IsWithinRange(const CString& userStr,
                                          const double& minMetric,
                                          const double& maxMetric);

#endif


    };

};


//  assuming that if it's included, we want to use it and no name clashes with other nSight
using namespace appUnits;



#endif // !C_APPUNITS_H

