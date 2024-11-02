// File: imgaos/compress.cpp

#include "compress.hpp"
#include "../common/binario.hpp"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <array>
#include <cstring>  // Incluir <cstring> para std::memcpy

namespace common {
namespace {  // Namespace anónimo para funciones internas

constexpr int MAX_BYTE_VALUE = 256;
constexpr int MAX_SHORT_VALUE = 65536;
constexpr int BYTE_MASK = 0xFF;
constexpr int SHORT_MASK = 0xFFFF;
constexpr int BIT_SHIFT_8 = 8;
constexpr int BIT_SHIFT_16 = 16;

// Función para escribir un valor binario
template <typename T>
void write_binary(std::ostream& output, const T& value) {
    std::array<char, sizeof(value)> buffer{};
    std::memcpy(buffer.data(), &value, sizeof(value));
    output.write(buffer.data(), sizeof(value));
}

// Genera la tabla de colores únicos e índices de píxeles
void generarTablaColores(const std::vector<uint8_t>& pixelData,
                         std::vector<uint32_t>& colorIndices,
                         std::unordered_map<uint32_t, uint32_t>& colorTable,
                         std::vector<uint32_t>& uniqueColors) {
    for (size_t i = 0; i < pixelData.size(); i += 3) {
        uint32_t const color = (static_cast<uint32_t>(pixelData[i]) << BIT_SHIFT_16) |
                         (static_cast<uint32_t>(pixelData[i + 1]) << BIT_SHIFT_8) |
                         static_cast<uint32_t>(pixelData[i + 2]);

        if (colorTable.find(color) == colorTable.end()) {
            auto index = static_cast<uint32_t>(uniqueColors.size());
            colorTable[color] = index;
            uniqueColors.push_back(color);
        }
        colorIndices[i / 3] = colorTable[color];
    }
}

// Escribe el encabezado en el archivo comprimido
void escribirEncabezado(std::ofstream& outputFile, const PPMImage& image, size_t uniqueColorCount) {
    outputFile << "C6 " << image.width << " " << image.height << " " << image.maxValue << " "
               << uniqueColorCount << "\n";
}

// Escribe la tabla de colores en el archivo comprimido
void escribirTablaColores(std::ofstream& outputFile, const std::vector<uint32_t>& uniqueColors, int colorSize) {
    for (const auto& color : uniqueColors) {
        if (colorSize == 3) {  // 1 byte por componente
            auto red = static_cast<uint8_t>((color >> BIT_SHIFT_16) & BYTE_MASK);
            auto green = static_cast<uint8_t>((color >> BIT_SHIFT_8) & BYTE_MASK);
            auto blue = static_cast<uint8_t>(color & BYTE_MASK);
            write_binary(outputFile, red);
            write_binary(outputFile, green);
            write_binary(outputFile, blue);
        } else {  // 2 bytes por componente
            auto red = static_cast<uint16_t>((color >> BIT_SHIFT_16) & SHORT_MASK);
            auto green = static_cast<uint16_t>((color >> BIT_SHIFT_8) & SHORT_MASK);
            auto blue = static_cast<uint16_t>(color & SHORT_MASK);
            write_binary(outputFile, red);
            write_binary(outputFile, green);
            write_binary(outputFile, blue);
        }
    }
}

// Escribe los índices de los píxeles en el archivo comprimido
void escribirIndicesPixeles(std::ofstream& outputFile, const std::vector<uint32_t>& pixelIndices, int bytesPerPixel) {
    for (const auto& index : pixelIndices) {
        if (bytesPerPixel == 1) {
            auto idx = static_cast<uint8_t>(index);
            write_binary(outputFile, idx);
        } else if (bytesPerPixel == 2) {
            auto idx = static_cast<uint16_t>(index);
            write_binary(outputFile, idx);
        } else {
            uint32_t const idx = index;
            write_binary(outputFile, idx);
        }
    }
}

} // namespace anónimo

// Función principal de compresión
int compress(const CompressionPaths& paths) {
    PPMImage image;
    if (!leerImagenPPM(paths.inputImagePath, image)) {
        std::cerr << "Error al leer la imagen en formato AOS.\n";
        return -1;
    }

    std::unordered_map<uint32_t, uint32_t> colorTable;
    std::vector<uint32_t> uniqueColors;
    std::vector<uint32_t> colorIndices(static_cast<size_t>(image.width) * static_cast<size_t>(image.height));

    generarTablaColores(image.pixelData, colorIndices, colorTable, uniqueColors);

    std::ofstream output(paths.outputImagePath, std::ios::binary);
    if (!output) {
        std::cerr << "Error al abrir el archivo de salida.\n";
        return -1;
    }

    // Determinar el tamaño de cada índice de píxel en bytes
    int bytesPerPixel = 0;
    if (uniqueColors.size() <= MAX_BYTE_VALUE) {
        bytesPerPixel = 1;
    } else if (uniqueColors.size() <= MAX_SHORT_VALUE) {
        bytesPerPixel = 2;
    } else {
        bytesPerPixel = 4;
    }

    // Determinar el tamaño de cada color en la tabla de colores
    const int colorSize = (image.maxValue <= BYTE_MASK) ? 3 : 6;

    // Escritura de la salida comprimida
    escribirEncabezado(output, image, uniqueColors.size());
    escribirTablaColores(output, uniqueColors, colorSize);
    escribirIndicesPixeles(output, colorIndices, bytesPerPixel);

    return 0;
}

} // namespace common
