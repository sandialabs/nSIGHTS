///////////////////////////////////////////////////////////////////////////////////
//
// DFO_ExecuteProcess.cpp
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

#ifdef _WINDOWS
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/errno.h>
#endif

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_File.h>

#include <objDFO/DFO_ExecuteProcess.h>

#include <genClass/SC_StringArray.h>

bool DFO_ExecuteProcess::globalDoNotExecute = false;
bool DFO_ExecuteProcess::globalHideConsoleWindow = false;

DFO_ExecuteProcess :: DFO_ExecuteProcess()
    : FuncObjC("Execute Process")
{
    useUpstreamTrigger = false;
    upstreamMustBeOK = false;
    executeOnApply = true;
    ignoreGlobalDoNotExecute = false;
    addExePar0 = true;

    useSpecificEXE = true;
    useCommandLine = true;
    setExeDirectory = false;
    hideConsoleWindow = false;

    CopyString(processToExecute, "app.exe", stdFileStrLen);
    SetToNull(processCmdLine);
    CopyString(processExeDirectory, "C:\\Temp", stdFileStrLen);

    addMPINodeNumber = false;
    MPIInit();
    InitCommon();
}

DFO_ExecuteProcess::DFO_ExecuteProcess(const DFO_ExecuteProcess& a) : FuncObjC(a)
{
    MPIInit();
    LocalCopy(a);
    InitCommon();
}

DFO_ExecuteProcess :: ~DFO_ExecuteProcess ()
{
}

void DFO_ExecuteProcess::MPIInit()
{
    mpiFiles += NodeFile(processToExecute, "exe file");
    mpiFiles += NodeFile(processCmdLine, "cmd line");
    mpiFiles += NodeFile(processExeDirectory, "exe directory");
}


void DFO_ExecuteProcess::InitCommon()
{
    DoStatusChk();
}

DFO_ExecuteProcess& DFO_ExecuteProcess::operator= (const DFO_ExecuteProcess& a)
{
    if (&a != this)
        LocalCopy(a);
    return *this;
}

bool DFO_ExecuteProcess::UsesAsInput(const FuncObjC* inputObj) const
{
    return useUpstreamTrigger && (upstreamTriggerObjRef.objRef == inputObj);
}

void  DFO_ExecuteProcess::LocalCopy(const DFO_ExecuteProcess& a)
{
    useUpstreamTrigger      = a.useUpstreamTrigger;
    upstreamMustBeOK        = a.upstreamMustBeOK;
    upstreamTriggerObjRef   = a.upstreamTriggerObjRef;
    executeOnApply          = a.executeOnApply;
    hideConsoleWindow       = a.hideConsoleWindow;
    useSpecificEXE          = a.useSpecificEXE;
    addExePar0              = a.addExePar0;
    useCommandLine          = a.useCommandLine;
    setExeDirectory         = a.setExeDirectory;

    CopyString(processToExecute, a.processToExecute, stdFileStrLen);
    CopyString(processCmdLine, a.processCmdLine, stdFileStrLen);
    CopyString(processExeDirectory, a.processExeDirectory, stdFileStrLen);

    mpiFiles[0].nodeBasedFile = a.mpiFiles[0].nodeBasedFile;
    mpiFiles[1].nodeBasedFile = a.mpiFiles[1].nodeBasedFile;
    mpiFiles[2].nodeBasedFile = a.mpiFiles[2].nodeBasedFile;
}

void DFO_ExecuteProcess::IsBeingDeleted(FuncObjC* delObj)
{
    if (upstreamTriggerObjRef.objRef == delObj)
        upstreamTriggerObjRef = FuncObjRef();
}

void DFO_ExecuteProcess::UpdateFuncObjRefs()
{
    PerformUpdate(upstreamTriggerObjRef);
}


void DFO_ExecuteProcess::SetDefault()
{
    useUpstreamTrigger = false;
}

void  DFO_ExecuteProcess:: DoStatusChk()
{
    // base class check
    FuncObjC::DoStatusChk();

    //check input process
    if (useSpecificEXE)
    {
        mpiFiles[0].UpdateNodeFile(true);
        mpiFiles[0].CheckNodeFile(*this);
        if (!CheckFileExists(processToExecute))
            SetObjErrMsg("Specified executable must exist");
    }

    if ((!useSpecificEXE) || useCommandLine)
    {
        mpiFiles[1].UpdateNodeFile(false);
        mpiFiles[1].CheckNodeFile(*this);
        if (IsNull(processCmdLine))
            SetObjErrMsg("Command line must be set");
    }

    if (setExeDirectory)
    {
        mpiFiles[2].UpdateNodeFile(true, true);
        mpiFiles[2].CheckNodeFile(*this, true);
        if (!mpiFiles[2].CheckExists(*this, true))
            SetObjErrMsg("Execution directory must exist");
    }

    if (useUpstreamTrigger && upstreamMustBeOK)
    {
        if (upstreamTriggerObjRef.objRef == NULL)
            SetObjErrMsg("Upstream trigger not set");
        else
            if (upstreamTriggerObjRef.objRef->StatusNotOK())
                SetObjErrMsg("Upstream trigger is not OK");
    }
}


