#include <gtest/gtest.h>
#include "../imgaos/resize.hpp"
#include "../common/binario.hpp"
#include <vector>
#include <stdexcept>
#include <fstream>  // Para la definición de std::ofstream

// Clase de prueba para la función de reescalado
class ResizeTest : public ::testing::Test {
protected:
    // Función de ayuda para crear una imagen PPM de prueba
    PPMImage createTestImage(int width, int height, int maxValue, const std::vector<unsigned char>& data) {
        PPMImage image;
        image.width = width;
        image.height = height;
        image.maxValue = maxValue;
        image.pixelData = data;
        return image;
    }

  PPMImage testImage;
  const std::string testInputPath{"test_input.ppm"};
  const std::string testOutputPath{"test_output.ppm"};

  [[nodiscard]] bool writeTestImageToDisk() const {
    return escribirImagenPPM(testInputPath, testImage);
  }

  [[nodiscard]] const PPMImage& getTestImage() const { return testImage; }
  void setTestImage(const PPMImage& image) { testImage = image; }
  [[nodiscard]] const std::string& getInputPath() const { return testInputPath; }
  [[nodiscard]] const std::string& getOutputPath() const { return testOutputPath; }
};



// TESTS DE VALIDACIÓN DE PARÁMETROS
TEST_F(ResizeTest, ThrowsOnInvalidResizeValue) {
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
TEST_F(ResizeTest, ResizeToSinglePixel) {
    PPMImage inputImage = createTestImage(4, 4, 255, {
        255, 0, 0,   0, 255, 0,   0, 0, 255,   255, 255, 0,
        255, 0, 255, 0, 255, 255, 255, 255, 255, 0, 0, 0,
        128, 128, 128, 64, 64, 64,  32, 32, 32,  16, 16, 16,
        0, 0, 0,     255, 255, 255, 127, 127, 127, 63, 63, 63
    });
    escribirImagenPPM("input_4x4.ppm", inputImage);

    performResizeOperation("input_4x4.ppm", "output_1x1.ppm", 1, 1);
    PPMImage outputImage;
    ASSERT_TRUE(leerImagenPPM("output_1x1.ppm", outputImage));

    // Verificar que el archivo de salida tenga las dimensiones esperadas (1x1)
    EXPECT_EQ(outputImage.width, 1);
    EXPECT_EQ(outputImage.height, 1);
}

// Redimensionar a la misma dimensión (identidad)
TEST_F(ResizeTest, ResizeToSameDimensions) {
    PPMImage inputImage = createTestImage(4, 4, 255, {
        255, 0, 0,   0, 255, 0,   0, 0, 255,   255, 255, 0,
        255, 0, 255, 0, 255, 255, 255, 255, 255, 0, 0, 0,
        128, 128, 128, 64, 64, 64,  32, 32, 32,  16, 16, 16,
        0, 0, 0,     255, 255, 255, 127, 127, 127, 63, 63, 63
    });
    escribirImagenPPM("input_4x4.ppm", inputImage);

    performResizeOperation("input_4x4.ppm", "output_same.ppm", 4, 4);
    PPMImage outputImage;
    ASSERT_TRUE(leerImagenPPM("output_same.ppm", outputImage));

    // Verificar que las dimensiones sean las mismas
    EXPECT_EQ(outputImage.width, 4);
    EXPECT_EQ(outputImage.height, 4);
    // Verificar que el contenido sea el mismo
    EXPECT_EQ(inputImage.pixelData, outputImage.pixelData);
}

// Redimensionar una imagen de 1x1 a un tamaño mayor
TEST_F(ResizeTest, ResizeSinglePixelToLarger) {
    PPMImage inputImage = createTestImage(1, 1, 255, {
        128, 128, 128  // Un solo píxel gris
    });
    escribirImagenPPM("input_1x1.ppm", inputImage);

    performResizeOperation("input_1x1.ppm", "output_large.ppm", 4, 4);
    PPMImage outputImage;
    ASSERT_TRUE(leerImagenPPM("output_large.ppm", outputImage));

    // Verificar que el archivo de salida tenga las dimensiones esperadas (4x4)
    EXPECT_EQ(outputImage.width, 4);
    EXPECT_EQ(outputImage.height, 4);

    // Verificar que todos los píxeles tengan el mismo color que el original (128, 128, 128)
    for (size_t i = 0; i < outputImage.pixelData.size(); i += 3) {
        EXPECT_EQ(outputImage.pixelData[i], 128);
        EXPECT_EQ(outputImage.pixelData[i + 1], 128);
        EXPECT_EQ(outputImage.pixelData[i + 2], 128);
    }
}

// TEST DE VALIDACIÓN DE FORMATO DE IMAGEN
// Verificar que se lance una excepción si el formato de la imagen es incorrecto (es distinto a P6)
TEST_F(ResizeTest, InvalidPPMFormat) {
    // Crear un archivo PPM malformado
    std::ofstream outFile("invalid.ppm");
    outFile << "P3\n"   // Encabezado incorrecto (debería ser P6)
            << "2 2\n"
            << "255\n"
            << "255 0 0 0 255 0 0 0 255 255 255 0";  // Datos en formato ASCII
    outFile.close();

    // Intentar realizar el redimensionado y verificar que lanza una excepción
    EXPECT_THROW({
        performResizeOperation("invalid.ppm", "output_invalid.ppm", 2, 2);
    }, std::runtime_error);
}

// TESTS DE ESCENARIOS GRANDES
// Redimensionar una imagen grande a un tamaño pequeño
TEST_F(ResizeTest, ResizeLargeImageToSmall) {
    // Crear una imagen grande de 1000x1000 píxeles con colores alternantes
    std::vector<unsigned char> largeImageData(1000 * 1000 * 3, 0);
    for (size_t i = 0; i < largeImageData.size(); i += 3) {
        largeImageData[i] = (i / 3) % 256;       // Rojo
        largeImageData[i + 1] = (i / 3 / 256) % 256; // Verde
        largeImageData[i + 2] = 255 - (i / 3) % 256; // Azul
    }
    PPMImage largeImage = createTestImage(1000, 1000, 255, largeImageData);
    escribirImagenPPM("large_input.ppm", largeImage);

    performResizeOperation("large_input.ppm", "small_output.ppm", 50, 50);
    PPMImage smallOutputImage;
    ASSERT_TRUE(leerImagenPPM("small_output.ppm", smallOutputImage));

    // Verificar dimensiones de salida
    EXPECT_EQ(smallOutputImage.width, 50);
    EXPECT_EQ(smallOutputImage.height, 50);

    // Verificación adicional (opcional): Comprobar que los valores se reducen de forma esperada
    // Esto podría incluir estadísticas como el promedio de colores o inspección visual
}

// Redimensionar una imagen pequeña a un tamaño grande
TEST_F(ResizeTest, ResizeSmallImageToLarge) {
    // Crear una imagen pequeña de 10x10 píxeles con un patrón básico
    std::vector<unsigned char> smallImageData(10 * 10 * 3, 128); // Píxeles grises
    PPMImage smallImage = createTestImage(10, 10, 255, smallImageData);
    escribirImagenPPM("small_input.ppm", smallImage);

    performResizeOperation("small_input.ppm", "large_output.ppm", 500, 500);
    PPMImage largeOutputImage;
    ASSERT_TRUE(leerImagenPPM("large_output.ppm", largeOutputImage));

    // Verificar dimensiones de salida
    EXPECT_EQ(largeOutputImage.width, 500);
    EXPECT_EQ(largeOutputImage.height, 500);

    // Verificar que los píxeles se interpolaron correctamente
    // Comprobación básica: todos los píxeles deberían mantener una tonalidad gris uniforme
    for (size_t i = 0; i < largeOutputImage.pixelData.size(); i += 3) {
        EXPECT_NEAR(largeOutputImage.pixelData[i], 128, 1);       // Rojo
        EXPECT_NEAR(largeOutputImage.pixelData[i + 1], 128, 1);   // Verde
        EXPECT_NEAR(largeOutputImage.pixelData[i + 2], 128, 1);   // Azul
    }
}






/*
// Test 5: Comportamiento con píxeles en el borde de la imagen
TEST_F(ResizeTest, ResizeWithEdgePixels) {
    PPMImage inputImage = createTestImage(3, 3, 255, {
        255, 0, 0,     0, 255, 0,   0, 0, 255,
        255, 255, 0,   0, 255, 255, 255, 0, 255,
        128, 128, 128, 64, 64, 64,  32, 32, 32
    });
    escribirImagenPPM("input_3x3.ppm", inputImage);

    performResizeOperation("input_3x3.ppm", "output_edge_test.ppm", 5, 5);
    PPMImage outputImage;
    ASSERT_TRUE(leerImagenPPM("output_edge_test.ppm", outputImage));

    // Verificar las dimensiones
    EXPECT_EQ(outputImage.width, 5);
    EXPECT_EQ(outputImage.height, 5);

    // Verificar que los píxeles en los bordes de la imagen de salida estén interpolados correctamente
    // (aquí se pueden realizar comparaciones específicas según la lógica de interpolación aplicada)
    // Ejemplo simple: Verificar que los píxeles de las esquinas sigan representando el color dominante de la esquina original
    EXPECT_EQ(outputImage.pixelData[0], 255);     // Esquina superior izquierda (rojo)
    EXPECT_EQ(outputImage.pixelData[1], 0);
    EXPECT_EQ(outputImage.pixelData[2], 0);

    EXPECT_EQ(outputImage.pixelData[3 * (5 - 1) * 3], 128);  // Esquina inferior izquierda (gris)
    EXPECT_EQ(outputImage.pixelData[3 * (5 - 1) * 3 + 1], 128);
    EXPECT_EQ(outputImage.pixelData[3 * (5 - 1) * 3 + 2], 128);
}
*/