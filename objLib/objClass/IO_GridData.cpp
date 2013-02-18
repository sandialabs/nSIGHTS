///////////////////////////////////////////////////////////////////////////////////
//
// IO_GridData.cpp
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
//      I/O for DC_GridData
//
///////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include <genClass/U_Str.h>
#include <genClass/U_Msg.h>
#include <genClass/U_Real.h>

#include <objClass/IO_GridData.h>

IO_GridData::I_GridDataFile::I_GridDataFile(const char* inputFile)
    : ConvFileC(inputFile, 8 * 1024 * 1024)
{
}

void IO_GridData::I_GridDataFile::Init()
{
    nextInc = 5;
    nextCount = 50;
}

void IO_GridData::I_GridDataFile::ReadAppDevGrid(DC_GridData&    gridData)
{
    gridData.DeAlloc();

    CFreadNonBlank();
    gridData.SetID(lastLineRead);

    // data starts after header
    CFreadNonBlank();

    int nX = CFgetNextInt();
    int nY = CFgetNextInt();

    if ((nX < 1) || (nY < 1) || (nX > 5000) || (nY > 5000))
        CFtextError("Grid dimension bad (<1 or > 5000)");

    if (!gridData.Alloc(nX, nY))
        CFtextError("could not allocate grid data");

    CFreadDoubleArray(gridData.xData, false);
    CFreadDoubleArray(gridData.yData, false);
    if (!(gridData.xData.IsAscending() && gridData.yData.IsAscending()))
        CFtextError("X and Y locations must be ascending");

    for (int i = 0; i < nX; i++)
        CFreadDoubleArray(gridData.gData[i], true);
}


void IO_GridData::I_GridDataFile::AddNewEntry(      SC_DoubleArray& inGrid,
                               const double&         newVal)
{
    for (int i = 0; i < inGrid.Size(); i++)
        if (fabs(newVal - inGrid[i]) < 1.0E-05)
            return;

    inGrid += newVal;
}

int IO_GridData::I_GridDataFile::GetEntryIndex(const SC_DoubleArray& inGrid,
                                const double&         newVal)
{
    for (int i = 0; i < inGrid.Size(); i++)
        if (fabs(newVal - inGrid[i]) < 1.0E-05)
            return i;

    return -1;
}


void IO_GridData::I_GridDataFile::ReadXYZList(DC_GridData&    gridData)
{
    gridData.DeAlloc();

    //  comment chars
    CFsetComment("#!*");

    SC_DoubleArray tempX, tempY;
    tempX.SetResizable(100);
    tempY.SetResizable(100);

    while(true)
    {
        if (CFeof())
            break;
        if (!CFreadText())
            continue;

        AddNewEntry(tempX, CFgetNextDouble());
        AddNewEntry(tempY, CFgetNextDouble());
    }
    tempX.Sort(true);
    tempY.Sort(true);
    if (!(tempX.IsAscending() && tempY.IsAscending()))
        CFtextError("X and Y locations must be ascending");

    if (!gridData.Alloc(tempX.Size(), tempY.Size()))
        CFtextError("could not allocate grid data");

    gridData.xData = tempX;
    gridData.yData = tempY;
    gridData.xData.SetID("X");
    gridData.yData.SetID("Y");

    // reset to start and scan for z's
    CFreset();
    while(true)
    {
        if (CFeof())
            break;
        if (!CFreadText())
            continue;

        int xIndx = GetEntryIndex(tempX, CFgetNextDouble());
        int yIndx = GetEntryIndex(tempY, CFgetNextDouble());

        if ((xIndx >= 0) && (yIndx >= 0))
            gridData.gData[xIndx][yIndx] = CFgetNextDouble();

    }

}

void IO_GridData::I_GridDataFile::ReadSurferGrid(DC_GridData&    gridData)
{
    gridData.DeAlloc();

    CFreadNonBlank();
    if (!CFmatches("DSAA"))
        CFtextError("Not a Surfer .GRD file");

    // data starts after header
    CFreadNonBlank();
    int nX = CFgetNextInt();
    int nY = CFgetNextInt();

//  if ((nX < 1) || (nY < 1) || (nX > 5000) || (nY > 5000))
//      CFtextError("Grid dimension bad (<1 or >5000)");
    if ((nX < 1) || (nY < 1))
        CFtextError("Grid dimension bad (<1)");

    if (!gridData.Alloc(nX, nY))
        CFtextError("could not allocate grid data");


    ReadSurferCoord(gridData.xData);
    ReadSurferCoord(gridData.yData);

    CFreadNonBlank();                   // skip z min max

    SC_DoubleArray rowData(gridData.xData.Size());

    CFreadNonBlank();                   // get the first data
    for (int i = 0; i < gridData.yData.Size(); i++)
    {
        CFreadDoubleArray(rowData, false);
        for (int j = 0; j < gridData.xData.Size(); j++)
        {
            double temp = rowData[j];
            if (temp > 1.0E+38)
                temp = nullReal;
            gridData.gData[j][i] = temp;
        }
    }
}

