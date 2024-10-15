//
// Created by alba on 9/10/24.
//
#ifndef CUTFREQ_HPP
#define CUTFREQ_HPP

#include <vector>
#include <unordered_map>
#include <cstddef>

namespace imgaos {

  struct Color {
    int r;
    int g;
    int b;

    bool operator==(const Color& other) const {
      return r == other.r && g == other.g && b == other.b;
    }
  };

  struct ColorHash {
    std::size_t operator()(const Color& color) const {
      return std::hash<int>()(color.r) ^ (std::hash<int>()(color.g) << 1) ^ (std::hash<int>()(color.b) << 2);
    }
  };

  struct ImageAOS {
    std::vector<Color> pixels;
  };

  void cutfreq(ImageAOS& image, int n);

} // namespace imgaos

#endif // CUTFREQ_HPP
