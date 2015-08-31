if(BUILD_TESTING)
   add_definitions("-DANGLECORRECTION_TEST_DATA_DIR=\"${CX_LARGE_DATA_ROOT}/testing\"")

    set(CX_TEST_PLUGINCALIBRATION_SOURCE_FILES_GUI
        cxtestAngleCorr.cpp
        testExportWinHack.h
        testExportWinHack.cpp
        )

    add_library(cxtest_org_custusx_anglecorrection_gui ${CX_TEST_PLUGINCALIBRATION_SOURCE_FILES_GUI})
    generate_export_header(cxtest_org_custusx_anglecorrection_gui)
    target_include_directories(cxtest_org_custusx_anglecorrection_gui
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_anglecorrection_gui PRIVATE org_custusx_anglecorrection cxtestUtilities cxCatch)
    cx_add_tests_to_catch(cxtest_org_custusx_anglecorrection_gui)


endif(BUILD_TESTING)
