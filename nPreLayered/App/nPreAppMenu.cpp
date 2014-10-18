///////////////////////////////////////////////////////////////////////////////////
//
// nPreAppMenu.cpp
//
///////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2012 Sandia Corporation. Under the terms of Contract
// DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
// retains certain rights in this software.
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


#include "StdAfx.h"

#include "nPreAppMenu.h"

#include <genListClass/C_ListDef.h>
#include <genListClass/LFO_VersionListing.h>

#include <Var/G_Sequence.h>
#include <Var/G_Parameter.h>
#include <Var/G_DataCapture.h>
#include <Var/G_OutputFiles.h>
#include <osMain/G_Sample.h>
#include <osMain/G_Optimize.h>
#include <osMain/G_OptRange.h>

#include <osDFO/DFO_SelectRTRangeGrid.h>
#include <osDFO/DFO_SelectRTRangeCube.h>

#include <Sim/C_SimCore.h>
#include <App/AppPages.h>

nPreAppMenuC nPreAppMenu;

void nPreAppMenuC::InitAppData(int init_obj_ID_VALUE)
{
  AppMenuC::InitAppData(init_obj_ID_VALUE);

  // initialize other objects
  UnitsBase::AppUnitsInitialize();
  sample.InitAppGlobals();
  optimize.InitAppGlobals();
  sequence.InitAppGlobals();
  dataCapture.InitAppGlobals();
  parameters.InitAppGlobals();
  outputFiles.InitAppGlobals();

  // default listing
  ListDefC::defaultListObj += &appVersionListing::versionListObj;

  // kluges for RT range
  DFO_SelectRTRangeGrid::SetDefaultRangeFO(optRangeOutputFO);
  DFO_SelectRTRangeCube::SetDefaultRangeFO(optRangeOutputFO);

  // set menu bar stuff
  menuBarObjPos = 4;
  objectNewPos  = 0;
  objectDupPos  = 1;
  objectCopyPos = 2;
  objectCopyPagePos= 3;
  objectPastePos  = 4;
  objectDelPos    = 5;
  objectApplyPos  = 6;

  menuBarPagePos  = 5;
  pageDupPos    = 3;
  pageDelPos    = 4;
  pageCopyCurrentPos  = 5;
  pageCopyAllPos  = 6;
  pagePastePos  = 7;
  pageBringToTopPos = 8;

  windowListStartPos  = 2;
}

