///////////////////////////////////////////////////////////////////////////////////
//
// UnitRealCtrl.h
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
//      combined value entry/unit selection for resource based dialogs
//
//
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef UNITREALCTRL_H
#define UNITREALCTRL_H

#include <genClass/SC_RealFormat.h>

#include <genUnits/UnitIndexCtrl.h>
#include <genCtrl/RealCtrl.h>

class UnitRealRealCtrl : public CRealCtrl
{
private:
    UnitReal*       m_pUnitReal;
    bool            m_boverRideFormat;
    SC_RealFormat   m_overRideFormat;
    double          m_fminMetricVal;
    double          m_fmaxMetricVal;
    double          m_userVal;


public:
                    UnitRealRealCtrl();

    void            Set(CWnd* pParent, int nIDEdit, UnitReal& inUnitReal,
                        const SC_RealFormat* overrideFormat,
                        const double& inMinMetricVal, const double& inMaxMetricVal);

    void            ResetLimits();

    // CUpdateableCtrl virtual
    virtual void    DoCtrlReset();

        // Operations
public:
    virtual void    UpdateText();


    void            DoUnitConvChange();

};


class CUnitRealCtrl : public CCtrlUpdate
{
public:
    UnitRealRealCtrl        m_uRealCtrl;
    CUnitIndexCtrl          m_uIndexCtrl;

public:
    CUnitRealCtrl();
    virtual ~CUnitRealCtrl();

    void Set(CWnd* pParent, int nIDCombo, int nIDEdit,
             UnitReal* pUnitReal, const SC_RealFormat* poverRideFormat = NULL);

    void Set(CWnd* pParent, int nIDCombo, int nIDEdit,
             UnitReal* pUnitReal, const SC_RealFormat* poverRideFormat,
            const double& inMinMetricVal, const double& inMaxMetricVal);

    BOOL EnableWindowEx(BOOL bEnable = TRUE );

    // CUpdateableCtrl virtual
    virtual void    DoCtrlReset();


    //  callback to reset real text after unit change
    virtual void    DoCtrlUpdate();
    void            DoUnitConvChange(int newConvIndex);


public:
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CUnitRealCtrl)
    //}}AFX_VIRTUAL

};

#endif // !UNITREALCTRL_H

