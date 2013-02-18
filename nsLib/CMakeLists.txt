
add_source_subdir(nsClass       NS_LIB_SRC)
add_source_subdir(nsDataClass   NS_LIB_SRC)
add_source_subdir(nsDFO         NS_LIB_SRC)
add_source_subdir(nsDPO         NS_LIB_SRC)
add_source_subdir(nsHelp        NS_LIB_SRC)
add_source_subdir(nsPFO         NS_LIB_SRC)
add_source_subdir(nsPFOGL       NS_LIB_SRC)
add_source_subdir(nsPlotClass   NS_LIB_SRC)
add_source_subdir(nsPPO         NS_LIB_SRC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("../genLib/")
include_directories("../objLib/")
include_directories("../osLib/")

ADD_MSVC_PRECOMPILED_HEADER("NsLibAfx.h" "NsLibAfx.cpp" NS_LIB_SRC)
add_library(NsLib STATIC ${NS_LIB_SRC})

if (BUILD_NPRE_X)
    # NsLibX is composed of a subset of the source code, along with the ADCONSOLEAPP define

    add_source_subdir(nsClass       NS_LIB_X_SRC)
    add_source_subdir(nsDataClass   NS_LIB_X_SRC)
    add_source_subdir(nsDFO         NS_LIB_X_SRC)
    add_source_subdir(nsDPO         NS_LIB_X_SRC  DPO_BarometricCompensation
                                                  DPO_BasicSequenceFit
                                                  DPO_BasicTimeExtract
                                                  DPO_BEETCompensation
                                                  DPO_CreateBEETResponseFunction
                                                  DPO_Derivative
                                                  DPO_ExtendedProfileScaleTransform
                                                  DPO_ExtractSequenceInterval
                                                  DPO_PulseNormalize
                                                  DPO_ReadMiniTroll
                                                  DPO_ReadProfileSimResults
                                                  DPO_ReadSequenceTimes
                                                  DPO_ReadXYSimResults
                                                  DPO_SelectExtendedProfile
                                                  DPO_SelectProfilefromProfileSimResults
                                                  DPO_SelectXYfromXYSimResults
                                                  DPO_TimeProcess
                                                  nsDPOConfigFile)

    ADD_MSVC_PRECOMPILED_HEADER("NsLibAfx.h" "NsLibAfx.cpp" NS_LIB_X_SRC)

    add_library(NsLibX STATIC ${NS_LIB_X_SRC})

    set_target_properties(NsLibX PROPERTIES COMPILE_FLAGS "-DADCONSOLEAPP")

endif()
