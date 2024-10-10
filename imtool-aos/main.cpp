//
// Created by diego-san-roman on 7/10/24.
//

#include <iostream>
#include <vector>
#include <tuple>
#include <cmath>
#include "../imgaos/cutfreq.hpp"  // Ruta relativa a cutfreq.hpp


// Función principal para probar cutfreq
int main() {
  // Crear una imagen de prueba
  std::vector<Pixel> testImage = {
    {255, 255, 255}, {255, 0, 0}, {0, 255, 0},
    {255, 255, 255}, {0, 0, 255}, {0, 0, 255},
    {255, 255, 0}, {255, 255, 0}, {255, 255, 255},
    {255, 255, 0}, {255, 0, 0}, {255, 255, 0},
  };

  // Imprimir la imagen original
  std::cout << "Imagen original:" << std::endl;
  for (const auto& pixel : testImage) {
    std::cout << "R: " << pixel.r << ", G: " << pixel.g << ", B: " << pixel.b << std::endl;
  }

  // Ejecutar cutfreq con n = 1
  int n = 1;
  cutfreq(testImage, n);

  // Imprimir la imagen después de aplicar cutfreq
  std::cout << "\nImagen después de cutfreq:" << std::endl;
  for (const auto& pixel : testImage) {
    std::cout << "R: " << pixel.r << ", G: " << pixel.g << ", B: " << pixel.b << std::endl;
  }

  return 0;
}
