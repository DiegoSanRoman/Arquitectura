#ifndef BINARIO_HPP
#define BINARIO_HPP

#include <string>
#include <vector>

// Estructura para almacenar los datos del archivo PPM
struct PPMImage {
  int width;
  int height;
  int maxColorValue;  // Debe ser > 0 y <= 65535
  std::vector<unsigned char> pixelData;  // 3 bytes/pixel si maxColorValue <= 255
  // 6 bytes/pixel si 256 <= maxColorValue <= 65535
};

// Funciones para leer y escribir archivos PPM en formato binario (P6)
// Soporta tanto imágenes de 8 bits por canal (maxColorValue <= 255)
// como imágenes de 16 bits por canal (256 <= maxColorValue <= 65535)
bool leerImagenPPM(const std::string& filename, PPMImage& image);
bool escribirImagenPPM(const std::string& filename, const PPMImage& image);

#endif // BINARIO_HPP