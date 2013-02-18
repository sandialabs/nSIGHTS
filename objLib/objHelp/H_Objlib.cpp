///////////////////////////////////////////////////////////////////////////////////
//
// H_Objlib.cpp
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

#include "StdAfx.h"
#include "ObjLibAfx.h"


#include <genApp/C_ObjHelp.h>
#include <objHelp/H_Objlib.h>

static HelpMap objHelpMap[] =

    {
    HelpMap("DPO_CDFExtract",       IDH_OBJ_DPO_CDFEXTRACT),
    HelpMap("DPO_CombineTables",    IDH_OBJ_DPO_COMBINETABLES),
    HelpMap("DPO_CreateCurve",      IDH_OBJ_DPO_CREATECURVE),
    HelpMap("DPO_CreateDataLabel",  IDH_OBJ_DPO_CREATEDATALABEL),
    HelpMap("DPO_CreateReal",       IDH_OBJ_DPO_CREATEREAL),
    HelpMap("DPO_CreateXYArray",    IDH_OBJ_DPO_CREATEXYARRAY),
    HelpMap("DPO_CubeExtractGrid",  IDH_OBJ_DPO_CUBEEXTRACTGRID),
    HelpMap("DPO_CubeHistogram",    IDH_OBJ_DPO_CUBEHISTOGRAM),
    HelpMap("DPO_CubeMath",         IDH_OBJ_DPO_CUBEMATH),
    HelpMap("DPO_CubeNormalize",    IDH_OBJ_DPO_CUBENORMALIZE),
    HelpMap("DPO_CubeScaleTransform",   IDH_OBJ_DPO_CUBESCALETRANSFORM),
    HelpMap("DPO_CubeStatistics",   IDH_OBJ_DPO_CUBESTATISTICS),
    HelpMap("DPO_CubeToIndex",      IDH_OBJ_DPO_CUBETOINDEX),
    HelpMap("DPO_CurveInterp",      IDH_OBJ_DPO_CURVEINTERP),
    HelpMap("DPO_EnterTable",       IDH_OBJ_DPO_ENTERTABLE),
    HelpMap("DPO_EnterXY",          IDH_OBJ_DPO_ENTERXY),
    HelpMap("DPO_GridExtractXY",    IDH_OBJ_DPO_GRIDEXTRACTXY),
    HelpMap("DPO_GridHistogram",    IDH_OBJ_DPO_GRIDHISTOGRAM),
    HelpMap("DPO_GridInterpolate",  IDH_OBJ_DPO_GRIDINTERPOLATE),
    HelpMap("DPO_GridMath",         IDH_OBJ_DPO_GRIDMATH),
    HelpMap("DPO_GridNormalize",    IDH_OBJ_DPO_GRIDNORMALIZE),
    HelpMap("DPO_GridScaleTransform",   IDH_OBJ_DPO_GRIDSCALETRANSFORM),
    HelpMap("DPO_GridStatistics",   IDH_OBJ_DPO_GRIDSTATISTICS),
    HelpMap("DPO_InterpTableColumn",    IDH_OBJ_DPO_INTERPTABLECOLUMN),
    HelpMap("DPO_ReadCubeData",     IDH_OBJ_DPO_READCUBEDATA),
    HelpMap("DPO_ReadCurveArray",   IDH_OBJ_DPO_READCURVEARRAY),
    HelpMap("DPO_ReadGridData",     IDH_OBJ_DPO_READGRIDDATA),
    HelpMap("DPO_ReadLabelArray",   IDH_OBJ_DPO_READLABELARRAY),
    HelpMap("DPO_ReadTable",        IDH_OBJ_DPO_READTABLE),
    HelpMap("DPO_ReadXY",           IDH_OBJ_DPO_READXY),
    HelpMap("DPO_ReadXYDataArray",  IDH_OBJ_DPO_READXYARRAY),
    HelpMap("DPO_RealScaleTransform",   IDH_OBJ_DPO_REALSCALETRANSFORM),
    HelpMap("DPO_RealToTable",      IDH_OBJ_DPO_REALTOTABLE),
    HelpMap("DPO_SelectCurve",      IDH_OBJ_DPO_SELECTCURVE),
    HelpMap("DPO_SelectXY",         IDH_OBJ_DPO_SELECTXY),
    HelpMap("DPO_TableAdd",         IDH_OBJ_DPO_TABLEADD),
    HelpMap("DPO_TableColScaleTransform", IDH_OBJ_DPO_TABLECOLSCALETRANSFORM),
    HelpMap("DPO_TableColumnCorrelation", IDH_OBJ_DPO_TABLECOLUMNCORRELATION),
    HelpMap("DPO_TableColumnMath",  IDH_OBJ_DPO_TABLECOLUMNMATH),
    HelpMap("DPO_TableConcat",      IDH_OBJ_DPO_TABLECONCAT),
    HelpMap("DPO_TableFullCorrelation", IDH_OBJ_DPO_TABLEFULLCORRELATION),
    HelpMap("DPO_TableHistogram",   IDH_OBJ_DPO_TABLEHISTOGRAM),
    HelpMap("DPO_TableInterpVal",   IDH_OBJ_DPO_TABLEINTERPVAL),
    HelpMap("DPO_TableQuantile",    IDH_OBJ_DPO_TABLEQUANTILE),
    HelpMap("DPO_TableRangeExtract",    IDH_OBJ_DPO_TABLERANGEEXTRACT),
    HelpMap("DPO_TableRowIndexLogic",   IDH_OBJ_DPO_TABLEROWINDEXLOGIC),
    HelpMap("DPO_TableRowStatistics",   IDH_OBJ_DPO_TABLEROWSTATISTICS),
    HelpMap("DPO_TableStatistics",  IDH_OBJ_DPO_TABLESTATISTICS),
    HelpMap("DPO_TableToGrid",      IDH_OBJ_DPO_TABLETOGRID),
    HelpMap("DPO_TableToReal",      IDH_OBJ_DPO_TABLETOREAL),
    HelpMap("DPO_TableToXY",        IDH_OBJ_DPO_TABLETOXY),
    HelpMap("DPO_TableTranspose",   IDH_OBJ_DPO_TABLETRANSPOSE),
    HelpMap("DPO_ViewTable",        IDH_OBJ_DPO_VIEWTABLE),
    HelpMap("DPO_ViewXY",       IDH_OBJ_DPO_VIEWXY),
    HelpMap("DPO_XYAddNoise",       IDH_OBJ_DPO_XYADDNOISE),
    HelpMap("DPO_XYArrayScaleTransform",IDH_OBJ_DPO_XYARRAYSCALETRANSFORM),
    HelpMap("DPO_XYDualScaleTransform", IDH_OBJ_DPO_XYDUALSCALETRANSFORM),
    HelpMap("DPO_XYFourier",        IDH_OBJ_DPO_XYFOURIER),
    HelpMap("DPO_XYHistogram",      IDH_OBJ_DPO_XYHISTOGRAM),
    HelpMap("DPO_XYIntegrate",      IDH_OBJ_DPO_XYINTEGRATE),
    HelpMap("DPO_XYMath",           IDH_OBJ_DPO_XYMATH),
    HelpMap("DPO_XYQuantile",       IDH_OBJ_DPO_XYQUANTILE),
    HelpMap("DPO_XYRangeExtract",   IDH_OBJ_DPO_XYRANGEEXTRACT),
    HelpMap("DPO_XYReduction",      IDH_OBJ_DPO_XYREDUCTION),
    HelpMap("DPO_XYRemoveDuplicate",IDH_OBJ_DPO_XYREMOVEDUPLICATE),
    HelpMap("DPO_XYScaleTransform", IDH_OBJ_DPO_XYSCALETRANSFORM),
    HelpMap("DPO_XYSmoothFilter",   IDH_OBJ_DPO_XYSMOOTHFILTER),
    HelpMap("DPO_XYStatistics",     IDH_OBJ_DPO_XYSTATISTICS),
    HelpMap("DPO_XYToCDF",          IDH_OBJ_DPO_XYTOCDF),
    HelpMap("DPO_XYToXYArray",      IDH_OBJ_DPO_XYTOXYARRAY),
    HelpMap("DPO_XYTranspose",      IDH_OBJ_DPO_XYTRANSPOSE),

    HelpMap("LPO_Table",            IDH_OBJ_LPO_TABLE),
    HelpMap("LPO_TableStatistics",  IDH_OBJ_LPO_TABLESTATISTICS),
    HelpMap("LPO_XYData",           IDH_OBJ_LPO_XYDATA),

    HelpMap("PPO_2DAnalytic",       IDH_OBJ_PPO_2DANALYTIC),
    HelpMap("PPO_2DMultTables",     IDH_OBJ_PPO_2DMULTTABLES),
    HelpMap("PPO_TableHistogram",   IDH_OBJ_PPO_2DTABLEHISTOGRAM),
    HelpMap("PPO_2DTableSeries",    IDH_OBJ_PPO_2DTABLESERIES),
    HelpMap("PPO_2DXYData",         IDH_OBJ_PPO_2DXYDATA),
    HelpMap("PPO_2DXYDataArray",    IDH_OBJ_PPO_2DXYARRAY),
    HelpMap("PPO_3DTableSeries",    IDH_OBJ_PPO_3DTABLESERIES),
    HelpMap("PPO_3DXYData",         IDH_OBJ_PPO_3DXYDATA),
    HelpMap("PPO_CubeColorBlock",   IDH_OBJ_PPO_CUBECOLORBLOCK),
    HelpMap("PPO_ColorCubeIsovolume",   IDH_OBJ_PPO_CUBEISOVOLUME),
    HelpMap("PPO_CubeColorPoint",   IDH_OBJ_PPO_CUBECOLORPOINT),
    HelpMap("PPO_EnterXYOnPlot",    IDH_OBJ_PPO_ENTERXYONPLOT),
    HelpMap("PPO_GridColorBlockFill",   IDH_OBJ_PPO_GRIDCOLORBLOCKFILL),
    HelpMap("PPO_GridColorPoint",   IDH_OBJ_PPO_GRIDCOLORPOINT),
    HelpMap("PPO_GridColorRangeFill",   IDH_OBJ_PPO_GRIDCOLORRANGEFILL),
    HelpMap("PPO_GridContour",      IDH_OBJ_PPO_GRIDCONTOUR),
    HelpMap("PPO_GridFishnet",      IDH_OBJ_PPO_GRIDFISHNET),
    HelpMap("PPO_NSXYLabels",       IDH_OBJ_PPO_NSXYLABELS),
    HelpMap("PPO_NSXYZLabels",      IDH_OBJ_PPO_NSXYZLABELS),
    HelpMap("PPO_TableHorsetail",   IDH_OBJ_PPO_TABLEHORSETAIL),
    HelpMap("PPO_XYHistogram",      IDH_OBJ_PPO_XYHISTOGRAM),
    HelpMap("PPO_XYHorsetail",      IDH_OBJ_PPO_XYHORSETAIL),

    HelpMap("UPO_WriteCurveArray",  IDH_OBJ_UPO_WRITECURVEARRAY),
    HelpMap("UPO_WriteGrid",        IDH_OBJ_UPO_WRITEGRID),
    HelpMap("UPO_WriteLabelArray",  IDH_OBJ_UPO_WRITELABELARRAY),
    HelpMap("UPO_WriteTable",       IDH_OBJ_UPO_WRITETABLE),
    HelpMap("UPO_WriteXY",          IDH_OBJ_UPO_WRITEXY),
    HelpMap("UPO_WriteXYDataArray", IDH_OBJ_UPO_WRITEXYARRAY),

    HelpMap(NULL, -1)

    };

static ObjHelpC objHelp(objHelpMap);



