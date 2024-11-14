#include "resize.hpp"
#include "../common/binario.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <stdexcept>
#include <array>
#include <algorithm> // Para std::clamp

namespace {
  constexpr double MAX_VALUE = 255.0;

  template <typename T>
  T clamp(T valor, T minimo, T maximo) {
    return std::max(minimo, std::min(maximo, valor));
  }

  void validateValue(int newValue) {
    if (newValue <= 0) {
      throw std::invalid_argument("Nuevo tamaño fuera de rango.");
    }
  }

  struct Color {
    unsigned char red = 0;
    unsigned char green = 0;
    unsigned char blue = 0;
  };

  /*Nuevo*/
  struct ImageDimensions {
    int width;
    int height;
  };

  struct InterpolacionParams {
    double xLow, xHigh;
    double yLow, yHigh;
    double xRatio, yRatio;
  };

  struct IndicesInterpolacion {
    size_t index00;
    size_t index01;
    size_t index10;
    size_t index11;
  };

  struct ImageSOA {
    std::vector<unsigned char> redChannel;
    std::vector<unsigned char> greenChannel;
    std::vector<unsigned char> blueChannel;
    int width = 0;
    int height = 0;
  };

  struct EscaladoParams {
    double xLow;
    double xHigh;
    double yLow;
    double yHigh;
    double xRatio;
    double yRatio;
  };


