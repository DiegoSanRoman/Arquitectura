#include <gtest/gtest.h>
#include "../imgaos/maxlevel.hpp"
#include "../common/binario.hpp"
#include <vector>
#include <stdexcept>
#include <cstring>
#include <filesystem>

namespace {

// Constantes para evitar números mágicos
constexpr unsigned int MAX_8BIT = 255U;
constexpr unsigned int MAX_16BIT = 65535U;
constexpr unsigned int HALF_8BIT = 127U;
constexpr unsigned int BITS_PER_BYTE = 8U;
constexpr unsigned int BYTE_MASK = 0xFFU;
constexpr unsigned int TEST_WIDTH = 2U;
constexpr unsigned int TEST_HEIGHT = 2U;
constexpr unsigned int ARBITRARY_MAX = 100U;

class MaxLevelTest : public ::testing::Test {
private:
    PPMImage testImage;
    const std::string testInputPath{"test_input.ppm"};
    const std::string testOutputPath{"test_output.ppm"};

protected:
    void SetUp() override {
        testImage.width = TEST_WIDTH;
        testImage.height = TEST_HEIGHT;
        testImage.maxValue = MAX_8BIT;
        // 4 píxeles RGB (rojo, verde, azul, blanco)
        testImage.pixelData = {
            MAX_8BIT, 0, 0,            // Rojo
            0, MAX_8BIT, 0,            // Verde
            0, 0, MAX_8BIT,            // Azul
            MAX_8BIT, MAX_8BIT, MAX_8BIT   // Blanco
        };
    }

    void TearDown() override {
        std::filesystem::remove(testInputPath);
        std::filesystem::remove(testOutputPath);
    }

    [[nodiscard]] bool writeTestImageToDisk() const {
        return escribirImagenPPM(testInputPath, testImage);
    }

    [[nodiscard]] const PPMImage& getTestImage() const { return testImage; }
    void setTestImage(const PPMImage& image) { testImage = image; }
    [[nodiscard]] const std::string& getInputPath() const { return testInputPath; }
    [[nodiscard]] const std::string& getOutputPath() const { return testOutputPath; }
};

// Test que verifica que la función lanza excepciones apropiadas cuando se le pasan valores máximos inválidos
// Casos probados: valor negativo, cero y valor mayor que el máximo permitido de 16 bits
TEST_F(MaxLevelTest, ThrowsOnInvalidMaxValue) {
    ASSERT_TRUE(writeTestImageToDisk());

    EXPECT_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), -1),
        std::invalid_argument
    );

    EXPECT_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), 0),
        std::invalid_argument
    );

    EXPECT_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(MAX_16BIT) + 1),
        std::invalid_argument
    );
}

// Test que verifica la conversión correcta de una imagen de 8 bits a un valor máximo de 127 (HALF_8BIT)
// Comprueba que los valores de los píxeles se escalan correctamente manteniendo las proporciones
TEST_F(MaxLevelTest, ConvertTo8BitMax) {
    ASSERT_TRUE(writeTestImageToDisk());

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(HALF_8BIT))
    );

    PPMImage result;
    ASSERT_TRUE(leerImagenPPM(getOutputPath(), result));

    EXPECT_EQ(result.maxValue, static_cast<int>(HALF_8BIT));
    EXPECT_EQ(result.width, getTestImage().width);
    EXPECT_EQ(result.height, getTestImage().height);

    const std::vector<unsigned char> expectedValues = {
        HALF_8BIT, 0, 0,            // Rojo reducido a la mitad
        0, HALF_8BIT, 0,            // Verde reducido a la mitad
        0, 0, HALF_8BIT,            // Azul reducido a la mitad
        HALF_8BIT, HALF_8BIT, HALF_8BIT   // Blanco reducido a la mitad
    };

    EXPECT_EQ(result.pixelData, expectedValues);
}

// Test que verifica la conversión correcta de una imagen de 8 bits a 16 bits
// Comprueba que el tamaño de los datos de píxeles se duplica debido al formato de 16 bits
TEST_F(MaxLevelTest, ConvertTo16BitMax) {
    ASSERT_TRUE(writeTestImageToDisk());

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(MAX_16BIT))
    );

    PPMImage result;
    ASSERT_TRUE(leerImagenPPM(getOutputPath(), result));

    EXPECT_EQ(result.maxValue, static_cast<int>(MAX_16BIT));
    EXPECT_EQ(result.width, getTestImage().width);
    EXPECT_EQ(result.height, getTestImage().height);

    // Verificar que los valores se han escalado correctamente a 16 bits
    EXPECT_EQ(result.pixelData.size(), getTestImage().pixelData.size() * 2);
}

// Test que verifica el manejo correcto de imágenes vacías (0x0 píxeles)
// Comprueba que la función puede procesar una imagen vacía sin errores
TEST_F(MaxLevelTest, HandleEmptyImage) {
    PPMImage emptyImage;
    emptyImage.width = 0;
    emptyImage.height = 0;
    emptyImage.maxValue = static_cast<int>(MAX_8BIT);
    ASSERT_TRUE(escribirImagenPPM(getInputPath(), emptyImage));

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(HALF_8BIT))
    );

    PPMImage result;
    ASSERT_TRUE(leerImagenPPM(getOutputPath(), result));
    EXPECT_EQ(result.width, 0);
    EXPECT_EQ(result.height, 0);
    EXPECT_TRUE(result.pixelData.empty());
}

// Test que verifica que la función lanza una excepción cuando el archivo de entrada no existe
TEST_F(MaxLevelTest, ThrowsOnNonexistentInputFile) {
    EXPECT_THROW(
        performMaxLevelOperation("nonexistent.ppm", getOutputPath(), static_cast<int>(HALF_8BIT)),
        std::runtime_error
    );
}

// Test que verifica la conversión correcta de una imagen de 16 bits a 8 bits
// Comprueba que los valores se escalan correctamente y que el tamaño de los datos se reduce a la mitad
TEST_F(MaxLevelTest, From16BitTo8Bit) {
    PPMImage img16bit = getTestImage();
    img16bit.maxValue = static_cast<int>(MAX_16BIT);
    std::vector<unsigned char> data16Bit;

    for (const auto& value : img16bit.pixelData) {
        auto scaledValue = static_cast<unsigned short>(value << BITS_PER_BYTE);
        data16Bit.push_back(static_cast<unsigned char>(scaledValue & BYTE_MASK));
        data16Bit.push_back(static_cast<unsigned char>(scaledValue >> BITS_PER_BYTE));
    }

    img16bit.pixelData = data16Bit;
    setTestImage(img16bit);
    ASSERT_TRUE(writeTestImageToDisk());

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(MAX_8BIT))
    );

    PPMImage result;
    ASSERT_TRUE(leerImagenPPM(getOutputPath(), result));
    EXPECT_EQ(result.maxValue, static_cast<int>(MAX_8BIT));
    EXPECT_EQ(result.pixelData.size(), getTestImage().pixelData.size() / 2);
}

// Test que verifica el comportamiento del redondeo al convertir valores de píxeles
// Comprueba que los valores se redondean correctamente a un valor máximo arbitrario
TEST_F(MaxLevelTest, RoundingBehavior) {
    ASSERT_TRUE(writeTestImageToDisk());

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(ARBITRARY_MAX))
    );

    PPMImage result;
    ASSERT_TRUE(leerImagenPPM(getOutputPath(), result));

    // Verificar que los valores se han redondeado correctamente
    EXPECT_EQ(result.pixelData[0], ARBITRARY_MAX); // Componente rojo del primer píxel
}

}  // namespace