// File: imgsoa/main.cpp
#include "../common/progargs.hpp"         // Para ProgramArgs
#include "../imgsoa/maxlevel.hpp"         // Para performMaxLevelOperation
#include "../common/binario.hpp"          // Para leerImagenPPMSoA, escribirImagenPPMSoA
#include <iostream>                       // Para std::cout, std::cerr
#include <exception>                      // Para std::exception
#include <stdexcept>                      // Para std::invalid_argument
#include <string>                         // Para std::string

namespace {
  // Valor máximo permitido para los colores en una imagen PPM
  constexpr int MAX_COLOR_VALUE = 65535;

  // Función para validar los parámetros de la operación "maxlevel"
  void validateMaxlevelParams(const ProgramArgs& args) {
    // Verificar que se haya proporcionado un solo parámetro adicional
    if (args.getAdditionalParams().size() != 1) {
      throw std::invalid_argument("Invalid number of extra arguments for maxlevel: " +
                                  std::to_string(args.getAdditionalParams().size() + 3));
      // +3 porque estamos quitando los 3 primeros parámetros (imtool, archivo_fuente, archivo salida)
    }

    // Verificar que el nuevo valor máximo sea válido
    const int newMaxValue = std::stoi(args.getAdditionalParams()[0]);
    if (newMaxValue < 0 || newMaxValue > MAX_COLOR_VALUE) {
      throw std::invalid_argument("The max level must be between 0 and " +
                                  std::to_string(MAX_COLOR_VALUE));
    }
  }

  // Función para procesar la operación "maxlevel"
  void processMaxlevel(const ProgramArgs& args) {
    // Validar los parámetros de la operación
    validateMaxlevelParams(args);
    // Obtener el nuevo valor máximo
    const int newMaxValue = std::stoi(args.getAdditionalParams()[0]);
    // Llamar a la función que realiza la operación
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