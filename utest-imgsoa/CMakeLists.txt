# Executable for all unit tests with list of sources
# For example, you may have one ∗_test.cpp for each ∗.cpp in img-soa
add_executable(utest-imgsoa
        imgsoa-test.cpp)
# Library dependencies
target_link_libraries (utest-imgsoa
        PRIVATE
        img-soa
        GTest::gtest_main
        Microsoft.GSL::GSL)