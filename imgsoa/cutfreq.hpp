// cutfreq.hpp
#ifndef CUTFREQ_HPP_SOA
#define CUTFREQ_HPP_SOA

#include "../common/binario.hpp"
#include <limits>

// Supongamos que las constantes están definidas de la siguiente manera:
const int GRID_SHIFT_RED = 8;
const int GRID_SHIFT_GREEN = 4;
const int GRID_SIZE = 512; // Ajuste según el tamaño deseado
const int GRID_STEP = 16; // Ajuste según el tamaño deseado
const uint32_t SHIFT_RED = 16;
const uint32_t SHIFT_GREEN = 8;
const uint32_t MASK = 0xFF;

// Estructuras proporcionadas
// Estructuras para encapsular parámetros
struct GridIndices {
  int redIndex = 0;
  int greenIndex = 0;
  int blueIndex = 0;
};

struct ColorDistance {
  uint32_t closestColor = 0;
  double minDistance = 0;
  bool found = false;
};

void cutfreq(PPMImageSoA& image, int n);
#endif