// File: imtool-soa/main.cpp
#include "../common/progargs.hpp"           // Para ProgramArgs
#include "../imgsoa/maxlevel.hpp"           // Para performMaxLevelOperation
#include "../imgsoa/resize.hpp"           // Para performResizeOperation
#include "../common/binario.hpp"            // Para leerImagenPPMSoA, escribirImagenPPMSoA
#include "../common/process_functions.hpp"  // Para validateMaxlevelParams, processMaxlevel
#include <iostream>                         // Para std::cout, std::cerr
#include <exception>                        // Para std::exception
#include <stdexcept>                        // Para std::invalid_argument
#include <string>                           // Para std::string

namespace {
  using namespace common;

  // Función para validar parámetros de la operación "maxlevel"
  void processMaxlevel(ProgramArgs const & args) {
    validateMaxlevelParams(args);
    int const newMaxValue = std::stoi(args.getAdditionalParams()[0]);
    performMaxLevelOperation(args.getInputFile(), args.getOutputFile(), newMaxValue);
  }

  // Función para validar parámetros de la operación "resize"
  void validateResizeParams(const ProgramArgs& args) {
    if (args.getAdditionalParams().size() != 2) {
      throw std::invalid_argument("Invalid number of extra arguments for resize: " +
                                  std::to_string(args.getAdditionalParams().size() + 3));
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

}  // namespace

int main(int argc, char* argv[]) {
  try {
    const ProgramArgs args(argc, argv);

    PPMImageSoA image;
    if (!leerImagenPPMSoA(args.getInputFile(), image)) {
      return -1;
    }

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