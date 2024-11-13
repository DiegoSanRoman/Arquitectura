// File: common/binario.cpp
#include "binario.hpp"

#include <bit>
#include <fstream>
#include <iostream>
#include <span>
#include <vector>
#include <ostream>
#include <istream>
#include <cstring>

namespace {
    constexpr std::streamsize MAX_HEADER_SIZE = 256;
    constexpr int MAX_8BIT_VALUE = 255;
    constexpr std::size_t COMPONENTS_PER_PIXEL = 3U;
    constexpr int BYTE_COLOR_LIMIT = 256;
    constexpr int SHORT_COLOR_LIMIT = 65536;

    bool leerEncabezadoPPM(std::ifstream& file, PPMImage& image) {
        std::string magicNumber;
        file >> magicNumber;
        if (magicNumber != "P6") {
            std::cerr << "Formato incorrecto: se esperaba 'P6'.\n";
            return false;
        }

        file >> image.width >> image.height >> image.maxValue;
        file.ignore(MAX_HEADER_SIZE, '\n');
        return true;
    }

    bool leerEncabezadoPPM(std::ifstream& file, PPMImageSoA& image) {
        std::string magicNumber;
        file >> magicNumber;
        if (magicNumber != "P6") {
            std::cerr << "Formato incorrecto: se esperaba 'P6'.\n";
            return false;
        }

        file >> image.width >> image.height >> image.maxValue;
        file.ignore(MAX_HEADER_SIZE, '\n');
        return true;
    }

    void swapBytes(std::vector<unsigned char>& data) {
        for (std::size_t i = 0; i < data.size(); i += 2) {
            std::swap(data[i], data[i + 1]);
        }
    }

    std::size_t calcularTotalBytes(int width, int height, int bytesPerComponent) {
        return static_cast<std::size_t>(width) *
               static_cast<std::size_t>(height) *
               COMPONENTS_PER_PIXEL *
               static_cast<std::size_t>(bytesPerComponent);
    }

