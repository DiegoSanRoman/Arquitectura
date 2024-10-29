#include "binario.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <span>

namespace {
    constexpr std::streamsize MAX_HEADER_SIZE = 256;
    constexpr int MAX_8BIT_VALUE = 255;
    constexpr std::size_t COMPONENTS_PER_PIXEL = 3U;

    bool leerEncabezadoPPM(std::ifstream& file, PPMImage& image) {
        std::string magicNumber;
        file >> magicNumber;
        if (magicNumber != "P6") {
            std::cerr << "Formato incorrecto: se esperaba 'P6'.\n";
            return false;
        }

        file >> image.width >> image.height >> image.maxValue;
        file.ignore(MAX_HEADER_SIZE, '\n');
        return true;
    }

    void swapBytes(std::vector<unsigned char>& data) {
        for (std::size_t i = 0; i < data.size(); i += 2) {
            std::swap(data[i], data[i + 1]);
        }
    }

    std::size_t calcularTotalBytes(int width, int height, int bytesPerComponent) {
        return static_cast<std::size_t>(width) *
               static_cast<std::size_t>(height) *
               COMPONENTS_PER_PIXEL *
               static_cast<std::size_t>(bytesPerComponent);
    }

    bool leerDatosPixeles(std::ifstream& file, PPMImage& image, int bytesPerComponent) {
        const std::size_t totalBytes = calcularTotalBytes(image.width, image.height, bytesPerComponent);
        image.pixelData.resize(totalBytes);

        if (!file.read(std::bit_cast<char*>(image.pixelData.data()),
                      static_cast<std::streamsize>(totalBytes))) {
            std::cerr << "Error al leer los datos de la imagen.\n";
            return false;
        }

        if (bytesPerComponent == 2) {
            swapBytes(image.pixelData);
        }
        return true;
    }

    bool escribirEncabezadoPPM(std::ofstream& file, const PPMImage& image) {
        file << "P6\n" << image.width << " " << image.height << "\n" << image.maxValue << "\n";
        return file.good();
    }

    bool escribirDatosPixeles(std::ofstream& file, const PPMImage& image, int bytesPerComponent) {
        const std::size_t totalBytes = calcularTotalBytes(image.width, image.height, bytesPerComponent);

        if (bytesPerComponent == 2) {
            std::vector<unsigned char> tempData = image.pixelData;
            swapBytes(tempData);
            return file.write(std::bit_cast<const char*>(tempData.data()),
                            static_cast<std::streamsize>(totalBytes)).good();
        }

        return file.write(std::bit_cast<const char*>(image.pixelData.data()),
                         static_cast<std::streamsize>(totalBytes)).good();
    }
}

bool leerImagenPPM(const std::string& filePath, PPMImage& image) {
    try {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            std::cerr << "Error al abrir el archivo para lectura: " << filePath << '\n';
            return false;
        }

        if (!leerEncabezadoPPM(file, image)) {
            return false;
        }

        const int bytesPerComponent = (image.maxValue <= MAX_8BIT_VALUE) ? 1 : 2;
        return leerDatosPixeles(file, image, bytesPerComponent);

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

        if (!escribirEncabezadoPPM(file, image)) {
            std::cerr << "Error al escribir el encabezado de la imagen.\n";
            return false;
        }

        const int bytesPerComponent = (image.maxValue <= MAX_8BIT_VALUE) ? 1 : 2;
        if (!escribirDatosPixeles(file, image, bytesPerComponent)) {
            std::cerr << "Error al escribir los datos de la imagen.\n";
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error al escribir imagen PPM: " << e.what() << '\n';
        return false;
    }}