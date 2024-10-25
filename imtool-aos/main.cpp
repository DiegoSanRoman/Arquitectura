//
// Created by diego-san-roman on 7/10/24.
//
// Created by diego-san-roman on 7/10/24.
#include <iostream>
#include <common/progargs.hpp>
#include <imgaos/imageaos.hpp>  // Include específico para AOS

int main(int argc, char* argv[]) {
  try {
    // Parsear argumentos del programa
    auto args = common::ProgramArguments::parse(argc, argv);

    // Verificar la operación seleccionada
    if (args.get_operation() == common::ProgramArguments::Operation::MaxLevel) {
      // Crear y procesar imagen usando la implementación AOS
      imgaos::ImageAOS image;
      image.load(args.get_input_file());
      image.scale_intensity(args.get_maxlevel());
      image.save(args.get_output_file());
    } else {
      throw common::ArgumentError("Operation not implemented");
    }

    return 0; // Retorno exitoso
  } catch (const common::ArgumentError& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return -1; // Error en argumentos
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return -1; // Otro tipo de error
  }
}

