// Adaptación de la función encontrarColoresReemplazoOptimizado para SOA

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
  std::unordered_map<uint32_t, int> calcularFrecuenciaColores(const PPMImageSoA& image) {
    std::cout << "calculando la frecuencia de los colores \n";
    std::unordered_map<uint32_t, int> colorFrequency;
    for (std::size_t i = 0; i < image.redChannel.size(); ++i) {
      const uint32_t color = (static_cast<uint32_t>(image.redChannel[i]) << SHIFT_RED) |
                             (static_cast<uint32_t>(image.greenChannel[i]) << SHIFT_GREEN) |
                             static_cast<uint32_t>(image.blueChannel[i]);
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

  // Función para obtener el índice de la cuadrícula
  inline int getGridIndex(const ColorIndices& indices) {
    return (indices.redIndex << (2 * GRID_SHIFT_RED)) |
           (indices.greenIndex << GRID_SHIFT_GREEN) |
           indices.blueIndex;
  }

  // Nueva función getGridIndices que toma un Color
  inline void getGridIndices(const Color& color, ColorIndices& indices) {
    indices.redIndex = color.red / GRID_STEP;
    indices.greenIndex = color.green / GRID_STEP;
    indices.blueIndex = color.blue / GRID_STEP;
  }

  // Nueva función distanciaEuclidianaCuadrada que toma dos colores
  inline double distanciaEuclidianaCuadrada(const Color& colorA, const Color& colorB) {
    const int deltaRed = static_cast<int>(colorA.red) - static_cast<int>(colorB.red);
    const int deltaGreen = static_cast<int>(colorA.green) - static_cast<int>(colorB.green);
    const int deltaBlue = static_cast<int>(colorA.blue) - static_cast<int>(colorB.blue);
    return (deltaRed * deltaRed) + (deltaGreen * deltaGreen) + (deltaBlue * deltaBlue);
  }

  // Función para procesar vecinos en la cuadrícula
  inline void processNeighbor(const std::unordered_map<int, std::vector<uint32_t>>& grid,
                              const ColorIndices& neighborIndices, NeighborSearchData& data) {
    const int neighborIndex = getGridIndex(neighborIndices);
    auto gridIt = grid.find(neighborIndex);
    if (gridIt != grid.end()) {
      for (const auto& candidateColor : gridIt->second) {
        const Color candidate = {
          .red = static_cast<uint8_t>((candidateColor >> SHIFT_RED) & MASK),
          .green = static_cast<uint8_t>((candidateColor >> SHIFT_GREEN) & MASK),
          .blue = static_cast<uint8_t>(candidateColor & MASK)
      };

        const Color colorToRemove = {
          .red = static_cast<uint8_t>((data.colorToRemove >> SHIFT_RED) & MASK),
          .green = static_cast<uint8_t>((data.colorToRemove >> SHIFT_GREEN) & MASK),
          .blue = static_cast<uint8_t>(data.colorToRemove & MASK)
      };
        const double distance = distanciaEuclidianaCuadrada(colorToRemove, candidate);
        if (distance < data.minDistance) {
          data.minDistance = distance;
          data.closestColor = candidateColor;
          data.found = true;
        }
      }
    }
  }


  // Función para encontrar el color más cercano en los vecinos
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

  // Función para encontrar el color más cercano en la lista de frecuencias
  uint32_t findClosestColorInFrequencyList(const std::vector<std::pair<uint32_t, int>>& frequencyList,
                                           const std::unordered_set<uint32_t>& colorsToRemoveSet,
                                           uint32_t colorToRemove) {
    uint32_t closestColor = 0;
    double minDistance = std::numeric_limits<double>::max();

    // Definimos colorToRemove como una estructura Color
    const Color colorToRemoveComponents = {
      .red = static_cast<uint8_t>((colorToRemove >> SHIFT_RED) & MASK),
      .green = static_cast<uint8_t>((colorToRemove >> SHIFT_GREEN) & MASK),
      .blue = static_cast<uint8_t>(colorToRemove & MASK)
    };

    // Iteramos sobre la lista de colores candidatos
    for (const auto& [candidateColor, _] : frequencyList) {
      if (colorsToRemoveSet.find(candidateColor) == colorsToRemoveSet.end()) {
        // Extraemos componentes del color candidato como una estructura Color
        const Color candidateColorComponents = {
          .red = static_cast<uint8_t>((candidateColor >> SHIFT_RED) & MASK),
          .green = static_cast<uint8_t>((candidateColor >> SHIFT_GREEN) & MASK),
          .blue = static_cast<uint8_t>(candidateColor & MASK)
        };

        // Calculamos la distancia usando estructuras Color
        const double distance = distanciaEuclidianaCuadrada(colorToRemoveComponents, candidateColorComponents);

        // Comparamos y guardamos el color más cercano
        if (distance < minDistance) {
          minDistance = distance;
          closestColor = candidateColor;
        }
      }
    }
    return closestColor;
  }

  // Función principal adaptada para PPMImageSoA
  std::unordered_map<uint32_t, uint32_t> encontrarColoresReemplazo(
     const PPMImageSoA& image,
     const std::unordered_set<uint32_t>& colorsToRemoveSet,
     const std::vector<std::pair<uint32_t, int>>& frequencyList) {
    // Ignoramos el parámetro `image` ya que no se usa
    (void)image;
    // Construir el mapa de cuadrícula
    std::unordered_map<int, std::vector<uint32_t>> grid;
    for (const auto& [color, _] : frequencyList) {
      if (colorsToRemoveSet.find(color) == colorsToRemoveSet.end()) {
        const Color colorComponents = {
          .red = static_cast<uint8_t>((color >> SHIFT_RED) & MASK),
          .green = static_cast<uint8_t>((color >> SHIFT_GREEN) & MASK),
          .blue = static_cast<uint8_t>(color & MASK)
        };
        ColorIndices indices{};
        getGridIndices(colorComponents, indices);
        const int gridIndex = getGridIndex(indices);
        grid[gridIndex].push_back(color);
      }
    }
    std::unordered_map<uint32_t, uint32_t> replacementMap;
    for (const auto& colorToRemove : colorsToRemoveSet) {
      const Color colorToRemoveComponents = {
        .red = static_cast<uint8_t>((colorToRemove >> SHIFT_RED) & MASK),
        .green = static_cast<uint8_t>((colorToRemove >> SHIFT_GREEN) & MASK),
        .blue = static_cast<uint8_t>(colorToRemove & MASK)
      };

      ColorIndices indices{};
      getGridIndices(colorToRemoveComponents, indices);
      bool found = false;
      uint32_t closestColor = findClosestColorInNeighbors(grid, indices, colorToRemove, found);
      if (!found) {
        closestColor = findClosestColorInFrequencyList(frequencyList, colorsToRemoveSet, colorToRemove);
      }
      replacementMap[colorToRemove] = closestColor;
    }

    return replacementMap;
  }


  // Función para reemplazar colores en la imagen SOA
  void reemplazarColores(PPMImageSoA& image, const std::unordered_map<uint32_t, uint32_t>& replacementMap) {
    const std::size_t pixelCount = image.redChannel.size();

    for (std::size_t i = 0; i < pixelCount; ++i) {
      const uint32_t color = (static_cast<uint32_t>(image.redChannel[i]) << SHIFT_RED) |
                       (static_cast<uint32_t>(image.greenChannel[i]) << SHIFT_GREEN) |
                       static_cast<uint32_t>(image.blueChannel[i]);

      auto iterator = replacementMap.find(color);
      if (iterator != replacementMap.end()) {
        const uint32_t newColor = iterator->second;
        image.redChannel[i] = (newColor >> SHIFT_RED) & MASK;
        image.greenChannel[i] = (newColor >> SHIFT_GREEN) & MASK;
        image.blueChannel[i] = newColor & MASK;
      }
    }
  }

}

// Función principal adaptada para SOA
void cutfreq(PPMImageSoA& image, int n) {
  auto colorFrequency = calcularFrecuenciaColores(image);
  auto colorsToRemove = obtenerColoresMenosFrecuentes(colorFrequency, n);

  std::vector<std::pair<uint32_t, int>> frequencyList(colorFrequency.begin(), colorFrequency.end());
  std::ranges::sort(frequencyList, [](const auto& colorA, const auto& colorB) {
      if (colorA.second != colorB.second) {
          return colorA.second < colorB.second;
      }
      if ((colorA.first & MASK) != (colorB.first & MASK)) {
          return (colorA.first & MASK) > (colorA.first & MASK);
      }
      if (((colorA.first >> SHIFT_GREEN) & MASK) != ((colorB.first >> SHIFT_GREEN) & MASK)) {
          return ((colorA.first >> SHIFT_GREEN) & MASK) > ((colorB.first >> SHIFT_GREEN) & MASK);
      }
      return ((colorA.first >> SHIFT_RED) & MASK) > ((colorB.first >> SHIFT_RED) & MASK);
  });

  const std::unordered_set<uint32_t> colorsToRemoveSet(colorsToRemove.begin(), colorsToRemove.end());
  const auto replacementMap = encontrarColoresReemplazo(image, colorsToRemoveSet, frequencyList);

  reemplazarColores(image, replacementMap);
}