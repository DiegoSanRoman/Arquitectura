//
// Created by alba on 9/10/24.
//
#include <map>
#include <tuple>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <iostream>

// Estructura para representar un píxel RGB
struct Pixel {
    int r, g, b;
};

// Función para calcular la distancia euclidiana entre dos colores
double euclideanDistance(const std::tuple<int, int, int>& color1, const std::tuple<int, int, int>& color2) {
    int r_diff = std::get<0>(color1) - std::get<0>(color2);
    int g_diff = std::get<1>(color1) - std::get<1>(color2);
    int b_diff = std::get<2>(color1) - std::get<2>(color2);
    return std::sqrt(r_diff * r_diff + g_diff * g_diff + b_diff * b_diff);
}

// Función para encontrar el color más cercano en el espacio RGB
std::tuple<int, int, int> findClosestColor(const std::tuple<int, int, int>& targetColor,
                                           const std::vector<std::tuple<int, int, int>>& remainingColors) {
    double minDistance = std::numeric_limits<double>::max();
    std::tuple<int, int, int> closestColor = remainingColors[0];

    for (const auto& color : remainingColors) {
        double distance = euclideanDistance(targetColor, color);
        if (distance < minDistance) {
            minDistance = distance;
            closestColor = color;
        }
    }

    return closestColor;
}

bool colorCompare(const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b) {
  return (std::get<2>(a) > std::get<2>(b)) ||
         (std::get<2>(a) == std::get<2>(b) && std::get<1>(a) > std::get<1>(b)) ||
         (std::get<2>(a) == std::get<2>(b) && std::get<1>(a) == std::get<1>(b) && std::get<0>(a) > std::get<0>(b));
}

// Función cutfreq que elimina los colores menos frecuentes y los reemplaza
void cutfreq(std::vector<Pixel>& image, int n) {
  std::map<std::tuple<int, int, int>, int> colorFrequency;
  for (const auto& pixel : image) {
    auto color = std::make_tuple(pixel.r, pixel.g, pixel.b);
    colorFrequency[color]++;
  }

  std::vector<std::pair<std::tuple<int, int, int>, int>> sortedColors(colorFrequency.begin(), colorFrequency.end());
  std::sort(sortedColors.begin(), sortedColors.end(), [](const auto& a, const auto& b) {
      return a.second == b.second ? colorCompare(a.first, b.first) : a.second < b.second;
  });

  std::cout << "Frecuencias de los colores:" << std::endl;
  for (const auto& pair : colorFrequency) {
    auto color = pair.first;
    int freq = pair.second;
    std::cout << "Color (R: " << std::get<0>(color) << ", G: " << std::get<1>(color) << ", B: " << std::get<2>(color) << ") - Frecuencia: " << freq << std::endl;
  }

  std::vector<std::tuple<int, int, int>> colorsToRemove, remainingColors;
  for (size_t i = 0; i < sortedColors.size(); ++i) {
    (i < static_cast<size_t>(n) ? colorsToRemove : remainingColors).push_back(sortedColors[i].first);
  }
  std::cout << "Colores a eliminar:" << std::endl;
  for (const auto& color : colorsToRemove) {
    std::cout << "Color (R: " << std::get<0>(color) << ", G: " << std::get<1>(color) << ", B: " << std::get<2>(color) << ")" << std::endl;
  }

  std::map<std::tuple<int, int, int>, std::tuple<int, int, int>> replacementMap;
  for (const auto& color : colorsToRemove) {
    replacementMap[color] = findClosestColor(color, remainingColors);
  }
  std::cout << "Reemplazo de colores:" << std::endl;
  for (const auto& pair : replacementMap) {
    auto oldColor = pair.first;
    auto newColor = pair.second;
    std::cout << "Color original (R: " << std::get<0>(oldColor) << ", G: " << std::get<1>(oldColor) << ", B: " << std::get<2>(oldColor) << ") "
              << "reemplazado por (R: " << std::get<0>(newColor) << ", G: " << std::get<1>(newColor) << ", B: " << std::get<2>(newColor) << ")" << std::endl;
  }

  for (auto& pixel : image) {
    auto color = std::make_tuple(pixel.r, pixel.g, pixel.b);
    if (replacementMap.contains(color)) {
      auto newColor = replacementMap[color];
      pixel.r = std::get<0>(newColor);
      pixel.g = std::get<1>(newColor);
      pixel.b = std::get<2>(newColor);
    }
  }
}

