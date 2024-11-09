// File: imtool-aos/main.cpp
#include "../common/progargs.hpp"           // Para ProgramArgs
#include "../imgaos/maxlevel.hpp"           // Para performMaxLevelOperation
#include "../common/binario.hpp"            // Para leerImagenPPM, escribirImagenPPM, info
#include "../imgaos/cutfreq.hpp"            // Para cutfreq
#include "../imgaos/resize.hpp"             // Para performResizeOperation
#include "../common/process_functions.hpp"  // Para validateMaxlevelParams, processMaxlevel
#include "../common/info.hpp"               // Para info
#include "../imgaos/compress.hpp"           // Para compress
#include <iostream>                         // Para std::cout, std::cerr
#include <exception>                        // Para std::exception
#include <stdexcept>                        // Para std::invalid_argument
#include <string>                           // Para std::string

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

  int processCutFreq(const ProgramArgs& args) {
    int number = 0;
    if (!validarParametrosCutfreq(args, number)) {
      return -1;
    }

    PPMImage image;
    // Leer la imagen de entrada
    if (!leerImagenPPM(args.getInputFile(), image)) {
      std::cerr << "Error al leer la imagen de entrada.\n";
      return -1;
    }

    // Aplicar la operación cutfreq
    cutfreq(image, number);

    // Escribir la imagen de salida
    if (!escribirImagenPPM(args.getOutputFile(), image)) {
      std::cerr << "Error al escribir la imagen de salida.\n";
      return -1;
    }

    return 0;
  }

  void processInfo(const ProgramArgs& args) {
    info(args.getInputFile());
  }

  void processCompress(const ProgramArgs& args) {
    CompressionPaths const paths = {.inputImagePath=args.getInputFile(), .outputImagePath=args.getOutputFile()};
    if (!args.getAdditionalParams().empty()) {
      std::cerr << "Error: Invalid extra arguments for compress.\n";
      throw std::invalid_argument("Número incorrecto de argumentos para 'compress'");
    }
    if (compress(paths) != 0) {
      std::cerr << "Error en la compresión de la imagen.\n";
      throw std::runtime_error("Fallo en la operación 'compress'");
    }
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
      processCutFreq(args);
    }
    else if (args.getOperation() == "info") {
      processInfo(args);
    }
    else if (args.getOperation() == "compress") {
      processCompress(args);
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