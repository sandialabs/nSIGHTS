///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ExecuteProcessPS.h
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

#ifndef DPO_EXECUTEPROCESSPS_H
#define DPO_EXECUTEPROCESSPS_H

#include <genApp/C_MenuBase.h>
#include <genApp/U_MenuDPO.h>

#include "DPO_ExecuteProcess.h"


class DPO_ExecuteProcessPS : public MenuBaseC {

  private:
          BaseUIC                 triggerFrame;
          ToggleBoxWidget         useUpstreamTriggerUI;
          FuncObjRefComboBox      upstreamTriggerObjRefUI;

          FormWidget              topRow;
          BoolRadioBoxUIC         useSpecificEXEUI;

          BaseUIC                 optionFrame;
          FormWidget              optionRow1;
          ToggleBoxWidget         upstreamMustBeOKUI;
          ToggleBoxWidget         useCommandLineUI;
          ToggleBoxWidget         addExePar0UI;
          FormWidget              optionRow2;
          ToggleBoxWidget         setExeDirectoryUI;
          ToggleBoxWidget         executeOnApplyUI;
          ToggleBoxWidget         ignoreGlobalDoNotExecuteUI;
          ToggleBoxWidget         hideConsoleWindowUI;

          FormWidget              optionRow3;
          ToggleBoxWidget         addMPINodeNumberUI;

          FileEntryUIC            processToExecuteUI;
          TextEntryUIC            processCmdLineUI;
          FileEntryUIC            processExeDirectoryUI;

          NodeFileArrayUIC        mpiFilesUI;

  public:
                      DPO_ExecuteProcessPS();
                      ~DPO_ExecuteProcessPS(){};

              void    InitMenu();

              void    MenuOpen(DPO_ExecuteProcess&   inData);

              void    SetMenuData(DPO_ExecuteProcess&    inData);
              bool    UIdataOK();
              void    GetMenuData(DPO_ExecuteProcess&   outData);

              void    MenuClear();


  private:
              void    SetAllSensitive();
              static  void    SetAllSensitiveCB(void* inObj);

};

#endif // !DPO_EXECUTEPROCESSPS_H

