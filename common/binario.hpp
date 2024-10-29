// File: common/binario.hpp
#ifndef BINARIO_HPP
#define BINARIO_HPP

#include <string>
#include <vector>
#include <cstdint>

// Structures for AOS (Array of Structures) representation
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

// Structures for SOA (Structure of Arrays) representation
struct PPMImageSoA {
  int width = 0;
  int height = 0;
  int maxValue = 0;
  std::vector<uint8_t> redChannel;
  std::vector<uint8_t> greenChannel;
  std::vector<uint8_t> blueChannel;

  PPMImageSoA() = default;

  PPMImageSoA(const PPMAttributes& attrs)
      : width(attrs.width), height(attrs.height), maxValue(attrs.maxValue) {}
};

// Function declarations
bool leerImagenPPM(const std::string& filePath, PPMImage& image);
bool escribirImagenPPM(const std::string& filePath, const PPMImage& image);

// New function declarations for SoA
bool leerImagenPPMSoA(const std::string& filePath, PPMImageSoA& image);
bool escribirImagenPPMSoA(const std::string& filePath, const PPMImageSoA& image);

#endif // BINARIO_HPP