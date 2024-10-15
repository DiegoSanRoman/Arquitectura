// cutfreq.cpp

#include "imgaos/cutfreq.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility> // para std::pair
#include <cstddef> // para std::size_t

namespace imgaos {

namespace {

// Función para calcular la frecuencia absoluta de cada color
  std::unordered_map<Color, int, ColorHash> calcularFrecuenciaColores(const ImageAOS& image) {
    std::unordered_map<Color, int, ColorHash> colorFrequency;
    // Recorrer todos los píxeles y aumentar la frecuencia de cada color
    for (const auto& pixel : image.pixels) {
      colorFrequency[pixel]++;
    }
    return colorFrequency;
  }


// Función de comparación para ordenar los colores según las reglas especificadas
bool compararColores(const std::pair<Color, int>& colorA, const std::pair<Color, int>& colorB) {
    if (colorA.second != colorB.second) {
        return colorA.second < colorB.second;
    }
    if (colorA.first.b != colorB.first.b) {
        return colorA.first.b < colorB.first.b;
    }
    if (colorA.first.g != colorB.first.g) {
        return colorA.first.g < colorB.first.g;
    }
    return colorA.first.r < colorB.first.r;
}

// Función para obtener los n colores menos frecuentes
std::vector<Color> obtenerColoresMenosFrecuentes(
    const std::unordered_map<Color, int, ColorHash>& colorFrequency, int n) {
    std::vector<std::pair<Color, int>> frequencyList(colorFrequency.begin(), colorFrequency.end());
    std::ranges::sort(frequencyList, compararColores);

    std::vector<Color> colorsToRemove;
    const std::size_t limit = static_cast<std::size_t>(std::min(n, static_cast<int>(frequencyList.size())));
    colorsToRemove.reserve(limit);
    for (std::size_t i = 0; i < limit; ++i) {
        colorsToRemove.push_back(frequencyList[i].first);
    }
    return colorsToRemove;
}

  std::unordered_map<Color, Color, ColorHash> encontrarColoresReemplazo(
    const std::vector<std::pair<Color, int>>& frequencyList,
    const std::unordered_set<Color, ColorHash>& colorsToRemoveSet) {

    std::unordered_map<Color, Color, ColorHash> replacementMap;

    for (const auto& colorToRemove : colorsToRemoveSet) {
      double minDistance = std::numeric_limits<double>::max();
      Color closestColor = {.r = 0, .g = 0, .b = 0};  // Inicializar el color más cercano usando inicializadores designados

      for (const auto& colorPair : frequencyList) {
        const Color& candidateColor = colorPair.first;
        if (colorsToRemoveSet.find(candidateColor) == colorsToRemoveSet.end()) {
          const double distance = std::pow(candidateColor.r - colorToRemove.r, 2) +
                                  std::pow(candidateColor.g - colorToRemove.g, 2) +
                                  std::pow(candidateColor.b - colorToRemove.b, 2);
          if (distance < minDistance) {
            minDistance = distance;
            closestColor = candidateColor;
          }
        }
      }
      replacementMap[colorToRemove] = closestColor;
    }
    return replacementMap;
  }

// Función para sustituir los colores en la imagen
void sustituirColoresEnImagen(ImageAOS& image,
                              const std::unordered_map<Color, Color, ColorHash>& replacementMap) {
    for (auto& pixel : image.pixels) {
        auto iterator = replacementMap.find(pixel);
        if (iterator != replacementMap.end()) {
            pixel = iterator->second;
        }
    }
}

} // namespace

void cutfreq(ImageAOS& image, int n) {
    if (n <= 0) {
        std::cerr << "Error: n debe ser un número positivo.\n";
        return;
    }

    const auto colorFrequency = calcularFrecuenciaColores(image);
    const auto colorsToRemove = obtenerColoresMenosFrecuentes(colorFrequency, n);

    std::vector<std::pair<Color, int>> frequencyList(colorFrequency.begin(), colorFrequency.end());
    std::ranges::sort(frequencyList, compararColores);

    const std::unordered_set<Color, ColorHash> colorsToRemoveSet(colorsToRemove.begin(), colorsToRemove.end());

    const auto replacementMap = encontrarColoresReemplazo(frequencyList, colorsToRemoveSet);

    sustituirColoresEnImagen(image, replacementMap);
}

} // namespace imgaos