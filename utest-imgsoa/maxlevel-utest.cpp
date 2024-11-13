#include <gtest/gtest.h>
#include "../imgsoa/maxlevel.hpp"
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
constexpr unsigned int QUARTER_8BIT = 63U;
constexpr unsigned int THIRD_16BIT = 21845U;
constexpr unsigned int TEST_WIDTH_RECTANGULAR = 4;
constexpr unsigned int TEST_HEIGHT_RECTANGULAR = 2;

class MaxLevelSoATest : public ::testing::Test {
private:
    PPMImageSoA testImage;
    const std::string testInputPath{"test_input.ppm"};
    const std::string testOutputPath{"test_output.ppm"};

protected:
    void SetUp() override {
        testImage.width = TEST_WIDTH;
        testImage.height = TEST_HEIGHT;
        testImage.maxValue = MAX_8BIT;

        // 4 píxeles RGB (rojo, verde, azul, blanco)
        testImage.redChannel = {
            MAX_8BIT, 0, 0, MAX_8BIT  // Rojo, Verde, Azul, Blanco
        };
        testImage.greenChannel = {
            0, MAX_8BIT, 0, MAX_8BIT  // Rojo, Verde, Azul, Blanco
        };
        testImage.blueChannel = {
            0, 0, MAX_8BIT, MAX_8BIT  // Rojo, Verde, Azul, Blanco
        };
    }

    void TearDown() override {
        std::filesystem::remove(testInputPath);
        std::filesystem::remove(testOutputPath);
    }

    [[nodiscard]] bool writeTestImageToDisk() const {
        return escribirImagenPPMSoA(testInputPath, testImage);
    }

    [[nodiscard]] const PPMImageSoA& getTestImage() const { return testImage; }
    void setTestImage(const PPMImageSoA& image) { testImage = image; }
    [[nodiscard]] const std::string& getInputPath() const { return testInputPath; }
    [[nodiscard]] const std::string& getOutputPath() const { return testOutputPath; }
};

// Verifica que la función lance excepciones apropiadas para valores máximos inválidos:
// - Valores negativos
// - Valor cero
// - Valores mayores que el máximo permitido de 16 bits
TEST_F(MaxLevelSoATest, ThrowsOnInvalidMaxValue) {
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

// Verifica la conversión correcta de una imagen de 8 bits a la mitad de su valor máximo (127)
// Comprueba que los valores de los píxeles se escalan proporcionalmente
TEST_F(MaxLevelSoATest, ConvertTo8BitMax) {
    ASSERT_TRUE(writeTestImageToDisk());

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(HALF_8BIT))
    );

    PPMImageSoA result;
    ASSERT_TRUE(leerImagenPPMSoA(getOutputPath(), result));

    EXPECT_EQ(result.maxValue, static_cast<int>(HALF_8BIT));
    EXPECT_EQ(result.width, getTestImage().width);
    EXPECT_EQ(result.height, getTestImage().height);

    // Verificar valores de los canales
    const std::vector<uint8_t> expectedRed = {
        HALF_8BIT, 0, 0, HALF_8BIT
    };
    const std::vector<uint8_t> expectedGreen = {
        0, HALF_8BIT, 0, HALF_8BIT
    };
    const std::vector<uint8_t> expectedBlue = {
        0, 0, HALF_8BIT, HALF_8BIT
    };

    EXPECT_EQ(result.redChannel, expectedRed);
    EXPECT_EQ(result.greenChannel, expectedGreen);
    EXPECT_EQ(result.blueChannel, expectedBlue);
}

// Test de conversión a un cuarto del valor máximo de 8 bits
TEST_F(MaxLevelSoATest, ConvertToQuarter8Bit) {
    ASSERT_TRUE(writeTestImageToDisk());

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(QUARTER_8BIT))
    );

    PPMImageSoA result;
    ASSERT_TRUE(leerImagenPPMSoA(getOutputPath(), result));

    EXPECT_EQ(result.maxValue, static_cast<int>(QUARTER_8BIT));

    const std::vector<uint8_t> expectedRed = {
        QUARTER_8BIT, 0, 0, QUARTER_8BIT
    };
    const std::vector<uint8_t> expectedGreen = {
        0, QUARTER_8BIT, 0, QUARTER_8BIT
    };
    const std::vector<uint8_t> expectedBlue = {
        0, 0, QUARTER_8BIT, QUARTER_8BIT
    };

    EXPECT_EQ(result.redChannel, expectedRed);
    EXPECT_EQ(result.greenChannel, expectedGreen);
    EXPECT_EQ(result.blueChannel, expectedBlue);
}

