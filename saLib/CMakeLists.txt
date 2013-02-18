
add_source_subdir(saClass        SA_LIB_SRC)
add_source_subdir(saDFO          SA_LIB_SRC)
add_source_subdir(saDPO          SA_LIB_SRC)
add_source_subdir(saLFO          SA_LIB_SRC)
add_source_subdir(saLPO          SA_LIB_SRC)
add_source_subdir(saPFO          SA_LIB_SRC)
add_source_subdir(saPFOGL        SA_LIB_SRC)
add_source_subdir(saPPO          SA_LIB_SRC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("../genLib/")
include_directories("../objLib/")

ADD_MSVC_PRECOMPILED_HEADER("saLibAfx.h" "saLibAfx.cpp" SA_LIB_SRC)

add_library(SaLib STATIC ${SA_LIB_SRC})

