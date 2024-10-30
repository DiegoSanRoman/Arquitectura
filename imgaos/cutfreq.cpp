// cutfreq.cpp
#include "../common/binario.hpp"
#include "cutfreq.hpp"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>

namespace {

// Calcular frecuencia de colores
std::unordered_map<uint32_t, int> calcularFrecuenciaColores(const PPMImage& image) {
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

// Encontrar colores de reemplazo
std::unordered_map<uint32_t, uint32_t> encontrarColoresReemplazo(const std::vector<std::pair<uint32_t, int>>& frequencyList, const std::unordered_set<uint32_t>& colorsToRemoveSet) {
    std::unordered_map<uint32_t, uint32_t> replacementMap;
    for (const auto& colorToRemove : colorsToRemoveSet) {
        double minDistance = std::numeric_limits<double>::max();
        uint32_t closestColor = 0;
        for (const auto& [candidateColor, freq] : frequencyList) {
            if (colorsToRemoveSet.find(candidateColor) == colorsToRemoveSet.end()) {
                const double distance = std::sqrt(
                    std::pow(static_cast<int>((colorToRemove >> SHIFT_RED) & MASK) - static_cast<int>((candidateColor >> SHIFT_RED) & MASK), 2) +
                    std::pow(static_cast<int>((colorToRemove >> SHIFT_GREEN) & MASK) - static_cast<int>((candidateColor >> SHIFT_GREEN) & MASK), 2) +
                    std::pow(static_cast<int>(colorToRemove & MASK) - static_cast<int>(candidateColor & MASK), 2)
                );
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

// Reemplazar colores en la imagen
void reemplazarColores(PPMImage& image, const std::unordered_map<uint32_t, uint32_t>& replacementMap) {
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

// Función principal cutfreq
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
    auto replacementMap = encontrarColoresReemplazo(frequencyList, colorsToRemoveSet);

    reemplazarColores(image, replacementMap);

}
