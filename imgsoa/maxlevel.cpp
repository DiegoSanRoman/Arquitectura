#include <iostream>
#include <string>
#include <cmath>
#include "maxlevel.hpp"
#include "../common/binario.hpp"

namespace {
  constexpr int BITS_PER_BYTE = 8;
  constexpr unsigned int BYTE_MASK = 0xFF;
  constexpr unsigned int MAX_COLOR_8BIT = 255U;
  constexpr unsigned int MAX_COLOR_16BIT = 65535U;

  struct PixelProcessingParams {
    double scaleFactor;
    bool inputIs16Bit;
    bool outputIs16Bit;
    std::size_t totalComponents;
    int newMaxValue;
  };

  struct ScaleParams {
    unsigned int value;
    double scaleFactor;
    unsigned int maxValue;
  };

  struct PixelValues {
    unsigned int red;
    unsigned int green;
    unsigned int blue;
  };

  void validateMaxValue(int newMaxValue) {
    if (newMaxValue <= 0 || newMaxValue > static_cast<int>(MAX_COLOR_16BIT)) {
      throw std::invalid_argument("Invalid max value");
    }
  }

  PixelProcessingParams calculateProcessingParams(const PPMImageSoA& inputImage, int newMaxValue) {
    PixelProcessingParams params{};
    params.scaleFactor = static_cast<double>(newMaxValue) / inputImage.maxValue;
    params.inputIs16Bit = static_cast<unsigned int>(inputImage.maxValue) > MAX_COLOR_8BIT;
    params.outputIs16Bit = newMaxValue > static_cast<int>(MAX_COLOR_8BIT);
    params.totalComponents = static_cast<std::size_t>(inputImage.width) *
                           static_cast<std::size_t>(inputImage.height) *
                           (params.outputIs16Bit ? 2 : 1);
    params.newMaxValue = newMaxValue;
    return params;
  }

  void initializeOutputImage(PPMImageSoA& outputImage, const PPMImageSoA& inputImage,
                           const PixelProcessingParams& params) {
    outputImage.width = inputImage.width;
    outputImage.height = inputImage.height;
    outputImage.maxValue = params.newMaxValue;
    outputImage.redChannel.resize(params.totalComponents);
    outputImage.greenChannel.resize(params.totalComponents);
    outputImage.blueChannel.resize(params.totalComponents);
  }

  unsigned int scaleAndClampValue(const ScaleParams& params) {
    return std::min(static_cast<unsigned int>(std::round(params.value * params.scaleFactor)),
                   params.maxValue);
  }

  void write8BitValue(std::vector<uint8_t>& channel, std::size_t index, unsigned int value) {
    channel[index] = static_cast<uint8_t>(value);
  }

  void write16BitValue(unsigned int value, std::vector<uint8_t>& channel, std::size_t index) {
    channel[index] = static_cast<uint8_t>(value & BYTE_MASK);
    channel[index + 1] = static_cast<uint8_t>((value >> BITS_PER_BYTE) & BYTE_MASK);
  }

  unsigned int read8BitValue(const std::vector<uint8_t>& channel, std::size_t index) {
    return channel[index];
  }

  unsigned int read16BitValue(const std::vector<uint8_t>& channel, std::size_t index) {
    return (static_cast<unsigned int>(channel[index + 1]) << BITS_PER_BYTE) | channel[index];
  }

  PixelValues readPixelValues(const PPMImageSoA& image, std::size_t index, bool is16Bit) {
    PixelValues values{};
    values.red = is16Bit ? read16BitValue(image.redChannel, index) :
                          read8BitValue(image.redChannel, index);
    values.green = is16Bit ? read16BitValue(image.greenChannel, index) :
                            read8BitValue(image.greenChannel, index);
    values.blue = is16Bit ? read16BitValue(image.blueChannel, index) :
                           read8BitValue(image.blueChannel, index);
    return values;
  }

  PixelValues scalePixelValues(const PixelValues& values, double scaleFactor,
                              unsigned int maxValue) {
    PixelValues scaled{};
    scaled.red = scaleAndClampValue({.value = values.red,
                                    .scaleFactor = scaleFactor,
                                    .maxValue = maxValue});
    scaled.green = scaleAndClampValue({.value = values.green,
                                      .scaleFactor = scaleFactor,
                                      .maxValue = maxValue});
    scaled.blue = scaleAndClampValue({.value = values.blue,
                                     .scaleFactor = scaleFactor,
                                     .maxValue = maxValue});
    return scaled;
  }

  void writePixelValues(PPMImageSoA& image, const PixelValues& values,
                       std::size_t index, bool is16Bit) {
    if (is16Bit) {
      write16BitValue(values.red, image.redChannel, index);
      write16BitValue(values.green, image.greenChannel, index);
      write16BitValue(values.blue, image.blueChannel, index);
    } else {
      write8BitValue(image.redChannel, index, values.red);
      write8BitValue(image.greenChannel, index, values.green);
      write8BitValue(image.blueChannel, index, values.blue);
    }
  }

  void processPixelData(const PPMImageSoA& inputImage, PPMImageSoA& outputImage,
                       const PixelProcessingParams& params) {
    const std::size_t pixelCount = static_cast<std::size_t>(inputImage.width) *
                                  static_cast<std::size_t>(inputImage.height);
    const std::size_t inputStride = params.inputIs16Bit ? 2 : 1;
    const std::size_t outputStride = params.outputIs16Bit ? 2 : 1;

    for (std::size_t i = 0; i < pixelCount; ++i) {
      const std::size_t inputIndex = i * inputStride;
      const std::size_t outputIndex = i * outputStride;

      const auto pixelValues = readPixelValues(inputImage, inputIndex, params.inputIs16Bit);
      const auto scaledValues = scalePixelValues(pixelValues, params.scaleFactor,
                                               static_cast<unsigned int>(params.newMaxValue));
      writePixelValues(outputImage, scaledValues, outputIndex, params.outputIs16Bit);
    }
  }
}

void performMaxLevelOperation(const FilePaths& paths, int newMaxValue) {
  validateMaxValue(newMaxValue);

  PPMImageSoA inputImage{};
  if (!leerImagenPPMSoA(paths.inputPath, inputImage)) {
    throw std::runtime_error("Error al leer la imagen de entrada");
  }

  const PixelProcessingParams params = calculateProcessingParams(inputImage, newMaxValue);

  PPMImageSoA outputImage{};
  initializeOutputImage(outputImage, inputImage, params);

  processPixelData(inputImage, outputImage, params);

  if (!escribirImagenPPMSoA(paths.outputPath, outputImage)) {
    throw std::runtime_error("Error al escribir la imagen de salida");
  }
}