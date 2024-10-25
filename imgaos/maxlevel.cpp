#include "maxlevel.hpp"
#include "../common/binario.hpp"
#include <iostream>
#include <string>

void performMaxLevelOperation(const std::string& inputFile, const std::string& outputFile, int newMaxValue) {
  std::cout << "Realizando la operación de maxlevel en imgsoa con el nuevo valor máximo: " << newMaxValue << "\n";
  std::cout << "Archivo de entrada: " << inputFile << "\n";
  std::cout << "Archivo de salida: " << outputFile << "\n";
  try {
    // Leer el archivo PPM de entrada
    PPMImage image = readPPMFile(inputFile);

    // Mostrar el valor máximo de color original
    std::cout << "La intensidad máxima del archivo de entrada es: " << image.maxColorValue << "\n";

    // De momento, no hacemos nada con el archivo de salida ni el nuevo valor máximo
  } catch (const std::exception& e) {
    std::cerr << "Error al procesar la imagen: " << e.what() << "\n";
  }
}
