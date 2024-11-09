#include "compress.hpp"
#include "../common/binario.hpp"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <array>
#include <cstring>
#include <algorithm>

namespace common {
namespace {

constexpr int MAX_BYTE_VALUE = 256;
constexpr int MAX_SHORT_VALUE = 65536;
constexpr int BYTE_MASK = 0xFF;
constexpr int RED_SHIFT = 16;
constexpr int GREEN_SHIFT = 8;

template <typename T>
void write_binary(std::ostream& output, const T& value) {
    std::array<char, sizeof(value)> buffer{};
    std::memcpy(buffer.data(), &value, sizeof(value));
    output.write(buffer.data(), sizeof(value));
}

// Estructura para representar un color RGB con hash personalizado
struct Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    bool operator==(const Color& other) const {
        return red == other.red && green == other.green && blue == other.blue;
    }
};

// Función hash para la estructura Color
struct ColorHash {
    std::size_t operator()(const Color& color) const noexcept {
        return (static_cast<std::size_t>(color.red) << RED_SHIFT) |
               (static_cast<std::size_t>(color.green) << GREEN_SHIFT) |
               static_cast<std::size_t>(color.blue);
    }
};

struct ColorChannels {
    std::vector<uint8_t> red;
    std::vector<uint8_t> green;
    std::vector<uint8_t> blue;
};

void llenarCanales(const std::vector<uint8_t>& pixelData, ColorChannels& channels) {
    const size_t numPixels = pixelData.size() / 3;
    channels.red.resize(numPixels);
    channels.green.resize(numPixels);
    channels.blue.resize(numPixels);

    for (size_t i = 0; i < numPixels; ++i) {
        channels.red[i] = pixelData[i * 3];
        channels.green[i] = pixelData[(i * 3) + 1];
        channels.blue[i] = pixelData[(i * 3) + 2];
    }
}

void generarTablaColores(const ColorChannels& channels, ColorChannels& uniqueColors, std::vector<uint32_t>& colorIndices) {
    const size_t numPixels = channels.red.size();
    std::unordered_map<Color, uint32_t, ColorHash> colorTable;

    for (size_t i = 0; i < numPixels; ++i) {
        const Color color = { .red = channels.red[i], .green = channels.green[i], .blue = channels.blue[i] };

        auto result = colorTable.find(color);
        if (result == colorTable.end()) {
            const auto index = static_cast<uint32_t>(uniqueColors.red.size());
            colorTable[color] = index;
            uniqueColors.red.push_back(channels.red[i]);
            uniqueColors.green.push_back(channels.green[i]);
            uniqueColors.blue.push_back(channels.blue[i]);
            colorIndices[i] = index;
        } else {
            colorIndices[i] = result->second;
        }
    }
}

void ordenarTablaColores(ColorChannels& uniqueColors) {
    std::vector<size_t> indices(uniqueColors.red.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        indices[i] = i;
    }

    std::ranges::sort(indices, [&](size_t indexA, size_t indexB) {
        return std::tie(uniqueColors.red[indexA], uniqueColors.green[indexA], uniqueColors.blue[indexA]) <
               std::tie(uniqueColors.red[indexB], uniqueColors.green[indexB], uniqueColors.blue[indexB]);
    });

    ColorChannels sortedColors;
    for (const size_t index : indices) {
        sortedColors.red.push_back(uniqueColors.red[index]);
        sortedColors.green.push_back(uniqueColors.green[index]);
        sortedColors.blue.push_back(uniqueColors.blue[index]);
    }
    uniqueColors = std::move(sortedColors);
}

void escribirEncabezado(std::ofstream& outputFile, const PPMImage& image, size_t uniqueColorCount) {
    outputFile << "C6 " << image.width << " " << image.height << " " << image.maxValue << " "
               << uniqueColorCount << "\n";
}

void escribirTablaColores(std::ofstream& outputFile, const ColorChannels& uniqueColors, int colorSize) {
    for (size_t i = 0; i < uniqueColors.red.size(); ++i) {
        if (colorSize == 3) {
            write_binary(outputFile, uniqueColors.red[i]);
            write_binary(outputFile, uniqueColors.green[i]);
            write_binary(outputFile, uniqueColors.blue[i]);
        } else {
            write_binary(outputFile, static_cast<uint16_t>(uniqueColors.red[i]));
            write_binary(outputFile, static_cast<uint16_t>(uniqueColors.green[i]));
            write_binary(outputFile, static_cast<uint16_t>(uniqueColors.blue[i]));
        }
    }
}

void escribirIndicesPixeles(std::ofstream& outputFile, const std::vector<uint32_t>& pixelIndices, int bytesPerPixel) {
    for (const auto& index : pixelIndices) {
        if (bytesPerPixel == 1) {
            write_binary(outputFile, static_cast<uint8_t>(index));
        } else if (bytesPerPixel == 2) {
            write_binary(outputFile, static_cast<uint16_t>(index));
        } else {
            write_binary(outputFile, index);
        }
    }
}

int determinarBytesPorPixel(size_t uniqueColorCount) {
    if (uniqueColorCount <= MAX_BYTE_VALUE) {
        return 1;
    }
    if (uniqueColorCount <= MAX_SHORT_VALUE) {
        return 2;
    }
    return 4;
}

} // namespace

int compress(const CompressionPaths& paths) {
    PPMImage image;
    if (!leerImagenPPM(paths.inputImagePath, image)) {
        std::cerr << "Error al leer la imagen en formato SOA.\n";
        return -1;
    }

    ColorChannels channels;
    llenarCanales(image.pixelData, channels);

    ColorChannels uniqueColors;
    std::vector<uint32_t> colorIndices(channels.red.size());

    generarTablaColores(channels, uniqueColors, colorIndices);
    ordenarTablaColores(uniqueColors);

    std::ofstream output(paths.outputImagePath, std::ios::binary);
    if (!output) {
        std::cerr << "Error al abrir el archivo de salida.\n";
        return -1;
    }

    const int bytesPerPixel = determinarBytesPorPixel(uniqueColors.red.size());
    const int colorSize = (image.maxValue <= BYTE_MASK) ? 3 : 6;

    escribirEncabezado(output, image, uniqueColors.red.size());
    escribirTablaColores(output, uniqueColors, colorSize);
    escribirIndicesPixeles(output, colorIndices, bytesPerPixel);

    return 0;
}

} // namespace common
