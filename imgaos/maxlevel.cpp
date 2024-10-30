// File: imgaos/maxlevel.cpp
#include "maxlevel.hpp"                             // Para performMaxLevelOperation
#include "../common/binario.hpp"                    // Para leerImagenPPM, escribirImagenPPM
#include <iostream>                                 // Para std::cout, std::cerr
#include <string>                                   // Para std::string
#include <stdexcept>                                // Para std::invalid_argument
#include <cmath>                                    // Para std::lround
#include <exception>                                // Para std::exception
#include <cstddef>                                  // Para std::size_t
#include <algorithm>                                // Para std::min

// Definición de la función performMaxLevelOperationº
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

  // Función para leer un componente de color
  unsigned int readColorComponent(const std::vector<unsigned char>& pixelData,
                                   std::size_t index, bool is16Bit) {
      // Si es de 16 bits, se almacena en 2 bytes (16 bits)
      if (is16Bit) {
        // Calcular el índice del byte
        const std::size_t idx = index * 2;
        // Leer en little-endian (el byte menos significativo primero)
        return (static_cast<unsigned int>(pixelData[idx + 1]) << BITS_PER_BYTE) |
               static_cast<unsigned int>(pixelData[idx]);
      }
      // Si es de 8 bits, se almacena en 1 byte (8 bits)
      return static_cast<unsigned int>(pixelData[index]);
    }

  // Función para escribir un componente de color
  void writeColorComponent(std::vector<unsigned char>& pixelData,
                           std::size_t index, unsigned int value, bool is16Bit) {
      // Si es de 16 bits, se almacena en 2 bytes (16 bits)
      if (is16Bit) {
        // Calcular el índice del byte
        const std::size_t idx = index * 2;
        // Escribir en little-endian (el byte menos significativo primero)
        pixelData[idx] = static_cast<unsigned char>(value & BYTE_MASK);
        pixelData[idx + 1] = static_cast<unsigned char>(value >> BITS_PER_BYTE);
        return;
      }
      // Si es de 8 bits, se almacena en 1 byte (8 bits)
      pixelData[index] = static_cast<unsigned char>(value);
    }

  // Función para calcular los parámetros de procesamiento de píxeles
  PixelProcessingParams calculateProcessingParams(const PPMImage& inputImage, int newMaxValue) {
      return {
        .scaleFactor = static_cast<double>(newMaxValue) / inputImage.maxValue,
        .inputIs16Bit = inputImage.maxValue > MAX_COLOR_8BIT,
        .outputIs16Bit = newMaxValue > MAX_COLOR_8BIT,
        .totalComponents = static_cast<std::size_t>(inputImage.width) *
                           static_cast<std::size_t>(inputImage.height) * 3
    };
    }

    // Función para procesar los datos de píxeles
    void processPixelData(const PPMImage& inputImage, PPMImage& outputImage,
                          const PixelProcessingParams& params) {
        // Calcular el número de bytes por componente de salida
        const std::size_t outputBytesPerComponent = params.outputIs16Bit ? 2 : 1;
        const std::size_t outputTotalBytes = params.totalComponents * outputBytesPerComponent;

        // Redimensionar el vector de píxeles de salida
        outputImage.pixelData.resize(outputTotalBytes);

        // Procesar cada componente de color
        for (std::size_t i = 0; i < params.totalComponents; ++i) {
            // Leer el valor del píxel de entrada
            const unsigned int inputValue = readColorComponent(inputImage.pixelData, i, params.inputIs16Bit);
            // Escalar el valor del píxel
            auto outputValue = static_cast<unsigned int>(
                std::lround(inputValue * params.scaleFactor));
            // Asegúrate de que el valor no supere el nuevo máximo
            if (params.outputIs16Bit) {
                // Para 16 bits
                outputValue = std::min(outputValue, static_cast<unsigned int>(MAX_COLOR_16BIT));
            } else {
                // Para 8 bits
                outputValue = std::min(outputValue, static_cast<unsigned int>(MAX_COLOR_8BIT));
            }
            // Escribir el valor del píxel de salida
            writeColorComponent(outputImage.pixelData, i, outputValue, params.outputIs16Bit);
        }
    }
}

void performMaxLevelOperation(const std::string& inputFile,
                              const std::string& outputFile, int newMaxValue) {
    // Mostrar información sobre la operación (Esto después lo borraré, pero de momento lo dejo para que se vea)
    std::cout << "Maxlevel-soa con nuevo valor máximo: " << newMaxValue << "\n" << "Archivo de entrada: " << inputFile << "\n" << "Archivo de salida: " << outputFile << "\n";

    try {
        // Validar el nuevo valor máximo
        validateMaxValue(newMaxValue);

        // Leer la imagen de entrada
        PPMImage inputImage{};
        if (!leerImagenPPM(inputFile, inputImage)) {
            throw std::runtime_error("Error al leer el archivo de entrada");
        }

        // Esto también se borrará
        std::cout << "La intensidad máxima del archivo de entrada es: " << inputImage.maxValue << "\n";

        // Crear la imagen de salida con el mismo tamaño y el nuevo valor máximo
        PPMImage outputImage{};
        outputImage.width = inputImage.width;           // Mantiene el mismo ancho
        outputImage.height = inputImage.height;         // Mantiene la misma altura
        outputImage.maxValue = newMaxValue;             // Cambia solo el valor máximo

        // Calcular los parámetros de procesamiento
        const auto params = calculateProcessingParams(inputImage, newMaxValue);

        // Procesar los datos de píxeles
        processPixelData(inputImage, outputImage, params);

        // Escribir la imagen de salida
        if (!escribirImagenPPM(outputFile, outputImage)) {
            throw std::runtime_error("Error al escribir el archivo de salida");
        }

    } // Capturar excepciones
      catch (const std::exception& e) {
        std::cerr << "Error al procesar la imagen: " << e.what() << "\n";
        throw;
    }
}
