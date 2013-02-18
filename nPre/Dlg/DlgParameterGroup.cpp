///////////////////////////////////////////////////////////////////////////////////
//
// DlgParameterGroup.cpp
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
//      nPre dialog.  Why are there class definitions in this file?!
//
///////////////////////////////////////////////////////////////////////////////////

// DlgParameterGroup.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"

#include "DlgParameterGroup.h"

#include "ParameterFileDlg.h"
#include "VaryVarUnitsDlg.h"

#include <genGrid/TextGridCell.h>
#include <genGrid/ComboGridCell.h>
#include <genUnits/UnitGridCell.h>
#include "SampVarUnitsDlg.h"
#include "OptVarUnitsDlg.h"

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>

#include <osMain/G_Vary.h>

#include <Var/G_Control.h>
#include <Var/G_Parameter.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgParameterGroup dialog

IMPLEMENT_DYNCREATE(DlgParameterGroup,CGenericDialog)

DlgParameterGroup::DlgParameterGroup(CWnd* pParent /*=NULL*/) :
    CGenericDialog(DlgParameterGroup::IDD, pParent)
{
    //{{AFX_DATA_INIT(DlgParameterGroup)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void DlgParameterGroup::DoDataExchange(CDataExchange* pDX)
{
    CGenericDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(DlgParameterGroup)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgParameterGroup, CGenericDialog)
    //{{AFX_MSG_MAP(DlgParameterGroup)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgParameterGroup message handlers



///////////////////////////////////////////////////////////////////////////////////

static CString GetVaryText()
{
    CString sText;
    if (vary.IsSuite())
        sText = "Suite";
    else
        sText = "Range";
    return sText;
}

class TypeGridCell : public CComboGridCell
{
    friend class ParameterGridCtrl;
public:
    TypeGridCell(Parameter& parameter, const char* szText = NULL, Alignment alignment = alignLeft);
    TypeGridCell();

    virtual void    UpdateText();
    void            SetParameterType(int nValue);

protected:
    virtual void    GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable);

private:
    void            SetParTypeStrings();

protected:
    Parameter&      m_parameter;
    SC_IntArray     m_parTypeMap;
    SC_StringArray  m_parTypeStrings;
};

TypeGridCell::TypeGridCell(Parameter& parameter, const char* szText, Alignment alignment) :
    CComboGridCell(szText, alignment),
    m_parameter(parameter)
{
    m_parTypeStrings.Alloc(10);
    m_parTypeMap.Alloc(10);
    SetParTypeStrings();
}

void TypeGridCell::UpdateText()
{
    for (int i = 0; i < m_parTypeStrings.Size(); i++)
        if (int(m_parameter.parameterType) == m_parTypeMap[i])
        {
            SetText(m_parTypeStrings[i]);
            return;
        }
    SetText("????");

}

void TypeGridCell::SetParTypeStrings()
{
    m_parTypeStrings.SetEmpty();
    m_parTypeMap.SetEmpty();

    // always valid
    m_parTypeStrings += "Constant";
    m_parTypeMap += Parameter::ptFixed;

    //  range or vary
    if (!control.IsSampleSim())
    {
        m_parTypeStrings += GetVaryText();
        m_parTypeMap += Parameter::ptVary;
    }

    if (control.IsOptimization() && m_parameter.optIsOK)
    {
        m_parTypeStrings += "Optimize";
        m_parTypeMap += Parameter::ptOpt;
    }

    if (control.IsSampleSim())
    {
        m_parTypeStrings += "Sample";
        m_parTypeMap += Parameter::ptSample;
    }

    if (m_parameter.fpIsOK && control.NonLinearOK())
    {
        m_parTypeStrings += "f(P) Points";
        m_parTypeMap += Parameter::ptFPPoint;

        if (control.UseCurveFiles())
        {
            m_parTypeStrings += "f(P) File";
        }
        else
        {
            m_parTypeStrings += "f(P) Object";
        }
        m_parTypeMap += Parameter::ptFPFixed;

    }

    if (m_parameter.frIsOK)
    {
        m_parTypeStrings += "f(r) Points";
        m_parTypeMap += Parameter::ptFrPoint;

        if (control.UseCurveFiles())
        {
            m_parTypeStrings += "f(r) File";
        }
        else
        {
            m_parTypeStrings += "f(r) Object";
        }
        m_parTypeMap += Parameter::ptFrFixed;
    }

    if (m_parameter.ftIsOK && control.TimeVarOK())
    {
        m_parTypeStrings += "f(t) Points";
        m_parTypeMap += Parameter::ptFtPoint;

        if (control.UseCurveFiles())
        {
            m_parTypeStrings += "f(t) File";
        }
        else
        {
            m_parTypeStrings += "f(t) Object";
        }
        m_parTypeMap += Parameter::ptFtFixed;
    }


}

