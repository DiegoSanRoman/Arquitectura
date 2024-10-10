//
// Created by alba on 9/10/24.
//
#include "cutfreq.hpp"
#include <cmath>
#include <map>
#include <algorithm>
#include <limits>
#include <iostream>

// Calcula la distancia euclidiana entre dos colores en SOA
double euclideanDistance(const ImageSOA& image, size_t index1, size_t index2) {
    int r1 = image.red_channel[index1], g1 = image.green_channel[index1], b1 = image.blue_channel[index1];
    int r2 = image.red_channel[index2], g2 = image.green_channel[index2], b2 = image.blue_channel[index2];

    return std::sqrt((r2 - r1) * (r2 - r1) + (g2 - g1) * (g2 - g1) + (b2 - b1) * (b2 - b1));
}

// Encuentra el color más cercano en la imagen SOA usando la distancia euclidiana
size_t findClosestColor(const ImageSOA& image, size_t colorIndex, const std::vector<size_t>& remainingIndices) {
  double minDistance = std::numeric_limits<double>::max();
  size_t closestIndex = 0;

  for (const auto& candidateIndex : remainingIndices) {
    // Excluir el color que estamos eliminando de los candidatos
    if (candidateIndex == colorIndex) {
      continue;  // Saltar este color
    }

    double distance = euclideanDistance(image, colorIndex, candidateIndex);

    // Imprimir los colores que se están comparando
    std::cout << "Comparando color (R: " << image.red_channel[colorIndex]
              << ", G: " << image.green_channel[colorIndex]
              << ", B: " << image.blue_channel[colorIndex] << ")";
    std::cout << " con color candidato (R: " << image.red_channel[candidateIndex]
              << ", G: " << image.green_channel[candidateIndex]
              << ", B: " << image.blue_channel[candidateIndex] << ")";
    std::cout << " - Distancia: " << distance << "\n";

    if (distance < minDistance) {
      minDistance = distance;
      closestIndex = candidateIndex;
    }
  }

  return closestIndex;
}


// Compara dos colores en SOA por los valores de sus canales
bool colorCompare(const ImageSOA& image, size_t index1, size_t index2) {
    if (image.blue_channel[index1] != image.blue_channel[index2]) {
        return image.blue_channel[index1] > image.blue_channel[index2];
    } else if (image.green_channel[index1] != image.green_channel[index2]) {
        return image.green_channel[index1] > image.green_channel[index2];
    } else {
        return image.red_channel[index1] > image.red_channel[index2];
    }
}

// Función principal cutfreq adaptada para SOA
void cutfreq(ImageSOA& image, int n) {
    std::map<std::tuple<int, int, int>, int> colorFrequency;

    // Contar la frecuencia de los colores
    for (size_t i = 0; i < image.red_channel.size(); ++i) {
        auto color = std::make_tuple(image.red_channel[i], image.green_channel[i], image.blue_channel[i]);
        colorFrequency[color]++;
    }

    // Ordenar los colores por frecuencia
    std::vector<std::pair<std::tuple<int, int, int>, int>> sortedColors(colorFrequency.begin(), colorFrequency.end());
    std::sort(sortedColors.begin(), sortedColors.end(), [&image, &colorFrequency](const auto& a, const auto& b) {
    // Primero, comparar por frecuencia
    if (a.second != b.second) {
        return a.second < b.second; // Menor frecuencia primero
    }
    // Si las frecuencias son iguales, usar colorCompare para comparar los valores de los colores
    return colorCompare(image,
        std::distance(colorFrequency.begin(), colorFrequency.find(a.first)),
        std::distance(colorFrequency.begin(), colorFrequency.find(b.first)));
  });
  // Imprimir las frecuencias antes de la eliminación
  for (const auto& color : sortedColors) {
    std::cout << "Color (R: " << std::get<0>(color.first)
              << ", G: " << std::get<1>(color.first)
              << ", B: " << std::get<2>(color.first) << ")";
    std::cout << " - Frecuencia: " << color.second << "\n";
  }

    std::vector<size_t> colorsToRemoveIndices, remainingIndices;
    for (size_t i = 0; i < sortedColors.size(); ++i) {
        if (i < static_cast<size_t>(n)) {
            colorsToRemoveIndices.push_back(i);
        } else {
            remainingIndices.push_back(i);
        }
    }

    // Reemplazar los colores
    for (const auto& colorIndex : colorsToRemoveIndices) {
      size_t closestColorIndex = findClosestColor(image, colorIndex, remainingIndices);

      // Imprimir el color que se está eliminando y su reemplazo
      std::cout << "Eliminando color (R: " << image.red_channel[colorIndex]
                << ", G: " << image.green_channel[colorIndex]
                << ", B: " << image.blue_channel[colorIndex] << ")\n";
      std::cout << "Reemplazado por (R: " << image.red_channel[closestColorIndex]
                << ", G: " << image.green_channel[closestColorIndex]
                << ", B: " << image.blue_channel[closestColorIndex] << ")\n";

      // Reemplazar todos los píxeles que correspondan al color eliminado
      for (size_t i = 0; i < image.red_channel.size(); ++i) {
        if (image.red_channel[i] == image.red_channel[colorIndex] &&
            image.green_channel[i] == image.green_channel[colorIndex] &&
            image.blue_channel[i] == image.blue_channel[colorIndex]) {
          image.red_channel[i] = image.red_channel[closestColorIndex];
          image.green_channel[i] = image.green_channel[closestColorIndex];
          image.blue_channel[i] = image.blue_channel[closestColorIndex];
            }
      }
    }
}