// Test de conversión de 8 bits a 16 bits
TEST_F(MaxLevelSoATest, ConvertTo16BitMax) {
    ASSERT_TRUE(writeTestImageToDisk());

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(MAX_16BIT))
    );

    PPMImageSoA result;
    ASSERT_TRUE(leerImagenPPMSoA(getOutputPath(), result));

    EXPECT_EQ(result.maxValue, static_cast<int>(MAX_16BIT));
    EXPECT_EQ(result.width, getTestImage().width);
    EXPECT_EQ(result.height, getTestImage().height);

    // Verificar que los canales tienen el doble de bytes (formato 16 bits)
    EXPECT_EQ(result.redChannel.size(), getTestImage().redChannel.size() * 2);
    EXPECT_EQ(result.greenChannel.size(), getTestImage().greenChannel.size() * 2);
    EXPECT_EQ(result.blueChannel.size(), getTestImage().blueChannel.size() * 2);
}

// Test de manejo de imágenes vacías
TEST_F(MaxLevelSoATest, HandleEmptyImage) {
    PPMImageSoA emptyImage;
    emptyImage.width = 0;
    emptyImage.height = 0;
    emptyImage.maxValue = static_cast<int>(MAX_8BIT);
    ASSERT_TRUE(escribirImagenPPMSoA(getInputPath(), emptyImage));

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(HALF_8BIT))
    );

    PPMImageSoA result;
    ASSERT_TRUE(leerImagenPPMSoA(getOutputPath(), result));
    EXPECT_EQ(result.width, 0);
    EXPECT_EQ(result.height, 0);
    EXPECT_TRUE(result.redChannel.empty());
    EXPECT_TRUE(result.greenChannel.empty());
    EXPECT_TRUE(result.blueChannel.empty());
}

// Test de archivo de entrada no existente
TEST_F(MaxLevelSoATest, ThrowsOnNonexistentInputFile) {
    EXPECT_THROW(
        performMaxLevelOperation("nonexistent.ppm", getOutputPath(), static_cast<int>(HALF_8BIT)),
        std::runtime_error
    );
}

// Test de conversión de 16 bits a 8 bits
TEST_F(MaxLevelSoATest, From16BitTo8Bit) {
    PPMImageSoA img16bit = getTestImage();
    img16bit.maxValue = static_cast<int>(MAX_16BIT);

    // Convertir cada canal a formato 16 bits
    auto convertToChannel16Bit = [](const std::vector<uint8_t>& channel8bit) {
        std::vector<uint8_t> channel16bit;
        for (const auto& value : channel8bit) {
            auto scaledValue = static_cast<unsigned short>(value << BITS_PER_BYTE);
            channel16bit.push_back(static_cast<uint8_t>(scaledValue & BYTE_MASK));
            channel16bit.push_back(static_cast<uint8_t>(scaledValue >> BITS_PER_BYTE));
        }
        return channel16bit;
    };

    img16bit.redChannel = convertToChannel16Bit(img16bit.redChannel);
    img16bit.greenChannel = convertToChannel16Bit(img16bit.greenChannel);
    img16bit.blueChannel = convertToChannel16Bit(img16bit.blueChannel);

    setTestImage(img16bit);
    ASSERT_TRUE(writeTestImageToDisk());

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(MAX_8BIT))
    );

    PPMImageSoA result;
    ASSERT_TRUE(leerImagenPPMSoA(getOutputPath(), result));
    EXPECT_EQ(result.maxValue, static_cast<int>(MAX_8BIT));
    EXPECT_EQ(result.redChannel.size(), getTestImage().redChannel.size() / 2);
    EXPECT_EQ(result.greenChannel.size(), getTestImage().greenChannel.size() / 2);
    EXPECT_EQ(result.blueChannel.size(), getTestImage().blueChannel.size() / 2);
}

// Test de comportamiento de redondeo
TEST_F(MaxLevelSoATest, RoundingBehavior) {
    ASSERT_TRUE(writeTestImageToDisk());

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(ARBITRARY_MAX))
    );

    PPMImageSoA result;
    ASSERT_TRUE(leerImagenPPMSoA(getOutputPath(), result));
    EXPECT_EQ(result.redChannel[0], ARBITRARY_MAX);
}

