//
// Created by alba on 9/10/24.
//

#ifndef CUTFREQ_HPP
#define CUTFREQ_HPP

#include <vector>

// Definir la estructura Pixel si no está definida en otro lugar
struct Pixel {
  int r, g, b;
};

// Declaración de la función cutfreq
void cutfreq(std::vector<Pixel>& image, int n);

#endif
