
add_source_subdir(App   NPOST_SRC)

# Link in the alloc files. From code comments about these files:
#   "If linked into an application the application can create the associated objects.
#    Code in this directory is not physically included in the library itself but is
#    conceptually part of the library."
add_source_subdir(../genLib/genAlloc   NPRE_SRC)
add_source_subdir(../objLib/objAlloc   NPRE_SRC)
add_source_subdir(../osLib/osAlloc     NPRE_SRC)
add_source_subdir(../nsLib/nsAlloc     NPRE_SRC)
add_source_subdir(../saLib/saAlloc     NPRE_SRC)

list(APPEND NPOST_SRC MainFrm.h  MainFrm.cpp)
list(APPEND NPOST_SRC nPost.h    nPost.cpp)
list(APPEND NPOST_SRC ProjectDoc.h  ProjectDoc.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("../genLib/")
include_directories("../objLib/")
include_directories("../osLib/")
include_directories("../nsLib/")
include_directories("../saLib/")

ADD_MSVC_PRECOMPILED_HEADER("StdAfx.h" "StdAfx.cpp" NPOST_SRC)
list(APPEND NPOST_SRC nPost.rc)

add_executable(nPost WIN32 ${NPOST_SRC})

target_link_libraries(nPost 
    ${OPENGL_LIBRARIES}
    ${BLAS_LIBRARY}
    ${LAPACK_LIBRARY}
    GenLib
    ObjLib
    OsLib
    NsLib
    SaLib)



