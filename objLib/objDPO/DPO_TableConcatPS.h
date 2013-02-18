///////////////////////////////////////////////////////////////////////////////////
//
// DPO_TableConcatPS.h
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

#ifndef DPO_TABLECONCATPS_H
#define DPO_TABLECONCATPS_H

#include <genClass/T_SC_AllocPtrArray.h>
#include <genApp/C_MenuBase.h>

#include "DPO_TableConcat.h"


class DPO_TableConcatPS : public MenuBaseC {
  private:

      class TableInputDataUIC {
          public:
              FuncObjRefComboBox  tableObjRefUI;
              ToggleBoxWidget     doTableUI;
              FormWidget          outsideForm;
              HorizontalSeparator hSep;

                          TableInputDataUIC(const type_info& tblType);
                          TableInputDataUIC();
              void        InitTableInputData();  // parent is outsideForm
              void        SetTableInputData(const DFO_TableConcat::TableInputData& tableInputData);
              bool        TableInputDataOK(bool& prevOK);
              void        GetTableInputData(DFO_TableConcat::TableInputData& tableInputData);
              void        SetAllSensitive();
          private:

              static  void    SetAllSensitiveCB(void* inObj);
      };

      class TableScrolledArea : public ScrolledForm {
          protected:
              CSize       SetCalculateFormSize();
      };

  protected:

      FuncObjRefUIC           inputTableObjRefUI;
      FormWidget              topRowForm;
      BoolRadioBoxUIC         concatRowsUI;
      BaseUIC                 optionFrame;
      ToggleBoxWidget         skipFirstColumnUI;

      T_SC_AllocPtrArray<TableInputDataUIC>   tableInputDataUI;
      BaseUIC                 tableColFrame;
      TableScrolledArea       scrollArea;


  public:
                      DPO_TableConcatPS();
                      ~DPO_TableConcatPS();

              void    InitMenu();

              void    MenuOpen(DPO_TableConcat&   inData);

              void    SetMenuData(DPO_TableConcat&    inData);
              bool    UIdataOK();
              void    GetMenuData(DPO_TableConcat&   outData);

  private:
              void    SetAllSensitive();
      static  void    SetAllSensitiveCB(void* inObj);


};

#endif // !DPO_TABLECONCATPS_H

