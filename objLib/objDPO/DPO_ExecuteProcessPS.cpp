///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ExecuteProcessPS.cpp
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
//    I/O and user interface for respective ../objDFO/DFO_XXX functional objects
//
//
///////////////////////////////////////////////////////////////////////////////////

#include "ObjLibAfx.h"

#include <genClass/U_Str.h>

#include "DPO_ExecuteProcess.h"
#include "DPO_ExecuteProcessPS.h"



DPO_ExecuteProcessPS::DPO_ExecuteProcessPS()
: upstreamTriggerObjRefUI(typeid(FuncObjC))
{
    mpiFilesUI.nodeFileID += "EXE file";
    mpiFilesUI.nodeFileID += "command line";
    mpiFilesUI.nodeFileID += "EXE directory";
}


void DPO_ExecuteProcessPS::SetAllSensitive()
{
    upstreamTriggerObjRefUI.SetSensitive(useUpstreamTriggerUI.GetValue());
    upstreamMustBeOKUI.SetSensitive(useUpstreamTriggerUI.GetValue());

    useCommandLineUI.SetSensitive(useSpecificEXEUI.GetBoolChoice());

    bool useExe = useSpecificEXEUI.GetBoolChoice();
    processToExecuteUI.SetSensitive(useExe);
    mpiFilesUI.nodeFileUI[0].SetSensitive(useExe);

    bool cmdLine = (!useSpecificEXEUI.GetBoolChoice()) ||
                                    useCommandLineUI.GetValue();
    processCmdLineUI.SetSensitive(cmdLine);
    mpiFilesUI.nodeFileUI[1].SetSensitive(cmdLine);


    addExePar0UI.SetSensitive(useSpecificEXEUI.GetBoolChoice() &&
                            useCommandLineUI.GetValue());

    bool exeDir = setExeDirectoryUI.GetValue();
    processExeDirectoryUI.SetSensitive(exeDir);
    mpiFilesUI.nodeFileUI[2].SetSensitive(exeDir);
}

void DPO_ExecuteProcessPS::SetAllSensitiveCB(void* inObj)
{
    static_cast<DPO_ExecuteProcessPS*>(inObj)->SetAllSensitive();
}

void DPO_ExecuteProcessPS::InitMenu()

{
    MenuBaseC::InitMenu();

    ArgListC args;

    args.AllFormAttach();
    triggerFrame.InitFramedUI("Upstream Trigger ?", URform.GetWidget(), args);

    args.StdToggleSetup(2);
    useUpstreamTriggerUI.StdToggleInit(" ", triggerFrame.GetForm(), args);
    useUpstreamTriggerUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(85);
    upstreamTriggerObjRefUI.InitWidget(triggerFrame.GetForm(), args);

    args.StdFormAttach();
    topRow.InitWidget(PPform.GetWidget(), args);

    args.StdSplitSetup(20);
    useSpecificEXEUI.PreInitSetUp("CMD.EXE", "Specific EXE", false);
    useSpecificEXEUI.InitFramedUI("Process Type", topRow.GetWidget(), args);
    useSpecificEXEUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdSplitFinish(80);
    optionFrame.InitFramedUI("Options", topRow.GetWidget(), args);

    args.StdFormAttach();
    optionRow1.InitWidget(optionFrame.GetForm(), args);

    args.StdToggleSetup(2);
    upstreamMustBeOKUI.StdToggleInit("trigger must be OK", optionRow1.GetWidget(), args);

    args.StdToggleSetup(33);
    useCommandLineUI.StdToggleInit("use parameters", optionRow1.GetWidget(), args);
    useCommandLineUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(68);
    addExePar0UI.StdToggleInit("EXE is parameter 0", optionRow1.GetWidget(), args);

    args.StdTopObjectAttach(optionRow1.GetWidget());
    optionRow2.InitWidget(optionFrame.GetForm(), args);

    args.StdToggleSetup(2);
    setExeDirectoryUI.StdToggleInit("set directory", optionRow2.GetWidget(), args);
    setExeDirectoryUI.AddOtherObjectCallback(SetAllSensitiveCB, this);

    args.StdToggleSetup(33);
    executeOnApplyUI.StdToggleInit("execute on Apply", optionRow2.GetWidget(), args);

    args.StdToggleSetup(68);
    ignoreGlobalDoNotExecuteUI.StdToggleInit("ignore suspend calcs", optionRow2.GetWidget(), args);

    args.StdTopObjectAttach(optionRow2.GetWidget());
    optionRow3.InitWidget(optionFrame.GetForm(), args);

    args.StdToggleSetup(2);
    hideConsoleWindowUI.StdToggleInit("hide console window", optionRow3.GetWidget(), args);

    args.StdToggleSetup(33);
    addMPINodeNumberUI.StdToggleInit("add MPI Node# to cmd line", optionRow3.GetWidget(), args);


    args.StdTopObjectAttach(topRow.GetWidget());
    processToExecuteUI.InitFramedUI("Process to Execute", PPform.GetWidget(), args);

    args.StdTopObjectAttach(processToExecuteUI);
    processCmdLineUI.AttachObjectToFrame();
    processCmdLineUI.InitFramedUI("Command Line", PPform.GetWidget(), args);

    args.StdTopObjectAttach(processCmdLineUI);
    processExeDirectoryUI.InitFramedUI("Directory to Execute In", PPform.GetWidget(), args);
    processExeDirectoryUI.SetFileType(true);

    args.StdTopObjectAttach(processExeDirectoryUI);
    mpiFilesUI.StdInit(PPform.GetWidget(), args);
}


