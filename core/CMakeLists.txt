cmake_minimum_required(VERSION 2.8.9)
get_directory_property(hasParent PARENT_DIRECTORY)

project(AngleCorrection)

if(NOT hasParent)
    option(BUILD_TESTS "Build tests." ON)
    set(ANGLECORRECTION_TEST_DATA_DIR "${PROJECT_SOURCE_DIR}/../../Testdata/" CACHE STRING "Directory of test data")
endif()



if(NOT hasParent)
    # Remove any existing "
    string(REPLACE "\"" "" ANGLECORRECTION_TEST_DATA_DIR ${ANGLECORRECTION_TEST_DATA_DIR})
    message("-- ANGLECORRECTION test data directory set to: \"${ANGLECORRECTION_TEST_DATA_DIR}\"")
    add_definitions("-DANGLECORRECTION_TEST_DATA_DIR=\"${ANGLECORRECTION_TEST_DATA_DIR}\"")
endif()


## VTK
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
set(LIBRARIES ${LIBRARIES} ${VTK_LIBRARIES})

## Eigen
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})
set(LIBRARIES ${LIBRARIES} ${EIGEN3_LIBRARIES})


set(AngleCorrection_SOURCE_FILES
    AngleCorrection.h
    AngleCorrection.cpp
    lib/adjlist.hpp
    lib/helpers.hpp
    lib/intersection.hpp
    lib/intersection_set.hpp
    lib/matrix.hpp
    lib/metaimage.hpp
    lib/plane3d.cpp
    lib/plane3d.hpp
    lib/precision.hpp
    lib/quadratic_spline_fitter.hpp
    lib/spline3d.hpp
    lib/ErrorHandler.hpp
)



add_library(angleCorrLib SHARED ${AngleCorrection_SOURCE_FILES})
target_include_directories (angleCorrLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR} "${PROJECT_SOURCE_DIR}/lib")


if(NOT hasParent)

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

#    if(BUILD_TESTS)
#   add_subdirectory(testing)
#        add_executable(test ${AngleCorrection_TEST_SOURCE_FILES} ${AngleCorrection_SOURCE_FILES})
#        target_link_libraries(test ${LIBRARIES})
#    endif()

    add_executable(angle_correction main.cpp )
    target_link_libraries(angle_correction  ${angleCorrLib} ${LIBRARIES})
endif()

