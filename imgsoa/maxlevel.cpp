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
    params.totalComponents = static_cast<std::size_t>(inputImage.width) * static_cast<std::size_t>(inputImage.height) * 3;
    params.newMaxValue = newMaxValue;
    return params;
  }

  void initializeOutputImage(PPMImageSoA& outputImage, const PPMImageSoA& inputImage, const PixelProcessingParams& params) {
    outputImage.width = inputImage.width;
    outputImage.height = inputImage.height;
    outputImage.maxValue = params.newMaxValue;
    outputImage.redChannel.resize(params.totalComponents);
    outputImage.greenChannel.resize(params.totalComponents);
    outputImage.blueChannel.resize(params.totalComponents);
  }

  unsigned int scaleAndClampValue(const ScaleParams& params) {
    return std::min(static_cast<unsigned int>(std::round(params.value * params.scaleFactor)), params.maxValue);
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

  void processPixelComponent(std::vector<uint8_t>& outputChannel, const std::vector<uint8_t>& inputChannel, const ScaleParams& params, std::size_t index) {
    const unsigned int value = params.maxValue > MAX_COLOR_8BIT ? read16BitValue(inputChannel, index) : read8BitValue(inputChannel, index);
    const unsigned int scaledValue = scaleAndClampValue({.value=value, .scaleFactor=params.scaleFactor, .maxValue=params.maxValue});
    if (params.maxValue > MAX_COLOR_8BIT) {
      write16BitValue(scaledValue, outputChannel, index);
    } else {
      write8BitValue(outputChannel, index, scaledValue);
    }
  }

  void processPixelData(const PPMImageSoA& inputImage, PPMImageSoA& outputImage, const PixelProcessingParams& params) {
    for (std::size_t i = 0; i < params.totalComponents; i += params.inputIs16Bit ? 2 : 1) {
      processPixelComponent(outputImage.redChannel, inputImage.redChannel, {.value=inputImage.redChannel[i], .scaleFactor=params.scaleFactor, .maxValue=static_cast<unsigned int>(params.newMaxValue)}, i);
      processPixelComponent(outputImage.greenChannel, inputImage.greenChannel, {.value=inputImage.greenChannel[i], .scaleFactor=params.scaleFactor, .maxValue=static_cast<unsigned int>(params.newMaxValue)}, i);
      processPixelComponent(outputImage.blueChannel, inputImage.blueChannel, {.value=inputImage.blueChannel[i], .scaleFactor=params.scaleFactor, .maxValue=static_cast<unsigned int>(params.newMaxValue)}, i);
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