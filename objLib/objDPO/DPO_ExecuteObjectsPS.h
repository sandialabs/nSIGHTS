///////////////////////////////////////////////////////////////////////////////////
//
// DPO_ExecuteObjectsPS.h
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

#ifndef DPO_EXECUTEOBJECTSPS_H
#define DPO_EXECUTEOBJECTSPS_H

#include <genApp/C_MenuBase.h>

#include "DPO_ExecuteObjects.h"


class DPO_ExecuteObjectsPS : public MenuBaseC {
  private:

      class ObjectDataUIC {
          public:
              ToggleBoxWidget         executeObjectUI;
              FuncObjRefComboBox      execObjectRefUI;
              OptionChooserWidget     execDownstreamUI;
              ToggleBoxWidget         stopOnErrorUI;
              ToggleBoxWidget         statusMustBeOKUI;
              FormWidget              outsideForm;
              HorizontalSeparator     hSep;

          public:

                          ObjectDataUIC();
              void        InitObjectData();  // parent is outsideForm
              void        SetObjectData(const DFO_ExecuteObjects::ObjectData& tableData);
              bool        ObjectDataOK(bool& prevOK);
              void        GetObjectData(DFO_ExecuteObjects::ObjectData& tableData);
              void            SetAllSensitive();
          private:
              static  void    SetAllSensitiveCB(void* inObj);
          };

      class RealScrolledArea : public ScrolledForm {
          protected:
              CSize       SetCalculateFormSize();
      };

  private:
          ObjectDataUIC           objectDataUI[DFO_ExecuteObjects::maxExecObject];
          BaseUIC                 objectFrame;
          RealScrolledArea        scrollArea;
          FormWidget              labelRow;
          LabelWidget             objLabel;
          LabelWidget             downstreamLabel;
          LabelWidget             stopLabel;
          LabelWidget             statusOKLabel;

          BaseUIC                 triggerFrame;
          ToggleBoxWidget         useUpstreamTriggerUI;
          FuncObjRefComboBox      upstreamTriggerObjRefUI;


  public:
                      DPO_ExecuteObjectsPS();
                      ~DPO_ExecuteObjectsPS(){};

              void    InitMenu();

              void    MenuOpen(DPO_ExecuteObjects&   inData);

              void    SetMenuData(DPO_ExecuteObjects&    inData);
              bool    UIdataOK();
              void    GetMenuData(DPO_ExecuteObjects&   outData);

              void    MenuClear();


  private:
              void    SetAllSensitive();
              static  void    SetAllSensitiveCB(void* inObj);

};

#endif // !DPO_EXECUTEOBJECTSPS_H