void DFO_ExecuteProcess:: CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
    {
        GenAppInfoMsg("ExecProc", objStatusMsg);
        return;
    }

    if (globalDoNotExecute && (!ignoreGlobalDoNotExecute))
        return;

    if (calcType == foc_Apply)
    {
        if (!executeOnApply)
            return;
    }
    else
    {
        // no execute on load or port unless by trigger
        if (!useUpstreamTrigger)
            return;
    }

    doInfoMsg = (calcType == foc_Apply);
    DoExecute();
}

void DFO_ExecuteProcess::ExecInfoMsg(const char* infoMsg)
{
    if (doInfoMsg)
        GenAppInfoMsg("ExecProc", infoMsg);
}

void DFO_ExecuteProcess::ExecInfoMsg(const char* infoMsg,
                                        const char* msgDetails)
{
    char fullMsg[120];
    MakeString(fullMsg, infoMsg, ":", msgDetails, 120);
    ExecInfoMsg(fullMsg);
}

void DFO_ExecuteProcess::ExecInfoMsg(const char* infoMsg,
                                        const char* msgDetail1,
                                        const char* msgDetail2)
{
    char fullMsg[120];
    MakeString(fullMsg, infoMsg, ":", msgDetail1, ":", msgDetail2, 120);
    ExecInfoMsg(fullMsg);
}

#ifdef _WINDOWS

void DFO_ExecuteProcess::DoExecute()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    const char* pExe = NULL;
    if (useSpecificEXE)
        pExe = processToExecute;

    char* pCmdLine = NULL;
    char tempCmdLine[stdFileStrLen+1];
    if ((!useSpecificEXE) || useCommandLine)
    {
        if (useSpecificEXE && useCommandLine && addExePar0)
            MakeString(tempCmdLine, processToExecute, " ", processCmdLine, stdFileStrLen);
        else
            CopyString(tempCmdLine, processCmdLine, stdFileStrLen);

        if (addMPINodeNumber && (NodeFile::mpiNodeRank >= 0))
        {
            ConcatString(tempCmdLine, "  ", stdFileStrLen);
            ConcatInt(NodeFile::mpiNodeRank, tempCmdLine, stdFileStrLen);
        }
        pCmdLine = tempCmdLine;
    }

    const char* pCD = NULL;
    if (setExeDirectory)
        pCD  = processExeDirectory;

    // Start the child process.

    ExecInfoMsg("Executing", pExe, pCmdLine);

    DWORD dwCreationFlags = HIGH_PRIORITY_CLASS;
    if (hideConsoleWindow || globalHideConsoleWindow)
    {
        dwCreationFlags = CREATE_NO_WINDOW | HIGH_PRIORITY_CLASS;
    }

    if( !CreateProcess(
        pExe,
        pCmdLine,       // Command line.
        NULL,             // Process handle not inheritable.
        NULL,             // Thread handle not inheritable.
        FALSE,            // Set handle inheritance to FALSE.
        dwCreationFlags,  // creation flags.
        NULL,             // Use parent's environment block.
        pCD,
        &si,              // Pointer to STARTUPINFO structure.
        &pi )             // Pointer to PROCESS_INFORMATION structure.
    )
    {
        ExecInfoMsg("ExecProcess exe", pExe);
        ExecInfoMsg("ExecProcess cmd", pCmdLine);
        ExecInfoMsg("ExecProcess cd", pCD);
        SetObjErrMsg("CreateProcess failed.");
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE );

    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    ExecInfoMsg("Process appears to have executed OK");
}

#else //_WINDOWS