    bool leerDatosPixeles(std::ifstream& file, PPMImage& image, int bytesPerComponent) {
        const std::size_t totalBytes = calcularTotalBytes(image.width, image.height, bytesPerComponent);
        image.pixelData.resize(totalBytes);

        if (!file.read(std::bit_cast<char*>(image.pixelData.data()),
                      static_cast<std::streamsize>(totalBytes))) {
            std::cerr << "Error al leer los datos de la imagen.\n";
            return false;
        }

        if (bytesPerComponent == 2) {
            swapBytes(image.pixelData);
        }
        return true;
    }
  // Leer datos de píxeles en formato SOA
  bool leerDatosPixeles(std::ifstream& file, PPMImageSoA& image, int bytesPerComponent) {
      const std::size_t totalPixels = static_cast<std::size_t>(image.width) * static_cast<std::size_t>(image.height);

      // Redimensionar los canales RGB
      image.redChannel.resize(totalPixels);
      image.greenChannel.resize(totalPixels);
      image.blueChannel.resize(totalPixels);

      // Crear un búfer intermedio para leer cada componente de color
      std::vector<char> buffer(totalPixels * COMPONENTS_PER_PIXEL);

      if (!file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()))) {
        std::cerr << "Error al leer los datos de la imagen.\n";
        return false;
      }

      // Copiar los datos desde el búfer intermedio a los canales RGB
      for (std::size_t i = 0; i < totalPixels; ++i) {
        image.redChannel[i] = static_cast<uint8_t>(buffer[i * COMPONENTS_PER_PIXEL]);
        image.greenChannel[i] = static_cast<uint8_t>(buffer[(i * COMPONENTS_PER_PIXEL) + 1]);
        image.blueChannel[i] = static_cast<uint8_t>(buffer[(i * COMPONENTS_PER_PIXEL) + 2]);
      }

      // Si es de 16 bits, hacer el intercambio de bytes
      if (bytesPerComponent == 2) {
        swapBytes(image.redChannel);
        swapBytes(image.greenChannel);
        swapBytes(image.blueChannel);
      }

      return true;
    }


    bool escribirEncabezadoPPM(std::ofstream& file, const PPMImage& image) {
        file << "P6\n" << image.width << " " << image.height << "\n" << image.maxValue << "\n";
        return file.good();
    }

    bool escribirEncabezadoPPM(std::ofstream& file, const PPMImageSoA& image) {
        file << "P6\n" << image.width << " " << image.height << "\n" << image.maxValue << "\n";
        return file.good();
    }

    bool escribirDatosPixeles(std::ofstream& file, const PPMImage& image, int bytesPerComponent) {
        const std::size_t totalBytes = calcularTotalBytes(image.width, image.height, bytesPerComponent);

        if (bytesPerComponent == 2) {
            std::vector<unsigned char> tempData = image.pixelData;
            swapBytes(tempData);
            return file.write(std::bit_cast<const char*>(tempData.data()),
                            static_cast<std::streamsize>(totalBytes)).good();
        }

        return file.write(std::bit_cast<const char*>(image.pixelData.data()),
                         static_cast<std::streamsize>(totalBytes)).good();
    }

  // Escribir datos de píxeles en formato SOA sin reinterpret_cast
  bool escribirDatosPixeles(std::ofstream& file, const PPMImageSoA& image, int bytesPerComponent) {
      const std::size_t totalPixels = static_cast<std::size_t>(image.width) * static_cast<std::size_t>(image.height);
      const std::size_t totalBytesPerChannel = totalPixels * static_cast<std::size_t>(bytesPerComponent);

      // Crear un búfer intermedio para almacenar los datos RGB en formato binario
      std::vector<char> buffer(totalBytesPerChannel * COMPONENTS_PER_PIXEL * 2);

      // Copiar los datos de los canales RGB al búfer intermedio
      for (std::size_t i = 0; i < totalBytesPerChannel; ++i) {
        buffer[i * COMPONENTS_PER_PIXEL] = static_cast<char>(image.redChannel[i]);
        buffer[(i * COMPONENTS_PER_PIXEL) + 1] = static_cast<char>(image.greenChannel[i]);
        buffer[(i * COMPONENTS_PER_PIXEL) + 2] = static_cast<char>(image.blueChannel[i]);
      }

      // Si es de 16 bits, hacer el intercambio de bytes en el búfer antes de escribir
      if (bytesPerComponent == 2) {
        std::vector<unsigned char> tempRed = image.redChannel;
        std::vector<unsigned char> tempGreen = image.greenChannel;
        std::vector<unsigned char> tempBlue = image.blueChannel;
        swapBytes(tempRed);
        swapBytes(tempGreen);
        swapBytes(tempBlue);

        for (std::size_t i = 0; i < totalBytesPerChannel; ++i) {
          buffer[i * COMPONENTS_PER_PIXEL] = static_cast<char>(tempRed[i]);
          buffer[(i * COMPONENTS_PER_PIXEL) + 1] = static_cast<char>(tempGreen[i]);
          buffer[(i * COMPONENTS_PER_PIXEL) + 2] = static_cast<char>(tempBlue[i]);
        }
      }

      // Escribir el búfer intermedio en el archivo
      if (!file.write(buffer.data(), static_cast<std::streamsize>(buffer.size()))) {
        std::cerr << "Error al escribir los datos de la imagen.\n";
        return false;
      }

      return true;
    }

  bool leerEncabezadoCPPM(std::ifstream& file, PPMImage& image, size_t& uniqueColorCount) {
      std::string magicNumber;
      file >> magicNumber;
      if (magicNumber != "C6") {
          std::cerr << "Formato incorrecto: se esperaba 'C6'.\n";
          return false;
      }

      file >> image.width >> image.height >> image.maxValue >> uniqueColorCount;
      file.ignore(MAX_HEADER_SIZE, '\n');
      return true;
  }

  bool leerDatosTablaColores(std::ifstream& file, PPMImage& image, size_t uniqueColorCount) {
      const size_t colorSize = (image.maxValue <= MAX_8BIT_VALUE) ? 3U : 6U;
      image.pixelData.resize(uniqueColorCount * colorSize);

      // Leer datos binarios de colores usando std::memcpy en lugar de reinterpret_cast
      std::vector<char> buffer(uniqueColorCount * colorSize);
      if (!file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()))) {
          std::cerr << "Error al leer la tabla de colores.\n";
          return false;
      }
      std::memcpy(image.pixelData.data(), buffer.data(), buffer.size());
      return true;
  }

  bool leerIndicesPixeles(std::ifstream& file, PPMImage& image, size_t uniqueColorCount) {
    int bytesPerPixel = 0;
    if (uniqueColorCount <= BYTE_COLOR_LIMIT) {
      bytesPerPixel = 1;
    } else if (uniqueColorCount <= SHORT_COLOR_LIMIT) {
      bytesPerPixel = 2;
    } else {
      bytesPerPixel = 4;
    }

      const std::size_t totalPixels = static_cast<std::size_t>(image.width) * static_cast<std::size_t>(image.height);
      std::vector<uint32_t> pixelIndices(totalPixels);

      std::vector<char> buffer(static_cast<std::size_t>(bytesPerPixel));
      for (std::size_t i = 0; i < totalPixels; ++i) {
          if (!file.read(buffer.data(), bytesPerPixel)) {
              std::cerr << "Error al leer índices de píxeles.\n";
              return false;
          }
      std::memcpy(&pixelIndices[i], buffer.data(), static_cast<std::size_t>(bytesPerPixel));
      }
      return true;
  }

    // Función genérica para escribir un solo valor en binario
    template <typename T>
    void write_binary(std::ostream& output, const T& value) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        output.write(reinterpret_cast<const char*>(&value), sizeof(value));
      }

    // Función genérica para escribir un array de valores en binario
    template <typename T>
    void write_binary_array(std::ostream& output, const std::vector<T>& values) {
        for (const auto& value : values) {
          write_binary(output, value);
        }
      }
}

