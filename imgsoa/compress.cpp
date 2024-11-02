// File: imgsoa/compress.cpp

#include "compress.hpp"
#include "../common/binario.hpp"            // Para leerImagenPPMSoA y escribirImagenPPMSoA
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>

namespace common {

  int compress(const std::string& inputFilePath, const std::string& outputFilePath) {
    PPMImageSoA image;
    if (!leerImagenPPMSoA(inputFilePath, image)) {
      std::cerr << "Error al leer la imagen en formato SOA.\n";
      return -1;
    }

    std::unordered_map<uint32_t, uint32_t> colorTable;
    std::vector<uint32_t> uniqueColors;
    std::vector<uint32_t> pixelIndices(image.redChannel.size());

    for (size_t i = 0; i < image.redChannel.size(); ++i) {
      uint32_t color = (image.redChannel[i] << 16) | (image.greenChannel[i] << 8) | image.blueChannel[i];
      if (colorTable.find(color) == colorTable.end()) {
        uint32_t index = uniqueColors.size();
        colorTable[color] = index;
        uniqueColors.push_back(color);
      }
      pixelIndices[i] = colorTable[color];
    }

    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile) {
      std::cerr << "Error al abrir el archivo de salida.\n";
      return -1;
    }

    int bytesPerPixel = (uniqueColors.size() <= 256) ? 1 : (uniqueColors.size() <= 65536 ? 2 : 4);

    outputFile << "C6 " << image.width << " " << image.height << " " << image.maxValue << " "
               << uniqueColors.size() << "\n";

    for (const auto& color : uniqueColors) {
      outputFile.write(reinterpret_cast<const char*>(&color), (image.maxValue <= 255 ? 3 : 6));
    }

    for (const auto& index : pixelIndices) {
      outputFile.write(reinterpret_cast<const char*>(&index), bytesPerPixel);
    }

    return 0;
  }

}