void TypeGridCell::SetParameterType(int nValue)
{
    m_parameter.SetParameterType(Parameter::ParameterType(m_parTypeMap[nValue]), true);
    m_parameter.ClearWasActive();
}

void TypeGridCell::GetSelectionList(CStringArray& aText, CArray<bool,bool>& aEnable)
{
    // update for possible vary changes
    SetParTypeStrings();

    aText.RemoveAll();
    aEnable.RemoveAll();
    for (int i = 0; i < m_parTypeStrings.Size(); i++)
    {
        aText.Add(m_parTypeStrings[i]);
        aEnable.Add(true);
    }

}

///////////////////////////////////////////////////////////////////////////////////
class ValueGridCell : public CTextGridCell
{
    friend class ParameterGridCtrl;
public:
    ValueGridCell(Parameter& parameter, const char* szText = NULL, Alignment alignment = alignRight);

    virtual void UpdateText();
protected:
    virtual bool CanEdit();
    virtual CWnd* CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar);
    virtual CSize GetExtent(CDC* pDC) const;

protected:
    Parameter& m_parameter;
};

ValueGridCell::ValueGridCell(Parameter& parameter, const char* szText, Alignment alignment) :
    CTextGridCell(szText, alignment),
    m_parameter(parameter)
{
}

CSize ValueGridCell::GetExtent(CDC* pDC) const
{
    CSize sz = GetNewlineTextExtent(pDC, m_sText);
    sz.cx += 30;
    return sz;
}

void ValueGridCell::UpdateText()
{
    CString sText;
    SC_SetupErr errData;

    // default values
    bool parOK = m_parameter.SetupOK(errData);
    if (parOK)
    {
        sText = "OK";
    }
    else
    {
        sText = "BAD";
    }

    switch (m_parameter.parameterType) {
    case Parameter::ptFixed: {
            // override
            m_parameter.ParValToString(m_parameter.GetParValue(), sText);
            SetText(sText);
            return;
        }
    case Parameter::ptVary: {
            if (parOK)
            {
                char tempStr[40];
                m_parameter.varyParVal.GetMenuStr(tempStr, 40);
                sText = tempStr;
            }
            break;
        }

    case Parameter::ptOpt:
        break;
    case Parameter::ptSample: {
            if (parOK)
            {
                char tempStr[40];
                m_parameter.sampParVal.GetMenuStr(tempStr, 40);
                sText = tempStr;
            }
            break;
        }
    case Parameter::ptFrFixed: {
            if (control.UseCurveFiles())
            {
                sText += "/f(r) File";
            }
            else
            {
                sText += "/f(r) Object";
            }
            break;
        }
    case Parameter::ptFrPoint: {
            sText += "/other menu";
            break;
        }
    case Parameter::ptFPFixed: {
            if (control.UseCurveFiles())
            {
                sText += "/f(P) File";
            }
            else
            {
                sText += "/f(P) Object";
            }
            break;
        }
    case Parameter::ptFPPoint: {
            sText += "/other menu";
            break;
        }
    case Parameter::ptFtFixed: {
            if (control.UseCurveFiles())
            {
                sText += "/f(t) File";
            }
            else
            {
                sText += "/f(t) Object";
            }
            break;
       }
    case Parameter::ptFtPoint: {
            sText += "/other menu";
            break;
        }
    }
    SetText(sText);
}

bool ValueGridCell::CanEdit()
{
    return ! m_parameter.IsPoints();
}