void DFO_ExecuteProcess::DoExecute()
{
    pid_t pID = vfork();
    if (pID == 0)
    {
        ExecInfoMsg("Fork created OK.");
        char* pExe = NULL;

        const int maxArg = 20;
        char* argv[maxArg];
        int argc = 0;

        if (useSpecificEXE)
            pExe = processToExecute;
        else
        {
            pExe = "/bin/sh";
            argv[argc++] = "sh";
            argv[argc++] = "-c";
        }

        char tempCmdLine[stdFileStrLen+1];

        SC_StringArray nextStr;
        nextStr.SetStringLen(stdFileStrLen);
        nextStr.Alloc(maxArg);
        if ((!useSpecificEXE) || useCommandLine)
        {
            if (useSpecificEXE && useCommandLine && addExePar0)
                argv[argc++] = processToExecute;

            CopyString(tempCmdLine, processCmdLine, stdFileStrLen);
            int count = 0;
            while (ParseGetNextString(tempCmdLine, nextStr[count], stdFileStrLen))
            {
                argv[argc++] = nextStr[count++];
                if (argc >= maxArg)
                    GenAppInfoMsg("ExecProcError", "Too many arguments, increase maxArg.");
            }

            if (addMPINodeNumber && (NodeFile::mpiNodeRank >= 0))
            {
                IntToString(NodeFile::mpiNodeRank, nextStr[count], stdFileStrLen);
                argv[argc++] = nextStr[count++];

                if (argc >= maxArg)
                    GenAppInfoMsg("ExecProcError", "Too many arguments, increase maxArg");
            }

        }

        argv[argc] = NULL;

        //check args - output
        ExecInfoMsg("Exename", pExe);
        for (int i = 0; i < argc; i++)
            ExecInfoMsg("Arg", argv[i]);

        if (setExeDirectory)
        {
            ExecInfoMsg("Setting Env variables", processExeDirectory);
            if(chdir(processExeDirectory) != 0)
                ExecInfoMsg("Chdir", "Not okay");
        }
        int check = execv(pExe, argv);

        doInfoMsg = true;
        ExecInfoMsg("Exec failed - return from execv");
        GetErrorMessage();

        _exit(0);
    }
    else if (pID < 0)
    {
        SetObjErrMsg("fork failed.");
        return;
    }
    else
    {
        ExecInfoMsg("Parent waiting...");
        int status;
        pid_t res = waitpid(pID, &status, 0);
        ExecInfoMsg("Parent done waiting.");

        if (res > 0)
        {
            if (WIFEXITED(status))
                ExecInfoMsg("Process appears to have executed OK");
        }
        else if (res < 0)
        {
            if (errno == ECHILD)
            {
                ExecInfoMsg("ERROR", "Child does not exist.");
                SetObjErrMsg("Child does not exist.");
                return;
            }
            else
            {
                ExecInfoMsg("ERROR", "Bad argument passed to waitpid");
                SetObjErrMsg("Bad argument passed to waitpid");
                return;
            }
        }
        return;
    }
}

void DFO_ExecuteProcess::GetErrorMessage()
{
    switch (errno)
    {
#ifdef EACCES
        case EACCES :
        {
            ExecInfoMsg("ERROR", "EACCES Permission denied");
            break;
        }
#endif

#ifdef EPERM
        case EPERM :
        {
            ExecInfoMsg("ERROR", "EPERM Not super-user");
            break;
        }
#endif

#ifdef E2BIG
        case E2BIG :
        {
            ExecInfoMsg("ERROR", "E2BIG Arg list too long");
            break;
        }
#endif

#ifdef ENOEXEC
        case ENOEXEC :
        {
            ExecInfoMsg("ERROR", "ENOEXEC Exec format error");
            break;
        }
#endif

#ifdef EFAULT
        case EFAULT :
        {
            ExecInfoMsg("ERROR", "EFAULT Bad address");
            break;
        }
#endif

#ifdef ENAMETOOLONG
        case ENAMETOOLONG :
        {
            ExecInfoMsg("ERROR", "ENAMETOOLONG path name is too long");
            break;
        }
#endif

#ifdef ENOENT
        case ENOENT :
        {
            ExecInfoMsg("ERROR", "ENOENT No such file or directory");
            break;
        }
#endif

#ifdef ENOMEM
        case ENOMEM :
        {
            ExecInfoMsg("ERROR", "ENOMEM Not enough core");
            break;
        }
#endif

#ifdef ENOTDIR
        case ENOTDIR :
        {
            ExecInfoMsg("ERROR", "ENOTDIR Not a directory");
            break;
        }
#endif

#ifdef ELOOP
        case ELOOP :
        {
            ExecInfoMsg("ERROR", "ELOOP Too many symbolic links");
            break;
        }
#endif

#ifdef ETXTBSY
        case ETXTBSY :
        {
            ExecInfoMsg("ERROR - ETXTBSY Text file busy");
            break;
        }
#endif

#ifdef EIO
        case EIO :
        {
            ExecInfoMsg("ERROR - EIO I/O error");
            break;
        }
#endif

#ifdef ENFILE
        case ENFILE :
        {
            ExecInfoMsg("ERROR - ENFILE Too many open files in system");
            break;
        }
#endif

#ifdef EINVAL
        case EINVAL :
        {
            ExecInfoMsg("ERROR - EINVAL Invalid argument");
            break;
        }
#endif

#ifdef EISDIR
        case EISDIR :
        {
            ExecInfoMsg("ERROR - EISDIR Is a directory");
            break;
        }
#endif

#ifdef ELIBBAD
        case ELIBBAD :
        {
            ExecInfoMsg("ERROR - ELIBBAD Accessing a corrupted shared lib");
            break;
        }
#endif
    }
}
#endif //_WINDOWS

