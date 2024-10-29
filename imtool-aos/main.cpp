// File: imtool-aos/main.cpp
#include "../common/progargs.hpp"         // Para ProgramArgs
#include "../imgaos/maxlevel.hpp"         // Para performMaxLevelOperation
#include "../common/binario.hpp"          // Para leerImagenPPM, escribirImagenPPM
#include "../imgaos/cutfreq.hpp"          // Para cutfreq
#include "../imgaos/resize.hpp"           // Para performResizeOperation
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

  bool validarParametrosCutfreq(const ProgramArgs& args, int& number) {
    if (args.getAdditionalParams().empty()) {
      std::cerr << "Error: Se requiere el número de colores a eliminar.\n";
      return false;
    }
    try {
      number = std::stoi(args.getAdditionalParams()[0]);
      if (number <= 0) {
        std::cerr << "Error: El número de colores a eliminar debe ser positivo.\n";
        return false;
      }
    } catch (const std::exception&) {
      std::cerr << "Error: El parámetro proporcionado no es un número válido.\n";
      return false;
    }
    return true;
  }

  void ejecutarCutfreq(PPMImage& image, int number) {
    cutfreq(image, number);
  }
}

int main(int argc, char* argv[]) {
  try {
    const ProgramArgs args(argc, argv);

    PPMImage image;
    if (!leerImagenPPM(args.getInputFile(), image)) {
      return -1;
    }

    if (args.getOperation() == "maxlevel") {
      processMaxlevel(args);
    }
    else if (args.getOperation() == "resize") {
      processResize(args);
    }
    else if (args.getOperation() == "cutfreq") {
      int number = 0;
      if (!validarParametrosCutfreq(args, number)) {
        return -1;
      }
      ejecutarCutfreq(image, number);
      if (!escribirImagenPPM(args.getOutputFile(), image)) {
        std::cerr << "Error al escribir la imagen de salida.\n";
        return -1;
      }
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