bool leerImagenPPM(const std::string& filePath, PPMImage& image) {
    try {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            std::cerr << "Error al abrir el archivo para lectura: " << filePath << '\n';
            return false;
        }

        if (!leerEncabezadoPPM(file, image)) {
            return false;
        }

        const int bytesPerComponent = (image.maxValue <= MAX_8BIT_VALUE) ? 1 : 2;
        return leerDatosPixeles(file, image, bytesPerComponent);

    } catch (const std::exception& e) {
        std::cerr << "Error al leer imagen PPM: " << e.what() << '\n';
        return false;
    }
}

bool escribirImagenPPM(const std::string& filePath, const PPMImage& image) {
    try {
        std::ofstream file(filePath, std::ios::binary);
        if (!file) {
            std::cerr << "Error al abrir el archivo para escritura: " << filePath << '\n';
            return false;
        }

        if (!escribirEncabezadoPPM(file, image)) {
            std::cerr << "Error al escribir el encabezado de la imagen.\n";
            return false;
        }

        const int bytesPerComponent = (image.maxValue <= MAX_8BIT_VALUE) ? 1 : 2;
        if (!escribirDatosPixeles(file, image, bytesPerComponent)) {
            std::cerr << "Error al escribir los datos de la imagen.\n";
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error al escribir imagen PPM: " << e.what() << '\n';
        return false;
    }
}

bool leerImagenPPMSoA(const std::string& filePath, PPMImageSoA& image) {
    try {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            std::cerr << "Error al abrir el archivo para lectura: " << filePath << '\n';
            return false;
        }

        if (!leerEncabezadoPPM(file, image)) {
            return false;
        }

        const int bytesPerComponent = (image.maxValue <= MAX_8BIT_VALUE) ? 1 : 2;
        return leerDatosPixeles(file, image, bytesPerComponent);

    } catch (const std::exception& e) {
        std::cerr << "Error al leer imagen PPM: " << e.what() << '\n';
        return false;
    }
}

bool escribirImagenPPMSoA(const std::string& filePath, const PPMImageSoA& image) {
    try {
        std::ofstream file(filePath, std::ios::binary);
        if (!file) {
            std::cerr << "Error al abrir el archivo para escritura: " << filePath << '\n';
            return false;
        }

        if (!escribirEncabezadoPPM(file, image)) {
            std::cerr << "Error al escribir el encabezado de la imagen.\n";
            return false;
        }

        const int bytesPerComponent = (image.maxValue <= MAX_8BIT_VALUE) ? 1 : 2;
        if (!escribirDatosPixeles(file, image, bytesPerComponent)) {
            std::cerr << "Error al escribir los datos de la imagen.\n";
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error al escribir imagen PPM: " << e.what() << '\n';
        return false;
    }
}

bool leerImagenCPPM(const std::string& filePath, PPMImage& image) {
  try {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
      std::cerr << "Error al abrir el archivo para lectura: " << filePath << '\n';
      return false;
    }

    size_t uniqueColorCount = 0;
    if (!leerEncabezadoCPPM(file, image, uniqueColorCount)) {
      return false;
    }

    if (!leerDatosTablaColores(file, image, uniqueColorCount)) {
      return false;
    }

    return leerIndicesPixeles(file, image, uniqueColorCount);

  } catch (const std::exception& e) {
    std::cerr << "Error al leer imagen CPPM: " << e.what() << '\n';
    return false;
  }
}