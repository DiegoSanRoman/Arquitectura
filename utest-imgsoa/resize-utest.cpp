#include <gtest/gtest.h>
#include "../imgsoa/resize.hpp"
#include "../common/binario.hpp"
#include <vector>
#include <stdexcept>
#include <fstream>  // Para la definición de std::ofstream

// Definición de constantes para evitar el uso de números mágicos
namespace {
  constexpr int MAX_PIXEL_VALUE = 255;
  constexpr int CONST_256 = 256;
  constexpr int CONST_128 = 128;
  constexpr int CONST_1000 = 1000;
  constexpr int CONST_10 = 10;
  constexpr int CONST_50 = 50;
}
// Estructura para almacenar las dimensiones de la imagen y el valor máximo del píxel
struct ImageParams {
  int width;
  int height;
  int maxValue;
};

// Clase de prueba para la función de reescalado SoA
class ResizeSOATest : public ::testing::Test {
protected:
  // Función de ayuda para crear una imagen PPM de prueba con datos específicos
  static PPMImage createTestImage(const ImageParams& params, const std::vector<unsigned char>& data) {
    PPMImage image;
    image.width = params.width;
    image.height = params.height;
    image.maxValue = params.maxValue;
    image.pixelData = data;
    return image;
  }
  // Función de ayuda para crear una imagen de prueba predeterminada
  static PPMImage createDefaultTestImage() {
      return createTestImage({.width=4, .height=4, .maxValue=MAX_PIXEL_VALUE}, {
          MAX_PIXEL_VALUE, 0, 0,         0, MAX_PIXEL_VALUE, 0,         0, 0, MAX_PIXEL_VALUE,       MAX_PIXEL_VALUE, MAX_PIXEL_VALUE, 0,
          MAX_PIXEL_VALUE, 0, MAX_PIXEL_VALUE, 0, MAX_PIXEL_VALUE, MAX_PIXEL_VALUE, MAX_PIXEL_VALUE, MAX_PIXEL_VALUE, MAX_PIXEL_VALUE, 0, 0, 0,
          CONST_128, CONST_128, CONST_128,  CONST_128 / 2, CONST_128 / 2, CONST_128 / 2,
          CONST_128 / 4, CONST_128 / 4, CONST_128 / 4,  CONST_128, CONST_128, CONST_128,
          0, 0, 0,                MAX_PIXEL_VALUE, MAX_PIXEL_VALUE, MAX_PIXEL_VALUE, CONST_128 , CONST_128, CONST_128,
          CONST_128 / 4, CONST_128 / 4, CONST_128 / 4
      });
    }

    [[nodiscard]] bool writeTestImageToDisk() const {
        return escribirImagenPPM(testInputPath, testImage);
    }

    [[nodiscard]] const PPMImage& getTestImage() const { return testImage; }
    void setTestImage(const PPMImage& image) { testImage = image; }
    [[nodiscard]] const std::string& getInputPath() const { return testInputPath; }
    [[nodiscard]] const std::string& getOutputPath() const { return testOutputPath; }

  private:
  PPMImage testImage;
  const std::string testInputPath{"test_input.ppm"};
  const std::string testOutputPath{"test_output.ppm"};
};

// TESTS DE VALIDACIÓN DE PARÁMETROS
TEST_F(ResizeSOATest, ThrowsOnInvalidResizeValue) {
    ASSERT_TRUE(writeTestImageToDisk());

    // Prueba con un valor negativo para el ancho
    EXPECT_THROW(
        performResizeOperation(getInputPath(), getOutputPath(), -1, 100),
        std::invalid_argument
    );

    // Prueba con un valor negativo para el alto
    EXPECT_THROW(
        performResizeOperation(getInputPath(), getOutputPath(), 100, -1),
        std::invalid_argument
    );

    // Prueba con un valor cero para el ancho
    EXPECT_THROW(
        performResizeOperation(getInputPath(), getOutputPath(), 0, 100),
        std::invalid_argument
    );

    // Prueba con un valor cero para el alto
    EXPECT_THROW(
        performResizeOperation(getInputPath(), getOutputPath(), 100, 0),
        std::invalid_argument
    );
}

// TESTS DE CASOS DE BORDE
// Redimensionar a un solo píxel (1x1)
TEST_F(ResizeSOATest, ResizeToSinglePixel) {
  const PPMImage inputImage = createDefaultTestImage();
    escribirImagenPPM("input_4x4.ppm", inputImage);

    performResizeOperation("input_4x4.ppm", "output_1x1.ppm", 1, 1);
    PPMImage outputImage;
    ASSERT_TRUE(leerImagenPPM("output_1x1.ppm", outputImage));

    EXPECT_EQ(outputImage.width, 1);
    EXPECT_EQ(outputImage.height, 1);
}

