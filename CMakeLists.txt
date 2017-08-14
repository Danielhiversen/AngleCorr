project(org_custusx_anglecorrection)

#### Enable C++11
set (CMAKE_CXX_STANDARD 11)

add_subdirectory(core)

include(GenerateExportHeader)
set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
  cxAngleCorrectionPluginActivator.cpp
  cxAngleCorrectionExecuter.cpp
  cxAngleCorrectionGUIExtenderService.h
  cxAngleCorrectionGUIExtenderService.cpp
  cxAngleCorrectionWidget.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  cxAngleCorrectionPluginActivator.h
  cxAngleCorrectionExecuter.h
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
  ${CX_CTK_PLUGIN_NO_INSTALL}
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
