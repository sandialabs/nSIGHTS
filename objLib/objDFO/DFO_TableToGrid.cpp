///////////////////////////////////////////////////////////////////////////////////
//
// DFO_TableToGrid.cpp
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

#include <objDFO/DFO_TableToGrid.h>

DFO_TableToGrid :: DFO_TableToGrid() :
    FuncObjC("Convert Table To Grid")
{
    ycolumnIndex = 1;
    dataColumnIndex = 2;
    convertFormat = cfGrid;
    columnValueIsHeader = false;
    useColumnIndex      = false;
    gridXIsColumns      = true;
    CopyString(xdataID, "XData", DC_DataCoord::dataIDLen);
    CopyString(ydataID, "YData", DC_DataCoord::dataIDLen);
    tableRowModulus = 1;
    tableColModulus = 1;
    includeLastRow = true;
    includeLastCol = true;

    InitCommon();
}

DFO_TableToGrid::DFO_TableToGrid(const DFO_TableToGrid& a) :
    FuncObjC(a), DSC_TableColSel(a)
{
    LocalCopy(a);
    InitCommon();
}

DFO_TableToGrid :: ~DFO_TableToGrid ()
{
}

void DFO_TableToGrid::InitCommon()
{
    AddPort(*this);
    outputGridDO.gridData = &outputGrid;
    AddOutPort(outputGridDO);
    outputGrid.SetID("TableToGrid");
    DoStatusChk();
}

DFO_TableToGrid& DFO_TableToGrid::operator= (const DFO_TableToGrid& a)
{
    if (&a != this)
    {
        FuncObjC::FullCopy(a);
        DSC_TableColSel::operator =(a);
        LocalCopy(a);
    }
    return *this;
}


void  DFO_TableToGrid::LocalCopy(const DFO_TableToGrid& a)
{
    convertFormat = a.convertFormat;
    ycolumnIndex = a.ycolumnIndex;
    dataColumnIndex = a.dataColumnIndex;

    columnValueIsHeader = a.columnValueIsHeader;
    useColumnIndex = a.useColumnIndex;
    gridXIsColumns  = a.gridXIsColumns;
    CopyString(xdataID, a.xdataID, DC_DataCoord::dataIDLen);
    CopyString(ydataID, a.ydataID, DC_DataCoord::dataIDLen);
    tableRowModulus = a.tableRowModulus;
    tableColModulus = a.tableColModulus;
    includeLastRow = a.includeLastRow;
    includeLastCol = a.includeLastCol;
}


void  DFO_TableToGrid:: DoStatusChk()
{
    FuncObjC::DoStatusChk();


    DoCheck(*this);
    if (convertFormat == cfOther)
        SetObjErrMsg("No support for other format");

    if (StatusNotOK())
        return;

    inputTableDC = inputData;

    if (convertFormat == cfGrid)
    {
        navailCol = inputTableDC->GetNCols() - 1; // first col is data
        ncol = navailCol / tableColModulus;
        if (includeLastCol && (ncol * tableColModulus < navailCol))
            ncol++;

        navailRow = inputTableDC->GetNRows(); // first col is data
        if (!columnValueIsHeader)
            navailRow--;

        nrow = navailRow / tableRowModulus;
        if (includeLastRow && (nrow * tableRowModulus < navailRow))
            nrow++;

        if (ncol  < 3)
            SetObjErrMsg("table must have more than 2 columns after modulus");
        if (nrow < 3)
            SetObjErrMsg("table must have more than 2 rows after modulus");

        if (columnValueIsHeader && (!useColumnIndex))
        {
            double temp;
            for (int i = 1; i < inputTableDC->GetNCols(); i++)
                if (!IsValidReal(inputTableDC->columnDesc[i], temp))
                {
                    SetObjErrMsg("column header is not a real number");
                    break;
                }
        }
    }
    else
    {
        ncol = inputTableDC->GetNCols();
        nrow = inputTableDC->GetNRows(); // first col is data
        if ((selectedCol == ycolumnIndex) ||
            (selectedCol == dataColumnIndex) ||
            (ycolumnIndex == dataColumnIndex))
            SetObjErrMsg("X, Y, and data columns must be different");
        if (selectedCol >= ncol)
            SetObjErrMsg("X column data out-of-range");
        if (ycolumnIndex >= ncol)
            SetObjErrMsg("Y column data out-of-range");
        if (dataColumnIndex >= ncol)
            SetObjErrMsg("data column out-of-range");

        if (StatusOK())
        {
            yDataCol = &inputTableDC->dataTable[ycolumnIndex];
            xDataCol = &inputTableDC->dataTable[selectedCol];
            gridDataCol = &inputTableDC->dataTable[dataColumnIndex];
        }
    }

}

void DFO_TableToGrid::CalcOutput(FOcalcType  calcType)
{
    DoStatusChk();
    if (StatusNotOK())
        return;

    if (convertFormat == cfGrid)
        GridConvert();
    else
        XYZListConvert();


    // set IDs
    outputGrid.xData.SetID(xdataID);
    outputGrid.yData.SetID(ydataID);
    outputGrid.SetID(GetID());
}

