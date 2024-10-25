#include "maxlevel.hpp"
#include "../common/binario.hpp"
#include <iostream>
#include <string>
#include <stdexcept>
#include <cmath>
#include <exception>
#include <cstddef>  // Para std::size_t

namespace {
    // Constantes para evitar números mágicos
    constexpr int BITS_PER_BYTE = 8;
    constexpr unsigned int BYTE_MASK = 0xFF;
    constexpr int MAX_COLOR_8BIT = 255;
    constexpr int MAX_COLOR_16BIT = 65535;
}

void performMaxLevelOperation(const std::string& inputFile, const std::string& outputFile, int newMaxValue) {
    std::cout << "Realizando la operación de maxlevel en imgsoa con el nuevo valor máximo: " << newMaxValue << "\n";
    std::cout << "Archivo de entrada: " << inputFile << "\n";
    std::cout << "Archivo de salida: " << outputFile << "\n";

    try {
        // Validar el nuevo valor máximo
        if (newMaxValue <= 0 || newMaxValue > MAX_COLOR_16BIT) {
            throw std::invalid_argument("Nuevo valor máximo fuera de rango válido (1-65535)");
        }

        // Leer la imagen de entrada
        PPMImage inputImage{};  // Initialize all members
        if (!leerImagenPPM(inputFile, inputImage)) {
            throw std::runtime_error("Error al leer el archivo de entrada");
        }

        // Mostrar el valor máximo de color original
        std::cout << "La intensidad máxima del archivo de entrada es: " << inputImage.maxColorValue << "\n";

        // Crear una nueva imagen para el resultado
        PPMImage outputImage{};  // Initialize all members
        outputImage.width = inputImage.width;
        outputImage.height = inputImage.height;
        outputImage.maxColorValue = newMaxValue;

        // Calcular el factor de escala para ajustar los valores
        const double scaleFactor = static_cast<double>(newMaxValue) / inputImage.maxColorValue;

        // Determinar el número de bytes por componente de color para entrada y salida
        const bool inputIs16Bit = inputImage.maxColorValue > MAX_COLOR_8BIT;
        const bool outputIs16Bit = newMaxValue > MAX_COLOR_8BIT;
        const std::size_t outputBytesPerComponent = outputIs16Bit ? 2 : 1;

        // Calcular el tamaño total de los datos de píxeles para la imagen de salida
        const std::size_t totalPixels = static_cast<std::size_t>(outputImage.width) *
                                      static_cast<std::size_t>(outputImage.height);
        const std::size_t totalComponents = totalPixels * 3; // RGB
        const std::size_t outputTotalBytes = totalComponents * outputBytesPerComponent;
        outputImage.pixelData.resize(outputTotalBytes);

        // Procesar cada componente de color
        for (std::size_t i = 0; i < totalComponents; ++i) {
            // Leer el valor del color de entrada
            unsigned int inputValue = 0;  // Initialize to 0
            if (inputIs16Bit) {
                const std::size_t inputIdx = i * 2;
                inputValue = (static_cast<unsigned int>(inputImage.pixelData[inputIdx]) << BITS_PER_BYTE) |
                            static_cast<unsigned int>(inputImage.pixelData[inputIdx + 1]);
            } else {
                inputValue = static_cast<unsigned int>(inputImage.pixelData[i]);
            }

            // Escalar el valor al nuevo rango usando lround para un redondeo correcto
            const auto outputValue = static_cast<unsigned int>(
                std::lround(inputValue * scaleFactor));

            // Escribir el valor escalado
            if (outputIs16Bit) {
                const std::size_t outputIdx = i * 2;
                outputImage.pixelData[outputIdx] = static_cast<unsigned char>(outputValue >> BITS_PER_BYTE);
                outputImage.pixelData[outputIdx + 1] = static_cast<unsigned char>(outputValue & BYTE_MASK);
            } else {
                outputImage.pixelData[i] = static_cast<unsigned char>(outputValue);
            }
        }

        // Guardar la imagen resultante
        if (!escribirImagenPPM(outputFile, outputImage)) {
            throw std::runtime_error("Error al escribir el archivo de salida");
        }

        std::cout << "Operación completada exitosamente.\n";

    } catch (const std::exception& e) {
        std::cerr << "Error al procesar la imagen: " << e.what() << "\n";
        throw;
    }
}