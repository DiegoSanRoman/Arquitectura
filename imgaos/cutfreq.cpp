// cutfreq.cpp AOS
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

// Esta función recorre la imagen y cuenta cuántas veces aparece cada color
// Almacena la frecuencia de cada color en un mapa de colores
std::unordered_map<uint32_t, int> calcularFrecuenciaColores(const PPMImage& image) {
    std::unordered_map<uint32_t, int> colorFrequency;

    // Iteramos sobre cada píxel de la imagen 'i' aumenta de tres en tres porque cada píxel tiene tres valores (RGB)
    for (std::size_t i = 0; i < image.pixelData.size(); i += 3) {

        // Combina los valores de rojo, verde y azul en un solo valor para representar el color
        const uint32_t color = (static_cast<uint32_t>(image.pixelData[i]) << SHIFT_RED) |
                               (static_cast<uint32_t>(image.pixelData[i + 1]) << SHIFT_GREEN) |
                               static_cast<uint32_t>(image.pixelData[i + 2]);

        // Incrementa la cuenta para este color en el mapa
        colorFrequency[color]++;
    }

    // Devuelve el mapa con la frecuencia de cada color
    return colorFrequency;
}

// Esta función toma el mapa de frecuencias de color y selecciona los 'n' colores menos frecuentes
std::vector<uint32_t> obtenerColoresMenosFrecuentes(const std::unordered_map<uint32_t, int>& colorFrequency, int n) {
    // Convierte el mapa de frecuencias en una lista para poder ordenar los colores
    std::vector<std::pair<uint32_t, int>> frequencyList(colorFrequency.begin(), colorFrequency.end());

    // Ordena la lista por frecuencia ascendente; en caso de empate, usa varios criterios adicionales
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

    // Crea un vector para almacenar los colores menos frecuentes hasta el límite de n
    std::vector<uint32_t> colorsToRemove;
    const std::size_t limit = static_cast<std::size_t>(std::min(n, static_cast<int>(frequencyList.size())));
    colorsToRemove.reserve(limit);

    // Añade los colores menos frecuentes al vector `colorsToRemove`
    for (std::size_t i = 0; i < limit; ++i) {
        colorsToRemove.push_back(frequencyList[i].first);
    }
    return colorsToRemove;
}

// Calcula la distancia euclidiana al cuadrado entre dos colores,
// lo cual permite comparar qué tan "cerca" están sin calcular la raíz cuadrada, lo que mejora el rendimiento
inline double distanciaEuclidianaCuadrada(uint32_t colorA, uint32_t colorB) {
    return std::pow(static_cast<int>((colorA >> SHIFT_RED) & MASK) - static_cast<int>((colorB >> SHIFT_RED) & MASK), 2) +
           std::pow(static_cast<int>((colorA >> SHIFT_GREEN) & MASK) - static_cast<int>((colorB >> SHIFT_GREEN) & MASK), 2) +
           std::pow(static_cast<int>(colorA & MASK) - static_cast<int>(colorB & MASK), 2);
}

// Calcula el índice de la cuadrícula para un color específico utilizando sus índices de componente RGB
inline int getGridIndex(const ColorIndices& indices) {
    return (indices.redIndex << GRID_SHIFT_RED) | (indices.greenIndex << GRID_SHIFT_GREEN) | indices.blueIndex;
}

// Calcula los índices de la cuadrícula para cada componente de color (rojo, verde, azul) dividiendo por el paso
inline void getGridIndices(uint32_t color, ColorIndices& indices) {
    indices.redIndex = static_cast<int>((color >> SHIFT_RED) & MASK) / GRID_STEP;
    indices.greenIndex = static_cast<int>((color >> SHIFT_GREEN) & MASK) / GRID_STEP;
    indices.blueIndex = static_cast<int>(color & MASK) / GRID_STEP;
}

// Procesa los vecinos de un color en la cuadrícula, buscando el color más cercano al color que queremos reemplazar
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

