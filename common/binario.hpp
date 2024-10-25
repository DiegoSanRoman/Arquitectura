#ifndef BINARIO_HPP
#define BINARIO_HPP

#include <string>
#include <vector>

struct PPMImage {
  int width;
  int height;
  int maxValue;
  std::vector<unsigned char> pixelData;
};

bool leerImagenPPM(const std::string& filePath, PPMImage& image);
bool escribirImagenPPM(const std::string& filePath, const PPMImage& image);

#endif // BINARIO_HPP
