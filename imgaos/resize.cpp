#include "resize.hpp"
#include "../common/binario.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <stdexcept>

namespace {
    // Implementación de clamp
    template <typename T>
    T clamp(T valor, T minimo, T maximo) {
        return std::max(minimo, std::min(maximo, valor));
    }

    // Estructura para almacenar un píxel (RGB)
    struct Pixel {
        int red, green, blue;
    };

    // Estructura para almacenar los píxeles y las razones de interpolación, con inicialización designada
    struct PixelCoords {
        Pixel p00{.red = 0, .green = 0, .blue = 0};
        Pixel p01{.red = 0, .green = 0, .blue = 0};
        Pixel p10{.red = 0, .green = 0, .blue = 0};
        Pixel p11{.red = 0, .green = 0, .blue = 0};
        double xRatio = 0.0;
        double yRatio = 0.0;
    };

    struct Dimensions {
        int width;
        int height;
    };

    struct Coords {
        double x;
        double y;
    };

    struct ImageDimensions {
        int width;
        int height;
        int widthNueva;
        int heightNueva;
    };

    void validateSize(int newSize) {
        if (newSize <= 0) {
            throw std::invalid_argument("Nuevo tamaño fuera de rango. ");
        }
    }

    // Función para interpolar linealmente entre dos colores
    int interpolar(int color1, int color2, double factor_itp) {
        const int interpolatedValue = static_cast<int>(std::round(color1 + (factor_itp * (color2 - color1))));
        return interpolatedValue;
    }

    // Función para interpolar bilinealmente los colores
    Pixel interpolacionBilineal(const PixelCoords& coords) {
        Pixel resultado{.red = 0, .green = 0, .blue = 0};

        resultado.red = interpolar(interpolar(coords.p00.red, coords.p01.red, coords.xRatio),
                                   interpolar(coords.p10.red, coords.p11.red, coords.xRatio), coords.yRatio);
        resultado.green = interpolar(interpolar(coords.p00.green, coords.p01.green, coords.xRatio),
                                     interpolar(coords.p10.green, coords.p11.green, coords.xRatio), coords.yRatio);
        resultado.blue = interpolar(interpolar(coords.p00.blue, coords.p01.blue, coords.xRatio),
                                    interpolar(coords.p10.blue, coords.p11.blue, coords.xRatio), coords.yRatio);

        return resultado;
    }

    // Nueva función para obtener las coordenadas correspondientes en la imagen original
    Coords obtenerCoordenadasOriginales(int xNueva, int yNueva, const ImageDimensions& dims) {
        return Coords{
            .x = static_cast<double>(xNueva) * dims.width / dims.widthNueva,
            .y = static_cast<double>(yNueva) * dims.height / dims.heightNueva
        };
    }

    // Nueva función para obtener los píxeles y ratios de interpolación
    PixelCoords obtenerPixelCoords(const std::vector<std::vector<Pixel>>& imagenOriginal, const Dimensions& dims, const Coords& originalCoords) {
        int xLow = static_cast<int>(std::floor(originalCoords.x));
        int xHigh = static_cast<int>(std::ceil(originalCoords.x));
        int yLow = static_cast<int>(std::floor(originalCoords.y));
        int yHigh = static_cast<int>(std::ceil(originalCoords.y));

        // Asegurarse de que los índices estén dentro del rango de la imagen
        xLow = clamp(xLow, 0, dims.width - 1);
        xHigh = clamp(xHigh, 0, dims.width - 1);
        yLow = clamp(yLow, 0, dims.height - 1);
        yHigh = clamp(yHigh, 0, dims.height - 1);

        PixelCoords coords;
        coords.p00 = imagenOriginal[static_cast<std::size_t>(yLow)][static_cast<std::size_t>(xLow)];
        coords.p01 = imagenOriginal[static_cast<std::size_t>(yLow)][static_cast<std::size_t>(xHigh)];
        coords.p10 = imagenOriginal[static_cast<std::size_t>(yHigh)][static_cast<std::size_t>(xLow)];
        coords.p11 = imagenOriginal[static_cast<std::size_t>(yHigh)][static_cast<std::size_t>(xHigh)];
        coords.xRatio = originalCoords.x - xLow;
        coords.yRatio = originalCoords.y - yLow;

        return coords;
    }

    // Nueva función para obtener el píxel escalado
    Pixel obtenerPixelEscalado(const std::vector<std::vector<Pixel>>& imagenOriginal, const Dimensions& dims, const Coords& originalCoords) {
        const PixelCoords coords = obtenerPixelCoords(imagenOriginal, dims, originalCoords);
        return interpolacionBilineal(coords);
    }

