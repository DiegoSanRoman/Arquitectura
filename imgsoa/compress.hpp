// File: imgsoa/compress.hpp

#ifndef COMPRESS_HPP
#define COMPRESS_HPP

#include <string>

namespace common {
  struct CompressionPaths {
    std::string inputImagePath;
    std::string outputImagePath;
  };

  int compress(const CompressionPaths& paths);
}

#endif // COMPRESS_HPP
