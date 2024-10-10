//
// Created by alba on 9/10/24.
//
#include "cutfreq.hpp"
#include <iostream>
#include <map>
#include <cmath>
#include <limits>
#include <algorithm>

// Función para calcular la distancia euclidiana entre dos colores
// Función para calcular la distancia euclidiana entre dos colores
double euclideanDistance(const ImageSOA& image, size_t colorIndex1, size_t colorIndex2) {
  int rDiff = image.red_channel[colorIndex1] - image.red_channel[colorIndex2];
  int gDiff = image.green_channel[colorIndex1] - image.green_channel[colorIndex2];
  int bDiff = image.blue_channel[colorIndex1] - image.blue_channel[colorIndex2];

  // Imprimir las diferencias para depuración
  std::cout << "Comparando color (R: " << image.red_channel[colorIndex1]
            << ", G: " << image.green_channel[colorIndex1]
            << ", B: " << image.blue_channel[colorIndex1] << ") ";
  std::cout << "con color (R: " << image.red_channel[colorIndex2]
            << ", G: " << image.green_channel[colorIndex2]
            << ", B: " << image.blue_channel[colorIndex2] << ") ";
  std::cout << "- Diferencias: R = " << rDiff << ", G = " << gDiff << ", B = " << bDiff << "\n";

  return std::sqrt(rDiff * rDiff + gDiff * gDiff + bDiff * bDiff);
}


// Función para encontrar el color más cercano
size_t findClosestColor(const ImageSOA& image, size_t colorIndex, const std::vector<size_t>& remainingIndices) {
    double minDistance = std::numeric_limits<double>::max();
    size_t closestIndex = 0;

    for (const auto& candidateIndex : remainingIndices) {
        if (candidateIndex == colorIndex) {
            continue;  // Evitar que se compare el color con sí mismo
        }

        double distance = euclideanDistance(image, colorIndex, candidateIndex);

        if (distance < minDistance) {
            minDistance = distance;
            closestIndex = candidateIndex;
        }
    }

    return closestIndex;
}

// Función para comparar dos colores en base a las reglas establecidas
bool colorCompare(const ImageSOA& image, size_t colorIndex1, size_t colorIndex2) {
    if (image.blue_channel[colorIndex1] != image.blue_channel[colorIndex2]) {
        return image.blue_channel[colorIndex1] < image.blue_channel[colorIndex2];
    }
    if (image.green_channel[colorIndex1] != image.green_channel[colorIndex2]) {
        return image.green_channel[colorIndex1] < image.green_channel[colorIndex2];
    }
    return image.red_channel[colorIndex1] < image.red_channel[colorIndex2];
}

// Función principal cutfreq
void cutfreq(ImageSOA& image, int n) {
    std::map<std::tuple<int, int, int>, int> colorFrequency;

    // 1. Determinar la frecuencia absoluta de cada color
    for (size_t i = 0; i < image.red_channel.size(); ++i) {
        std::tuple<int, int, int> color = std::make_tuple(image.red_channel[i], image.green_channel[i], image.blue_channel[i]);
        colorFrequency[color]++;
    }

    // 2. Convertir el mapa a un vector y ordenar por frecuencia (y luego por componente de color)
    std::vector<std::pair<std::tuple<int, int, int>, int>> sortedColors(colorFrequency.begin(), colorFrequency.end());

    // Ordenar por frecuencia, luego por azul, verde y rojo
    std::sort(sortedColors.begin(), sortedColors.end(), [&image, &colorFrequency](const auto& a, const auto& b) {
        // Comparar por frecuencia
        return a.second == b.second
            ? colorCompare(image,
                           static_cast<size_t>(std::distance(colorFrequency.begin(), colorFrequency.find(a.first))),
                           static_cast<size_t>(std::distance(colorFrequency.begin(), colorFrequency.find(b.first))))
            : a.second < b.second;  // Menor frecuencia primero
    });

    // 3. Seleccionar los n colores menos frecuentes
    std::vector<size_t> colorsToRemoveIndices;
    std::vector<size_t> remainingIndices;

    for (size_t i = 0; i < sortedColors.size(); ++i) {
        if (i < static_cast<size_t>(n)) {
            colorsToRemoveIndices.push_back(i);
        } else {
            remainingIndices.push_back(i);
        }
    }

  // 4. Reemplazar los colores menos frecuentes por el color más cercano
  for (const auto& colorIndex : colorsToRemoveIndices) {
    size_t closestColorIndex = findClosestColor(image, colorIndex, remainingIndices);

    std::tuple<int, int, int> colorToRemove = sortedColors[colorIndex].first;

    // Imprimir para depuración
    std::cout << "Eliminando color (R: " << std::get<0>(colorToRemove)
              << ", G: " << std::get<1>(colorToRemove)
              << ", B: " << std::get<2>(colorToRemove) << ")\n";
    std::cout << "Reemplazado por (R: " << image.red_channel[closestColorIndex]
              << ", G: " << image.green_channel[closestColorIndex]
              << ", B: " << image.blue_channel[closestColorIndex] << ")\n";

    // Reemplazar todos los píxeles que correspondan al color eliminado
    for (size_t i = 0; i < image.red_channel.size(); ++i) {
      if (image.red_channel[i] == std::get<0>(colorToRemove) &&
          image.green_channel[i] == std::get<1>(colorToRemove) &&
          image.blue_channel[i] == std::get<2>(colorToRemove)) {

        image.red_channel[i] = image.red_channel[closestColorIndex];
        image.green_channel[i] = image.green_channel[closestColorIndex];
        image.blue_channel[i] = image.blue_channel[closestColorIndex];
          }
    }
  }
}
