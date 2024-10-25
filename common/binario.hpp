#ifndef BINARIO_HPP
#define BINARIO_HPP

#include <string>
#include <vector>
#include <cstdint>

struct PPMAttributes {
  int width;
  int height;
  int maxValue;
};

struct PPMImage {
  int width = 0;
  int height = 0;
  int maxValue = 0;
  std::vector<uint8_t> pixelData;

  PPMImage() = default;

  PPMImage(const PPMAttributes& attrs)
      : width(attrs.width), height(attrs.height), maxValue(attrs.maxValue) {}
};


bool leerImagenPPM(const std::string& filePath, PPMImage& image);
bool escribirImagenPPM(const std::string& filePath, const PPMImage& image);

#endif // BINARIO_HPP