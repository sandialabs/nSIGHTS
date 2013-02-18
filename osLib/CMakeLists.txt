
add_source_subdir(osClass       OS_LIB_SRC)
add_source_subdir(osDataClass   OS_LIB_SRC)
add_source_subdir(osDFO         OS_LIB_SRC)
add_source_subdir(osDlg         OS_LIB_SRC)
add_source_subdir(osDPO         OS_LIB_SRC)
add_source_subdir(osHelp        OS_LIB_SRC)
add_source_subdir(osLFO         OS_LIB_SRC)
add_source_subdir(osLPO         OS_LIB_SRC)
add_source_subdir(osMain        OS_LIB_SRC)
add_source_subdir(osPFO         OS_LIB_SRC)
add_source_subdir(osPFOGL       OS_LIB_SRC)
add_source_subdir(osPPO         OS_LIB_SRC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("../genLib/")
include_directories("../objLib/")

ADD_MSVC_PRECOMPILED_HEADER("OsLibAfx.h" "OsLibAfx.cpp" OS_LIB_SRC)
list(APPEND OS_LIB_SRC osLib.rc)

add_library(OsLib STATIC ${OS_LIB_SRC})

if (BUILD_NPRE_X)
    # OsLibX is composed of a subset of the source code, along with the ADCONSOLEAPP define

    add_source_subdir(osClass       OS_LIB_X_SRC)
    add_source_subdir(osDataClass   OS_LIB_X_SRC)
    add_source_subdir(osDFO         OS_LIB_X_SRC)
    add_source_subdir(osDPO         OS_LIB_X_SRC  DPO_BasicResidual
                                                  DPO_CalcConfidence
                                                  DPO_CalcConfidenceGrid
                                                  DPO_CalcConfidenceTable
                                                  DPO_CompositeFit
                                                  DPO_JacobToTable
                                                  DPO_ReadOptSimResults
                                                  DPO_ReadRangeSimResults
                                                  DPO_ResidualDiagnostic
                                                  DPO_ResidualHistogram
                                                  DPO_SelectOpt
                                                  DPO_SelectOptCovar
                                                  DPO_SelectOptJacob
                                                  DPO_SelectOptResid
                                                  DPO_SelectOptResults
                                                  DPO_SelectRange
                                                  DPO_SelectRangeCube
                                                  DPO_SelectRangeGrid
                                                  DPO_SelectRTRangeCube
                                                  DPO_SelectRTRangeGrid
                                                  DPO_SingleFit)
    add_source_subdir(osMain        OS_LIB_X_SRC)

    ADD_MSVC_PRECOMPILED_HEADER("OsLibAfx.h" "OsLibAfx.cpp" OS_LIB_X_SRC)

    add_library(OsLibX STATIC ${OS_LIB_X_SRC})

    set_target_properties(OsLibX PROPERTIES COMPILE_FLAGS "-DADCONSOLEAPP")

endif()

