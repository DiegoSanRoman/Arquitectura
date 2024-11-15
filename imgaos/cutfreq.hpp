// cutfreq.hpp
#ifndef CUTFREQ_HPP
#define CUTFREQ_HPP

#include "../common/binario.hpp"
#include <limits>


const uint32_t SHIFT_RED = 16; // Debes definir el valor adecuado
const uint32_t SHIFT_GREEN = 8; // Debes definir el valor adecuado
const uint32_t MASK = 0xFF; // Máscara para extraer componentes de color

void cutfreq(PPMImage& image, int n);

#endif // CUTFREQ_HPP