// Redimensionar a la misma dimensión (identidad)
TEST_F(ResizeSOATest, ResizeToSameDimensions) {
  const PPMImage inputImage = createDefaultTestImage();
    escribirImagenPPM("input_4x4.ppm", inputImage);

    performResizeOperation("input_4x4.ppm", "output_same.ppm", 4, 4);
    PPMImage outputImage;
    ASSERT_TRUE(leerImagenPPM("output_same.ppm", outputImage));

    EXPECT_EQ(outputImage.width, 4);
    EXPECT_EQ(outputImage.height, 4);
    EXPECT_EQ(inputImage.pixelData, outputImage.pixelData);
}

// Redimensionar una imagen de 1x1 a un tamaño mayor
TEST_F(ResizeSOATest, ResizeSinglePixelToLarger) {
  const PPMImage inputImage = createTestImage({.width=1, .height=1, .maxValue=MAX_PIXEL_VALUE}, {
      CONST_128, CONST_128, CONST_128  // Un solo píxel gris
  });
    escribirImagenPPM("input_1x1.ppm", inputImage);

    performResizeOperation("input_1x1.ppm", "output_large.ppm", 4, 4);
    PPMImage outputImage;
    ASSERT_TRUE(leerImagenPPM("output_large.ppm", outputImage));

    EXPECT_EQ(outputImage.width, 4);
    EXPECT_EQ(outputImage.height, 4);

    for (size_t i = 0; i < outputImage.pixelData.size(); i += 3) {
        EXPECT_EQ(outputImage.pixelData[i], 128);
        EXPECT_EQ(outputImage.pixelData[i + 1], 128);
        EXPECT_EQ(outputImage.pixelData[i + 2], 128);
    }
}

// TEST DE VALIDACIÓN DE FORMATO DE IMAGEN
TEST_F(ResizeSOATest, InvalidPPMFormat) {
    std::ofstream outFile("invalid.ppm");
    outFile << "P3\n2 2\n255\n255 0 0 0 255 0 0 0 255 255 255 0";
    outFile.close();

    EXPECT_THROW({
        performResizeOperation("invalid.ppm", "output_invalid.ppm", 2, 2);
    }, std::runtime_error);
}

// TESTS DE ESCENARIOS GRANDES
TEST_F(ResizeSOATest, ResizeLargeImageToSmall) {
  // Crear una imagen grande de 1000x1000 píxeles con colores alternantes
  std::vector<unsigned char> largeImageData(static_cast<long>(CONST_1000 * CONST_1000 * 3), 0);
  for (size_t i = 0; i < largeImageData.size(); i += 3) {
    largeImageData[i] = static_cast<unsigned char>((i / 3) % CONST_256);       // Rojo
    largeImageData[i + 1] = static_cast<unsigned char>((i / 3 / CONST_256) % CONST_256); // Verde
    largeImageData[i + 2] = static_cast<unsigned char>(MAX_PIXEL_VALUE - ((i / 3) % CONST_256)); // Azul
  }
  const PPMImage largeImage = createTestImage({.width=CONST_1000, .height=CONST_1000, .maxValue=MAX_PIXEL_VALUE}, largeImageData);
    escribirImagenPPM("large_input.ppm", largeImage);

    performResizeOperation("large_input.ppm", "small_output.ppm", CONST_50, CONST_50);
    PPMImage smallOutputImage;
    ASSERT_TRUE(leerImagenPPM("small_output.ppm", smallOutputImage));

    EXPECT_EQ(smallOutputImage.width, 50);
    EXPECT_EQ(smallOutputImage.height, 50);
}

TEST_F(ResizeSOATest, ResizeSmallImageToLarge) {
  const std::vector<unsigned char> smallImageData(static_cast<unsigned long>(CONST_10 * CONST_10 * 3), CONST_128); // Píxeles grises
  const PPMImage smallImage = createTestImage({.width=CONST_10, .height=CONST_10, .maxValue=255}, smallImageData);
    escribirImagenPPM("small_input.ppm", smallImage);

    performResizeOperation("small_input.ppm", "large_output.ppm", CONST_1000/2, CONST_1000/2);
    PPMImage largeOutputImage;
    ASSERT_TRUE(leerImagenPPM("large_output.ppm", largeOutputImage));

    EXPECT_EQ(largeOutputImage.width, 500);
    EXPECT_EQ(largeOutputImage.height, 500);

    for (size_t i = 0; i < largeOutputImage.pixelData.size(); i += 3) {
        EXPECT_NEAR(largeOutputImage.pixelData[i], 128, 1);
        EXPECT_NEAR(largeOutputImage.pixelData[i + 1], 128, 1);
        EXPECT_NEAR(largeOutputImage.pixelData[i + 2], 128, 1);
    }
}

