# File: ftest-soa/CMakeLists.txt

# Crear el ejecutable de prueba funcional
add_executable(ftest-soa
        compress-ftest.cpp)

# Vincular bibliotecas necesarias
target_link_libraries(ftest-soa
        PRIVATE
        imgsoa
        GTest::gtest_main
        Microsoft.GSL::GSL)

include(GoogleTest)
gtest_discover_tests(ftest-soa)