  ImageSOA leerImagenSOA(const PPMImage& inputImage) {
    ImageSOA imageSOA;
    imageSOA.width = inputImage.width;
    imageSOA.height = inputImage.height;
    const size_t totalPixels = static_cast<size_t>(inputImage.width) * static_cast<size_t>(inputImage.height);

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

  double interpolar(double color1, double color2, double factor_itp) {
    return color1 + (factor_itp * (color2 - color1));
  }

  /*Cambio de unsigned char a double*/
  unsigned char interpolarComponente(const std::vector<unsigned char>& channel, const IndicesInterpolacion& indices, double xRatio, double yRatio) {
    const double interpoladoX1 = interpolar(channel.at(indices.index00), channel.at(indices.index01), xRatio);
    const double interpoladoX2 = interpolar(channel.at(indices.index10), channel.at(indices.index11), xRatio);
    const double top = interpolar(interpoladoX1, interpoladoX2, yRatio);

    return static_cast<unsigned char>(clamp(top, 0.0, MAX_VALUE));
  }

  void interpolacionBilineal(const ImageSOA& img, const InterpolacionParams& params, Color& colorInterpolado) {
    const auto index00 = static_cast<size_t>((params.yLow * img.width) + params.xLow);
    const auto index01 = static_cast<size_t>((params.yLow * img.width) + params.xHigh);
    const auto index10 = static_cast<size_t>((params.yHigh * img.width) + params.xLow);
    const auto index11 = static_cast<size_t>((params.yHigh * img.width) + params.xHigh);

    const IndicesInterpolacion indices = {.index00 = index00, .index01 = index01, .index10 = index10, .index11 = index11};
    colorInterpolado.red = interpolarComponente(img.redChannel, indices, params.xRatio, params.yRatio);
    colorInterpolado.green = interpolarComponente(img.greenChannel, indices, params.xRatio, params.yRatio);
    colorInterpolado.blue = interpolarComponente(img.blueChannel, indices, params.xRatio, params.yRatio);
  }
  void procesarPixelEscalado(const ImageSOA& original, Color& colorInterpolado, const EscaladoParams& params) {
    const InterpolacionParams interpolParams = {
      .xLow = params.xLow,
      .xHigh = params.xHigh,
      .yLow = params.yLow,
      .yHigh = params.yHigh,
      .xRatio = params.xRatio,
      .yRatio = params.yRatio
  };
    interpolacionBilineal(original, interpolParams, colorInterpolado);
  }

  void escalarImagen(const ImageSOA& original, ImageSOA& escalada) {
    const size_t totalPixels = static_cast<size_t>(escalada.width) * static_cast<size_t>(escalada.height);
    escalada.redChannel.resize(totalPixels);
    escalada.greenChannel.resize(totalPixels);
    escalada.blueChannel.resize(totalPixels);

    // Evitar divisiones por cero para imágenes de 1x1
    const double xScale = (escalada.width > 1) ? (static_cast<double>(original.width - 1) / (escalada.width - 1)) : 0.0;
    const double yScale = (escalada.height > 1) ? (static_cast<double>(original.height - 1) / (escalada.height - 1)) : 0.0;

    for (int y_nueva = 0; y_nueva < escalada.height; ++y_nueva) {
      for (int x_nueva = 0; x_nueva < escalada.width; ++x_nueva) {
        // Evitar cálculos incorrectos cuando width o height son 1
        const double x_original = (escalada.width > 1) ? x_nueva * xScale : 0.0;
        const double y_original = (escalada.height > 1) ? y_nueva * yScale : 0.0;

        const double xLow = std::floor(x_original);
        const double xHigh = std::ceil(x_original);
        const double yLow = std::floor(y_original);
        const double yHigh = std::ceil(y_original);

        const double xRatio = x_original - xLow;
        const double yRatio = y_original - yLow;

        Color colorInterpolado;
        const EscaladoParams params = {.xLow = xLow, .xHigh = xHigh, .yLow = yLow, .yHigh = yHigh, .xRatio = xRatio, .yRatio = yRatio};
        procesarPixelEscalado(original, colorInterpolado, params);

        const auto idx = (static_cast<size_t>(y_nueva) * static_cast<size_t>(escalada.width)) + static_cast<size_t>(x_nueva);
        escalada.redChannel[idx] = colorInterpolado.red;
        escalada.greenChannel[idx] = colorInterpolado.green;
        escalada.blueChannel[idx] = colorInterpolado.blue;
      }
    }
  }



  PPMImage convertirSOAAImagePPM(const ImageSOA& imagenSOA) {
    PPMImage outputImage;
    outputImage.width = imagenSOA.width;
    outputImage.height = imagenSOA.height;
    outputImage.maxValue = static_cast<int>(MAX_VALUE);

    const size_t totalPixels = static_cast<size_t>(imagenSOA.width) * static_cast<size_t>(imagenSOA.height) * 3;
    outputImage.pixelData.resize(totalPixels);

    for (size_t i = 0; i < imagenSOA.redChannel.size(); ++i) {
      outputImage.pixelData[i * 3] = imagenSOA.redChannel[i];
      outputImage.pixelData[(i * 3) + 1] = imagenSOA.greenChannel[i];
      outputImage.pixelData[(i * 3) + 2] = imagenSOA.blueChannel[i];
    }
    return outputImage;
  }

  ImageSOA inicializarImagenEscalada(ImageDimensions dims) {
    ImageSOA imagenEscalada;
    imagenEscalada.width = dims.width;
    imagenEscalada.height = dims.height;
    return imagenEscalada;
  }
}

void performResizeOperation(const std::string& inputFile, const std::string& outputFile, int newWidth, int newHeight) {
  std::cout << "Realizando la operación de resize en imgsoa con el nuevo tamaño: "
            << newWidth << " " << newHeight << "\n";
  std::cout << "Archivo de entrada: " << inputFile << "\n";
  std::cout << "Archivo de salida: " << outputFile << "\n";

  // Validaciones que pueden lanzar std::invalid_argument
  validateValue(newWidth);
  validateValue(newHeight);

  PPMImage inputImage{};
  if (!leerImagenPPM(inputFile, inputImage)) {
    throw std::runtime_error("Error al leer el archivo de entrada.");
  }

  const ImageSOA imageSOA = leerImagenSOA(inputImage);
  const ImageDimensions dims = {.width = newWidth, .height = newHeight};
  ImageSOA imagenEscaladaSOA = inicializarImagenEscalada(dims);

  escalarImagen(imageSOA, imagenEscaladaSOA);

  const PPMImage outputImage = convertirSOAAImagePPM(imagenEscaladaSOA);
  if (!escribirImagenPPM(outputFile, outputImage)) {
    throw std::runtime_error("Error al guardar el archivo de salida.");
  }

  std::cout << "Operación completada exitosamente.\n";
}