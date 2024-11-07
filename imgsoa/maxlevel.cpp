// File: imgsoa/maxlevel.cpp
#include <iostream>
#include <string>
#include <cmath>
#include "maxlevel.hpp"
#include "../common/binario.hpp"

namespace {
  // Constantes para el procesamiento de píxeles
  constexpr int BITS_PER_BYTE = 8;                // Bits por byte
  constexpr unsigned int BYTE_MASK = 0xFF;        // Máscara para un byte
  constexpr int MAX_COLOR_8BIT = 255;             // Valor máximo para un color de 8 bits
  constexpr int MAX_COLOR_16BIT = 65535;          // Valor máximo para un color de 16 bits

  // Parámetros para el procesamiento de píxeles
  struct PixelProcessingParams {
    double scaleFactor;                         // Factor de escala
    bool inputIs16Bit;                          // Si los valores de entrada son de 16 bits
    bool outputIs16Bit;                         // Si los valores de salida son de 16 bits
    std::size_t totalComponents;                // Total de componentes de píxeles
  };

  // Función para validar el nuevo valor máximo
  void validateMaxValue(int newMaxValue) {
    if (newMaxValue <= 0 || newMaxValue > MAX_COLOR_16BIT) {
      throw std::invalid_argument("Nuevo valor máximo fuera de rango válido (1-65535)");
    }
  }

  // Función para calcular los parámetros de procesamiento de píxeles
  PixelProcessingParams calculateProcessingParams(const PPMImageSoA& inputImage, int newMaxValue) {
    return {
        .scaleFactor = static_cast<double>(newMaxValue) / inputImage.maxValue,
        .inputIs16Bit = inputImage.maxValue > MAX_COLOR_8BIT,
        .outputIs16Bit = newMaxValue > MAX_COLOR_8BIT,
        .totalComponents = static_cast<std::size_t>(inputImage.width) *
                           static_cast<std::size_t>(inputImage.height)
    };
  }

  // Función para leer un componente de color
  unsigned int readColorComponent(const std::vector<uint8_t>& channel, std::size_t index, bool is16Bit) {
    if (is16Bit) {
      const std::size_t idx = index * 2;
      return (static_cast<unsigned int>(channel[idx + 1]) << BITS_PER_BYTE) |
             static_cast<unsigned int>(channel[idx]);
    }
    return static_cast<unsigned int>(channel[index]);
  }

  // Función para escribir un componente de color
  void writeColorComponent(std::vector<uint8_t>& channel, std::size_t index, unsigned int value, bool is16Bit) {
    if (is16Bit) {
      const std::size_t idx = index * 2;
      channel[idx] = static_cast<uint8_t>(value >> BITS_PER_BYTE);
      channel[idx + 1] = static_cast<uint8_t>(value & BYTE_MASK);
    } else {
      channel[index] = static_cast<uint8_t>(value);
    }
  }

  // Función para procesar los datos de píxeles
  void processPixelData(const PPMImageSoA& inputImage, PPMImageSoA& outputImage, const PixelProcessingParams& params) {
    const std::size_t outputBytesPerComponent = params.outputIs16Bit ? 2 : 1;
    const std::size_t totalPixels = params.totalComponents;

    outputImage.width = inputImage.width;
    outputImage.height = inputImage.height;
    outputImage.maxValue = static_cast<int>(params.scaleFactor * inputImage.maxValue);

    outputImage.redChannel.resize(totalPixels * outputBytesPerComponent);
    outputImage.greenChannel.resize(totalPixels * outputBytesPerComponent);
    outputImage.blueChannel.resize(totalPixels * outputBytesPerComponent);

    for (std::size_t i = 0; i < totalPixels; ++i) {
      unsigned int redValue = readColorComponent(inputImage.redChannel, i, params.inputIs16Bit);
      unsigned int greenValue = readColorComponent(inputImage.greenChannel, i, params.inputIs16Bit);
      unsigned int blueValue = readColorComponent(inputImage.blueChannel, i, params.inputIs16Bit);

      redValue = static_cast<unsigned int>(std::lround(redValue * params.scaleFactor));
      greenValue = static_cast<unsigned int>(std::lround(greenValue * params.scaleFactor));
      blueValue = static_cast<unsigned int>(std::lround(blueValue * params.scaleFactor));

      if (params.outputIs16Bit) {
        redValue = std::min(redValue, static_cast<unsigned int>(MAX_COLOR_16BIT));
        greenValue = std::min(greenValue, static_cast<unsigned int>(MAX_COLOR_16BIT));
        blueValue = std::min(blueValue, static_cast<unsigned int>(MAX_COLOR_16BIT));
      } else {
        redValue = std::min(redValue, static_cast<unsigned int>(MAX_COLOR_8BIT));
        greenValue = std::min(greenValue, static_cast<unsigned int>(MAX_COLOR_8BIT));
        blueValue = std::min(blueValue, static_cast<unsigned int>(MAX_COLOR_8BIT));
      }

      writeColorComponent(outputImage.redChannel, i, redValue, params.outputIs16Bit);
      writeColorComponent(outputImage.greenChannel, i, greenValue, params.outputIs16Bit);
      writeColorComponent(outputImage.blueChannel, i, blueValue, params.outputIs16Bit);
    }
  }
}

void performMaxLevelOperation(const std::string& inputFile, const std::string& outputFile, int newMaxValue) {
  std::cout << "Maxlevel-soa con nuevo valor máximo: " << newMaxValue << "\n"
            << "Archivo de entrada: " << inputFile << "\n"
            << "Archivo de salida: " << outputFile << "\n";

  try {
    // Validar el nuevo valor máximo
    validateMaxValue(newMaxValue);

    // Leer la imagen de entrada
    PPMImageSoA inputImage{};
    if (!leerImagenPPMSoA(inputFile, inputImage)) {
      return;
    }

    // Calcular los parámetros de procesamiento
    const PixelProcessingParams params = calculateProcessingParams(inputImage, newMaxValue);

    // Procesar los datos de píxeles
    PPMImageSoA outputImage{};
    processPixelData(inputImage, outputImage, params);

    // Escribir la imagen de salida
    if (!escribirImagenPPMSoA(outputFile, outputImage)) {
      std::cerr << "Error al escribir la imagen de salida.\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}