void IO_GridData::I_GridDataFile::ReadSurferCoord(DC_DataCoord& inCoord)
{
    CFreadNonBlank();
    double dMin = CFgetNextDouble();
    double dMax = CFgetNextDouble();
    double dd = (dMax - dMin) / double(inCoord.Size() - 1);
    inCoord[0] = dMin;
    for (int i = 1; i < inCoord.Size(); i++)
        inCoord[i] = inCoord[i - 1] + dd;
}


void IO_GridData::I_GridDataFile::ReadArcInfoAsciiGrid(DC_GridData&    gridData)
{

/*
ncols         686
nrows         726
xllcorner     908990
yllcorner     6889990
cellsize      20
NODATA_value  -9999
*/
    int ncols = int(GetArcInfoAsciiParameter("ncols"));
    int nrows = int(GetArcInfoAsciiParameter("nrows"));
    double xll = GetArcInfoAsciiParameter("xllcorner");
    double yll = GetArcInfoAsciiParameter("yllcorner");
    double dxy = GetArcInfoAsciiParameter("cellsize");
    double nullVal = GetArcInfoAsciiParameter("NODATA_value");

    if (!gridData.Alloc(ncols, nrows))
        CFtextError("could not allocate grid data");


    gridData.xData[0] = xll;
    for (int i = 1; i < ncols; i++)
        gridData.xData[i] = gridData.xData[i - 1] + dxy;

    gridData.yData[0] = yll;
    for (int i = 1; i < nrows; i++)
        gridData.yData[i] = gridData.yData[i - 1] + dxy;


    CFreadNonBlank();
    for (int i = 0; i < nrows; i++)
    {

        // read from yMax down ?
        int rowIndx = nrows - i - 1;
        for (int j = 0; j < ncols; j++)
        {
            double temp = CFstreamDouble();
            if (temp == nullVal)
                temp = nullReal;
            gridData.gData[j][rowIndx] = temp;
        }
    }
}


double  IO_GridData::I_GridDataFile::GetArcInfoAsciiParameter(const char* parID)
{
    CFreadNonBlank();
    char tempStr[80];
    if (!CFgetNextStr(tempStr, 80))
        CFtextError("unexpecting blank found");

    if (!SameString(tempStr, parID))
        CFtextError("no match on parameter name");

    return CFgetNextDouble();
}

IO_GridData::O_GridDataFile::O_GridDataFile(const char* outputFile)
    :TxtFileC(outputFile, false)
{
}

void IO_GridData::O_GridDataFile::Init()
{
}

void IO_GridData::O_GridDataFile::WriteAppDevGrid(const DC_GridData& gridData)
{
    WriteText(gridData.GetID());

    WriteInt(gridData.xData.Size());
    WriteInt(gridData.yData.Size());
    WriteLine();

    WriteDoubleArray(gridData.xData, false);
    WriteDoubleArray(gridData.yData, false);

    for (int i = 0; i < gridData.xData.Size(); i++)
        WriteDoubleArray(gridData.gData[i], false);
};

void IO_GridData::O_GridDataFile::WriteSurferGrid(const DC_GridData& gridData)
{
    WriteText("DSAA");

    WriteInt(gridData.xData.Size());
    WriteInt(gridData.yData.Size());
    WriteLine();

    WriteSurferCoord(gridData.xData);
    WriteSurferCoord(gridData.yData);


    SC_DoubleArray rowData(gridData.xData.Size());

    for (int i = 0; i < gridData.yData.Size(); i++)
    {
        for (int j = 0; j < gridData.xData.Size(); j++)
        {
            double temp = gridData.gData[j][i];
            if (RealIsNull(temp))
                temp = 3.0E+38;
            rowData[j] = temp;
        }
        WriteDoubleArray(rowData, false);
    }
};

