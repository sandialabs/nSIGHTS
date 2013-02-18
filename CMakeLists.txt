cmake_minimum_required(VERSION 2.8.0)

project(nSIGHTS)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ----------------------------------------------------------------------------
# Options
# ----------------------------------------------------------------------------

set (BUILD_NPRE_X 0 CACHE BOOL "Set to true to build console version of nPre (nPreX)")

# ----------------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------------

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

set (BLAS_LIBRARY "" CACHE FILEPATH "Path to libblas.lib")
set (LAPACK_LIBRARY "" CACHE FILEPATH "Path to liblapack.lib")

if (NOT BLAS_LIBRARY OR NOT LAPACK_LIBRARY)
    message(FATAL_ERROR
        "BLAS_LIBRARY and/or LAPACK_LIBRARY are not set.  "
        "They should be set to the appropriate files (typically libblas.lib and liblapack.lib) "
    )
endif ()

if (BUILD_NPRE_X)
    find_package(MPI REQUIRED)
    include_directories(${MPI_C_INCLUDE_PATH})
endif()

# MFC Stuff, will go away once we move to wxWidgets
add_definitions(-D_AFXDLL -DMSCVS2005 -D_WINDOWS -DWINVER=0x0500 -D_WIN32_WINNT=0x0501)
set(CMAKE_MFC_FLAG 2) 

macro(add_msvc_precompiled_header PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "$(IntDir)/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
endmacro(add_msvc_precompiled_header)

macro(add_source_subdir fname sourcesvar)

    set(FOLDER_SRC)
    set(FOLDER_INC)

    if(${ARGC} GREATER 2)
        # use optional filter
        foreach(f ${ARGN})
            #message(STATUS " Filter: " ${f})
            file(GLOB_RECURSE FOLDER_SRC_FILT "${fname}/${f}.cpp")
            list(LENGTH FOLDER_SRC_FILT LISTLEN)
            if(${LISTLEN} EQUAL 0)
                message(ERROR " Could not find source file " ${f}.cpp ", check CMake script")
            endif()
            file(GLOB_RECURSE FOLDER_INC_FILT "${fname}/${f}.h")
            list(APPEND FOLDER_SRC ${FOLDER_SRC_FILT})
            list(APPEND FOLDER_INC ${FOLDER_INC_FILT})
        endforeach()
        #message(STATUS " Result: " ${FOLDER_SRC})
    else()
        file(GLOB_RECURSE FOLDER_SRC "${fname}/*.cpp")
        file(GLOB_RECURSE FOLDER_INC "${fname}/*.h")
    endif()


    source_group("Source Files\\${fname}" FILES ${FOLDER_SRC})
    source_group("Header Files\\${fname}" FILES ${FOLDER_INC})

    list(APPEND ${sourcesvar} ${FOLDER_SRC})
    list(APPEND ${sourcesvar} ${FOLDER_INC})
endmacro()


# ----------------------------------------------------------------------------
# Components
# ----------------------------------------------------------------------------

add_subdirectory(genLib)
add_subdirectory(objLib)
add_subdirectory(osLib)
add_subdirectory(nsLib)
add_subdirectory(saLib)

add_subdirectory(nPre)
add_subdirectory(nPost)

# ----------------------------------------------------------------------------
# Warn in the case of in-source build
# ----------------------------------------------------------------------------
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  message(WARNING "The source directory is the same as binary directory. \"make clean\" may damage the source tree")
endif()


