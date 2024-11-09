//cutfreq.cpp SOA
#include "../common/binario.hpp"
#include "cutfreq.hpp"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {

// Calcular la frecuencia de colores en la imagen
std::unordered_map<uint32_t, int> calcularFrecuenciaColores(const PPMImageSoA& image) {
    std::cout << "Calculando la frecuencia de los colores...\n";
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
std::vector<uint32_t> obtenerColoresMenosFrecuentes(const std::unordered_map<uint32_t, int>& colorFrequency, int number) {
    std::cout << "Obteniendo los colores menos frecuentes...\n";
    std::vector<std::pair<uint32_t, int>> frequencyList(colorFrequency.begin(), colorFrequency.end());
    std::ranges::sort(frequencyList, [](const auto& colorA, const auto& colorB) {
        return colorA.second < colorB.second;
    });

    std::vector<uint32_t> colorsToRemove;
    const std::size_t limit = std::min(static_cast<std::size_t>(number), frequencyList.size());
    colorsToRemove.reserve(limit);
    for (std::size_t i = 0; i < limit; ++i) {
        colorsToRemove.push_back(frequencyList[i].first);
    }
    return colorsToRemove;
}

// Calcular la distancia euclidiana entre dos colores
double calcularDistancia(uint32_t colorA, uint32_t colorB) {
    const int redDiff = static_cast<int>((colorA >> SHIFT_RED) & MASK) - static_cast<int>((colorB >> SHIFT_RED) & MASK);
    const int greenDiff = static_cast<int>((colorA >> SHIFT_GREEN) & MASK) - static_cast<int>((colorB >> SHIFT_GREEN) & MASK);
    const int blueDiff = static_cast<int>(colorA & MASK) - static_cast<int>(colorB & MASK);
    return ((redDiff * redDiff) + (greenDiff * greenDiff) + (blueDiff * blueDiff));
}

// Obtener índices de cuadrícula
GridIndices obtenerIndicesCuadricula(uint32_t color) {
    return {
        .redIndex = static_cast<int>((color >> SHIFT_RED) & MASK) / GRID_STEP,
        .greenIndex = static_cast<int>((color >> SHIFT_GREEN) & MASK) / GRID_STEP,
        .blueIndex = static_cast<int>(color & MASK) / GRID_STEP
    };
}

// Procesar vecino y actualizar el color más cercano
void procesarVecino(const std::unordered_map<int, std::vector<uint32_t>>& grid,
                    const GridIndices& indices, uint32_t colorToRemove, ColorDistance& colorDist) {
    const int neighborIndex = (indices.redIndex << GRID_SHIFT_RED) |
                              (indices.greenIndex << GRID_SHIFT_GREEN) |
                              indices.blueIndex;
    auto iterator = grid.find(neighborIndex);
    if (iterator != grid.end()) {
        for (const auto& candidateColor : iterator->second) {
            const double distancia = calcularDistancia(colorToRemove, candidateColor);
            if (distancia < colorDist.minDistance) {
                colorDist.minDistance = distancia;
                colorDist.closestColor = candidateColor;
                colorDist.found = true;
            }
        }
    }
}

// Encontrar el color más cercano
uint32_t encontrarColorCercano(const std::unordered_map<int, std::vector<uint32_t>>& grid,
                               const GridIndices& indices, const uint32_t colorToRemove, bool& encontrado) {
    ColorDistance colorDist{.closestColor = 0, .minDistance = std::numeric_limits<double>::max(), .found = false};
    for (int dred = -1; dred <= 1; ++dred) {
        const int red = indices.redIndex + dred;
        if (red < 0 || red >= GRID_SIZE) {
          continue;
        }
        for (int dgreen = -1; dgreen <= 1; ++dgreen) {
            const int green = indices.greenIndex + dgreen;
            if (green < 0 || green >= GRID_SIZE) {
              continue;
            }
            for (int dblue = -1; dblue <= 1; ++dblue) {
                const int blue = indices.blueIndex + dblue;
                if (blue < 0 || blue >= GRID_SIZE) {
                  continue;
                }
                procesarVecino(grid, {.redIndex = red, .greenIndex = green, .blueIndex = blue}, colorToRemove, colorDist);
            }
        }
    }
    encontrado = colorDist.found;
    return colorDist.closestColor;
}

// Reemplazar colores en la imagen
void reemplazarColores(PPMImageSoA& image, const std::unordered_map<uint32_t, uint32_t>& replacementMap) {
  std::cout << "Iniciando reemplazo de colores...\n";

  for (std::size_t i = 0; i < image.redChannel.size(); ++i) {
    // Construir el color original desde los tres canales
    const uint32_t color = (static_cast<uint32_t>(image.redChannel[i]) << SHIFT_RED) |
                           (static_cast<uint32_t>(image.greenChannel[i]) << SHIFT_GREEN) |
                           static_cast<uint32_t>(image.blueChannel[i]);

    // Buscar el color en el mapa de reemplazo
    auto const_iterator = replacementMap.find(color);
    if (const_iterator != replacementMap.end()) {
      const uint32_t newColor = const_iterator->second;

      // Desglosar el nuevo color en componentes
      const uint8_t newRed = (newColor >> SHIFT_RED) & MASK;
      const uint8_t newGreen = (newColor >> SHIFT_GREEN) & MASK;
      const uint8_t newBlue = newColor & MASK;

      // Realizar el reemplazo de color en la imagen
      image.redChannel[i] = newRed;
      image.greenChannel[i] = newGreen;
      image.blueChannel[i] = newBlue;
    }
  }
}

  // Encuentra el color más cercano en la lista de frecuencia cuando no está en la cuadrícula
  uint32_t findClosestColorInFrequencyList(const std::vector<std::pair<uint32_t, int>>& frequencyList,
                                          const std::unordered_set<uint32_t>& colorsToRemoveSet,
                                          uint32_t colorToRemove) {
  uint32_t closestColor = 0;
  double minDistance = std::numeric_limits<double>::max();

  for (const auto& [candidateColor, _] : frequencyList) {
    // Omitir el color si está en el conjunto de colores a eliminar
    if (colorsToRemoveSet.find(candidateColor) == colorsToRemoveSet.end()) {
      const double distance = calcularDistancia(colorToRemove, candidateColor);
      if (distance < minDistance) {
        minDistance = distance;
        closestColor = candidateColor;
      }
    }
  }

  return closestColor;
}


// Construir el mapa de reemplazo de colores
std::unordered_map<uint32_t, uint32_t> construirMapaReemplazo(
        const std::unordered_set<uint32_t>& colorsToRemoveSet,
        const std::unordered_map<int, std::vector<uint32_t>>& grid,
        const std::vector<std::pair<uint32_t, int>>& frequencyList) {

    std::unordered_map<uint32_t, uint32_t> replacementMap;
    for (const auto& colorToRemove : colorsToRemoveSet) {
        const GridIndices indices = obtenerIndicesCuadricula(colorToRemove);
        bool encontrado = false;
        uint32_t closestColor = encontrarColorCercano(grid, indices, colorToRemove, encontrado);
        if (!encontrado) {
            closestColor = findClosestColorInFrequencyList(frequencyList, colorsToRemoveSet, colorToRemove);
        }
        replacementMap[colorToRemove] = closestColor;
    }
    return replacementMap;
}

} // namespace

