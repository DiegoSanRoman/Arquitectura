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

  // Nueva función para validar parámetros de la operación "resize"
  void validateResizeParams(const ProgramArgs& args) {
    if (args.getAdditionalParams().size() != 2) {
      throw std::invalid_argument("Invalid number of extra arguments for resize: " +
                                  std::to_string(args.getAdditionalParams().size()+3));
      // +3 porque estamos quitando los 3 primeros parámetros (imtool, archivo_fuente, arhivo salida)
    }

    const int newWidth = std::stoi(args.getAdditionalParams()[0]);
    if (newWidth <= 0) {
      throw std::invalid_argument("Invalid resize width: " + std::to_string(newWidth));
    }
    const int newHeight = std::stoi(args.getAdditionalParams()[1]);
    if (newHeight <= 0) {
      throw std::invalid_argument("Invalid resize height: " + std::to_string(newHeight));
    }
  }

  // Nueva función para procesar la operación "resize"
  void processResize(const ProgramArgs& args) {
    validateResizeParams(args);
    const int newWidth = std::stoi(args.getAdditionalParams()[0]);
    const int newHeight = std::stoi(args.getAdditionalParams()[1]);
    performResizeOperation(args.getInputFile(), args.getOutputFile(), newWidth, newHeight);
  }
}

int main(int argc, char* argv[]) {
  try {
    const ProgramArgs args(argc, argv);

    if (args.getOperation() == "maxlevel") {
      processMaxlevel(args);
    }
    else if (args.getOperation() == "resize") {
      processResize(args);
    }
    else {
      throw std::invalid_argument("Operación no válida");
    }

    return 0;

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return -1;
  }
}