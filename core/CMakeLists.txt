cmake_minimum_required(VERSION 2.8.9)
get_directory_property(hasParent PARENT_DIRECTORY)

project(AngleCorrection)

cmake_policy(SET CMP0012 NEW)

if(NOT hasParent)
    option(BUILD_TESTS "Build tests." ON)
    set(ANGLECORRECTION_TEST_DATA_DIR "${PROJECT_SOURCE_DIR}/../../Testdata/" CACHE STRING "Directory of test data")
    set(EIGEN_DIR "" CACHE STRING "Directory of Eigen")

    string(REPLACE "\"" "" ANGLECORRECTION_TEST_DATA_DIR ${ANGLECORRECTION_TEST_DATA_DIR})
    message("-- ANGLECORRECTION test data directory set to: \"${ANGLECORRECTION_TEST_DATA_DIR}\"")
    add_definitions("-DANGLECORRECTION_TEST_DATA_DIR=\"${ANGLECORRECTION_TEST_DATA_DIR}\"")
    add_definitions("-DEIGEN_DIR=\"${EIGEN_DIR}\"")
endif()

## VTK
if(NOT VTK_FOUND)
    find_package(VTK REQUIRED)
    include(${VTK_USE_FILE})
endif()
set(LIBRARIES ${LIBRARIES} ${VTK_LIBRARIES})


## Eigen
if(NOT EIGEN_FOUND AND EIGEN_DIR)
    include_directories(${EIGEN_DIR})
elseif(NOT EIGEN_FOUND)
    find_package(Eigen3 REQUIRED)
    include_directories(${EIGEN3_INCLUDE_DIR} )
endif()


set(AngleCorrection_SOURCE_FILES
    AngleCorrection.h
    AngleCorrection.cpp
    adjlist.hpp
    helpers.hpp
    intersection.hpp
    intersection_set.hpp
    matrix.hpp
    metaimage.hpp
    plane3d.hpp
    precision.hpp
    quadratic_spline_fitter.hpp
    spline3d.hpp
    ErrorHandler.hpp
    ErrorHandler.cpp
)

add_library(AngleCorr STATIC ${AngleCorrection_SOURCE_FILES})
target_link_libraries(AngleCorr ${LIBRARIES})


target_include_directories (AngleCorr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR})

if(NOT hasParent)
    add_executable(angle_correction main.cpp )
    target_link_libraries(angle_correction AngleCorr)


    add_subdirectory(Tests)
endif()
