//
// Created by alba on 9/10/24.
//

#ifndef CUTFREQ_HPP
#define CUTFREQ_HPP

#include <vector>
#include <tuple>

// Estructura para representar la imagen en SOA (Structure of Arrays)
struct ImageSOA {
  std::vector<int> red_channel;
  std::vector<int> green_channel;
  std::vector<int> blue_channel;
};

// Declaraciones de las funciones para SOA
void cutfreq(ImageSOA& image, int n);
double euclideanDistance(const ImageSOA& image, size_t index1, size_t index2);
size_t findClosestColor(const ImageSOA& image, size_t colorIndex, const std::vector<size_t>& remainingIndices);
bool colorCompare(const ImageSOA& image, size_t index1, size_t index2);

#endif // CUTFREQ_HPP