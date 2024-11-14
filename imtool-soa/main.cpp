// File: imtool-soa/main.cpp
#include "../common/progargs.hpp"           // Para ProgramArgs
#include "../imgsoa/maxlevel.hpp"           // Para performMaxLevelOperation
#include "../imgsoa/resize.hpp"             // Para performResizeOperation
#include "../common/binario.hpp"            // Para leerImagenPPMSoA, escribirImagenPPMSoA
#include "../common/info.hpp"               // Para processInfo
#include "../imgsoa/compress.hpp"           // Para processCompress
#include "../imgsoa/cutfreq.hpp"            // Para cutfreq (SOA)
#include <iostream>                         // Para std::cout, std::cerr
#include <exception>                        // Para std::exception
#include <stdexcept>                        // Para std::invalid_argument
#include <string>                           // Para std::string

namespace {
  using namespace common;
  constexpr int MAX_COLOR_VALUE = 65535;

  // Función para validar parámetros de la operación "maxlevel"
  void validateMaxlevelParams(const ProgramArgs& args) {
    if (args.getAdditionalParams().size() != 1) {
      throw std::invalid_argument("Invalid number of extra arguments for maxlevel: " +
                                  std::to_string(args.getAdditionalParams().size() + 3));
    }

    const int newMaxValue = std::stoi(args.getAdditionalParams()[0]);
    if (newMaxValue < 0 || newMaxValue > MAX_COLOR_VALUE) {
      throw std::invalid_argument("The max level must be between 0 and " +
                                  std::to_string(MAX_COLOR_VALUE));
    }
  }

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

  // Nueva función para procesar la operación "cutfreq" en SOA
  void processCutfreq(const ProgramArgs& args) {
    if (args.getAdditionalParams().size() != 1) {
      throw std::invalid_argument("Invalid number of arguments for cutfreq.");
    }

    const int number = std::stoi(args.getAdditionalParams()[0]);
    PPMImageSoA image;

    if (!leerImagenPPMSoA(args.getInputFile(), image)) {
      throw std::runtime_error("Error al leer la imagen en formato SOA.");
    }

    cutfreq(image, number);  // Procesa la frecuencia de corte en SOA

    if (!escribirImagenPPMSoA(args.getOutputFile(), image)) {
      throw std::runtime_error("Error al escribir la imagen en formato SOA.");
    }
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
    else if (args.getOperation() == "info") {
      processInfo(args);
    }
    else if (args.getOperation() == "compress") {
      processCompress(args);
    }
    else if (args.getOperation() == "cutfreq") {  // Añadimos la operación cutfreq
      processCutfreq(args);
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