// Verifica que se mantiene la relación de aspecto en imágenes no cuadradas
TEST_F(MaxLevelSoATest, PreserveAspectRatio) {
    PPMImageSoA rectangularImage;
    rectangularImage.width = TEST_WIDTH_RECTANGULAR;
    rectangularImage.height = TEST_HEIGHT_RECTANGULAR;
    rectangularImage.maxValue = static_cast<int>(MAX_8BIT);

    // Llenar los canales con datos de ejemplo
    rectangularImage.redChannel = {
        MAX_8BIT, 0, 0, MAX_8BIT,
        0, MAX_8BIT, 0, MAX_8BIT
    };
    rectangularImage.greenChannel = {
        0, MAX_8BIT, 0, 0,
        MAX_8BIT, 0, MAX_8BIT, MAX_8BIT
    };
    rectangularImage.blueChannel = {
        0, 0, MAX_8BIT, MAX_8BIT,
        0, 0, MAX_8BIT, 0
    };

    ASSERT_TRUE(escribirImagenPPMSoA(getInputPath(), rectangularImage));

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(HALF_8BIT))
    );

    PPMImageSoA result;
    ASSERT_TRUE(leerImagenPPMSoA(getOutputPath(), result));

    EXPECT_EQ(result.width, rectangularImage.width);
    EXPECT_EQ(result.height, rectangularImage.height);
}

// Test de manejo de imagen de un solo píxel
TEST_F(MaxLevelSoATest, HandleSinglePixelImage) {
    PPMImageSoA singlePixel;
    singlePixel.width = 1;
    singlePixel.height = 1;
    singlePixel.maxValue = static_cast<int>(MAX_8BIT);

    singlePixel.redChannel = {MAX_8BIT};
    singlePixel.greenChannel = {MAX_8BIT};
    singlePixel.blueChannel = {MAX_8BIT};

    setTestImage(singlePixel);
    ASSERT_TRUE(writeTestImageToDisk());

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(HALF_8BIT))
    );

    PPMImageSoA result;
    ASSERT_TRUE(leerImagenPPMSoA(getOutputPath(), result));
    EXPECT_EQ(result.width, 1);
    EXPECT_EQ(result.height, 1);
    EXPECT_EQ(result.redChannel.size(), 1);
    EXPECT_EQ(result.greenChannel.size(), 1);
    EXPECT_EQ(result.blueChannel.size(), 1);
    EXPECT_EQ(result.redChannel[0], HALF_8BIT);
    EXPECT_EQ(result.greenChannel[0], HALF_8BIT);
    EXPECT_EQ(result.blueChannel[0], HALF_8BIT);
}

// Test de error de escritura
TEST_F(MaxLevelSoATest, ThrowsOnWriteError) {
    ASSERT_TRUE(writeTestImageToDisk());

    EXPECT_THROW(
        performMaxLevelOperation(getInputPath(), "/nonexistent/dir/output.ppm", static_cast<int>(HALF_8BIT)),
        std::runtime_error
    );
}

// Test de preservación de píxeles negros
TEST_F(MaxLevelSoATest, PreserveBlackPixels) {
    PPMImageSoA blackImage;
    blackImage.width = 2;
    blackImage.height = 2;
    blackImage.maxValue = static_cast<int>(MAX_8BIT);
    blackImage.redChannel = std::vector<uint8_t>(4, 0);
    blackImage.greenChannel = std::vector<uint8_t>(4, 0);
    blackImage.blueChannel = std::vector<uint8_t>(4, 0);

    setTestImage(blackImage);
    ASSERT_TRUE(writeTestImageToDisk());

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(MAX_16BIT))
    );

    PPMImageSoA result;
    ASSERT_TRUE(leerImagenPPMSoA(getOutputPath(), result));

    // Verificar que todos los canales mantienen sus píxeles negros
    for (const auto& value : result.redChannel) {
        EXPECT_EQ(value, 0);
    }
    for (const auto& value : result.greenChannel) {
        EXPECT_EQ(value, 0);
    }
    for (const auto& value : result.blueChannel) {
        EXPECT_EQ(value, 0);
    }
}

}  // namespace