void IO_GridData::O_GridDataFile::WriteSurferCoord(const DC_DataCoord& outCoord)
{
    double dMin = outCoord[0];
    double dMax = outCoord.LastIndex();

    // check 
    double dd = (dMax - dMin) / double(outCoord.Size() - 1);
    for (int i = 1; i < outCoord.Size(); i++)
        if (!RealsAreSame(outCoord[i], outCoord[i - 1] + dd, 1.0E-05))
            ThrowTextError("grid is irregular: not SURFER compatible");

    WriteDouble(dMin);
    WriteDouble(dMax);
    WriteLine();
}


void IO_GridData::O_GridDataFile::WriteArcInfoAsciiGrid(const DC_GridData&    gridData)
{

/*
ncols         686
nrows         726
xllcorner     908990
yllcorner     6889990
cellsize      20
NODATA_value  -9999
*/

    int ncols = gridData.xData.Size();
    int nrows = gridData.yData.Size();

    WriteSolidText("ncols");
    WriteInt(ncols);
    WriteLine();

    WriteSolidText("nrows");
    WriteInt(nrows);
    WriteLine();

    WriteSolidText("xllcorner");
    WriteDouble(gridData.xData[0]);
    WriteLine();

    WriteSolidText("yllcorner");
    WriteDouble(gridData.yData[0]);
    WriteLine();

    WriteSolidText("cellsize");
    WriteDouble(gridData.xData[1] - gridData.xData[0]);
    WriteLine();

    const double arcNull = -9999.0;

    WriteSolidText("NODATA_value");
    WriteDouble(arcNull);
    WriteLine();

    for (int i = 0; i < nrows; i++)
    {
        // write from yMax down
        int rowIndx = nrows - i - 1;
        for (int j = 0; j < ncols; j++)
        {
            double temp = gridData.gData[j][rowIndx];
            if (RealIsNull(temp))
                temp = arcNull;
            WriteDouble(temp);
        }
        WriteLine();
    }
}



void IO_GridData::O_GridDataFile::WriteXYZList(const DC_GridData& gridData)
{
    for (int i = 0; i < gridData.xData.Size(); i++)
        for (int j = 0; j < gridData.yData.Size(); j++)
        {
            WriteDouble(gridData.xData[i]);
            WriteDouble(gridData.yData[j]);
            WriteDouble(gridData.gData[i][j]);
            WriteLine();
        }
};


bool IO_GridData::ReadGridFile(      GridFileFormat gridFileFormat,
                               const char*          inputFileName,
                                     DC_GridData&   gridData)
{
    try
    {
        I_GridDataFile cmFile(inputFileName);
        cmFile.CFsetMsgStuff("ReadGridData", "GridDataFile", 1000);
        cmFile.Init();
        switch (gridFileFormat)
        {
            case gffAppDev : {
                cmFile.ReadAppDevGrid(gridData);
                break;
            }
            case gffSurfer : {
                cmFile.ReadSurferGrid(gridData);
                break;
            }
            case gffXYZList : {
                cmFile.ReadXYZList(gridData);
                break;
            }
            case gffArcInfoAscii : {
                cmFile.ReadArcInfoAsciiGrid(gridData);
                break;
            }
            case gffOther : {
                GenAppErrorMsg("ReadGridData", "other not supported");
                return false;
                break;
            }
        }
    }
    catch (ConvFileC::CFReadError re) {
        gridData.DeAlloc();
        GenAppErrorMsg("ReadGridData", re.errMsg);
        return false;
    }

    using namespace std;
    ostringstream ostr;
    ostr <<  "Grid Data read OK: " << gridData.xData.Size() << " rows " << " by " << gridData.yData.Size() << " columns"<< ends;
    GenAppInfoMsg("ReadGridData", ostr.str().c_str());
    return true;
}

void IO_GridData::WriteGridFile(      GridFileFormat gridFileFormat,
                                const char*          outputFileName,
                                const DC_GridData&   gridData)
{
    try
    {
        O_GridDataFile cmFile(outputFileName);
        cmFile.Init();
        switch (gridFileFormat)
        {
            case gffAppDev : {
                cmFile.WriteAppDevGrid(gridData);
                break;
            }
            case gffSurfer : {
                cmFile.WriteSurferGrid(gridData);
                break;
            }
            case gffXYZList : {
                cmFile.WriteXYZList(gridData);
                break;
            }
            case gffArcInfoAscii : {
                cmFile.WriteArcInfoAsciiGrid(gridData);
                break;
            }
            case gffOther : {
                GenAppErrorMsg("ReadGridData", "other not supported");
                return;
            }
        }
    }
    catch (TextC::TextError re) {
        GenAppErrorMsg("WriteGridData", re.errMsg);
        return;
    }

    GenAppInfoMsg("WriteGridData", "file written successfully");
}


