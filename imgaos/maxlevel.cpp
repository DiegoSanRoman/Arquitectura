#include "maxlevel.hpp"
#include "../common/binario.hpp"
#include <iostream>
#include <string>
#include <stdexcept>
#include <cmath>
#include <exception>
#include <cstddef>
#include <algorithm>
#include <vector>

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

    unsigned int readColorComponent(const std::vector<unsigned char>& pixelData, std::size_t index, bool is16Bit) {
        if (is16Bit) {
            const std::size_t idx = index * 2;
            return (static_cast<unsigned int>(pixelData[idx + 1]) << BITS_PER_BYTE) |
                   static_cast<unsigned int>(pixelData[idx]);
        }
        return static_cast<unsigned int>(pixelData[index]);
    }

    void writeColorComponent(std::vector<unsigned char>& pixelData, std::size_t index, unsigned int value, bool is16Bit) {
        if (is16Bit) {
            const std::size_t idx = index * 2;
            pixelData[idx] = static_cast<unsigned char>(value >> BITS_PER_BYTE);
            pixelData[idx + 1] = static_cast<unsigned char>(value & BYTE_MASK);
        } else {
            pixelData[index] = static_cast<unsigned char>(value);
        }
    }

    PixelProcessingParams calculateProcessingParams(const PPMImage& inputImage, int newMaxValue) {
        return {
            .scaleFactor = static_cast<double>(newMaxValue) / inputImage.maxValue,
            .inputIs16Bit = inputImage.maxValue > MAX_COLOR_8BIT,
            .outputIs16Bit = newMaxValue > MAX_COLOR_8BIT,
            .totalComponents = static_cast<std::size_t>(inputImage.width) *
                               static_cast<std::size_t>(inputImage.height) * 3
        };
    }

    void processPixelData(const PPMImage& inputImage, PPMImage& outputImage, const PixelProcessingParams& params) {
        const std::size_t outputBytesPerComponent = params.outputIs16Bit ? 2 : 1;
        const std::size_t outputTotalBytes = params.totalComponents * outputBytesPerComponent;

        outputImage.pixelData.resize(outputTotalBytes);

        for (std::size_t i = 0; i < params.totalComponents; ++i) {
            const unsigned int inputValue = readColorComponent(inputImage.pixelData, i, params.inputIs16Bit);
            auto outputValue = static_cast<unsigned int>(std::lround(inputValue * params.scaleFactor));
            if (params.outputIs16Bit) {
                outputValue = std::min(outputValue, static_cast<unsigned int>(MAX_COLOR_16BIT));
            } else {
                outputValue = std::min(outputValue, static_cast<unsigned int>(MAX_COLOR_8BIT));
            }
            writeColorComponent(outputImage.pixelData, i, outputValue, params.outputIs16Bit);
        }
    }
}

void performMaxLevelOperation(const std::string& inputFile, const std::string& outputFile, int newMaxValue) {
    std::cout << "Maxlevel-aos con nuevo valor máximo: " << newMaxValue << "\n"
              << "Archivo de entrada: " << inputFile << "\n"
              << "Archivo de salida: " << outputFile << "\n";

    try {
        validateMaxValue(newMaxValue);

        PPMImage inputImage{};
        if (!leerImagenPPM(inputFile, inputImage)) {
            throw std::runtime_error("Error al leer el archivo de entrada");
        }

        std::cout << "La intensidad máxima del archivo de entrada es: " << inputImage.maxValue << "\n";

        PPMImage outputImage{};
        outputImage.width = inputImage.width;
        outputImage.height = inputImage.height;
        outputImage.maxValue = newMaxValue;

        const auto params = calculateProcessingParams(inputImage, newMaxValue);

        processPixelData(inputImage, outputImage, params);

        if (!escribirImagenPPM(outputFile, outputImage)) {
            throw std::runtime_error("Error al escribir el archivo de salida");
        }

    } catch (const std::exception& e) {
        std::cerr << "Error al procesar la imagen: " << e.what() << "\n";
        throw;
    }
}