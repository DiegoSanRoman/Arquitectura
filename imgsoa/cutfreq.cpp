// cutfreq.cpp SOA

#include "../common/binario.hpp"
#include "cutfreq.hpp"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <limits>

namespace { // Definimos funciones internas que serán visibles solo dentro de este archivo.

// Funciones internas para trabajar con la imagen

// Calcular frecuencia de colores
// Esta función calcula cuántas veces aparece cada color en la imagen.
std::unordered_map<uint32_t, int> calcularFrecuenciaColores(const PPMImageSoA& image) {
    std::unordered_map<uint32_t, int> colorFrequency;
    // Recorremos cada pixel de la imagen
    for (std::size_t i = 0; i < image.redChannel.size(); ++i) {
        // Componemos un color combinando los valores de los canales rojo, verde y azul en un solo valor de 32 bits.
        const uint32_t color = (static_cast<uint32_t>(image.redChannel[i]) << SHIFT_RED) |
                               (static_cast<uint32_t>(image.greenChannel[i]) << SHIFT_GREEN) |
                               static_cast<uint32_t>(image.blueChannel[i]);
        // Aumentamos la frecuencia del color encontrado
        colorFrequency[color]++;
    }

    return colorFrequency; // Devolvemos la frecuencia de cada color.
}

// Obtener colores menos frecuentes
// Esta función devuelve los colores menos frecuentes, hasta un límite dado por "n".
std::vector<uint32_t> obtenerColoresMenosFrecuentes(const std::unordered_map<uint32_t, int>& colorFrequency, int n) {
    // Convertimos el mapa de frecuencias a una lista de pares para ordenar los colores según su frecuencia.
    std::vector<std::pair<uint32_t, int>> frequencyList(colorFrequency.begin(), colorFrequency.end());

    // Ordenamos la lista de colores por frecuencia de menor a mayor, y luego por canal para romper empates.
    std::ranges::sort(frequencyList, [](const auto& colorA, const auto& colorB) {
        if (colorA.second != colorB.second) {
            return colorA.second < colorB.second; // Ordenamos primero por frecuencia.
        }
        // Ordenar por azul, luego verde y luego rojo en caso de empate en frecuencia.
        if ((colorA.first & MASK) != (colorB.first & MASK)) {
            return (colorA.first & MASK) > (colorB.first & MASK);
        }
        if (((colorA.first >> SHIFT_GREEN) & MASK) != ((colorB.first >> SHIFT_GREEN) & MASK)) {
            return ((colorA.first >> SHIFT_GREEN) & MASK) > ((colorB.first >> SHIFT_GREEN) & MASK);
        }
        return ((colorA.first >> SHIFT_RED) & MASK) > ((colorB.first >> SHIFT_RED) & MASK);
    });

    // Tomamos los primeros "n" colores menos frecuentes.
    std::vector<uint32_t> colorsToRemove;
    const std::size_t limit = static_cast<std::size_t>(std::min(n, static_cast<int>(frequencyList.size())));
    colorsToRemove.reserve(limit); // Reservamos espacio para mejorar el rendimiento.
    for (std::size_t i = 0; i < limit; ++i) {
        colorsToRemove.push_back(frequencyList[i].first); // Guardamos los colores a eliminar.
    }

    return colorsToRemove; // Devolvemos los colores que serán eliminados.
}

// Encontrar colores de reemplazo
// Esta función encuentra un color para reemplazar cada uno de los colores menos frecuentes eliminados.
std::unordered_map<uint32_t, uint32_t> encontrarColoresReemplazo(const std::unordered_set<uint32_t>& colorsToRemoveSet,
                                                                 const std::vector<std::pair<uint32_t, int>>& frequencyList) {
    std::unordered_map<uint32_t, uint32_t> replacementMap;

    // Filtramos los colores candidatos para reemplazar (solo aquellos que no están en la lista de eliminados).
    std::vector<uint32_t> candidateColors;
    for (const auto& [candidateColor, _] : frequencyList) {
        if (colorsToRemoveSet.find(candidateColor) == colorsToRemoveSet.end()) {
            candidateColors.push_back(candidateColor); // Guardamos solo los colores que no han sido eliminados.
        }
    }

    // Buscamos el color más cercano de los candidatos para cada color eliminado.
    for (const auto& colorToRemove : colorsToRemoveSet) {
        uint32_t closestColor = 0;
        double minDistance = std::numeric_limits<double>::max(); // Inicializamos la distancia más corta como un valor muy alto.

        // Recorremos los candidatos y calculamos la distancia euclidiana en el espacio RGB.
        for (const auto& candidateColor : candidateColors) {
            const int redDiff = static_cast<int>((colorToRemove >> SHIFT_RED) & MASK) - static_cast<int>((candidateColor >> SHIFT_RED) & MASK);
            const int greenDiff = static_cast<int>((colorToRemove >> SHIFT_GREEN) & MASK) - static_cast<int>((candidateColor >> SHIFT_GREEN) & MASK);
            const int blueDiff = static_cast<int>(colorToRemove & MASK) - static_cast<int>(candidateColor & MASK);
            const double distance = ((redDiff * redDiff) + (greenDiff * greenDiff) + (blueDiff * blueDiff));

            // Si encontramos una distancia menor, actualizamos el color más cercano.
            if (distance < minDistance) {
                minDistance = distance;
                closestColor = candidateColor;
            }
        }
        // Guardamos en el mapa de reemplazo el color que debe sustituir al eliminado.
        replacementMap[colorToRemove] = closestColor;
    }
    return replacementMap; // Devolvemos el mapa de reemplazo de colores.
}

// Reemplazar colores en la imagen
// Esta función reemplaza en la imagen todos los colores eliminados por sus respectivos reemplazos.
void reemplazarColores(PPMImageSoA& image, const std::unordered_map<uint32_t, uint32_t>& replacementMap) {
    for (std::size_t i = 0; i < image.redChannel.size(); ++i) {
        // Reconstruimos el color original del pixel a partir de sus canales RGB.
        const uint32_t color = (static_cast<uint32_t>(image.redChannel[i]) << SHIFT_RED) |
                               (static_cast<uint32_t>(image.greenChannel[i]) << SHIFT_GREEN) |
                               static_cast<uint32_t>(image.blueChannel[i]);
        // Buscamos si el color actual necesita ser reemplazado.
        auto iterator = replacementMap.find(color);
        if (iterator != replacementMap.end()) {
            const uint32_t newColor = iterator->second;

            // Reemplazamos los valores de los canales RGB con el nuevo color.
            image.redChannel[i] = (newColor >> SHIFT_RED) & MASK;
            image.greenChannel[i] = (newColor >> SHIFT_GREEN) & MASK;
            image.blueChannel[i] = newColor & MASK;
        }
    }
}

} // namespace

