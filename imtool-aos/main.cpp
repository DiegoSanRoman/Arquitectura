#include "../common/progargs.hpp"
#include "../imgaos/maxlevel.hpp"
#include "../common/binario.hpp"
#include "../imgaos/cutfreq.hpp"
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

  /*
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
  */

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
    /*
    else if (args.getOperation() == "resize") {
      processResize(args);
    }
    */
    if (args.getOperation() == "cutfreq") {
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