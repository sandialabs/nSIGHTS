///////////////////////////////////////////////////////////////////////////////////
//
// DPO_EnterXYPS.h
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

#ifndef DPO_ENTERXYPS_H
#define DPO_ENTERXYPS_H

#include <genApp/U_MenuDPO.h>
#include <genApp/U_MenuPPO.h>       // for real format
#include <genApp/C_MenuBase.h>

#include <genClassPS/PS_UIGrid.h>
#include <genGrid/TextGridCell.h>

#include "DPO_EnterXY.h"


struct XYItem {
  XYItem() : x(nullReal), y(nullReal)
      {}
  XYItem(double x, double y) : x(x), y(y)
      {}
  double x;
  double y;
};
class XYArray : public T_SC_Array<XYItem>
{
public:
          XYArray();
  bool    XYOK(int i, double& xVal, double& yVal);
  void    SetAtGrow(int nIndex, XYItem& newVal);
};


class XYGridControl : public GridControl
{
  friend class XCell;
  friend class YCell;
  friend class DPO_EnterXYPS;
public:
  XYGridControl();

  void    SetXYRows(int nRows);

protected:
  virtual void FinishEdit(const char* szText, int nValue, int nLastChar);

  void    InsertXY(int nIndex);
  void    DeleteXY(int nIndex);
  void    UpdateModifiedRows();
  void    UpdateAllRows();

  void    PasteFromClipboardXY(int nIndexForm);
  void    CopyToClipboardXY();

protected:
  int     m_nCurrent;     // for popup actions

  SC_RealFormat   xForm;
  SC_RealFormat   yForm;

  static int m_nAllowance;

protected:
  //{{AFX_MSG(XYGridControl)
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  //}}AFX_MSG
  afx_msg void OnPopupCommand(UINT nMenuID);
  afx_msg void OnEditPaste();
  afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()
};


class DPO_EnterXYPS : public MenuBaseC {
  private:

      FuncObjRefUIC       inputXYObjRefUI;
      PushButton          updateFromInput;
      FormWidget          formatForm;
      RealFormatUIC       xFormatUI;
      RealFormatUIC       yFormatUI;

      enum                {maxEnterXY = 100};

      XYGridControl       m_grid;

  public:
                      DPO_EnterXYPS();

              void    InitMenu();

              void    MenuOpen(DPO_EnterXY&   inData);

              void    SetMenuData(DPO_EnterXY&    inData);
              bool    UIdataOK();
              void    GetMenuData(DPO_EnterXY&  outData);

      virtual void    MenuClear();


  private:
      void            ClearData();

      void            UpdateFromObj();
      static  void    UpdateFromObjCB(void* inObj);

      void            UpdateFormat();
      static  void    UpdateFormatCB(void* inObj);

              void    SetAllSensitive();
      static  void    SetAllSensitiveCB(void* inObj);
};


#endif // !DPO_ENTERXYPS_H

