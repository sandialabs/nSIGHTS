///////////////////////////////////////////////////////////////////////////////////
//
// DPO_EnterTablePS.h
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

#ifndef DPO_ENTERTABLEPS_H
#define DPO_ENTERTABLEPS_H

#include <genApp/U_MenuDPO.h>
#include <genApp/U_MenuPPO.h>       // for real format
#include <genApp/C_MenuBase.h>

#include <genClassPS/PS_UIGrid.h>
#include <genGrid/TextGridCell.h>

#include "DPO_EnterTable.h"


class TableGridControl : public GridControl
{
  friend class ValueCell;
  friend class HeaderCell;
  friend class DPO_EnterTablePS;

  private:
      DC_TableData&   gTable;

  protected:
      int             m_nCurrRow;     // for popup actions
      int             m_nCurrCol;     // for popup actions
      SC_RealFormat&  tableFormat;

  public:
                      TableGridControl(DC_TableData& currTable, SC_RealFormat& currForm);

      void            UpdateSize();

  protected:
      virtual void    FinishEdit(const char* szText, int nValue, int nLastChar);

      void            InsertRow(int rowIndex);
      void            DeleteRow(int rowIndex);
      void            InsertCol(int colIndex);
      void            DeleteCol(int colIndex);

      void            UpdateModifiedRows();
      void            UpdateModifiedCols();
      void            UpdateAll();
      void            UpdateColSize();

      void            PasteFromClipboardTable(int startRow);
      void            CopyToClipboardTable();

  protected:
      //{{AFX_MSG(TableGridControl)
      afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
      afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
      //}}AFX_MSG
      afx_msg void OnPopupCommand(UINT nMenuID);
      afx_msg void OnEditPaste();
      afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
      afx_msg void OnEditCopy();
      DECLARE_MESSAGE_MAP()
  };


class DPO_EnterTablePS : public MenuBaseC {
  public:
      enum                {nextraRows = 20};

  private:
      FuncObjRefUIC       inputTableObjRefUI;
      PushButton          updateFromInput;

      FormWidget          topRowForm;
      RealFormatUIC       tableFormatUI;

      DC_TableData        currTable;
      SC_RealFormat       tableFormat;
      TableGridControl    gridUI;

  public:
                      DPO_EnterTablePS();

              void    InitMenu();

              void    MenuOpen(DPO_EnterTable&   inData);

              void    SetMenuData(DPO_EnterTable&    inData);
              bool    UIdataOK();
              void    GetMenuData(DPO_EnterTable&  outData);

      virtual void    MenuClear();


  private:
              void    ClearData();
              void    CopyFrom(const DC_TableData& inTable);
              void    CopyTo(DC_TableData& outTable);

              void    UpdateFromObj();
      static  void    UpdateFromObjCB(void* inObj);

              void    UpdateFormat();
      static  void    UpdateFormatCB(void* inObj);

              void    SetAllSensitive();
      static  void    SetAllSensitiveCB(void* inObj);


};


#endif // !DPO_ENTERTABLEPS_H