    // Función para escalar una imagen usando interpolación bilineal
    std::vector<std::vector<Pixel>> escalarImagen(const std::vector<std::vector<Pixel>>& imagenOriginal, const ImageDimensions& dims) {
        std::vector<std::vector<Pixel>> imagenEscalada(static_cast<size_t>(dims.heightNueva), std::vector<Pixel>(static_cast<size_t>(dims.widthNueva)));

        for (int yNueva = 0; yNueva < dims.heightNueva; ++yNueva) {
            for (int xNueva = 0; xNueva < dims.widthNueva; ++xNueva) {
                const Coords originalCoords = obtenerCoordenadasOriginales(xNueva, yNueva, dims);
                const Dimensions originalDims{.width = dims.width, .height = dims.height};

                imagenEscalada[static_cast<size_t>(yNueva)][static_cast<size_t>(xNueva)] = obtenerPixelEscalado(imagenOriginal, originalDims, originalCoords);
            }
        }
        return imagenEscalada;
    }

    // Nueva función para leer la imagen en formato PPM
    std::vector<std::vector<Pixel>> leerImagenOriginal(const PPMImage& inputImage) {
        std::vector<std::vector<Pixel>> originalData(static_cast<size_t>(inputImage.height), std::vector<Pixel>(static_cast<size_t>(inputImage.width)));
        for (size_t yCoord = 0; yCoord < static_cast<size_t>(inputImage.height); ++yCoord) {
            for (size_t xCoord = 0; xCoord < static_cast<size_t>(inputImage.width); ++xCoord) {
                const size_t pixelIndex = (yCoord * static_cast<size_t>(inputImage.width) + xCoord) * 3;
                originalData[yCoord][xCoord] = {
                    .red = inputImage.pixelData[pixelIndex],
                    .green = inputImage.pixelData[pixelIndex + 1],
                    .blue = inputImage.pixelData[pixelIndex + 2]
                };
            }
        }
        return originalData;
    }

    // Nueva función para crear la imagen escalada en formato PPMImage
    PPMImage crearImagenEscalada(const std::vector<std::vector<Pixel>>& scaledData, const PPMImage& inputImage, int newWidth, int newHeight) {
        PPMImage outputImage;
        outputImage.width = newWidth;
        outputImage.height = newHeight;
        outputImage.maxValue = inputImage.maxValue;
        outputImage.pixelData.resize(static_cast<std::size_t>(newWidth) * static_cast<std::size_t>(newHeight) * 3);
        for (size_t row = 0; row < static_cast<std::size_t>(newHeight); ++row) {
            for (size_t col = 0; col < static_cast<std::size_t>(newWidth); ++col) {
                const size_t pixelIndex = (row * static_cast<std::size_t>(newWidth) + col) * 3;
                outputImage.pixelData[pixelIndex] = static_cast<unsigned char>(scaledData[row][col].red);
                outputImage.pixelData[pixelIndex + 1] = static_cast<unsigned char>(scaledData[row][col].green);
                outputImage.pixelData[pixelIndex + 2] = static_cast<unsigned char>(scaledData[row][col].blue);
            }
        }
        return outputImage;
    }
}

//int main(int argc, char* argv[]) {
void performResizeOperation(const std::string& inputFile, const std::string& outputFile, int newWidth, int newHeight) {
    std::cout << "Realizando la operación de resize en imgsoa con el nuevo tamaño: " << newWidth << " " << newHeight << "\n";
    std::cout << "Archivo de entrada: " << inputFile << "\n";
    std::cout << "Archivo de salida: " << outputFile << "\n";

    try {
        validateSize(newWidth);
        validateSize(newHeight);

        PPMImage inputImage{};
        if (!leerImagenPPM(inputFile, inputImage)) {  // Leer la imagen original
            throw std::runtime_error("Error al leer el archivo de entrada");
        }

        // Leer los datos de la imagen original
        auto originalData = leerImagenOriginal(inputImage);

        const ImageDimensions dims = {.width = inputImage.width, .height = inputImage.height, .widthNueva = newWidth, .heightNueva = newHeight};
        auto scaledData = escalarImagen(originalData, dims); // Escalar la imagen

        // Crear la imagen escalada en formato PPMImage
        const PPMImage outputImage = crearImagenEscalada(scaledData, inputImage, newWidth, newHeight);

        if (!escribirImagenPPM(outputFile, outputImage)) {
            throw std::runtime_error("Error al escribir el archivo de salida");
        }
        std::cout << "Operación completada exitosamente.\n";
    } catch (const std::exception& e) {
        std::cerr << "Error al procesar la imagen: " << e.what() << "\n";
        throw;
    }
}
