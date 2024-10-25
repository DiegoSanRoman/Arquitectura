#include "../common/progargs.hpp"
#include "../imgaos/maxlevel.hpp"
#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>

namespace {
  constexpr int EXPECTED_ARGS_MAXLEVEL = 5;
  constexpr int MAX_COLOR_VALUE = 65535;

  void validateMaxlevelParams(const ProgramArgs& args) {
    if (args.getAdditionalParams().size() != 1) {
      throw std::invalid_argument("El número de argumentos para maxlevel debe ser exactamente " +
                                std::to_string(EXPECTED_ARGS_MAXLEVEL - 3));
    }

    const int newMaxValue = std::stoi(args.getAdditionalParams()[0]);
    if (newMaxValue < 0 || newMaxValue > MAX_COLOR_VALUE) {
      throw std::invalid_argument("El nivel máximo debe estar entre 0 y " +
                                std::to_string(MAX_COLOR_VALUE));
    }
  }

  void processMaxlevel(const ProgramArgs& args) {
    validateMaxlevelParams(args);
    const int newMaxValue = std::stoi(args.getAdditionalParams()[0]);
    performMaxLevelOperation(args.getInputFile(), args.getOutputFile(), newMaxValue);
  }
}

int main(int argc, char* argv[]) {
  try {
    const ProgramArgs args(argc, argv);

    if (args.getOperation() == "maxlevel") {
      processMaxlevel(args);
    } else {
      throw std::invalid_argument("Operación no válida");
    }

    return 0;

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return -1;
  }
}