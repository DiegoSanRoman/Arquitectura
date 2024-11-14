// cutfreq.hpp
#ifndef CUTFREQ_HPP_SOA
#define CUTFREQ_HPP_SOA

#include "../common/binario.hpp"


const uint32_t SHIFT_RED = 16;
const uint32_t SHIFT_GREEN = 8;
const uint32_t MASK = 0xFF;


void cutfreq(PPMImageSoA& image, int n);
#endif