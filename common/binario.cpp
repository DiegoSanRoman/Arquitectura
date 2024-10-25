#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstddef>
#include <algorithm>  // Para std::copy

constexpr int HEADER_IGNORE_LENGTH = 256;

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

    // Ignorar el salto de línea después de los valores de encabezado
    file.ignore(HEADER_IGNORE_LENGTH, '\n');

    // Reservar espacio en pixelData para todos los píxeles de la imagen
    image.pixelData.resize(static_cast<std::size_t>(image.width) *
                          static_cast<std::size_t>(image.height) * 3);

    // Leer los datos usando un vector temporal de char
    std::vector<char> buffer(image.pixelData.size());
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

    if (!file) {
        std::cerr << "Error al leer los datos de la imagen " << filename << '\n';
        return false;
    }

    // Copiar los datos al vector final
    std::copy(buffer.begin(), buffer.end(), image.pixelData.begin());

    return true;
}

bool escribirImagenPPM(const std::string& filename, const PPMImage& image) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo " << filename << '\n';
        return false;
    }

    // Escribir encabezado
    file << "P6\n" << image.width << ' ' << image.height << '\n' << image.maxColorValue << '\n';

    // Crear buffer temporal y copiar los datos
    std::vector<char> buffer(image.pixelData.size());
    std::copy(image.pixelData.begin(), image.pixelData.end(), buffer.begin());

    // Escribir los datos
    file.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));

    if (!file) {
        std::cerr << "Error al escribir los datos de la imagen " << filename << '\n';
        return false;
    }

    return true;
}