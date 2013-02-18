///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableIntegrate.cpp
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

#include <genClass/U_Str.h>
#include <genClass/U_Real.h>


#include <objDFO/DFO_TableIntegrate.h>


DFO_TableIntegrate :: DFO_TableIntegrate() : FuncObjC("Table Column Integrate")
{
    InitCommon();
}

DFO_TableIntegrate::DFO_TableIntegrate(const DFO_TableIntegrate& a) 
: FuncObjC(a), DSC_TableColSel(a)

{
    LocalCopy(a);
    InitCommon();
}


void DFO_TableIntegrate::InitCommon()
{
    integratedTableDO.tableData = &integratedTableDC;
    AddOutPort(integratedTableDO);
    DSC_TableColSel::AddPort(*this);

    DoStatusChk();
}

DFO_TableIntegrate& DFO_TableIntegrate::operator= (const DFO_TableIntegrate& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        DSC_TableColSel::operator=(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableIntegrate::LocalCopy(const DFO_TableIntegrate& a)
{
}

void  DFO_TableIntegrate:: DoStatusChk()
{
    FuncObjC::DoStatusChk();

    DSC_TableColSel::DoCheck(*this);

    if (StatusNotOK())
        return;


    int nrows = inputData->GetNRows();
    int ncols = inputData->GetNCols();

    if (nrows == 0) 
        SetObjErrMsg("no rows input table");
    if (ncols < 2)
        SetObjErrMsg("input table must have at least two columns");

    if (selectedCol >= ncols)
        SetObjErrMsg("X column index out of range");
}


void DFO_TableIntegrate:: CalcOutput(FOcalcType  calcType)
{
    integratedTableDC.ClearAll();
    DoStatusChk();
    if (StatusNotOK())
        return;

    // copy input to output
    integratedTableDC = *inputData;

    SC_DoubleMatrix& outTbl = integratedTableDC.dataTable;

    char tempStr[80];   
    for (int i = 0; i < outTbl.Size(); i++)
    {
        if (selectedCol == i)
            continue;

        MakeString(tempStr, "Int(", integratedTableDC.columnDesc[i], ")", 80);
        integratedTableDC.columnDesc.SetString(tempStr, i); 

        if (!Integrate(outTbl[selectedCol], outTbl[i]))
        {
            SetObjErrMsg("null data values found in table");
            return;
        }
    }

    MakeString(tempStr, "Int(", integratedTableDC.GetID(), ")", 80);
    integratedTableDC.SetID(tempStr);
}

bool DFO_TableIntegrate::Integrate(const SC_DoubleArray& x,
                                         SC_DoubleArray& y)
{
    if (RealIsNull(x[0]) || RealIsNull(y[0]))
        return false;


    double lastY = y[0];
    y[0] = 0.0;
    for ( int i = 1; i < x.Size(); i++)
    {
        if (RealIsNull(x[i]) || RealIsNull(y[i]))
            return false;

        double currY = y[i];
        double dx = fabs(x[i] - x[i - 1]);
        // check for zero crossing
        double area;
        if (((currY >= 0.0) && (lastY >= 0.0)) || ((currY < 0.0) && (lastY < 0.0)))
        {
            area  = (lastY + currY) / 2.0 * dx;
        }
        else
        {
            double dx1 = - lastY * dx / (currY - lastY);
            area = (lastY * dx1 + currY * (dx - dx1)) / 2.0;
        }

        y[i] = y[i - 1] + area;
        lastY = currY;
    }

    return true;
}

