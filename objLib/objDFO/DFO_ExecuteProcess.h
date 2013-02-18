///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ExecuteProcess.h
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
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef DFO_EXECUTEPROCESS_H
#define DFO_EXECUTEPROCESS_H

#include <genClass/C_FuncObj.h>
#include <genClass/C_MPISupport.h>

class DFO_ExecuteProcess : public FuncObjC {
    public:

        static bool globalDoNotExecute; // to stop execute on load or Apply
                                                // in paCalc tied to suspend complex
        static bool globalHideConsoleWindow;    // to suppress console window display
                                        // in paCalc tied to run minimized

        bool            useUpstreamTrigger;
        FuncObjRef      upstreamTriggerObjRef;
        bool            upstreamMustBeOK;
        bool            executeOnApply;     // local control 
        bool            ignoreGlobalDoNotExecute;
        bool            hideConsoleWindow;  // local control 

        bool            useSpecificEXE;
        bool            useCommandLine;
        bool            addExePar0;    // most exes expect par0 to be exec name
                                       // valid for SpecificExe and useCommandLine
        bool            setExeDirectory;
        char            processToExecute[stdFileStrLen];
        char            processCmdLine[stdFileStrLen];
        char            processExeDirectory[stdFileStrLen];

        //  MPI node naming support 
        NodeFileArray   mpiFiles;
        bool            addMPINodeNumber;

    private:
        bool            doInfoMsg;
    public:
                        DFO_ExecuteProcess();
                        DFO_ExecuteProcess(const DFO_ExecuteProcess& a);
                        ~DFO_ExecuteProcess();

        DFO_ExecuteProcess& operator= (const DFO_ExecuteProcess& a);

        void            SetDefault();

                        //  FuncObjC virtuals
        void            DoStatusChk();
        void            CalcOutput(FOcalcType  calcType);

        virtual bool    UsesAsInput(const FuncObjC* inputObj) const;
        virtual void    IsBeingDeleted(FuncObjC* delObj);
        virtual void    UpdateFuncObjRefs();


    private:
        void            InitCommon();                             // common to normal and copy constructor
        void            MPIInit(); // can't be in InitCommon 
        void            LocalCopy(const DFO_ExecuteProcess& a);  // common to copy constructor and = operator

        void            DoExecute();
#ifndef _WINDOWS
            void            GetErrorMessage();      // for Linux/gcc 
#endif
            void            ExecInfoMsg(const char* infoMsg);
            void            ExecInfoMsg(const char* infoMsg,
                                        const char* msgDetails);
            void            ExecInfoMsg(const char* infoMsg,
                                        const char* msgDetail1,
                                        const char* msgDetail2);

    };

#endif // DFO_EXECUTEPROCESS_H

