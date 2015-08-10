project(org_custusx_anglecorrection)


set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")


set(PLUGIN_SRCS
  cxAngleCorrectionPluginActivator.cpp
  cxAngleCorrectionExecuter.cpp
  cxAngleCorrectionGUIExtenderService.cpp
  cxAngleCorrectionWidget.cpp
  core/angle_correction_impl.cpp
  core/angle_correction_impl.h
)



add_subdirectory(core) 
include_directories(${AngleCorrection_INCLUDE_DIRS}) 

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxAngleCorrectionPluginActivator.h
  cxAngleCorrectionExecuter.h
  cxAngleCorrectionGUIExtenderService.h
  cxAngleCorrectionWidget.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)


#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries 
    ${PLUGIN_target_libraries}   
	cxResource
	cxResourceWidgets
	cxLogicManager
	cxGUIExtenderService
	org_custusx_acquisition
    )

set(PLUGIN_OUTPUT_DIR "")
if(CX_WINDOWS)
    #on windows we want dlls to be placed with the executables
    set(PLUGIN_OUTPUT_DIR "../")
endif(CX_WINDOWS)

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


ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
  OUTPUT_DIR ${PLUGIN_OUTPUT_DIR}
)

target_include_directories(org_custusx_anglecorrection
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
)


if(BUILD_TESTING)
    foreach (_src ${AngleCorrection_TEST_SOURCE_FILES})
        list (APPEND CX_TEST_PLUGINANGLECORR_SOURCE_FILES "core/${_src}")
    endforeach()

    #set(ANGLECORRECTION_TEST_DATA_DIR ${ANGLECORRECTION_TEST_DATA_DIR} CACHE)
    # Remove any existing "
    string(REPLACE "\"" "" ANGLECORRECTION_TEST_DATA_DIR ${ANGLECORRECTION_TEST_DATA_DIR})
    add_definitions("-DANGLECORRECTION_TEST_DATA_DIR=\"${ANGLECORRECTION_TEST_DATA_DIR}\"")

    add_library(cxtest_org_custusx_anglecorrection ${CX_TEST_PLUGINANGLECORR_SOURCE_FILES})
    include(GenerateExportHeader)
    generate_export_header(cxtest_org_custusx_anglecorrection)
    target_include_directories(cxtest_org_custusx_anglecorrection
        PUBLIC
        org_custusx_anglecorrection
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_anglecorrection PRIVATE org_custusx_anglecorrection cxCatch)
    cx_add_tests_to_catch(cxtest_org_custusx_anglecorrection)


endif(BUILD_TESTING)
