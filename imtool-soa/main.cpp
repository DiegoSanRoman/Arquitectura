//
// Created by diego-san-roman on 7/10/24.
//

#include <iostream>
#include <vector>
#include "../imgsoa/cutfreq.hpp" // Asegúrate de tener las declaraciones correctas en este archivo

int main() {
  // Crear una imagen de prueba en formato SOA
  ImageSOA testImage;
  testImage.red_channel = {255, 255, 0, 255, 0, 0, 255, 255, 255};
  testImage.green_channel = {0, 0, 255, 255, 0, 0, 255, 255, 0};
  testImage.blue_channel = {255, 0, 0, 0, 255, 255, 0, 0, 255};

  // Imprimir la imagen original
  std::cout << "Imagen original:" << std::endl;
  for (size_t i = 0; i < testImage.red_channel.size(); ++i) {
    std::cout << "R: " << testImage.red_channel[i] << ", G: " << testImage.green_channel[i] << ", B: " << testImage.blue_channel[i] << std::endl;
  }

  // Aplicar cutfreq
  int n = 1; // Queremos eliminar 1 color
  cutfreq(testImage, n);

  // Imprimir la imagen después de cutfreq
  std::cout << "\nImagen después de cutfreq:" << std::endl;
  for (size_t i = 0; i < testImage.red_channel.size(); ++i) {
    std::cout << "R: " << testImage.red_channel[i] << ", G: " << testImage.green_channel[i] << ", B: " << testImage.blue_channel[i] << std::endl;
  }

  return 0;
}
