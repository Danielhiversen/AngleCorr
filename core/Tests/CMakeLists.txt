if(BUILD_TESTS_CX_ANGLECORR)
   add_definitions("-DANGLECORRECTION_TEST_DATA_DIR=\"${CX_LARGE_DATA_ROOT}/testing\"")

    set(CX_TEST_CATCH_ORG_CUSTUSX_ANGLECORRECTION_SOURCE_FILES
        MethodTests.cpp
        testExportWinHack.h
        testExportWinHack.cpp
    )

    add_library(cxtest_org_custusx_anglecorrection ${CX_TEST_CATCH_ORG_CUSTUSX_ANGLECORRECTION_SOURCE_FILES})
    generate_export_header(cxtest_org_custusx_anglecorrection)
    target_include_directories(cxtest_org_custusx_anglecorrection
        PUBLIC
        .
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(cxtest_org_custusx_anglecorrection PRIVATE org_custusx_anglecorrection cxtestUtilities cxCatch)
    cx_add_tests_to_catch(cxtest_org_custusx_anglecorrection)

else()

    set(AngleCorrection_TEST_SOURCE_FILES
        catch.hpp
        CatchMain.cpp
        MethodTests.cpp
    )

    add_executable(Catch ${AngleCorrection_TEST_SOURCE_FILES})
    target_link_libraries(Catch AngleCorr)
endif(BUILD_TESTS_CX_ANGLECORR)