CWnd* ValueGridCell::CreateEditWnd(CRect& rect, CWnd* pParent, int nID, int nRow, int nCol, UINT nFirstChar)
{
    switch (m_parameter.parameterType) {
    case Parameter::ptFixed:
        {
            CString loLim, upLim;
            m_parameter.ParValToString(m_parameter.minLimit, loLim);
            m_parameter.ParValToString(m_parameter.maxLimit, upLim);
            GenAppParameterRangeMsg(loLim, upLim);
            return CTextGridCell::CreateEditWnd(rect, pParent, nID, nRow, nCol, nFirstChar);
        }
    case Parameter::ptVary:
        {
            VaryVarUnitsDlg dlg(m_parameter.varyParVal, m_parameter.GetOverrideFormat(),
                                m_parameter.minLimit, m_parameter.maxLimit);
        }
        break;
    case Parameter::ptOpt:
        {
            COptVarUnitsDlg dlg(m_parameter.optParVal, m_parameter.GetOverrideFormat(),
                                m_parameter.minLimit, m_parameter.maxLimit);
            dlg.DoModal();
        }
        break;
    case Parameter::ptSample:
        {
            CSampVarUnitsDlg dlg(m_parameter.sampParVal, m_parameter.GetOverrideFormat(),
                                m_parameter.minLimit, m_parameter.maxLimit);
            dlg.DoModal();
        }
        break;
    case Parameter::ptFrFixed:
        {
            CParameterFixedDlg dlg(m_parameter.rFixedCurve, Parameter::cptRadial);
            dlg.DoModal();
        }
        break;
    case Parameter::ptFPFixed:
        {
            CParameterFixedDlg dlg(m_parameter.pFixedCurve, Parameter::cptPressure);
            dlg.DoModal();
        }
        break;
    case Parameter::ptFtFixed:
        {
            CParameterFixedDlg dlg(m_parameter.tFixedCurve, Parameter::cptTime);
            dlg.DoModal();
        }
        break;
    case Parameter::ptFrPoint:
        break;
    case Parameter::ptFPPoint:
        break;
    case Parameter::ptFtPoint:
        break;
    }


    pParent->SetFocus();
    static_cast<CGridCtrlCell*>(pParent)->FinishEdit(NULL, -1, nFirstChar);
    return NULL;
}


///////////////////////////////////////////////////////////////////////////////////
class UnitsGridCell : public CUnitGridCell
{
    friend class ParameterGridCtrl;
public:
    UnitsGridCell(Parameter& parameter, Alignment alignment  = alignRight);
    virtual void UpdateText();

protected:
    Parameter& m_parameter;
};

UnitsGridCell::UnitsGridCell(Parameter& parameter, Alignment alignment) :
    CUnitGridCell(parameter.parVal.unitType, parameter.parVal.GetConvIndex(), alignment),
    m_parameter(parameter)
{
}

void UnitsGridCell::UpdateText()
{
    SetText(UnitsBase::GetUnitString(m_parameter.parVal.unitType, m_parameter.parVal.GetConvIndex()));
}


///////////////////////////////////////////////////////////////////////////////////
ParameterGridCtrl::ParameterGridCtrl() :
    CGridCtrlCell()
{
}

