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
      throw std::invalid_argument("Nuevo valor máximo fuera de rango válido (1-65535)");
    }
  }

  PixelProcessingParams calculateProcessingParams(const PPMImageSoA& inputImage, int newMaxValue) {
    return {
        .scaleFactor = static_cast<double>(newMaxValue) / inputImage.maxValue,
        .inputIs16Bit = inputImage.maxValue > static_cast<int>(MAX_COLOR_8BIT),
        .outputIs16Bit = newMaxValue > static_cast<int>(MAX_COLOR_8BIT),
        .totalComponents = static_cast<std::size_t>(inputImage.width) *
                           static_cast<std::size_t>(inputImage.height),
        .newMaxValue = newMaxValue
    };
  }

  void initializeOutputImage(PPMImageSoA& outputImage, const PPMImageSoA& inputImage, const PixelProcessingParams& params) {
    outputImage.width = inputImage.width;
    outputImage.height = inputImage.height;
    outputImage.maxValue = params.newMaxValue;

    const std::size_t outputBytesPerComponent = params.outputIs16Bit ? 2U : 1U;
    const std::size_t outputSize = params.totalComponents * outputBytesPerComponent;
    outputImage.redChannel.resize(outputSize);
    outputImage.greenChannel.resize(outputSize);
    outputImage.blueChannel.resize(outputSize);
  }

  unsigned int scaleAndClampValue(const ScaleParams& params) {
    const double scaled = params.value * params.scaleFactor;
    const auto rounded = static_cast<unsigned int>(std::lround(scaled));
    return std::min(rounded, params.maxValue);
  }

  void write8BitValue(std::vector<uint8_t>& channel, std::size_t index, unsigned int value) {
    channel[index] = static_cast<uint8_t>(value);
  }

  void write16BitValue(unsigned int value, std::vector<uint8_t>& channel, std::size_t index) {
    const std::size_t byteIndex = index * 2U;
    channel[byteIndex] = static_cast<uint8_t>(value & BYTE_MASK); // Least significant byte
    channel[byteIndex + 1] = static_cast<uint8_t>((value >> BITS_PER_BYTE) & BYTE_MASK); // Most significant byte
  }

  unsigned int read8BitValue(const std::vector<uint8_t>& channel, std::size_t index) {
    return static_cast<unsigned int>(channel[index]);
  }

  unsigned int read16BitValue(const std::vector<uint8_t>& channel, std::size_t index) {
    const std::size_t byteIndex = index * 2U;
    return (static_cast<unsigned int>(channel[byteIndex + 1]) << BITS_PER_BYTE) |
           static_cast<unsigned int>(channel[byteIndex]);
  }

  void processPixelComponent(std::vector<uint8_t>& outputChannel,
                             const std::vector<uint8_t>& inputChannel,
                             std::size_t pixelIndex,
                             const PixelProcessingParams& params) {
    // Leer el valor de entrada
    const auto inputValue = params.inputIs16Bit ?
        read16BitValue(inputChannel, pixelIndex) :
        read8BitValue(inputChannel, pixelIndex);

    const auto maxValue = static_cast<unsigned int>(params.newMaxValue);

    const ScaleParams scaleParams{
        .value = inputValue,
        .scaleFactor = params.scaleFactor,
        .maxValue = maxValue
    };

    const auto scaledValue = scaleAndClampValue(scaleParams);

    // Escribir el valor de salida
    if (params.outputIs16Bit) {
        write16BitValue(scaledValue, outputChannel, pixelIndex);
    } else {
        write8BitValue(outputChannel, pixelIndex, scaledValue);
    }
  }

  void processPixelData(const PPMImageSoA& inputImage, PPMImageSoA& outputImage,
                      const PixelProcessingParams& params) {
    initializeOutputImage(outputImage, inputImage, params);

    for (std::size_t i = 0; i < params.totalComponents; ++i) {
      processPixelComponent(outputImage.redChannel, inputImage.redChannel, i, params);
      processPixelComponent(outputImage.greenChannel, inputImage.greenChannel, i, params);
      processPixelComponent(outputImage.blueChannel, inputImage.blueChannel, i, params);
    }
  }
}

void performMaxLevelOperation(const std::string& inputFile, const std::string& outputFile, int newMaxValue) {
  std::cout << "Maxlevel-soa con nuevo valor máximo: " << newMaxValue << "\n"
            << "Archivo de entrada: " << inputFile << "\n"
            << "Archivo de salida: " << outputFile << "\n";

  validateMaxValue(newMaxValue);

  PPMImageSoA inputImage{};
  if (!leerImagenPPMSoA(inputFile, inputImage)) {
    throw std::runtime_error("Error al leer la imagen de entrada");
  }

  const PixelProcessingParams params = calculateProcessingParams(inputImage, newMaxValue);

  PPMImageSoA outputImage{};
  processPixelData(inputImage, outputImage, params);

  if (!escribirImagenPPMSoA(outputFile, outputImage)) {
    throw std::runtime_error("Error al escribir la imagen de salida");
  }
}