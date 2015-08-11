cmake_minimum_required(VERSION 2.8.9)
project(AngleCorrection)
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
option(BUILD_TESTS "Build tests." ON)
endif()
set(ANGLECORRECTION_TEST_DATA_DIR "${PROJECT_SOURCE_DIR}/../../Testdata/" CACHE STRING "Directory of test data")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/)

#### Macro for adding source files and directories
macro (AngleCorrection_add_sources)
    file (RELATIVE_PATH _relPath "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach (_src ${ARGN})
        if (_relPath)
            list (APPEND AngleCorrection_SOURCE_FILES "${_relPath}/${_src}")
        else()
            list (APPEND AngleCorrection_SOURCE_FILES "${_src}")
        endif()
    endforeach()
    if (_relPath)
        # propagate AngleCorrection_SOURCE_FILES to parent directory
        set (AngleCorrection_SOURCE_FILES ${AngleCorrection_SOURCE_FILES} PARENT_SCOPE)
    endif()
endmacro()

macro (AngleCorrection_add_test_sources)
    file (RELATIVE_PATH _relPath "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach (_src ${ARGN})
        if (_relPath)
            list (APPEND AngleCorrection_TEST_SOURCE_FILES "${_relPath}/${_src}")
        else()
            list (APPEND AngleCorrection_TEST_SOURCE_FILES "${_src}")
        endif()
    endforeach()
    if (_relPath)
        # propagate AngleCorrection_TEST_SOURCE_FILES to parent directory
        set (AngleCorrection_TEST_SOURCE_FILES ${AngleCorrection_TEST_SOURCE_FILES} PARENT_SCOPE)
    endif()
endmacro()

macro (AngleCorrection_add_subdirectories)
    file (RELATIVE_PATH _relPath "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach (_src ${ARGN})
        list (APPEND AngleCorrection_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
        add_subdirectory(${_src})
    endforeach()
    if (_relPath)
        # propagate AngleCorrection_INCLUDE_DIRS to parent directory
        set (AngleCorrection_INCLUDE_DIRS ${AngleCorrection_INCLUDE_DIRS} PARENT_SCOPE)
    endif()
endmacro()


# Remove any existing "
string(REPLACE "\"" "" ANGLECORRECTION_TEST_DATA_DIR ${ANGLECORRECTION_TEST_DATA_DIR})
message("-- ANGLECORRECTION test data directory set to: \"${ANGLECORRECTION_TEST_DATA_DIR}\"")
add_definitions("-DANGLECORRECTION_TEST_DATA_DIR=\"${ANGLECORRECTION_TEST_DATA_DIR}\"")

#### Set up library dependencies
set(AngleCorrection_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

## VTK
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
set(LIBRARIES ${LIBRARIES} ${VTK_LIBRARIES})

## Eigen
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})
set(LIBRARIES ${LIBRARIES} ${EIGEN3_LIBRARIES})

AngleCorrection_add_sources(
    angle_correction_impl.h
    angle_correction_impl.cpp
)

#### Add all subdirs
AngleCorrection_add_subdirectories(lib Tests)


#### Set include dirs
include_directories(${AngleCorrection_INCLUDE_DIRS})

if(hasParent)
    set (AngleCorrection_INCLUDE_DIRS ${AngleCorrection_INCLUDE_DIRS} PARENT_SCOPE)
    set (AngleCorrection_TEST_SOURCE_FILES ${AngleCorrection_TEST_SOURCE_FILES} PARENT_SCOPE)
    set (ANGLECORRECTION_TEST_DATA_DIR ${ANGLECORRECTION_TEST_DATA_DIR}  PARENT_SCOPE)
endif()

#### Enable C++11
# Note that Microsoft Visual C++ compiler enables C++11 by default
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(--std=c++11 SUPPORTS_STD_CXX11)
    check_cxx_compiler_flag(--std=c++0x SUPPORTS_STD_CXX01)
    if(SUPPORTS_STD_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c++11")
    elseif(SUPPORTS_STD_CXX01)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c++0x")
    else()
        message(ERROR "Compiler does not support --std=c++11 or --std=c++0x.")
    endif()
endif()



if(BUILD_TESTS AND NOT hasParent)
    add_executable(test ${AngleCorrection_TEST_SOURCE_FILES} ${AngleCorrection_SOURCE_FILES})
    target_link_libraries(test ${LIBRARIES})
endif()

if(NOT hasParent)
    add_executable(angle_correction main.cpp ${AngleCorrection_SOURCE_FILES})
    target_link_libraries(angle_correction ${LIBRARIES})
endif()

