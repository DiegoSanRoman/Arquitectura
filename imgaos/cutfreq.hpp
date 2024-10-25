// cutfreq.hpp
#ifndef CUTFREQ_HPP
#define CUTFREQ_HPP

#include "../common/binario.hpp"


constexpr int SHIFT_RED = 16;
constexpr int SHIFT_GREEN = 8;
constexpr int MASK = 0xFF;

void cutfreq(PPMImage& image, int n);

#endif // CUTFREQ_HPP