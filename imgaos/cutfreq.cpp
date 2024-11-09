//cutfreq.cpp
#include "../common/binario.hpp"
#include "cutfreq.hpp"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stack>


namespace {

// Calcular frecuencia de colores
std::unordered_map<uint32_t, int> calcularFrecuenciaColores(const PPMImage& image) {
   std:: cout << "calculando la frecuencia de los colores \n";
    std::unordered_map<uint32_t, int> colorFrequency;
    for (std::size_t i = 0; i < image.pixelData.size(); i += 3) {
        const uint32_t color = (static_cast<uint32_t>(image.pixelData[i]) << SHIFT_RED) |
                               (static_cast<uint32_t>(image.pixelData[i + 1]) << SHIFT_GREEN) |
                               static_cast<uint32_t>(image.pixelData[i + 2]);
        colorFrequency[color]++;
    }

    return colorFrequency;
}

// Obtener colores menos frecuentes
std::vector<uint32_t> obtenerColoresMenosFrecuentes(const std::unordered_map<uint32_t, int>& colorFrequency, int n) {
    std:: cout << "obteniendo los colores menos frecuentes \n";
    std::vector<std::pair<uint32_t, int>> frequencyList(colorFrequency.begin(), colorFrequency.end());
    std::ranges::sort(frequencyList, [](const auto& colorA, const auto& colorB) {
        if (colorA.second != colorB.second) {
            return colorA.second < colorB.second;
        }
        if ((colorA.first & MASK) != (colorB.first & MASK)) {
            return (colorA.first & MASK) > (colorB.first & MASK);
        }
        if (((colorA.first >> SHIFT_GREEN) & MASK) != ((colorB.first >> SHIFT_GREEN) & MASK)) {
            return ((colorA.first >> SHIFT_GREEN) & MASK) > ((colorB.first >> SHIFT_GREEN) & MASK);
        }
        return ((colorA.first >> SHIFT_RED) & MASK) > ((colorB.first >> SHIFT_RED) & MASK);
    });

    std::vector<uint32_t> colorsToRemove;
    const std::size_t limit = static_cast<std::size_t>(std::min(n, static_cast<int>(frequencyList.size())));
    colorsToRemove.reserve(limit);
    for (std::size_t i = 0; i < limit; ++i) {
        colorsToRemove.push_back(frequencyList[i].first);
    }


    return colorsToRemove;
}

// Calcular distancia euclidiana al cuadrado (sin `sqrt` para mejorar rendimiento)
inline double distanciaEuclidianaCuadrada(uint32_t colorA, uint32_t colorB) {
    return std::pow(static_cast<int>((colorA >> SHIFT_RED) & MASK) - static_cast<int>((colorB >> SHIFT_RED) & MASK), 2) +
           std::pow(static_cast<int>((colorA >> SHIFT_GREEN) & MASK) - static_cast<int>((colorB >> SHIFT_GREEN) & MASK), 2) +
           std::pow(static_cast<int>(colorA & MASK) - static_cast<int>(colorB & MASK), 2);
}


inline int getGridIndex(const ColorIndices& indices) {
    return (indices.redIndex << GRID_SHIFT_RED) | (indices.greenIndex << GRID_SHIFT_GREEN) | indices.blueIndex;
}

inline void getGridIndices(uint32_t color, ColorIndices& indices) {
    indices.redIndex = static_cast<int>((color >> SHIFT_RED) & MASK) / GRID_STEP;
    indices.greenIndex = static_cast<int>((color >> SHIFT_GREEN) & MASK) / GRID_STEP;
    indices.blueIndex = static_cast<int>(color & MASK) / GRID_STEP;
}

// Subfunción para procesar vecinos de color
inline void processNeighbor(const std::unordered_map<int, std::vector<uint32_t>>& grid,
                            const ColorIndices& neighborIndices, NeighborSearchData& data) {
    const int neighborIndex = getGridIndex(neighborIndices);
    auto gridIt = grid.find(neighborIndex);
    if (gridIt != grid.end()) {
        for (const auto& candidateColor : gridIt->second) {
            const double distance = distanciaEuclidianaCuadrada(data.colorToRemove, candidateColor);
            if (distance < data.minDistance) {
                data.minDistance = distance;
                data.closestColor = candidateColor;
                data.found = true;
            }
        }
    }
}

uint32_t findClosestColorInNeighbors(const std::unordered_map<int, std::vector<uint32_t>>& grid,
                                     const ColorIndices& indices, uint32_t colorToRemove,
                                     bool& found) {
   NeighborSearchData data{.colorToRemove = colorToRemove};
    for (int deltaRed = -1; deltaRed <= 1; ++deltaRed) {
        const int neighborRedIndex = indices.redIndex + deltaRed;
        if (neighborRedIndex < 0 || neighborRedIndex >= GRID_SIZE) {
            continue;
        }

        for (int deltaGreen = -1; deltaGreen <= 1; ++deltaGreen) {
            const int neighborGreenIndex = indices.greenIndex + deltaGreen;
            if (neighborGreenIndex < 0 || neighborGreenIndex >= GRID_SIZE) {
                continue;
            }

            for (int deltaBlue = -1; deltaBlue <= 1; ++deltaBlue) {
                const int neighborBlueIndex = indices.blueIndex + deltaBlue;
                if (neighborBlueIndex < 0 || neighborBlueIndex >= GRID_SIZE) {
                    continue;
                }

                const ColorIndices neighborIndices = {
                    .redIndex = neighborRedIndex,
                    .greenIndex = neighborGreenIndex,
                    .blueIndex = neighborBlueIndex
                };
                processNeighbor(grid, neighborIndices, data);
            }
        }
    }
    found = data.found;
    return data.closestColor;
}
uint32_t findClosestColorInFrequencyList(const std::vector<std::pair<uint32_t, int>>& frequencyList,
                                        const std::unordered_set<uint32_t>& colorsToRemoveSet,
                                        uint32_t colorToRemove) {
    uint32_t closestColor = 0;
    double minDistance = std::numeric_limits<double>::max();
    for (const auto& [candidateColor, _] : frequencyList) {
        if (colorsToRemoveSet.find(candidateColor) == colorsToRemoveSet.end()) {
            const double distance = distanciaEuclidianaCuadrada(colorToRemove, candidateColor);
            if (distance < minDistance) {
                minDistance = distance;
                closestColor = candidateColor;
            }
        }
    }
    return closestColor;
}


