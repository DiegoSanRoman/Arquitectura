// cutfreq.hpp
#ifndef CUTFREQ_HPP
#define CUTFREQ_HPP

#include "../common/binario.hpp"
#include <limits>

// Supongamos que las constantes están definidas de la siguiente manera:
const int GRID_SHIFT_RED = 8;
const int GRID_SHIFT_GREEN = 4;
const int GRID_SIZE = 16; // Ajuste según el tamaño deseado
const int GRID_STEP = 16; // Ajuste según el tamaño deseado
const uint32_t SHIFT_RED = 16;
const uint32_t SHIFT_GREEN = 8;
const uint32_t MASK = 0xFF;

// Estructuras proporcionadas
struct ColorIndices {
  int redIndex = 0;
  int greenIndex = 0;
  int blueIndex = 0;
};

struct NeighborSearchData {
  uint32_t colorToRemove = 0;
  uint32_t closestColor = 0;
  double minDistance = std::numeric_limits<double>::max();
  bool found = false;
};

struct Color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};


void cutfreq(PPMImageSoA& image, int n);
#endif