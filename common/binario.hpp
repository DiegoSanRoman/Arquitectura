#ifndef BINARIO_HPP
#define BINARIO_HPP

#include <string>
#include <vector>

// Estructura para almacenar los datos del archivo PPM
struct PPMImage {
  int width;
  int height;
  int maxColorValue;
  std::vector<unsigned char> pixelData;  // Cada pixel es 3 bytes (R,G,B)
};

// Funciones para leer y escribir archivos PPM en formato binario (P6)
bool leerImagenPPM(const std::string& filename, PPMImage& image);
bool escribirImagenPPM(const std::string& filename, const PPMImage& image);

#endif // BINARIO_HPP