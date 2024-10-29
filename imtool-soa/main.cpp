// File: imgsoa/main.cpp
#include "../common/progargs.hpp"         // Para ProgramArgs
#include "../imgsoa/maxlevel.hpp"         // Para performMaxLevelOperation
#include "../common/binario.hpp"          // Para leerImagenPPMSoA, escribirImagenPPMSoA
#include "../common/process_functions.hpp"  // Para validateMaxlevelParams, processMaxlevel
#include <iostream>                       // Para std::cout, std::cerr
#include <exception>                      // Para std::exception
#include <stdexcept>                      // Para std::invalid_argument
#include <string>                         // Para std::string

namespace {
  using namespace common;

  // Función para validar parámetros de la operación "maxlevel"
  void processMaxlevel(const ProgramArgs& args) {
    validateMaxlevelParams(args);
    const int newMaxValue = std::stoi(args.getAdditionalParams()[0]);
    performMaxLevelOperation(args.getInputFile(), args.getOutputFile(), newMaxValue);
  }
}

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
    else {
      throw std::invalid_argument("Operación no válida");
    }

    return 0;

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return -1;
  }
}