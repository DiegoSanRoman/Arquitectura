/*
#include "maxlevel.hpp"             // Para performMaxLevelOperation
#include "../common/binario.hpp"    // Para leerImagenPPM, escribirImagenPPM
#include <iostream>                 // Para std::cout, std::cerr
#include <string>                   // Para std::string
#include <stdexcept>                // Para std::invalid_argument
#include <cmath>                    // Para std::lround
#include <exception>                // Para std::exception
#include <cstddef>                  // Para std::size_t
#include <algorithm>                // Para std::min

namespace {
    constexpr int BITS_PER_BYTE = 8;
    constexpr unsigned int BYTE_MASK = 0xFF;
    constexpr int MAX_COLOR_8BIT = 255;
    constexpr int MAX_COLOR_16BIT = 65535;

    struct PixelProcessingParams {
        double scaleFactor;
        bool inputIs16Bit;
        bool outputIs16Bit;
        std::size_t totalComponents;
    };

    void validateMaxValue(int newMaxValue) {
        if (newMaxValue <= 0 || newMaxValue > MAX_COLOR_16BIT) {
            throw std::invalid_argument("Nuevo valor máximo fuera de rango válido (1-65535)");
        }
    }

    unsigned int readColorComponent(const std::vector<unsigned char>& pixelData,
                                     std::size_t index, bool is16Bit) {
        if (is16Bit) {
            const std::size_t idx = index * 2;
            // Leer en little-endian (el byte menos significativo primero)
            return (static_cast<unsigned int>(pixelData[idx + 1]) << BITS_PER_BYTE) |
                   static_cast<unsigned int>(pixelData[idx]);
        }
        return static_cast<unsigned int>(pixelData[index]);
    }

    void writeColorComponent(std::vector<unsigned char>& pixelData,
                             std::size_t index, unsigned int value, bool is16Bit) {
        if (is16Bit) {
            const std::size_t idx = index * 2;
            // Escribir en little-endian (el byte menos significativo primero)
            pixelData[idx] = static_cast<unsigned char>(value & BYTE_MASK);
            pixelData[idx + 1] = static_cast<unsigned char>(value >> BITS_PER_BYTE);
        } else {
            pixelData[index] = static_cast<unsigned char>(value);
        }
    }

    PixelProcessingParams calculateProcessingParams(const PPMImage& inputImage, int newMaxValue) {
        return {
            static_cast<double>(newMaxValue) / inputImage.maxValue,
            inputImage.maxValue > MAX_COLOR_8BIT,
            newMaxValue > MAX_COLOR_8BIT,
            static_cast<std::size_t>(inputImage.width) *
            static_cast<std::size_t>(inputImage.height) * 3 // Total de componentes para 3 canales RGB
        };
    }

    void processPixelData(const PPMImage& inputImage, PPMImage& outputImage,
                          const PixelProcessingParams& params) {
        const std::size_t outputBytesPerComponent = params.outputIs16Bit ? 2 : 1;
        const std::size_t outputTotalBytes = params.totalComponents * outputBytesPerComponent;

        outputImage.pixelData.resize(outputTotalBytes);

        for (std::size_t i = 0; i < params.totalComponents; ++i) {
            const unsigned int inputValue = readColorComponent(inputImage.pixelData, i, params.inputIs16Bit);
            // Escalar el valor del píxel
            auto outputValue = static_cast<unsigned int>(
                std::lround(inputValue * params.scaleFactor));

            // Asegúrate de que el valor no supere el nuevo máximo
            if (params.outputIs16Bit) {
                outputValue = std::min(outputValue, static_cast<unsigned int>(MAX_COLOR_16BIT));
            } else {
                outputValue = std::min(outputValue, static_cast<unsigned int>(MAX_COLOR_8BIT));
            }

            writeColorComponent(outputImage.pixelData, i, outputValue, params.outputIs16Bit);
        }
    }
}

void performMaxLevelOperation(const std::string& inputFile,
                              const std::string& outputFile, int newMaxValue) {
    std::cout << "Realizando la operación de maxlevel en imgsoa con el nuevo valor máximo: "
              << newMaxValue << "\n";
    std::cout << "Archivo de entrada: " << inputFile << "\n";
    std::cout << "Archivo de salida: " << outputFile << "\n";

    try {
        validateMaxValue(newMaxValue);

        PPMImage inputImage{};
        if (!leerImagenPPM(inputFile, inputImage)) {
            throw std::runtime_error("Error al leer el archivo de entrada");
        }

        std::cout << "La intensidad máxima del archivo de entrada es: " << inputImage.maxValue << "\n";

        PPMImage outputImage{};
        outputImage.width = inputImage.width;  // Mantiene el mismo ancho
        outputImage.height = inputImage.height; // Mantiene la misma altura
        outputImage.maxValue = newMaxValue; // Cambia solo el valor máximo

        const auto params = calculateProcessingParams(inputImage, newMaxValue);
        processPixelData(inputImage, outputImage, params);

        if (!escribirImagenPPM(outputFile, outputImage)) {
            throw std::runtime_error("Error al escribir el archivo de salida");
        }

        std::cout << "Operación completada exitosamente.\n";

    } catch (const std::exception& e) {
        std::cerr << "Error al procesar la imagen: " << e.what() << "\n";
        throw;
    }
}
*/