  std::unordered_map<uint32_t, uint32_t> encontrarColoresReemplazo(
      const std::unordered_set<uint32_t>& colorsToRemoveSet,
      const std::vector<std::pair<uint32_t, int>>& frequencyList) {

  // Construir el mapa de cuadrícula
  std::unordered_map<int, std::vector<uint32_t>> grid;
  for (const auto& [color, _] : frequencyList) {
    if (colorsToRemoveSet.find(color) == colorsToRemoveSet.end()) {
      ColorIndices indices{};
      getGridIndices(color, indices);
      const int gridIndex = getGridIndex(indices);
      grid[gridIndex].push_back(color);
    }
  }

  std::unordered_map<uint32_t, uint32_t> replacementMap;
  for (const auto& colorToRemove : colorsToRemoveSet) {
    ColorIndices indices{};
    getGridIndices(colorToRemove, indices);

    bool found = false;
    uint32_t closestColor = findClosestColorInNeighbors(grid, indices, colorToRemove, found);

    if (!found) {
      closestColor = findClosestColorInFrequencyList(frequencyList, colorsToRemoveSet, colorToRemove);
    }

    replacementMap[colorToRemove] = closestColor;
  }

  return replacementMap;
}

// Reemplazar colores en la imagen
void reemplazarColores(PPMImage& image, const std::unordered_map<uint32_t, uint32_t>& replacementMap) {
  std:: cout << "reemplazando colores \n";
    for (std::size_t i = 0; i < image.pixelData.size(); i += 3) {
        const uint32_t color = (static_cast<uint32_t>(image.pixelData[i]) << SHIFT_RED) |
                               (static_cast<uint32_t>(image.pixelData[i + 1]) << SHIFT_GREEN) |
                               static_cast<uint32_t>(image.pixelData[i + 2]);
        auto iterator = replacementMap.find(color);
        if (iterator != replacementMap.end()) {
            const uint32_t newColor = iterator->second;

            // Realizar el reemplazo de color en la imagen
            image.pixelData[i] = (newColor >> SHIFT_RED) & MASK;
            image.pixelData[i + 1] = (newColor >> SHIFT_GREEN) & MASK;
            image.pixelData[i + 2] = newColor & MASK;
        }
    }
}



} // namespace

// Uso en la función cutfreq
void cutfreq(PPMImage& image, int n) {
  auto colorFrequency = calcularFrecuenciaColores(image);
  auto colorsToRemove = obtenerColoresMenosFrecuentes(colorFrequency, n);

  std::vector<std::pair<uint32_t, int>> frequencyList(colorFrequency.begin(), colorFrequency.end());
  std::ranges::sort(frequencyList, [](const auto& colorA, const auto& colorB) {
      if (colorA.second != colorB.second) {
          return colorA.second < colorB.second;
      }
      if ((colorA.first & MASK) != (colorB.first & MASK)) {
          return (colorA.first & MASK) > (colorB.first & MASK);
      }
      if (((colorA.first >> SHIFT_GREEN) & MASK) != ((colorB.first >> SHIFT_GREEN) & MASK)) {
          return ((colorA.first >> SHIFT_GREEN) & MASK) > ((colorB.first >> SHIFT_GREEN) & MASK);
      }
      return ((colorA.first >> SHIFT_RED) & MASK) > ((colorB.first >> SHIFT_RED) & MASK);
  });

  const std::unordered_set<uint32_t> colorsToRemoveSet(colorsToRemove.begin(), colorsToRemove.end());
    // Usar la nueva función optimizada
    const auto replacementMap = encontrarColoresReemplazo(colorsToRemoveSet, frequencyList);

    reemplazarColores(image, replacementMap);
  }