// Función principal de corte de frecuencia en SOA
void cutfreq(PPMImageSoA& image, int n) {
  auto colorFrequency = calcularFrecuenciaColores(image);
  auto colorsToRemove = obtenerColoresMenosFrecuentes(colorFrequency, n);

  std::vector<std::pair<uint32_t, int>> frequencyList(colorFrequency.begin(), colorFrequency.end());
  std::ranges::sort(frequencyList, [](const auto& colorA, const auto& colorB) {
      return colorA.second < colorB.second;
  });

  std::unordered_set<uint32_t> colorsToRemoveSet(colorsToRemove.begin(), colorsToRemove.end());
  std::unordered_map<int, std::vector<uint32_t>> grid;

  for (const auto& [color, _] : frequencyList) {
    if (colorsToRemoveSet.find(color) == colorsToRemoveSet.end()) {
      const GridIndices indices = obtenerIndicesCuadricula(color);
      const int gridIndex = (indices.redIndex << GRID_SHIFT_RED) | (indices.greenIndex << GRID_SHIFT_GREEN) | indices.blueIndex;
      grid[gridIndex].push_back(color);
    }
  }

  auto replacementMap = construirMapaReemplazo(colorsToRemoveSet, grid, frequencyList);
  reemplazarColores(image, replacementMap);
}