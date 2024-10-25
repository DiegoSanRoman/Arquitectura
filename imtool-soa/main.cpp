//
// Created by diego-san-roman on 7/10/24.
//
#include <iostream>
#include <common/progargs.hpp>
#include <imgsoa/imagesoa.hpp>  // Include específico para SOA

int main(int argc, char* argv[]) {
  try {
    auto args = common::ProgramArguments::parse(argc, argv);

    switch (args.get_operation()) {
      case common::ProgramArguments::Operation::MaxLevel: {
        // Crear y procesar imagen usando la implementación SOA
        imgsoa::ImageSOA image;
        image.load(args.get_input_file());
        image.scale_intensity(args.get_maxlevel());
        image.save(args.get_output_file());
        break;
      }
      // Otros casos...
      default:
        throw common::ArgumentError("Operation not implemented");
    }

    return 0;
  }
  catch (const common::ArgumentError& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return -1;
  }
  catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return -1;
  }
}
