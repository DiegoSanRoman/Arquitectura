# File: ftest-aos/CMakeLists.txt
add_executable(ftest-aos
        maxlevel-ftest.cpp)

target_link_libraries(ftest-aos
        PRIVATE
        imgaos
        GTest::gtest_main
        Microsoft.GSL::GSL)

include(GoogleTest)
gtest_discover_tests(ftest-aos)