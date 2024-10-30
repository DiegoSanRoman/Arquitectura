#include "resize.hpp"
#include "../common/binario.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <stdexcept>
#include <array>

namespace {
  #define MAX_VALUE 255;

  // Implementación de clamp
  template <typename T>
  T clamp(T valor, T minimo, T maximo) {
    return std::max(minimo, std::min(maximo, valor));
  }

  void validateValue(int newValue) {
    if (newValue <= 0) {
      throw std::invalid_argument("Nuevo tamaño fuera de rango. ");
    }
  }

  // Estructura Color para almacenar componentes interpolados RGB
  struct Color {
    unsigned char red = 0;
    unsigned char green = 0;
    unsigned char blue = 0;
  };
  // Estructura para los parámetros de interpolación
  struct InterpolacionParams {
    int xLow, xHigh;
    int yLow, yHigh;
    float xRatio, yRatio;
  };
  // Estructura para almacenar los índices de interpolación
  struct IndicesInterpolacion {
    int index00;
    int index01;
    int index10;
    int index11;
  };
  struct DimensionesImagen {
    int width;
    int height;
  };
  struct CoordenadasEscaladas {
    int x_nueva;
    int y_nueva;
  };



  // Estructura para almacenar la imagen en formato SOA (Structure of Arrays)
  struct ImageSOA {
    std::vector<unsigned char> redChannel;
    std::vector<unsigned char> greenChannel;
    std::vector<unsigned char> blueChannel;
    int width = 0;
    int height = 0;
  };

  ImageSOA leerImagenSOA(const PPMImage& inputImage) {
    ImageSOA imageSOA;
    imageSOA.width = inputImage.width;
    imageSOA.height = inputImage.height;
    const size_t totalPixels = static_cast<size_t>(inputImage.width) * static_cast<size_t>(inputImage.height);

    // Reservar espacio para cada canal en la estructura SOA
    imageSOA.redChannel.resize(totalPixels);
    imageSOA.greenChannel.resize(totalPixels);
    imageSOA.blueChannel.resize(totalPixels);

    for (size_t i = 0; i < totalPixels; ++i) {
      imageSOA.redChannel[i] = inputImage.pixelData[i * 3];
      imageSOA.greenChannel[i] = inputImage.pixelData[(i * 3) + 1];
      imageSOA.blueChannel[i] = inputImage.pixelData[(i * 3) + 2];
    }
    return imageSOA;
  }

  // Función para interpolar linealmente entre dos valores
int interpolar(int color1, int color2, double factor_itp) {
    return static_cast<int>(color1 + (factor_itp * (color2 - color1)));
}
  // Función para realizar la interpolación de un único componente
  unsigned char interpolarComponente(const std::vector<unsigned char>& channel, const IndicesInterpolacion& indices, float xRatio, float yRatio) {
    const auto top = static_cast<float>(
        interpolar(interpolar(channel[static_cast<size_t>(indices.index00)], channel[static_cast<size_t>(indices.index01)], xRatio),
                   interpolar(channel[static_cast<size_t>(indices.index10)], channel[static_cast<size_t>(indices.index11)], xRatio),
                   yRatio)
    );
    return static_cast<unsigned char>(top);
  }

  // Función de interpolación bilineal
  void interpolacionBilineal(const ImageSOA& img, const InterpolacionParams& params, Color& colorInterpolado) {
    // Calcular los índices de los 4 píxeles
    const int index00 = (params.yLow * img.width) + params.xLow;  // Píxel en la esquina superior izquierda
    const int index01 = (params.yLow * img.width) + params.xHigh; // Píxel en la esquina superior derecha
    const int index10 = (params.yHigh * img.width) + params.xLow; // Píxel en la esquina inferior izquierda
    const int index11 = (params.yHigh * img.width) + params.xHigh; // Píxel en la esquina inferior derecha

    // Hacer la interpolación bilineal para los tres componentes
    // Inicializar la estructura IndicesInterpolacion
    const IndicesInterpolacion indices = {.index00 = index00, .index01 = index01, .index10 = index10, .index11 = index11};
    colorInterpolado.red = interpolarComponente(img.redChannel, indices, params.xRatio, params.yRatio);
    colorInterpolado.green = interpolarComponente(img.greenChannel, indices, params.xRatio, params.yRatio);
    colorInterpolado.blue = interpolarComponente(img.blueChannel, indices, params.xRatio, params.yRatio);
  }