void DPO_ExecuteProcessPS::MenuOpen(DPO_ExecuteProcess&   inData)
{
    MenuBaseC::MenuOpen(false, true);
    upstreamTriggerObjRefUI.StdMenuInit(inData.upstreamTriggerObjRef);
}


void DPO_ExecuteProcessPS::SetMenuData(DPO_ExecuteProcess&    inData)
{
    MenuBaseC::SetMenuData(inData);

    useUpstreamTriggerUI.SetValue(inData.useUpstreamTrigger);
    upstreamTriggerObjRefUI.SetFuncObjRef(inData.upstreamTriggerObjRef);
    useUpstreamTriggerUI.SetValue(inData.useUpstreamTrigger);

    executeOnApplyUI.SetValue(inData.executeOnApply);
    ignoreGlobalDoNotExecuteUI.SetValue(inData.ignoreGlobalDoNotExecute);
    hideConsoleWindowUI.SetValue(inData.hideConsoleWindow);
    addExePar0UI.SetValue(inData.addExePar0);

    useSpecificEXEUI.SetBoolChoice(inData.useSpecificEXE);
    useCommandLineUI.SetValue(inData.useCommandLine);
    setExeDirectoryUI.SetValue(inData.setExeDirectory);
    addMPINodeNumberUI.SetValue(inData.addMPINodeNumber);

    processToExecuteUI.SetFieldString(inData.processToExecute);
    processCmdLineUI.SetFieldString(inData.processCmdLine);
    processExeDirectoryUI.SetFieldString(inData.processExeDirectory);

    mpiFilesUI.SetNodeFileArray(inData.mpiFiles);

    SetAllSensitive();
}

bool DPO_ExecuteProcessPS::UIdataOK()
{
    MenuBaseC::UIdataOK();
    if (useUpstreamTriggerUI.GetValue())
        upstreamTriggerObjRefUI.FuncObjRefOK(allOK, "Upstream trigger");

    if (useSpecificEXEUI.GetBoolChoice())
        processToExecuteUI.FieldFileOK(allOK, "Process to execute", false, false);

    if ((!useSpecificEXEUI.GetBoolChoice()) || useCommandLineUI.GetValue())
        processCmdLineUI.FieldStringOK(allOK, "Command line string");

    if (setExeDirectoryUI.GetValue())
        processExeDirectoryUI.FieldPathOK(allOK, "Execution directory", false);

    return allOK;
}

void DPO_ExecuteProcessPS::GetMenuData(DPO_ExecuteProcess&    outData)
{
    MenuBaseC::GetMenuData(outData);

    outData.useUpstreamTrigger = useUpstreamTriggerUI.GetValue();
    if (outData.useUpstreamTrigger)
    {
        outData.upstreamTriggerObjRef = upstreamTriggerObjRefUI.GetFuncObjRef();
        outData.upstreamMustBeOK = upstreamMustBeOKUI.GetValue();
    }

    outData.useSpecificEXE = useSpecificEXEUI.GetBoolChoice();

    if (outData.useSpecificEXE)
    {
        outData.useCommandLine = useCommandLineUI.GetValue();
        processToExecuteUI.GetFieldFile(outData.processToExecute);
    }

    if ((!outData.useSpecificEXE) || outData.useCommandLine)
        processCmdLineUI.GetFieldString(outData.processCmdLine, stdFileStrLen);

    outData.setExeDirectory = setExeDirectoryUI.GetValue();
    if (outData.setExeDirectory)
        processExeDirectoryUI.GetFieldString(outData.processExeDirectory, stdFileStrLen);

    outData.executeOnApply = executeOnApplyUI.GetValue();
    outData.ignoreGlobalDoNotExecute = ignoreGlobalDoNotExecuteUI.GetValue();
    outData.hideConsoleWindow = hideConsoleWindowUI.GetValue();
    outData.addExePar0 = addExePar0UI.GetValue();
    outData.addMPINodeNumber = addMPINodeNumberUI.GetValue();

    mpiFilesUI.GetNodeFileArray(outData.mpiFiles);

}

void DPO_ExecuteProcessPS::MenuClear()
{
    useUpstreamTriggerUI.SetValue(false);
    SetAllSensitive();
}

