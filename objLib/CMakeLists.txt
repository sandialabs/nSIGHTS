
add_source_subdir(objClass       OBJ_LIB_SRC)
add_source_subdir(objDataClass   OBJ_LIB_SRC)
add_source_subdir(objDFO         OBJ_LIB_SRC)
add_source_subdir(objDPO         OBJ_LIB_SRC)
add_source_subdir(objHelp        OBJ_LIB_SRC)
add_source_subdir(objLFO         OBJ_LIB_SRC)
add_source_subdir(objLPO         OBJ_LIB_SRC)
add_source_subdir(objPFO         OBJ_LIB_SRC)
add_source_subdir(objPFOGL       OBJ_LIB_SRC)
add_source_subdir(objPlotClass   OBJ_LIB_SRC)
add_source_subdir(objPPO         OBJ_LIB_SRC)
add_source_subdir(objUFO         OBJ_LIB_SRC)
add_source_subdir(objUPO         OBJ_LIB_SRC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("../genLib/")

ADD_MSVC_PRECOMPILED_HEADER("ObjLibAfx.h" "ObjLibAfx.cpp" OBJ_LIB_SRC)
add_library(ObjLib STATIC ${OBJ_LIB_SRC})

if (BUILD_NPRE_X)
    # ObjLibX is composed of a subset of the source code, along with the ADCONSOLEAPP define

    add_source_subdir(objClass       OBJ_LIB_X_SRC)
    add_source_subdir(objDataClass   OBJ_LIB_X_SRC)
    add_source_subdir(objDFO         OBJ_LIB_X_SRC)
    add_source_subdir(objDPO         OBJ_LIB_X_SRC  DPO_CDFExtract
                                                    DPO_CombineTables
                                                    DPO_CreateCurve
                                                    DPO_CreateDataLabel
                                                    DPO_CreateReal
                                                    DPO_CreateSpecialCDF
                                                    DPO_CreateXYArray
                                                    DPO_CubeExtractGrid
                                                    DPO_CubeHistogram
                                                    DPO_CubeMath
                                                    DPO_CubeNormalize
                                                    DPO_CubeScaleTransform
                                                    DPO_CubeStatistics
                                                    DPO_CubeToIndex
                                                    DPO_CurveInterp
                                                    DPO_EnterTable
                                                    DPO_EnterXY
                                                    DPO_ExecuteObjects
                                                    DPO_ExecuteProcess
                                                    DPO_GridExtractIsoPoint
                                                    DPO_GridExtractXY
                                                    DPO_GridHistogram
                                                    DPO_GridInterpolate
                                                    DPO_GridMath
                                                    DPO_GridNormalize
                                                    DPO_GridScaleTransform
                                                    DPO_GridStatistics
                                                    DPO_GridTile
                                                    DPO_InterpTableColumn
                                                    DPO_Normalize
                                                    DPO_ReadCubeData
                                                    DPO_ReadCurveArray
                                                    DPO_ReadGridData
                                                    DPO_ReadLabelArray
                                                    DPO_ReadTable
                                                    DPO_ReadVariableArray
                                                    DPO_ReadXY
                                                    DPO_ReadXYDataArray
                                                    DPO_RealScaleTransform
                                                    DPO_RealToTable
                                                    DPO_SelectCurve
                                                    DPO_SelectVariable
                                                    DPO_SelectXY
                                                    DPO_TableAdd
                                                    DPO_TableAddBase
                                                    DPO_TableColScaleTransform
                                                    DPO_TableColumnCorrelation
                                                    DPO_TableColumnMath
                                                    DPO_TableColumnSort
                                                    DPO_TableConcat
                                                    DPO_TableFullCorrelation
                                                    DPO_TableHistogram
                                                    DPO_TableIntegrate
                                                    DPO_TableInterpVal
                                                    DPO_TableQuantile
                                                    DPO_TableRangeExtract
                                                    DPO_TableRowAccumulate
                                                    DPO_TableRowIndexLogic
                                                    DPO_TableRowStatistics
                                                    DPO_TableSample
                                                    DPO_TableStatistics
                                                    DPO_TableToGrid
                                                    DPO_TableToReal
                                                    DPO_TableToXY
                                                    DPO_TableTranspose
                                                    DPO_TransformTable
                                                    DPO_ViewGrid
                                                    DPO_ViewTable
                                                    DPO_ViewXY
                                                    DPO_XYAddNoise
                                                    DPO_XYArrayScaleTransform
                                                    DPO_XYArrayToTable
                                                    DPO_XYConcat
                                                    DPO_XYDualScaleTransform
                                                    DPO_XYFourier
                                                    DPO_XYHistogram
                                                    DPO_XYIntegrate
                                                    DPO_XYIntegrateFMom
                                                    DPO_XYInterpVal
                                                    DPO_XYMath
                                                    DPO_XYQuantile
                                                    DPO_XYRangeExtract
                                                    DPO_XYReduction
                                                    DPO_XYRemoveDuplicate
                                                    DPO_XYScaleTransform
                                                    DPO_XYSmoothFilter
                                                    DPO_XYStatistics
                                                    DPO_XYToCDF
                                                    DPO_XYToTable
                                                    DPO_XYToXYArray
                                                    DPO_XYTranspose
                                                    objDPOConfigFile)
    add_source_subdir(objUFO         OBJ_LIB_X_SRC)
    add_source_subdir(objUPO         OBJ_LIB_X_SRC  UPO_VariableProcess
                                                    UPO_WriteCurveArray
                                                    UPO_WriteGrid
                                                    UPO_WriteLabelArray
                                                    UPO_WriteTable
                                                    UPO_WriteXY
                                                    UPO_WriteXYDataArray)

    ADD_MSVC_PRECOMPILED_HEADER("ObjLibAfx.h" "ObjLibAfx.cpp" OBJ_LIB_X_SRC)

    add_library(ObjLibX STATIC ${OBJ_LIB_X_SRC})

    set_target_properties(ObjLibX PROPERTIES COMPILE_FLAGS "-DADCONSOLEAPP")

endif()

