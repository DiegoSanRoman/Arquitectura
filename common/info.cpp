// File: common/info.cpp

#include <iostream>
#include <fstream>
#include <string>
#include "info.hpp"

// Función que muestra los metadatos de una imagen en formato PPM
int info(const std::string& filePath) {
  /*if (argc > 4) {  // Verificar si hay más de tres argumentos
    std::cerr << "Error: Invalid extra arguments for info.\n";
    return -1;
  }*/

  std::ifstream file(filePath, std::ios::binary);
  if (!file) {
    std::cerr << "Error: No se pudo abrir el archivo " << filePath << "\n";
    return -1;
  }

  std::string magicNumber;
  int width = 0;
  int height= 0;
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

  // Mostrar los metadatos
  std::cout << "Formato: PPM (P6)\n";
  std::cout << "Tamaño: " << width << "x"<< height <<"\n";
  std::cout << "Valor máximo de color: " << maxColorValue << "\n";

  return 0;
}
