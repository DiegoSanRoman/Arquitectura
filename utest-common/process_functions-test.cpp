// File: utest-common/process_functions-test.cpp
#include "../common/process_functions.hpp"
#include "../common/progargs.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <string>

namespace {
    constexpr int VALID_MAX_LEVEL = 65535;  // Valor máximo válido
    constexpr int INVALID_MAX_LEVEL_LOW = -1;  // Valor inválido bajo
    constexpr int INVALID_MAX_LEVEL_HIGH = 70000;  // Valor inválido alto
}

// Clase de prueba para ProcessFunctions
class ProcessFunctionsTest : public ::testing::Test {
protected:
    static ProgramArgs createArgsWithParams(const std::vector<std::string>& additionalParams) {
        // Inicializa los argumentos necesarios
        std::vector<std::string> args = {"program", "input.ppm", "output.ppm", "maxlevel"};
        args.insert(args.end(), additionalParams.begin(), additionalParams.end());

        // Convierte std::vector<std::string> a std::vector<char*>
        std::vector<char*> argv;
        argv.reserve(args.size());
        for (auto& arg : args) {
            argv.push_back(arg.data());
        }
        int const argc = static_cast<int>(argv.size());
        return ProgramArgs(argc, argv.data());
    }
};

TEST_F(ProcessFunctionsTest, ValidateMaxlevelParams_ValidMaxLevel) {
    // Crea argumentos con un nivel de maxlevel dentro del rango permitido
    ProgramArgs const args = createArgsWithParams({std::to_string(VALID_MAX_LEVEL)});
    EXPECT_NO_THROW(common::validateMaxlevelParams(args));
}

TEST_F(ProcessFunctionsTest, ValidateMaxlevelParams_LevelTooLow) {
    // Crea argumentos con un nivel de maxlevel menor que el rango permitido
    ProgramArgs const args = createArgsWithParams({std::to_string(INVALID_MAX_LEVEL_LOW)});
    EXPECT_THROW(common::validateMaxlevelParams(args), std::invalid_argument);
}

TEST_F(ProcessFunctionsTest, ValidateMaxlevelParams_LevelTooHigh) {
    // Crea argumentos con un nivel de maxlevel mayor que el rango permitido
    ProgramArgs const args = createArgsWithParams({std::to_string(INVALID_MAX_LEVEL_HIGH)});
    EXPECT_THROW(common::validateMaxlevelParams(args), std::invalid_argument);
}

TEST_F(ProcessFunctionsTest, ValidateMaxlevelParams_InsufficientParams) {
    // Crea argumentos con un número insuficiente de parámetros adicionales
    ProgramArgs const args = createArgsWithParams({});
    EXPECT_THROW(common::validateMaxlevelParams(args), std::invalid_argument);
}

TEST_F(ProcessFunctionsTest, ValidateMaxlevelParams_ExcessiveParams) {
    // Crea argumentos con un número excesivo de parámetros adicionales
    ProgramArgs const args = createArgsWithParams({"100", "200"});
    EXPECT_THROW(common::validateMaxlevelParams(args), std::invalid_argument);
}
