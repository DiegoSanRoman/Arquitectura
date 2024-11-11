//CUTFREQ SOA//
#include "../common/binario.hpp"
#include "cutfreq.hpp"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <limits>

namespace {

// Calcular frecuencia de colores
std::unordered_map<uint32_t, int> calcularFrecuenciaColores(const PPMImageSoA& image) {
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
std::unordered_map<uint32_t, uint32_t> encontrarColoresReemplazo(const std::unordered_set<uint32_t>& colorsToRemoveSet,
                                                                 const std::vector<std::pair<uint32_t, int>>& frequencyList) {
    std::unordered_map<uint32_t, uint32_t> replacementMap;

    // Filtrar colores candidatos para reemplazo (solo colores no eliminados)
    std::vector<uint32_t> candidateColors;
    for (const auto& [candidateColor, _] : frequencyList) {
        if (colorsToRemoveSet.find(candidateColor) == colorsToRemoveSet.end()) {
            candidateColors.push_back(candidateColor);
        }
    }

    // Encontrar el color más cercano usando los candidatos filtrados
    for (const auto& colorToRemove : colorsToRemoveSet) {
        uint32_t closestColor = 0;
        double minDistance = std::numeric_limits<double>::max();

        for (const auto& candidateColor : candidateColors) {
            const int redDiff = static_cast<int>((colorToRemove >> SHIFT_RED) & MASK) - static_cast<int>((candidateColor >> SHIFT_RED) & MASK);
            const int greenDiff = static_cast<int>((colorToRemove >> SHIFT_GREEN) & MASK) - static_cast<int>((candidateColor >> SHIFT_GREEN) & MASK);
            const int blueDiff = static_cast<int>(colorToRemove & MASK) - static_cast<int>(candidateColor & MASK);
            const double distance = ((redDiff * redDiff) + (greenDiff * greenDiff) + (blueDiff * blueDiff));

            if (distance < minDistance) {
                minDistance = distance;
                closestColor = candidateColor;
            }
        }
        replacementMap[colorToRemove] = closestColor;
    }
    return replacementMap;
}

// Reemplazar colores en la imagen
void reemplazarColores(PPMImageSoA& image, const std::unordered_map<uint32_t, uint32_t>& replacementMap) {
    for (std::size_t i = 0; i < image.redChannel.size(); ++i) {
        const uint32_t color = (static_cast<uint32_t>(image.redChannel[i]) << SHIFT_RED) |
                               (static_cast<uint32_t>(image.greenChannel[i]) << SHIFT_GREEN) |
                               static_cast<uint32_t>(image.blueChannel[i]);
        auto iterator = replacementMap.find(color);
        if (iterator != replacementMap.end()) {
            const uint32_t newColor = iterator->second;

            // Realizar el reemplazo de color en la imagen
            image.redChannel[i] = (newColor >> SHIFT_RED) & MASK;
            image.greenChannel[i] = (newColor >> SHIFT_GREEN) & MASK;
            image.blueChannel[i] = newColor & MASK;
        }
    }
}

} // namespace

// Uso en la función cutfreq
void cutfreq(PPMImageSoA& image, int n) {
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
    const auto replacementMap = encontrarColoresReemplazo(colorsToRemoveSet, frequencyList);

    reemplazarColores(image, replacementMap);
}
