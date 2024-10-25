#include "binario.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <span>

constexpr std::streamsize MAX_HEADER_SIZE = 256;

bool leerImagenPPM(const std::string& filePath, PPMImage& image) {
    try {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            std::cerr << "Error al abrir el archivo para lectura: " << filePath << '\n';
            return false;
        }

        // Leer el encabezado
        std::string magicNumber;
        file >> magicNumber;
        if (magicNumber != "P6") {
            std::cerr << "Formato incorrecto: se esperaba 'P6'.\n";
            return false;
        }

        file >> image.width >> image.height >> image.maxValue;
        file.ignore(MAX_HEADER_SIZE, '\n');

        // Calcular el tamaño total de bytes
        const std::size_t totalBytes = static_cast<std::size_t>(image.width) *
                                     static_cast<std::size_t>(image.height) * 3;
        image.pixelData.resize(totalBytes);

        // Leer los datos de píxeles directamente en el vector
        if (!file.read(std::bit_cast<char*>(image.pixelData.data()),
                      static_cast<std::streamsize>(totalBytes))) {
            std::cerr << "Error al leer los datos de la imagen.\n";
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error al leer imagen PPM: " << e.what() << '\n';
        return false;
    }
}

bool escribirImagenPPM(const std::string& filePath, const PPMImage& image) {
    try {
        std::ofstream file(filePath, std::ios::binary);
        if (!file) {
            std::cerr << "Error al abrir el archivo para escritura: " << filePath << '\n';
            return false;
        }

        // Escribir el encabezado
        file << "P6\n" << image.width << " " << image.height << "\n" << image.maxValue << "\n";

        // Calcular el tamaño total de bytes
        const std::size_t totalBytes = static_cast<std::size_t>(image.width) *
                                     static_cast<std::size_t>(image.height) * 3;

        // Escribir los datos de píxeles
        if (!file.write(std::bit_cast<const char*>(image.pixelData.data()),
                       static_cast<std::streamsize>(totalBytes))) {
            std::cerr << "Error al escribir los datos de la imagen.\n";
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error al escribir imagen PPM: " << e.what() << '\n';
        return false;
    }
}