#include <gtest/gtest.h>
#include "../imgsoa/resize.hpp"
#include "../common/binario.hpp"
#include <vector>
#include <stdexcept>
#include <fstream>  // Para la definición de std::ofstream

// Clase de prueba para la función de reescalado SoA
class ResizeSOATest : public ::testing::Test {
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

    EXPECT_EQ(outputImage.width, 1);
    EXPECT_EQ(outputImage.height, 1);
}

// Redimensionar a la misma dimensión (identidad)
TEST_F(ResizeSOATest, ResizeToSameDimensions) {
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

    EXPECT_EQ(outputImage.width, 4);
    EXPECT_EQ(outputImage.height, 4);
    EXPECT_EQ(inputImage.pixelData, outputImage.pixelData);
}

// Redimensionar una imagen de 1x1 a un tamaño mayor
TEST_F(ResizeSOATest, ResizeSinglePixelToLarger) {
    PPMImage inputImage = createTestImage(1, 1, 255, { 128, 128, 128 });
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
    std::vector<unsigned char> largeImageData(1000 * 1000 * 3, 0);
    for (size_t i = 0; i < largeImageData.size(); i += 3) {
        largeImageData[i] = (i / 3) % 256;
        largeImageData[i + 1] = (i / 3 / 256) % 256;
        largeImageData[i + 2] = 255 - (i / 3) % 256;
    }
    PPMImage largeImage = createTestImage(1000, 1000, 255, largeImageData);
    escribirImagenPPM("large_input.ppm", largeImage);

    performResizeOperation("large_input.ppm", "small_output.ppm", 50, 50);
    PPMImage smallOutputImage;
    ASSERT_TRUE(leerImagenPPM("small_output.ppm", smallOutputImage));

    EXPECT_EQ(smallOutputImage.width, 50);
    EXPECT_EQ(smallOutputImage.height, 50);
}

TEST_F(ResizeSOATest, ResizeSmallImageToLarge) {
    std::vector<unsigned char> smallImageData(10 * 10 * 3, 128);
    PPMImage smallImage = createTestImage(10, 10, 255, smallImageData);
    escribirImagenPPM("small_input.ppm", smallImage);

    performResizeOperation("small_input.ppm", "large_output.ppm", 500, 500);
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