  // Función auxiliar para procesar un píxel escalado
  void procesarPixelEscalado(const ImageSOA& original, const CoordenadasEscaladas& coord, const ImageSOA& escalada, Color& colorInterpolado) {
    const float x_original = static_cast<float>(coord.x_nueva) * (static_cast<float>(original.width) / static_cast<float>(escalada.width));
    const float y_original = static_cast<float>(coord.y_nueva) * (static_cast<float>(original.height) / static_cast<float>(escalada.height));

    const int xLow = clamp(static_cast<int>(std::floor(x_original)), 0, original.width - 1);
    const int xHigh = clamp(static_cast<int>(std::ceil(x_original)), 0, original.width - 1);
    const int yLow = clamp(static_cast<int>(std::floor(y_original)), 0, original.height - 1);
    const int yHigh = clamp(static_cast<int>(std::ceil(y_original)), 0, original.height - 1);

    const float xRatio = x_original - static_cast<float>(xLow);
    const float yRatio = y_original - static_cast<float>(yLow);

    const InterpolacionParams params = {
      .xLow = xLow,
      .xHigh = xHigh,
      .yLow = yLow,
      .yHigh = yHigh,
      .xRatio = xRatio,
      .yRatio = yRatio
  };

    interpolacionBilineal(original, params, colorInterpolado);
  }


  // Función principal para escalar la imagen
  void escalarImagen(const ImageSOA& original, ImageSOA& escalada) {
    escalada.redChannel.resize(static_cast<size_t>(escalada.width) * static_cast<size_t>(escalada.height));
    escalada.greenChannel.resize(static_cast<size_t>(escalada.width) * static_cast<size_t>(escalada.height));
    escalada.blueChannel.resize(static_cast<size_t>(escalada.width) * static_cast<size_t>(escalada.height));


    for (int y_nueva = 0; y_nueva < escalada.height; ++y_nueva) {
      for (int x_nueva = 0; x_nueva < escalada.width; ++x_nueva) {
        Color colorInterpolado;
        const CoordenadasEscaladas coord = {.x_nueva = x_nueva, .y_nueva = y_nueva};
        procesarPixelEscalado(original, coord, escalada, colorInterpolado);


        const int idx = (y_nueva * escalada.width) + x_nueva;
        escalada.redChannel[static_cast<size_t>(idx)] = colorInterpolado.red;
        escalada.greenChannel[static_cast<size_t>(idx)] = colorInterpolado.green;
        escalada.blueChannel[static_cast<size_t>(idx)] = colorInterpolado.blue;
      }
    }
  }
  // Función auxiliar para convertir de ImageSOA a PPMImage
  PPMImage convertirSOAAImagePPM(const ImageSOA& imagenSOA) {
    PPMImage outputImage;
    outputImage.width = imagenSOA.width;
    outputImage.height = imagenSOA.height;
    outputImage.maxValue = MAX_VALUE;
    outputImage.pixelData.resize(static_cast<size_t>(imagenSOA.width) * static_cast<size_t>(imagenSOA.height) * 3);

    for (size_t i = 0; i < outputImage.pixelData.size() / 3; ++i) {
        outputImage.pixelData[i * 3] = imagenSOA.redChannel[i];
        outputImage.pixelData[(i * 3) + 1] = imagenSOA.greenChannel[i];
        outputImage.pixelData[(i * 3) + 2] = imagenSOA.blueChannel[i];
    }
    return outputImage;
}

  // Función auxiliar para inicializar la imagen escalada
  ImageSOA inicializarImagenEscalada(int newWidth, int newHeight) {
    ImageSOA imagenEscalada;
    imagenEscalada.width = newWidth;
    imagenEscalada.height = newHeight;
    imagenEscalada.redChannel.resize(static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight));
    imagenEscalada.greenChannel.resize(static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight));
    imagenEscalada.blueChannel.resize(static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight));
    return imagenEscalada;
  }
}

//performResizeOperation(args.getInputFile(), args.getOutputFile(), newWidth, newHeight);
// Función principal para realizar la operación de resize
void performResizeOperation(const std::string& inputFile, const std::string& outputFile, int newWidth, int newHeight) {
  std::cout << "Realizando la operación de resize en imgsoa con el nuevo tamaño: " << newWidth << " " << newHeight << "\n";
  std::cout << "Archivo de entrada: " << inputFile << "\n";
  std::cout << "Archivo de salida: " << outputFile << "\n";

  try {
    // Validar los valores de las dimensiones
    validateValue(newWidth);
    validateValue(newHeight);

    // Leer la imagen de entrada en formato PPM
    PPMImage inputImage{};
    if (!leerImagenPPM(inputFile, inputImage)) {
      throw std::runtime_error("Error al leer el archivo de entrada");
    }

    // Convertir la imagen PPM a formato SOA
    const ImageSOA imageSOA = leerImagenSOA(inputImage);
    //const DimensionesImagen dimensionesOriginal = {.width = imageSOA.width, .height = imageSOA.height};

    // Inicializar la imagen escalada
    ImageSOA imagenEscaladaSOA = inicializarImagenEscalada(newWidth, newHeight);

    // Escalar la imagen
    escalarImagen(imageSOA, imagenEscaladaSOA);

    // Convertir la imagen escalada a formato PPM para guardarla
    const PPMImage outputImage = convertirSOAAImagePPM(imagenEscaladaSOA);

    // Guardar la imagen en el archivo de salida
    if (!escribirImagenPPM(outputFile, outputImage)) {
      throw std::runtime_error("Error al guardar el archivo de salida");
    }

  } catch (const std::exception& e) {
    std::cerr << "Error al procesar la imagen: " << e.what() << "\n";
    throw;
  }
}