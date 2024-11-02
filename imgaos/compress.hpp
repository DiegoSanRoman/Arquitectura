// File: imgaos/compress.hpp

#ifndef COMPRESS_HPP
#define COMPRESS_HPP

#include <string>

namespace common {
  struct CompressionPaths {
    std::string inputImagePath;
    std::string outputImagePath;
  };

  // Declaración de la función compress usando CompressionPaths
  int compress(const CompressionPaths& paths);
}

#endif // COMPRESS_HPP