// Uso en la función cutfreq
// Esta es la función principal que ejecuta los pasos para reducir los colores menos frecuentes en la imagen.
void cutfreq(PPMImageSoA& image, int n) {
    // Calculamos la frecuencia de todos los colores en la imagen.
    auto colorFrequency = calcularFrecuenciaColores(image);

    // Si todos los colores son idénticos, no hacemos nada
    if (colorFrequency.size() == 1) {
      return; // No hay nada que reemplazar
    }
    // Obtenemos los colores menos frecuentes que queremos eliminar.
    auto colorsToRemove = obtenerColoresMenosFrecuentes(colorFrequency, n);

    // Si no hay colores a eliminar (n es mayor al número de colores únicos), no hacemos nada
    if (colorsToRemove.empty()) {
      return;
    }

    // Creamos una lista de frecuencias para todos los colores.
    std::vector<std::pair<uint32_t, int>> frequencyList(colorFrequency.begin(), colorFrequency.end());
    // Ordenamos la lista de colores de menor a mayor frecuencia.
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

    // Convertimos la lista de colores a eliminar en un conjunto para una búsqueda rápida.
    const std::unordered_set<uint32_t> colorsToRemoveSet(colorsToRemove.begin(), colorsToRemove.end());
    // Encontramos los colores de reemplazo para los colores que serán eliminados.
    const auto replacementMap = encontrarColoresReemplazo(colorsToRemoveSet, frequencyList);

    // Reemplazamos los colores menos frecuentes en la imagen por sus respectivos reemplazos.
    reemplazarColores(image, replacementMap);
} // Fin de la función cutfreq.