void nPreAppMenuC::DoObjectMenuCreate()
{
  //  Data page
  cascadeDataData.Alloc(15);
  int nextCascade = 0; // makes reorganizing easier ....
  cascadeDataData[nextCascade].cascadeID = "&Cube Processing";
  {
    ObjectItemArray& menuData = cascadeDataData[nextCascade++].cascadeItems;
    menuData  += ObjectItem("DPO_SelectRTRangeCube",    "Select Range Cube");
    menuData  += ObjectItem("DPO_CubeToIndex",        "Extract Cube Indexes");
    menuData  += ObjectItem("DPO_CubeExtractGrid",      "Extract Grid");
    menuData  += ObjectItem("DPO_CubeHistogram",      "Histogram");
    menuData  += ObjectItem("DPO_CubeMath",         "Matrix Math");
    menuData  += ObjectItem("DPO_CubeNormalize",      "Normalize");
    menuData  += ObjectItem("DPO_CubeScaleTransform",   "Scale/Transform");
    menuData  += ObjectItem("DPO_CubeStatistics",     "Statistics");
  }

  cascadeDataData[nextCascade].cascadeID = "&CDF/Quantile Processing";
  {
    ObjectItemArray& menuData = cascadeDataData[nextCascade++].cascadeItems;
    menuData += ObjectItem("DPO_TableQuantile",         "Calculate CDF of Table Column");
    menuData  += ObjectItem("DPO_XYQuantile",           "Calculate CDF of XY Data");
    menuData  += ObjectItem("DPO_CDFExtract",           "Extract Values from CDF");
    menuData  += ObjectItem("DPO_XYToCDF",              "Validate XY Data as CDF");
  }

  cascadeDataData[nextCascade].cascadeID = "&Curve Processing";
  {
    ObjectItemArray& menuData = cascadeDataData[nextCascade++].cascadeItems;
    menuData += ObjectItem("DPO_CreateCurve",       "Create Curve from XY Data");
    menuData += ObjectItem("DPO_CurveInterp",       "Interpolate XY Data from Curve");
    menuData += ObjectItem("DPO_ReadCurveArray",    "Read Curve File");
    menuData += ObjectItem("DPO_SelectCurve",       "Select Curve from File");
    menuData += ObjectItem("UPO_WriteCurveArray",       "Write Curve File");
  }

  cascadeDataData[nextCascade].cascadeID = "&Fit Specification";
  {
    ObjectItemArray& menuData = cascadeDataData[nextCascade++].cascadeItems;
    menuData  += ObjectItem("DPO_SingleFit",                "Single Fit");
    menuData  += ObjectItem("DPO_CompositeFit",       "Composite Fit");
    menuData  += ObjectItem("DPO_BasicSequenceFit",         "Sequence Fit");
  }

  cascadeDataData[nextCascade].cascadeID = "&Grid Processing";
  {
    ObjectItemArray& menuData = cascadeDataData[nextCascade++].cascadeItems;
    menuData  += ObjectItem("DPO_SelectRTRangeGrid",    "Select Range Grid");
    menuData  += ObjectItem("DPO_GridExtractXY",      "Extract XY");
    menuData  += ObjectItem("DPO_GridHistogram",      "Histogram");
    menuData  += ObjectItem("DPO_GridMath",         "Matrix Math");
    menuData  += ObjectItem("DPO_GridNormalize",      "Normalize");
    menuData  += ObjectItem("DPO_GridScaleTransform",   "Scale/Transform");
    menuData  += ObjectItem("DPO_GridStatistics",     "Statistics");
    menuData  += ObjectItem("DPO_ViewGrid",         "View Grid Data");
  }

  cascadeDataData[nextCascade].cascadeID = "&Pens && Color Maps";
  {
    ObjectItemArray& menuData = cascadeDataData[nextCascade++].cascadeItems;
    menuData  += ObjectItem("DPO_LinColorMap",      "Linear Color Map");
    menuData  += ObjectItem("DPO_MergeColorMap",    "Merge Color Maps");
    menuData  += ObjectItem("DPO_PenSet",           "Pen Set");
    menuData  += ObjectItem("DPO_ReadColorMap",   "Read Color Map");
    menuData  += ObjectItem("UPO_WriteColorMap",    "Write Color Map");
  }

  cascadeDataData[nextCascade].cascadeID = "&Read Input";
  {
    ObjectItemArray& menuData = cascadeDataData[nextCascade++].cascadeItems;
    menuData += ObjectItem("DPO_ReadCurveArray",    "Read Curve File");
    menuData  += ObjectItem("DPO_ReadMiniTroll",    "Read Mini-Troll Text File");
    menuData  += ObjectItem("DPO_ReadTable",      "Read Table File");
    menuData  += ObjectItem("DPO_ReadXY",         "Read XY Data");
    menuData  += ObjectItem("DPO_ReadXYDataArray",    "Read XY Data Array");
    menuData  += ObjectItem("DPO_ReadLabelArray",     "Read XYZ Label Data");
  }

  cascadeDataData[nextCascade].cascadeID = "Real  &Processing";
  {
    ObjectItemArray& menuData = cascadeDataData[nextCascade++].cascadeItems;
    menuData  += ObjectItem("DPO_CreateReal",         "Create Real Value");
    menuData  += ObjectItem("DPO_RealScaleTransform",   "Scale/Transform");
  }

  cascadeDataData[nextCascade].cascadeID = "&Table Operations";
  {
    ObjectItemArray& menuData = cascadeDataData[nextCascade++].cascadeItems;
    menuData += ObjectItem("DPO_EnterTable",      "Enter Table Data");
    menuData += ObjectItem("DPO_TableToReal",     "Extract Real from Table");
    menuData += ObjectItem("DPO_TableRangeExtract",     "Extract Table Rows");
    menuData += ObjectItem("DPO_TableInterpVal",    "Interpolate Table Columns");
    menuData += ObjectItem("DPO_TableFullCorrelation",  "Full Table Correlations");
    menuData += ObjectItem("DPO_ReadTable",       "Read Table File");
    menuData += ObjectItem("DPO_RealToTable",     "Real Value(s) To Table");
    menuData += ObjectItem("DPO_TableAdd",        "Sum Tables");
    menuData += ObjectItem("DPO_TableQuantile",         "Table Column CDF/Quantile");
    menuData += ObjectItem("DPO_TableColumnCorrelation","Table Column Correlations");
    menuData += ObjectItem("DPO_TableColumnMath",   "Table Column Math");
    menuData += ObjectItem("DPO_TableColScaleTransform", "Table Column Scale/Transform");
    menuData += ObjectItem("DPO_TableStatistics",     "Table Column Statistics");
    menuData += ObjectItem("DPO_TableHistogram",    "Table Column To Histogram");
    menuData += ObjectItem("DPO_TableToXY",       "Table Columns To XY");
    menuData += ObjectItem("DPO_TableRowIndexLogic",  "Table Row Index Logic");
    menuData += ObjectItem("DPO_TableRowStatistics",  "Table Row Statistics");
    menuData += ObjectItem("DPO_ViewTable",       "View Table Data");
    menuData += ObjectItem("UPO_WriteTable",            "Write Table File");

  }

  cascadeDataData[nextCascade].cascadeID = "&Well Test Analysis";
  {
    ObjectItemArray& menuData = cascadeDataData[nextCascade++].cascadeItems;
    menuData  += ObjectItem("DPO_CreateBEETResponseFunction",   "Create BP/ET Response Function");
    menuData  += ObjectItem("DPO_ExtractSequenceInterval",    "Extract Sequence(s)");
    menuData  += ObjectItem("DPO_BEETCompensation",       "P(t) BP/ET Compensation");
    menuData  += ObjectItem("DPO_BarometricCompensation",     "P(t) Barometric Compensation");
    menuData  += ObjectItem("DPO_Derivative",           "P(t) Derivative Calculation");
    menuData  += ObjectItem("DPO_TimeProcess",          "P(t) Time Processing");
    menuData  += ObjectItem("DPO_PulseNormalize",         "Pulse Normalization");
    menuData  += ObjectItem("DPO_BasicTimeExtract",       "Time Limits Extraction/Interpolation");
  }

  cascadeDataData[nextCascade].cascadeID = "&XY Data Processing";
  {
    ObjectItemArray& menuData = cascadeDataData[nextCascade++].cascadeItems;
    menuData  += ObjectItem("DPO_XYAddNoise",         "Add Noise");
    menuData  += ObjectItem("DPO_XYQuantile",       "CDF/Quantile");
    menuData  += ObjectItem("DPO_CreateXYArray",      "Create XY Array");
    menuData  += ObjectItem("DPO_XYConcat",         "Concatenate XY");
    menuData  += ObjectItem("DPO_XYDualScaleTransform",   "Dual Scale/Transform");
    menuData  += ObjectItem("DPO_EnterXY",          "Enter XY Data");
    menuData  += ObjectItem("DPO_XYRangeExtract",     "Extract Range");
    menuData  += ObjectItem("DPO_XYFourier",        "Fourier Transform on Y");
    menuData  += ObjectItem("DPO_XYHistogram",        "Histogram");
    menuData  += ObjectItem("DPO_XYIntegrate",        "Integrate");
    menuData  += ObjectItem("DPO_SelectXY",         "Select XY from XY Array");
    menuData  += ObjectItem("DPO_XYScaleTransform",     "Single Scale/Transform");
    menuData  += ObjectItem("DPO_XYSmoothFilter",       "Smooth/Filter");
    menuData  += ObjectItem("DPO_XYStatistics",       "Statistics");
    menuData  += ObjectItem("DPO_ReadXY",           "Read XY Data");
    menuData  += ObjectItem("DPO_ReadXYDataArray",      "Read XY Data Array");
    menuData  += ObjectItem("DPO_XYReduction",        "Reduction");
    menuData  += ObjectItem("DPO_XYRemoveDuplicate",    "Remove Duplicates");
    menuData  += ObjectItem("DPO_XYTranspose",        "Transpose");
    menuData  += ObjectItem("DPO_XYMath",         "Vector Math");
    menuData  += ObjectItem("DPO_ViewXY",         "View XY Data");
    menuData  += ObjectItem("UPO_WriteXY",          "Write XY File");
    menuData  += ObjectItem("UPO_WriteXYDataArray",     "Write XY Data Array File");

  }
  //  after last data folder
  cascadeDataData.SetSize(nextCascade);


  //  2DXY plot page
  cascadeData2DXY.AllocAndSetSize(1);
  nextCascade = 0;
  cascadeData2DXY[nextCascade].cascadeID = "2DXY";
  {
    ObjectItemArray& menuData = cascadeData2DXY[nextCascade++].cascadeItems;
    menuData  += ObjectItem( "PPO_ColorLegend",     "Anno: Color Legend");
    menuData  += ObjectItem( "PPO_UserLabels",      "Anno: User Labels");
    menuData  += ObjectItem( "PPO_DataLabels",      "Anno: Data Labels");
    menuData  += ObjectItem( "PPO_SeriesLegend",    "Anno: Series Legend");
    menuData  += ObjectItem( "PPO_GridLine",      "Anno: Extra Grid Lines");
    menuData  += ObjectItem( "PPO_SequenceGridLines", "Anno: Sequence Grid Lines");
    menuData  += ObjectItem( "PPO_LabelAxes",     "Anno: Label Axis");
    menuData  += ObjectItem( "PPO_ShadeRegion",     "Anno: Shade Region");
    menuData  += ObjectItem( "PPO_TimeAxes",      "Anno: Time Axis");
    menuData  += ObjectItem( "PPO_NSXYLabels",      "Anno: XY Labels");

    menuData  += ObjectItem( "PPO_GridColorBlockFill",  "Data: Grid Color Block");
    menuData  += ObjectItem( "PPO_GridColorRangeFill",  "Data: Grid Color Fill");
    menuData  += ObjectItem( "PPO_GridColorPoint",    "Data: Grid Color Point");
    menuData  += ObjectItem( "PPO_GridContour",     "Data: Grid Contour");
    menuData  += ObjectItem( "PPO_GridFishnet",     "Data: Grid Fishnet");

    menuData  += ObjectItem( "PPO_2DMultTables",    "Data: Multiple Table Series");
    menuData  += ObjectItem( "PPO_2DTableSeries",   "Data: Single Table Series");
    menuData  += ObjectItem( "PPO_TableColorSeries",  "Data: Table Color Series");
    menuData  += ObjectItem( "PPO_TableHistogram",    "Data: Table Histogram");
    menuData  += ObjectItem( "PPO_XYHistogram",     "Data: XY Histogram");
    menuData  += ObjectItem( "PPO_2DXYData",      "Data: XY Series");

    menuData  += ObjectItem( "PPO_EnterXYOnPlot",   "Modify: Enter/Edit XY");
    menuData  += ObjectItem( "PPO_2DAnalytic",      "Analytics: Line Data");
  }

  //  3DXYZ plot page
  cascadeData3DXYZ.AllocAndSetSize(1);
  nextCascade = 0;
  cascadeData3DXYZ[nextCascade].cascadeID = "3DXYZ";
  {
    ObjectItemArray& menuData = cascadeData3DXYZ[nextCascade++].cascadeItems;
    menuData  += ObjectItem( "PPO_ColorLegend",     "Anno: Color Legend");
    menuData  += ObjectItem( "PPO_UserLabels",      "Anno: User Labels");
    menuData  += ObjectItem( "PPO_DataLabels",      "Anno: Data Labels");
    menuData  += ObjectItem( "PPO_SeriesLegend",    "Anno: Series Legend");
    menuData  += ObjectItem( "PPO_NSXYZLabels",     "Anno: XYZ Labels");

    menuData  += ObjectItem( "PPO_CovarLimits",     "Data: Confidence Limits");
    menuData  += ObjectItem( "PPO_CubeColorBlock",    "Data: Cube Color Block");
    menuData  += ObjectItem( "PPO_ColorCubeIsovolume",  "Data: Cube Color Isovolume");
    menuData  += ObjectItem( "PPO_CubeColorPoint",    "Data: Cube Color Point");
    menuData  += ObjectItem( "PPO_GridColorBlockFill",  "Data: Grid Color Block");
    menuData  += ObjectItem( "PPO_GridColorRangeFill",  "Data: Grid Color Fill");
    menuData  += ObjectItem( "PPO_GridColorPoint",    "Data: Grid Color Point");
    menuData  += ObjectItem( "PPO_GridContour",     "Data: Grid Contour");
    menuData  += ObjectItem( "PPO_GridFishnet",     "Data: Grid Fishnet");
    menuData  += ObjectItem( "PPO_3DXYData",          "Data: XY Series");
    menuData  += ObjectItem( "PPO_3DTableSeries",   "Data: Table Series");
    menuData  += ObjectItem( "PPO_TableColorSeries",  "Data: Table Color Series");
  }

  // composite plot -- just annotation objects
  cascadeDataComposite.AllocAndSetSize(1);
  nextCascade = 0;
  cascadeDataComposite[nextCascade].cascadeID = "Composite";
  {
    ObjectItemArray& menuData = cascadeDataComposite[nextCascade++].cascadeItems;
    menuData  += ObjectItem( "PPO_ColorLegend",     "Anno: Color Legend");
    menuData  += ObjectItem( "PPO_UserLabels",      "Anno: User Labels");
    menuData  += ObjectItem( "PPO_DataLabels",      "Anno: Data Labels");
    menuData  += ObjectItem( "PPO_SeriesLegend",    "Anno: Series Legend");
  }

  PullMenuC::idMap.SetEmpty();
  dataPageAllocator.SetMenuData(cascadeDataData);
  plot2DXYPageAllocator.SetMenuData(cascadeData2DXY);
  plot3DXYZPageAllocator.SetMenuData(cascadeData3DXYZ);
  compositePlotPageAllocator.SetMenuData(cascadeDataComposite);
}


