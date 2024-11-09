#include <gtest/gtest.h>
#include "../imgaos/maxlevel.hpp"
#include "../common/binario.hpp"
#include <filesystem>

class MaxLevelFunctionalTest : public ::testing::Test {
private:
    // Rutas de archivos como miembros privados
    const std::string inputPath = "test_input.ppm";
    const std::string outputPath = "test_output.ppm";

protected:
    // Constantes para valores máximos de color (movidas a protected)
    static constexpr int MAX_COLOR_8BIT = 255;
    static constexpr int MAX_COLOR_16BIT = 65535;
    static constexpr int HALF_MAX_8BIT = 127;
    static constexpr int DEFAULT_WIDTH = 2;
    static constexpr int DEFAULT_HEIGHT = 2;
    static constexpr int ZERO = 0;

    void SetUp() override {
        cleanupFiles();
    }

    void TearDown() override {
        cleanupFiles();
    }

    [[nodiscard]] const std::string& getInputPath() const { return inputPath; }
    [[nodiscard]] const std::string& getOutputPath() const { return outputPath; }

    // Movido a protected para permitir acceso desde las pruebas
    [[nodiscard]] static PPMImage create8BitImage() {
        PPMImage image;
        image.width = DEFAULT_WIDTH;
        image.height = DEFAULT_HEIGHT;
        image.maxValue = MAX_COLOR_8BIT;
        image.pixelData = {
            MAX_COLOR_8BIT, ZERO, ZERO,      // Rojo puro
            ZERO, MAX_COLOR_8BIT, ZERO,      // Verde puro
            ZERO, ZERO, MAX_COLOR_8BIT,      // Azul puro
            MAX_COLOR_8BIT, MAX_COLOR_8BIT, MAX_COLOR_8BIT   // Blanco
        };
        return image;
    }

    [[nodiscard]] static PPMImage create16BitImage() {
        PPMImage image;
        image.width = DEFAULT_WIDTH;
        image.height = DEFAULT_HEIGHT;
        image.maxValue = MAX_COLOR_16BIT;

        const auto highByte = static_cast<unsigned char>(MAX_COLOR_8BIT);
        image.pixelData = {
            highByte, highByte, ZERO, ZERO, ZERO, ZERO,          // Rojo puro 16 bits
            ZERO, ZERO, highByte, highByte, ZERO, ZERO,          // Verde puro 16 bits
            ZERO, ZERO, ZERO, ZERO, highByte, highByte,          // Azul puro 16 bits
            highByte, highByte, highByte, highByte, highByte, highByte   // Blanco 16 bits
        };
        return image;
    }

private:
    void cleanupFiles() {
        std::filesystem::remove(inputPath);
        std::filesystem::remove(outputPath);
    }
};

// Prueba funcional: Conversión de imagen 8 bits a un valor máximo menor
TEST_F(MaxLevelFunctionalTest, Convert8BitImageToLowerMaxValue) {
    // Crear y guardar imagen de prueba de 8 bits
    auto inputImage = create8BitImage();
    ASSERT_TRUE(escribirImagenPPM(getInputPath(), inputImage));

    // Ejecutar la conversión a un valor máximo de 127
    ASSERT_NO_THROW(performMaxLevelOperation(getInputPath(), getOutputPath(), HALF_MAX_8BIT));

    // Verificar el resultado
    PPMImage outputImage;
    ASSERT_TRUE(leerImagenPPM(getOutputPath(), outputImage));

    // Verificar las propiedades básicas
    EXPECT_EQ(outputImage.width, DEFAULT_WIDTH);
    EXPECT_EQ(outputImage.height, DEFAULT_HEIGHT);
    EXPECT_EQ(outputImage.maxValue, HALF_MAX_8BIT);

    // Verificar que los colores se han escalado correctamente
    const std::vector<unsigned char> expectedData = {
        HALF_MAX_8BIT, ZERO, ZERO,    // Rojo reducido a la mitad
        ZERO, HALF_MAX_8BIT, ZERO,    // Verde reducido a la mitad
        ZERO, ZERO, HALF_MAX_8BIT,    // Azul reducido a la mitad
        HALF_MAX_8BIT, HALF_MAX_8BIT, HALF_MAX_8BIT // Blanco reducido a la mitad
    };
    EXPECT_EQ(outputImage.pixelData, expectedData);
}

// Prueba funcional: Conversión de imagen 8 bits a 16 bits
TEST_F(MaxLevelFunctionalTest, Convert8BitTo16BitImage) {
    auto inputImage = create8BitImage();
    ASSERT_TRUE(escribirImagenPPM(getInputPath(), inputImage));

    // Ejecutar la conversión a 16 bits
    ASSERT_NO_THROW(performMaxLevelOperation(getInputPath(), getOutputPath(), MAX_COLOR_16BIT));

    // Verificar el resultado
    PPMImage outputImage;
    ASSERT_TRUE(leerImagenPPM(getOutputPath(), outputImage));

    // Verificar las propiedades básicas
    EXPECT_EQ(outputImage.width, DEFAULT_WIDTH);
    EXPECT_EQ(outputImage.height, DEFAULT_HEIGHT);
    EXPECT_EQ(outputImage.maxValue, MAX_COLOR_16BIT);

    // En formato de 16 bits, el tamaño de pixelData debe ser el doble
    EXPECT_EQ(outputImage.pixelData.size(), inputImage.pixelData.size() * 2);
}

// Prueba funcional: Conversión de imagen 16 bits a 8 bits
TEST_F(MaxLevelFunctionalTest, Convert16BitTo8BitImage) {
    auto inputImage = create16BitImage();
    ASSERT_TRUE(escribirImagenPPM(getInputPath(), inputImage));

    // Ejecutar la conversión a 8 bits
    ASSERT_NO_THROW(performMaxLevelOperation(getInputPath(), getOutputPath(), MAX_COLOR_8BIT));

    // Verificar el resultado
    PPMImage outputImage;
    ASSERT_TRUE(leerImagenPPM(getOutputPath(), outputImage));

    // Verificar las propiedades básicas
    EXPECT_EQ(outputImage.width, DEFAULT_WIDTH);
    EXPECT_EQ(outputImage.height, DEFAULT_HEIGHT);
    EXPECT_EQ(outputImage.maxValue, MAX_COLOR_8BIT);

    // En formato de 8 bits, el tamaño de pixelData debe ser la mitad
    EXPECT_EQ(outputImage.pixelData.size(), inputImage.pixelData.size() / 2);
}

int main(int argc, char ** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}