// Busca el color más cercano en los colores vecinos de la cuadrícula
uint32_t findClosestColorInNeighbors(const std::unordered_map<int, std::vector<uint32_t>>& grid,
                                     const ColorIndices& indices, uint32_t colorToRemove,
                                     bool& found) {
    // Inicializa los datos para la búsqueda de vecinos
    NeighborSearchData data{.colorToRemove = colorToRemove};

    // Bucle para explorar los vecinos en las dimensiones de rojo, verde y azul
    for (int deltaRed = -1; deltaRed <= 1; ++deltaRed) {
        const int neighborRedIndex = indices.redIndex + deltaRed;
        if (neighborRedIndex < 0 || neighborRedIndex >= GRID_SIZE) {
            continue; // Salta si está fuera de los límites
        }
        for (int deltaGreen = -1; deltaGreen <= 1; ++deltaGreen) {
            const int neighborGreenIndex = indices.greenIndex + deltaGreen;
            if (neighborGreenIndex < 0 || neighborGreenIndex >= GRID_SIZE) {
                continue; // Salta si está fuera de los límites
            }
            for (int deltaBlue = -1; deltaBlue <= 1; ++deltaBlue) {
                const int neighborBlueIndex = indices.blueIndex + deltaBlue;
                if (neighborBlueIndex < 0 || neighborBlueIndex >= GRID_SIZE) {
                    continue; // Salta si está fuera de los límites
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
    found = data.found; // Indica si se encontró un color
    return data.closestColor; // Devuelve el color más cercano
}

// Busca el color más cercano en la lista de frecuencias, ignorando los colores que deben eliminarse
uint32_t findClosestColorInFrequencyList(const std::vector<std::pair<uint32_t, int>>& frequencyList,
                                        const std::unordered_set<uint32_t>& colorsToRemoveSet,
                                        uint32_t colorToRemove) {
    uint32_t closestColor = 0;
    double minDistance = std::numeric_limits<double>::max();
    for (const auto& [candidateColor, _] : frequencyList) {
        // Omite el color si está en el conjunto de colores a eliminar
        if (colorsToRemoveSet.find(candidateColor) == colorsToRemoveSet.end()) {
            const double distance = distanciaEuclidianaCuadrada(colorToRemove, candidateColor);
            if (distance < minDistance) {
                minDistance = distance;
                closestColor = candidateColor;
            }
        }
    }
    return closestColor; // Devuelve el color más cercano encontrado
}

// Construye el mapa de reemplazos de color para cada color que debe ser eliminado
std::unordered_map<uint32_t, uint32_t> encontrarColoresReemplazo(
    const std::unordered_set<uint32_t>& colorsToRemoveSet,
    const std::vector<std::pair<uint32_t, int>>& frequencyList) {

    // Crea una cuadrícula para organizar colores por índices RGB
    std::unordered_map<int, std::vector<uint32_t>> grid;
    for (const auto& [color, _] : frequencyList) {
        if (colorsToRemoveSet.find(color) == colorsToRemoveSet.end()) {
            ColorIndices indices{};
            getGridIndices(color, indices); // Calcula los índices de cuadrícula para el color actual
            const int gridIndex = getGridIndex(indices); // Obtiene el índice de cuadrícula
            grid[gridIndex].push_back(color); // Añade el color a la cuadrícula
        }
    }

    std::unordered_map<uint32_t, uint32_t> replacementMap; // Mapa para almacenar colores de reemplazo
    for (const auto& colorToRemove : colorsToRemoveSet) {
        ColorIndices indices{};
        getGridIndices(colorToRemove, indices); // Obtiene los índices de cuadrícula del color a eliminar

        bool found = false;
        uint32_t closestColor = findClosestColorInNeighbors(grid, indices, colorToRemove, found);

        if (!found) {
            closestColor = findClosestColorInFrequencyList(frequencyList, colorsToRemoveSet, colorToRemove);
        }

        replacementMap[colorToRemove] = closestColor; // Guarda el reemplazo en el mapa
    }

    return replacementMap; // Devuelve el mapa con colores de reemplazo
}

// Reemplaza en la imagen cada color que se encuentra en el mapa de reemplazos
void reemplazarColores(PPMImage& image, const std::unordered_map<uint32_t, uint32_t>& replacementMap) {
    for (std::size_t i = 0; i < image.pixelData.size(); i += 3) {
        // Convierte los componentes RGB en un único valor de color
        const uint32_t color = (static_cast<uint32_t>(image.pixelData[i]) << SHIFT_RED) |
                               (static_cast<uint32_t>(image.pixelData[i + 1]) << SHIFT_GREEN) |
                               static_cast<uint32_t>(image.pixelData[i + 2]);

        auto iterator = replacementMap.find(color); // Busca si el color actual está en el mapa
        if (iterator != replacementMap.end()) {
            const uint32_t newColor = iterator->second; // Obtiene el color de reemplazo

            // Actualiza el color en la imagen con los nuevos valores RGB
            image.pixelData[i] = (newColor >> SHIFT_RED) & MASK;
            image.pixelData[i + 1] = (newColor >> SHIFT_GREEN) & MASK;
            image.pixelData[i + 2] = newColor & MASK;
        }
    }
}

} // namespace

// Función principal que calcula la frecuencia de colores y aplica reemplazos en la imagen
void cutfreq(PPMImage& image, int n) {
    if (n < 0) {
      throw std::invalid_argument("El número de colores a eliminar debe ser mayor o igual a 0.");
    }
    // Calcular la frecuencia de los colores
    auto colorFrequency = calcularFrecuenciaColores(image);

    if (n > static_cast<int>(colorFrequency.size())) {
      throw std::invalid_argument("El número de colores a eliminar excede el número de colores únicos en la imagen.");
  }


    // Obtiene los `n` colores menos frecuentes
    auto colorsToRemove = obtenerColoresMenosFrecuentes(colorFrequency, n);

    // Convierte el mapa de frecuencias en una lista para ordenarla por frecuencia
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

    // Convierte los colores a eliminar en un conjunto para facilitar la búsqueda
    const std::unordered_set<uint32_t> colorsToRemoveSet(colorsToRemove.begin(), colorsToRemove.end());

    // Encuentra el mapa de reemplazos para cada color a eliminar
    const auto replacementMap = encontrarColoresReemplazo(colorsToRemoveSet, frequencyList);

    // Reemplaza los colores en la imagen según el mapa de reemplazos
    reemplazarColores(image, replacementMap);
}
