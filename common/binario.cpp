#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstddef>
#include <algorithm>

constexpr int HEADER_IGNORE_LENGTH = 256;
constexpr int MAX_COLOR_8BIT = 255;
constexpr int MAX_COLOR_16BIT = 65535;

struct PPMImage {
    int width;
    int height;
    int maxColorValue;
    std::vector<unsigned char> pixelData;
};

bool leerImagenPPM(const std::string& filename, PPMImage& image) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo " << filename << '\n';
        return false;
    }

    std::string header;
    file >> header;
    if (header != "P6") {
        std::cerr << "Formato de archivo PPM no soportado: " << header << '\n';
        return false;
    }

    file >> image.width >> image.height >> image.maxColorValue;

    if (image.maxColorValue <= 0 || image.maxColorValue > MAX_COLOR_16BIT) {
        std::cerr << "Valor máximo de color no válido: " << image.maxColorValue << '\n';
        return false;
    }

    // Ignorar el salto de línea después de los valores de encabezado
    file.ignore(HEADER_IGNORE_LENGTH, '\n');

    // Calcular el tamaño del buffer según el valor máximo de color
    const std::size_t bytesPerPixel = (image.maxColorValue <= MAX_COLOR_8BIT) ? 3 : 6;
    const std::size_t totalPixels = static_cast<std::size_t>(image.width) *
                                   static_cast<std::size_t>(image.height);
    const std::size_t totalBytes = totalPixels * bytesPerPixel;

    // Reservar espacio en pixelData
    image.pixelData.resize(totalBytes);

    // Leer los datos usando un vector temporal de char
    std::vector<char> buffer(totalBytes);
    file.read(buffer.data(), static_cast<std::streamsize>(totalBytes));

    if (!file) {
        std::cerr << "Error al leer los datos de la imagen " << filename << '\n';
        return false;
    }

    if (image.maxColorValue <= MAX_COLOR_8BIT) {
        // Para imágenes de 8 bits por canal, copiar directamente
        std::copy(buffer.begin(), buffer.end(), image.pixelData.begin());
    } else {
        // Para imágenes de 16 bits por canal, convertir de little-endian si es necesario
        for (std::size_t i = 0; i < totalPixels * 3; ++i) {
            const std::size_t srcIdx = i * 2;
            const auto low = static_cast<unsigned char>(buffer[srcIdx]);
            const auto high = static_cast<unsigned char>(buffer[srcIdx + 1]);
            image.pixelData[srcIdx] = high;
            image.pixelData[srcIdx + 1] = low;
        }
    }

    return true;
}

bool escribirImagenPPM(const std::string& filename, const PPMImage& image) {
    if (image.maxColorValue <= 0 || image.maxColorValue > MAX_COLOR_16BIT) {
        std::cerr << "Valor máximo de color no válido: " << image.maxColorValue << '\n';
        return false;
    }

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo " << filename << '\n';
        return false;
    }

    // Escribir encabezado
    file << "P6\n" << image.width << ' ' << image.height << '\n' << image.maxColorValue << '\n';

    const std::size_t bytesPerPixel = (image.maxColorValue <= MAX_COLOR_8BIT) ? 3 : 6;
    const std::size_t totalPixels = static_cast<std::size_t>(image.width) *
                                   static_cast<std::size_t>(image.height);
    const std::size_t totalBytes = totalPixels * bytesPerPixel;

    if (image.maxColorValue <= MAX_COLOR_8BIT) {
        // Para imágenes de 8 bits por canal, escribir a través de un buffer temporal
        std::vector<char> buffer(totalBytes);
        std::copy(image.pixelData.begin(), image.pixelData.end(), buffer.begin());
        file.write(buffer.data(), static_cast<std::streamsize>(totalBytes));
    } else {
        // Para imágenes de 16 bits por canal, convertir a little-endian
        std::vector<char> buffer(totalBytes);
        for (std::size_t i = 0; i < totalPixels * 3; ++i) {
            const std::size_t destIdx = i * 2;
            const auto high = image.pixelData[destIdx];
            const auto low = image.pixelData[destIdx + 1];
            buffer[destIdx] = static_cast<char>(low);
            buffer[destIdx + 1] = static_cast<char>(high);
        }
        file.write(buffer.data(), static_cast<std::streamsize>(totalBytes));
    }

    if (!file) {
        std::cerr << "Error al escribir los datos de la imagen " << filename << '\n';
        return false;
    }

    return true;
}