BEGIN_MESSAGE_MAP(ParameterGridCtrl,CGridCtrlCell)
    //{{AFX_MSG_MAP(ParameterGridCtrl)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void ParameterGridCtrl::FinishEdit(const char* szText, int nValue, int nLastChar)
{
    // Change values in Grid
    CGridCell* pCell = GetCell(m_cellActive.m_nRow, m_cellActive.m_nCol);
    switch (m_cellActive.m_nCol) {
    case 2: // Type
        {
            static_cast<TypeGridCell*>(pCell)->SetParameterType(nValue);
            pCell->SetText(szText);

            CGridCell* pValueCell = GetCell(m_cellActive.m_nRow, 3);
            pValueCell->UpdateText();

            CGridCellIndices cell(m_cellActive.m_nRow, 3);
            InvalidateCellRect(CGridCellRange(cell, cell));

            // Reset parameter priority tab sensitivity here
            CWnd* pWnd = GetParent(); //DlgParameterGroup
            pWnd = pWnd->GetParent(); //Res Tab Control
            pWnd = pWnd->GetParent(); //DlgParParameter
            DialogNode* pNode = static_cast<DialogNode*>(pWnd->GetParent());
            pNode->CheckRestrictions();
        }
        break;
    case 3: // Value
        {
            Parameter& parameter = static_cast<ValueGridCell*>(pCell)->m_parameter;
            if (parameter.parameterType == Parameter::ptFixed)
            {
                parameter.parVal.IsWithinRange(szText, parameter.minLimit, parameter.maxLimit);
            }
            else
            {
                // any other call could update units so update
                CGridCell* pUnitsCell = GetCell(m_cellActive.m_nRow, 4);
                pUnitsCell->UpdateText();
                CGridCellIndices cell(m_cellActive.m_nRow, 4);
                InvalidateCellRect(CGridCellRange(cell, cell));
            }

            pCell->UpdateText();
        }
        break;
    case 4: // Units
        {
            Parameter& parameter = static_cast<UnitsGridCell*>(pCell)->m_parameter;
            parameter.parVal.SetConvIndex(nValue);

            pCell->SetText(szText);
            CGridCell* pValueCell = GetCell(m_cellActive.m_nRow, 3);
            pValueCell->UpdateText();

            CGridCellIndices cell(m_cellActive.m_nRow, 3);
            InvalidateCellRect(CGridCellRange(cell, cell));
        }
        break;
    }

    InvalidateCellRect(CGridCellRange(m_cellActive, m_cellActive));

    StopEdit(false);

    // if the right key continue editing on next cell
    switch (nLastChar) {
        case VK_TAB:
        case VK_DOWN:
        case VK_UP:
        case VK_RIGHT:
        case VK_LEFT:
        case VK_NEXT:
        case VK_PRIOR:
        case VK_HOME:
        case VK_END:
            OnKeyDown(nLastChar, 0, 0);
            UpdateWindow();
            if (m_cellActive != m_cellActiveOld)
                StartEdit(nLastChar);
            break;
    }
}

void ParameterGridCtrl::InitGrid(ParameterArray& parameterArray)
{
    SetColCount(5);
    SetRowCount(parameterArray.Size()+1);
    SetCenteredText(0, 1, "Name");
    SetCenteredText(0, 2, "Type");
    SetCenteredText(0, 3, "Value");
    SetCenteredText(0, 4, "Units");

    for (int i = 0; i < parameterArray.Size(); i++) {
        // set edited flag
        parameterArray[i]->SetEdited();

        SetCenteredText(i+1, 0, "     ");
        CTextGridCell* pNameCell = new CTextGridCell(parameterArray[i]->fullName);
        pNameCell->SetReadOnly();
        SetCell(i+1, 1, pNameCell);

        TypeGridCell* pTypeCell = new TypeGridCell(*parameterArray[i]);
        SetCell(i+1, 2, pTypeCell);
        pTypeCell->UpdateText();

        ValueGridCell* pValueCell = new ValueGridCell(*parameterArray[i]);
        SetCell(i+1, 3, pValueCell);
        pValueCell->UpdateText();

        UnitsGridCell* pUnitCell = new UnitsGridCell(*parameterArray[i]);
        SetCell(i+1, 4, pUnitCell);
        pUnitCell->UpdateText();
    }
    SetEditMode();

    SetBorderThickness(4);
    AutoSize();
    SetColSize(1, GetColSize(1) + 10);
    SetColSize(2, GetColSize(2) + 20);
    SetColSize(4, GetColSize(4) + 20);
}


BOOL DlgParameterGroup::OnInitDialog()
{
    CGenericDialog::OnInitDialog();

    m_grid.SubclassDlgItem(IDC_GRID, this);
    SetControlInfo(IDC_GRID, resizeBoth);

    ParameterArray& parameterArray = static_cast<DlgNodeParameterGroup*>(GetInterfaceNode())->m_parameterArray;

    m_grid.InitGrid(parameterArray);

    return TRUE;
}

