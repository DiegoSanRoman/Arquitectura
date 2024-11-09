// cutfreq.hpp
#ifndef CUTFREQ_HPP
#define CUTFREQ_HPP

#include "../common/binario.hpp"
#include <limits>

// Estructura para encapsular los índices de color
struct ColorIndices {
  int redIndex = 0;
  int greenIndex = 0;
  int blueIndex = 0;
};

// Estructura para contener los datos necesarios en processNeighbor
struct NeighborSearchData {
  uint32_t colorToRemove = 0;
  uint32_t closestColor = 0;
  double minDistance = std::numeric_limits<double>::max();
  bool found = false;
};

const int GRID_SHIFT_RED = 8;
const int GRID_SHIFT_GREEN = 4;
const int GRID_SIZE = 512; // Debes definir el valor adecuado
const int GRID_STEP = 16; // Debes definir el valor adecuado
const uint32_t SHIFT_RED = 16; // Debes definir el valor adecuado
const uint32_t SHIFT_GREEN = 8; // Debes definir el valor adecuado
const uint32_t MASK = 0xFF; // Máscara para extraer componentes de color

void cutfreq(PPMImage& image, int n);

#endif // CUTFREQ_HPP