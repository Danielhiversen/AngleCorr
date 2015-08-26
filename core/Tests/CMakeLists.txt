set(AngleCorrection_TEST_SOURCE_FILES
    catch.hpp
    CatchMain.cpp
    MethodTests.cpp
)

add_executable(test ${AngleCorrection_TEST_SOURCE_FILES})
target_link_libraries(test AngleCorr ${LIBRARIES})
