// File: common/info.cpp
#include <iostream>
#include <fstream>
#include <string>
#include "info.hpp"
namespace {
  constexpr int MAX_COLOR = 255;
}
// Función que muestra los metadatos de una imagen en formato PPM
int info(const std::string& filePath) {
  std::ifstream file(filePath, std::ios::binary);
  if (!file) {
    std::cerr << "Error: No se pudo abrir el archivo " << filePath << "\n";
    return -1;
  }

  std::string magicNumber;
  int width = 0;
  int height = 0;
  int maxColorValue = 0;

  // Leer el número mágico
  file >> magicNumber;
  if (magicNumber != "P6") {
    std::cerr << "Error: Formato incorrecto. Se esperaba 'P6'.\n";
    return -1;
  }

  // Leer el ancho, la altura y el valor máximo de color
  file >> width >> height >> maxColorValue;
  file.ignore(1); // Ignorar el carácter de nueva línea después del encabezado

  // Validar los valores de ancho, altura y maxColorValue
  if (width <= 0 || height <= 0 || maxColorValue <= 0 || maxColorValue > MAX_COLOR) {
    std::cerr << "Error: Encabezado incorrecto o valores fuera de rango.\n";
    return -1;
  }

  // Mostrar los metadatos
  std::cout << "Formato: PPM (P6)\n";
  std::cout << "Tamaño: " << width << "x" << height << "\n";
  std::cout << "Valor máximo de color: " << maxColorValue << "\n";

  return 0;
}