void DFO_TableToGrid::GridConvert()
{
    int nX = ncol;
    int nY = nrow;
    DC_DataCoord* colData = &outputGrid.xData;
    DC_DataCoord* rowData = &outputGrid.yData;

    if (!gridXIsColumns)
    {
        nY = ncol;
        nX = nrow;
        colData = &outputGrid.yData;
        rowData = &outputGrid.xData;
    }

    if (!outputGrid.Alloc(nX, nY))
    {
        SetObjErrMsg("Grid allocation error");
        outputGrid.DeAlloc();
        return;
    }

    // set col data
    int nextCol = 0;
    for (int i = 0; i < navailCol; i++)
    {
        if (ColOK(i))
        {
            double cVal;
            if (columnValueIsHeader)
            {
                if (useColumnIndex)
                    cVal = double(i);
                else
                {
                    // this should already be OK
                    if (!IsValidReal(inputTableDC->columnDesc[i + 1], cVal))
                    {
                        SetObjErrMsg("column header is not a real number ??");
                        return;
                    }
                }
            }
            else
                cVal = inputTableDC->dataTable[i + 1][0];
            (*colData)[nextCol++] = cVal;
        }
    }

    // and rows
    int rowOffset = 1;
    if (columnValueIsHeader)
        rowOffset = 0;

    int nextRow = 0;
    for (int i = 0; i < navailRow; i++)
    {
        if (RowOK(i))
            (*rowData)[nextRow++] = inputTableDC->dataTable[0][i + rowOffset];
    }

    if (!(DataCoordOK(*colData) && DataCoordOK(*rowData)))
        return;

    // all OK fill in tables
    SC_DoubleMatrix& gData = outputGrid.gData;
    const SC_DoubleMatrix& tData = inputTableDC->dataTable;

    nextCol = 0;
    for (int i = 0; i < navailCol; i++)
    {
        if (!ColOK(i))
            continue;

        nextRow = 0;
        for (int j = 0; j < navailRow; j++)
        {
            if (RowOK(j))
            {
                const double& tVal = tData[i + 1][j + rowOffset];
                if (gridXIsColumns)
                    gData[nextCol][nextRow] = tVal;
                else
                    gData[nextRow][nextCol] = tVal;
                nextRow++;
            }
        }
        nextCol++;
    }
}

bool DFO_TableToGrid::ColOK(int colIndex)
{
    bool colOK = (colIndex % tableColModulus) == 0;
    if (includeLastCol && (colIndex == (navailCol - 1)))
        colOK = true;
    return colOK;
}

bool DFO_TableToGrid::RowOK(int rowIndex)
{
    bool rowOK = (rowIndex % tableRowModulus) == 0;
    if (includeLastRow && (rowIndex == (navailRow - 1)))
        rowOK = true;
    return rowOK;
}

bool DFO_TableToGrid::DataCoordOK(const DC_DataCoord& inCoord)
{
    // must be non-null and ascending
    for (int i = 0; i < inCoord.Size(); i++)
    {
        if (RealIsNull(inCoord[i]))
        {
            SetObjErrMsg("X/Y data coordinates cannot be null");
            return false;
        }

        if ((i > 0) && (inCoord[i] <= inCoord[i - 1]))
        {
            SetObjErrMsg("X/Y data coordinates must be ascending");
            return false;
        }
    }

    return true;
}

void DFO_TableToGrid::XYZListConvert()
{
    SC_DoubleArray tempX(nrow), tempY(nrow);
    tempX.SetEmpty();
    tempY.SetEmpty();

    for (int i = 0; i < nrow; i++)
    {
        AddNewEntry(tempX, (*xDataCol)[i]);
        AddNewEntry(tempY, (*yDataCol)[i]);
    }
    tempX.Sort(true);
    tempY.Sort(true);
    if (!(tempX.IsAscending() && tempY.IsAscending()))
    {
        SetObjErrMsg("X and Y coordinates must be ascending");
        return;
    }

    ProcessModulus(tempY, tableRowModulus, includeLastRow);
    ProcessModulus(tempX, tableColModulus, includeLastCol);

    if (!outputGrid.Alloc(tempX.Size(), tempY.Size()))
    {
        SetObjErrMsg("could not allocate grid data");
        return;
    }

    outputGrid.xData = tempX;
    outputGrid.yData = tempY;

    for (int i = 0; i < nrow; i++)
    {
        int xIndx = GetEntryIndex(tempX, (*xDataCol)[i]);
        int yIndx = GetEntryIndex(tempY, (*yDataCol)[i]);

        if ((xIndx >= 0) && (yIndx >= 0))
            outputGrid.gData[xIndx][yIndx] = (*gridDataCol)[i];
    }

}

void DFO_TableToGrid::AddNewEntry(      SC_DoubleArray& inGrid,
                               const double&         newVal)
{
    if (RealIsNull(newVal))
        return;

    for (int i = 0; i < inGrid.Size(); i++)
        if (fabs(newVal - inGrid[i]) < 1.0E-08)
            return;

    inGrid += newVal;
}

int DFO_TableToGrid::GetEntryIndex(const SC_DoubleArray& inGrid,
                                const double&         newVal)
{
    for (int i = 0; i < inGrid.Size(); i++)
        if (fabs(newVal - inGrid[i]) < 1.0E-08)
            return i;

    return -1;
}


void DFO_TableToGrid::ProcessModulus(SC_DoubleArray& inGrid,
                                       int             modulus,
                                       bool            keepLast)
{
    if (modulus < 2)
        return;

    int nOK = 0;
    for (int i = 0; i < inGrid.Size(); i++)
        if ((i % modulus) == 0)
        {
            inGrid[nOK++] = inGrid[i];
        }

    if (keepLast && ((inGrid.UpperBound() % modulus) != 0))
        inGrid[nOK++] = inGrid.LastIndex();

    inGrid.SetSize(nOK);
}

