
add_source_subdir(App   NPRE_SRC)
add_source_subdir(Auto  NPRE_SRC)
add_source_subdir(Run   NPRE_SRC)
add_source_subdir(Sim   NPRE_SRC)
add_source_subdir(Var   NPRE_SRC)
add_source_subdir(Dlg   NPRE_SRC)
add_source_subdir(UI    NPRE_SRC)

# Link in the alloc files. From code comments about these files:
#   "If linked into an application the application can create the associated objects.
#    Code in this directory is not physically included in the library itself but is
#    conceptually part of the library."
add_source_subdir(../genLib/genAlloc   NPRE_SRC)
add_source_subdir(../objLib/objAlloc   NPRE_SRC)
add_source_subdir(../osLib/osAlloc     NPRE_SRC)
add_source_subdir(../nsLib/nsAlloc     NPRE_SRC)

list(APPEND NPRE_SRC MainFrm.h  MainFrm.cpp)
list(APPEND NPRE_SRC nPre.h     nPre.cpp)
list(APPEND NPRE_SRC nPreDoc.h  nPreDoc.cpp)
list(APPEND NPRE_SRC nPreView.h nPreView.cpp)
list(APPEND NPRE_SRC ProgressThreadDlg.h ProgressThreadDlg.cpp)
list(APPEND NPRE_SRC PopupCmdDefines.h )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories("../genLib/")
include_directories("../objLib/")
include_directories("../osLib/")
include_directories("../nsLib/")

ADD_MSVC_PRECOMPILED_HEADER("StdAfx.h" "StdAfx.cpp" NPRE_SRC)
list(APPEND NPRE_SRC nPre.rc)

add_executable(nPre WIN32 ${NPRE_SRC})

target_link_libraries(nPre 
    ${OPENGL_LIBRARIES}
    ${BLAS_LIBRARY}
    ${LAPACK_LIBRARY}
    GenLib
    ObjLib
    OsLib
    NsLib)


# nPreX (console version w/MPI support)
if (BUILD_NPRE_X)

    add_source_subdir(App       NPRE_X_SRC  AppFuncObj
                                            AppPages
                                            nPreFile
                                            nPreRoot)
    add_source_subdir(Run       NPRE_X_SRC  RunCommon)
    add_source_subdir(CmdLine   NPRE_X_SRC)
    add_source_subdir(Sim       NPRE_X_SRC)
    add_source_subdir(Var       NPRE_X_SRC)

    add_source_subdir(../genLib/genAlloc   NPRE_SRC)
    add_source_subdir(../objLib/objAlloc   NPRE_SRC)
    add_source_subdir(../osLib/osAlloc     NPRE_SRC)
    add_source_subdir(../nsLib/nsAlloc     NPRE_SRC)

    ADD_MSVC_PRECOMPILED_HEADER("StdAfx.h" "StdAfx.cpp" NPRE_X_SRC)

    add_executable(nPreX ${NPRE_X_SRC}) # NOT WIN32

    set_target_properties(nPreX PROPERTIES COMPILE_FLAGS "-DADCONSOLEAPP")

    target_link_libraries(nPreX
        GenLibX
        ObjLibX
        OsLibX
        NsLibX
        ${BLAS_LIBRARY}
        ${LAPACK_LIBRARY}
        ${MPI_LIBRARY})
endif()


