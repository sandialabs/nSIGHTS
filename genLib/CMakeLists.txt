
add_source_subdir(genApp         GEN_LIB_SRC)
add_source_subdir(genClass       GEN_LIB_SRC)
add_source_subdir(genClassPS     GEN_LIB_SRC)
add_source_subdir(genCtrl        GEN_LIB_SRC)
add_source_subdir(genDataClass   GEN_LIB_SRC)
add_source_subdir(genDFO         GEN_LIB_SRC)
add_source_subdir(genDPO         GEN_LIB_SRC)
add_source_subdir(genGrid        GEN_LIB_SRC)
add_source_subdir(genHelp        GEN_LIB_SRC)
add_source_subdir(genInterface   GEN_LIB_SRC)
add_source_subdir(genListClass   GEN_LIB_SRC)
add_source_subdir(genListClassPS GEN_LIB_SRC)
add_source_subdir(genLPO         GEN_LIB_SRC)
add_source_subdir(genMFC         GEN_LIB_SRC)
add_source_subdir(genPFO         GEN_LIB_SRC)
add_source_subdir(genPFOGL       GEN_LIB_SRC)
add_source_subdir(genPlotClass   GEN_LIB_SRC)
add_source_subdir(genPlotClassPS GEN_LIB_SRC)
add_source_subdir(genPPD         GEN_LIB_SRC)
add_source_subdir(genPPO         GEN_LIB_SRC)
add_source_subdir(genUFO         GEN_LIB_SRC)
add_source_subdir(genUIExt       GEN_LIB_SRC)
add_source_subdir(genUnits       GEN_LIB_SRC)
add_source_subdir(genUPO         GEN_LIB_SRC)
add_source_subdir(genX           GEN_LIB_SRC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

ADD_MSVC_PRECOMPILED_HEADER("GenLibAfx.h" "GenLibAfx.cpp" GEN_LIB_SRC)
list(APPEND GEN_LIB_SRC GenLib.rc)

add_library(GenLib STATIC ${GEN_LIB_SRC})

find_library(HTML_HELP_LIB 
    "HtmlHelp.lib"
    ${CMAKE_CURRENT_SOURCE_DIR}/../genLib/HTMLHelp)

target_link_libraries(GenLib ${HTML_HELP_LIB})

if (BUILD_NPRE_X)
    # GenLibX is composed of a subset of the source code, along with the ADCONSOLEAPP define

    add_source_subdir(genApp         GEN_LIB_X_SRC  AppConfigFile 
                                                    C_AllocObj 
                                                    C_AllocPage
                                                    C_CfgFileObj
                                                    C_DataPage
                                                    C_ExposedObj
                                                    C_MenuObj
                                                    C_MenuPage
                                                    C_MenuRoot
                                                    DebugUtil
                                                    ExposedList
                                                    ExposedReal
                                                    G_AppObj
                                                    G_FileNames
                                                    G_Version
                                                    MPIAppSupport)
    add_source_subdir(genClass       GEN_LIB_X_SRC)
    add_source_subdir(genClassPS     GEN_LIB_X_SRC  PS_FindFiles)
    add_source_subdir(genDataClass   GEN_LIB_X_SRC)
    add_source_subdir(genDFO         GEN_LIB_X_SRC)
    add_source_subdir(genDPO         GEN_LIB_X_SRC  DPO_BlendColorMap
                                                    DPO_DataPg
                                                    DPO_EnterColorMap
                                                    DPO_Histogram
                                                    DPO_InterpVal
                                                    DPO_LinColorMap
                                                    DPO_MergeColorMap
                                                    DPO_PenSet
                                                    DPO_Quantile
                                                    DPO_RangeExtract
                                                    DPO_ReadColorMap
                                                    DPO_ScaleTransform
                                                    DPO_SelectBase
                                                    DPO_Statistics)
    add_source_subdir(genListClass   GEN_LIB_X_SRC  C_ErrorListObj
                                                    C_ListDef
                                                    C_ListFactory
                                                    C_ListObj
                                                    LSC_Listing)
    add_source_subdir(genUFO         GEN_LIB_X_SRC)
    add_source_subdir(genUnits       GEN_LIB_X_SRC  C_GenUnits
                                                    C_UnitListObj
                                                    ExposedUnitReal
                                                    UnitConfigFile
                                                    UnitTableCol)
    add_source_subdir(genX           GEN_LIB_X_SRC)

    ADD_MSVC_PRECOMPILED_HEADER("GenLibAfx.h" "GenLibAfx.cpp" GEN_LIB_X_SRC)

    add_library(GenLibX STATIC ${GEN_LIB_X_SRC})

    set_target_properties(GenLibX PROPERTIES COMPILE_FLAGS "-DADCONSOLEAPP")

endif()
