///////////////////////////////////////////////////////////////////////////////////
//
// G_DataCapture.h
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
//      Data capture specification vars and associated file I/O, error checking and
//      listing support.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef G_DATACAPTURE_H
#define G_DATACAPTURE_H

#include <genClass/C_GlobalFunc.h>
#include <genClass/DO_Real.h>
#include <genClass/DO_XYData.h>
#include <genClass/T_SC_AllocPtrArray.h>

#include <genListClass/C_ErrorListObj.h>
#include <genListClass/C_ListFactory.h>

#include <genApp/C_CfgFileObj.h>
#include <genApp/G_AppObj.h>

#include <objClass/DO_XYDataArray.h>
#include <osMain/C_OSListObj.h>

#include <Var/C_DataCapture.h>


namespace nsDataCapture  {

    class CaptureOutputFO : public FuncObjC {
        public:
            FuncObjRef  xyArrayObjRef;  // force linked to data capture table in G_DataCapture
            int         dcIndex;
            DO_XYData   xyDataDO;

        public:
                        CaptureOutputFO();
                        ~CaptureOutputFO();

            void        DoStatusChk(); 
            void        CalcOutput(FOcalcType  calcType);
    };

    // adds FO to basic spec
    class DataCaptureSpecGlob : public DataCaptureSpec {
        friend class DataCaptureStaticSupport;
        private:
            CaptureOutputFO         outputFO;

        public:
                DataCaptureSpecGlob() {};
                ~DataCaptureSpecGlob() {};

                FuncObjC*   GetObjRef() {return &outputFO;}
    };

    class DataCaptureArray: public T_SC_AllocPtrArray<DataCaptureSpecGlob> {
        public :
                        DataCaptureArray() : T_SC_AllocPtrArray<DataCaptureSpecGlob>() {}
                        ~DataCaptureArray();

    };

    // outputs captured data as XY data
    class DataCaptureOutput : public GlobalFunc {
        private:
            // output
            DO_XYDataArray      xyDataArrayDO;

        public:
            //  this is accessed by simulator
            DO_Real             staticPressureDO;

        public:
                                DataCaptureOutput();
                                ~DataCaptureOutput(){};

            void                DoStatusChk(); // reallocs if
            void                CalcOutput(FOcalcType  calcType);

            void                InitStaticPressure();  
                                // minor kluge to set static before fit calcs
                                // to avoid RameyC optimization problem

    };

    // globals
    extern DataCaptureArray     dataCaptureData;
    extern UnitIndex            productionRestartUnits;
    extern SC_DoubleArray       productionRestartTimes;
    extern DataCaptureOutput    capturedDataFO;
    extern SC_DoubleMatrix      capturedData;
    extern DC_XYDataArray       capturedDataXY;
    extern bool                 dataCaptureUIChange; // kluge to tell the UI to do recalcs


    //  for list of superposition data only
    typedef  T_SC_Array<DataCaptureSpec*> DataCapturePtrArray;


    class DataCaptureListing : public OSListObj {
        public: 
                        DataCaptureListing();
                        ~DataCaptureListing() {};

            virtual void  CreateListing();

        private:
            void        ListSuperComponent(const SuperComponent& outSuper);
            void        ListDataCapture(const DataCaptureSpec& outDC);
    };

    class DataCaptureErrorListing : public ErrorListObjC {
        public: 
                                DataCaptureErrorListing();
                                ~DataCaptureErrorListing() {};

                virtual void  CreateListing();

    };

    extern DataCaptureErrorListing  dataCaptureErrorListing;


    class DataCaptureStaticSupport : public ListFactory,
                                    public CfgFileObj
    {
        friend class CaptureOutputFO;

       private:
                // object versioning support
            static const char*      groupHeader;
            static const int        majorVersion;
            static const int        minorVersion;

            static appFuncObjGlobals::AppFuncArray capturedObj;

        public:
                        DataCaptureStaticSupport();     // constructor initializes all parameters and vars
                        ~DataCaptureStaticSupport() {};

        static void     InitAppGlobals();   // must be called once after creation and before use
        static void     SetToInitial();     // for file New
        static void     SetForFlags();      // modifies settings
        static bool     DataCaptureOK();

        // misc static routines for DataCapture data
        static int      GetNDataCapture() {return dataCaptureData.Size();}
        static bool     SetDataCaptureDesigs();
        static void     DataCaptureCleanup();
        static void     CheckAndAllocDataCapture(int dcIndex);

        //  autosetup support -- assumes DataCaptureOK() called
        //  gets list of all IsWell() && IsPressure() IDs,  
        //  wellID[0] is assumed to be pumping well
        static bool         GetWellIDs(SC_StringArray& wellIDs);
        static FuncObjC*    GetWellPFO(const char* wellID);
        static FuncObjC*    GetWellQFO(const char* wellID);


                        // list of superposition
        static bool     GetSuperpositionCapture(DataCapturePtrArray& superpositionCapture);


        virtual void    WriteToFile();
        virtual bool    ReadFromFile();

        virtual ListObjC*   CreateListObject() {return new DataCaptureListing();}
        virtual ErrorListObjC*  GetErrorListObject() {return &dataCaptureErrorListing;}

        private:
            void        WriteSuperComponent(SuperComponent& outSuper);
            void        WriteDataCapture(DataCaptureSpecGlob& outDC);

            void        ReadSuperComponent(SuperComponent& inSuper);
            void        ReadDataCapture(DataCaptureSpecGlob& inDC);

    };


    extern  DataCaptureStaticSupport   dataCapture;  // the one and only


};

using namespace nsDataCapture;


#endif // G_DATACAPTURE_H
