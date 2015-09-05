project(org_custusx_anglecorrection)

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
    if(APPLE)
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
endif()
add_subdirectory(core)

include(GenerateExportHeader)
set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
  cxAngleCorrectionPluginActivator.cpp
  cxAngleCorrectionExecuter.cpp
  cxAngleCorrectionGUIExtenderService.cpp
  cxAngleCorrectionWidget.cpp
)

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
        PUBLIC
	cxResource
	cxResourceWidgets
	cxLogicManager
	cxGUIExtenderService
	org_custusx_acquisition
        AngleCorr
    )

set(PLUGIN_OUTPUT_DIR "")
if(CX_WINDOWS)
    #on windows we want dlls to be placed with the executables
    set(PLUGIN_OUTPUT_DIR "../")
endif(CX_WINDOWS)



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
    ./core
    ${CMAKE_CURRENT_BINARY_DIR}
)

if(BUILD_TESTING)
    option(BUILD_TESTS_CX_ANGLECORR "Build cx angleCorr core tests." ON)
    mark_as_advanced(BUILD_TESTS_CX_ANGLECORR)
    add_subdirectory(core/Tests)
    add_subdirectory(testing)
endif(BUILD_TESTING)
