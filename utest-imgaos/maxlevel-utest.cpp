#include <gtest/gtest.h>
#include "../imgaos/maxlevel.hpp"
#include "../common/binario.hpp"
#include <vector>
#include <stdexcept>
#include <cstring>
#include <filesystem>

namespace {

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
constexpr unsigned int BLACK = 12U;

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
        testImage.pixelData = {
            MAX_8BIT, 0, 0,
            0, MAX_8BIT, 0,
            0, 0, MAX_8BIT,
            MAX_8BIT, MAX_8BIT, MAX_8BIT
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

// Verifica que la función lance excepciones apropiadas para valores máximos inválidos:
// - Valores negativos
// - Valor cero
// - Valores mayores que el máximo permitido de 16 bits
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

// Verifica la conversión correcta de una imagen de 8 bits a la mitad de su valor máximo (127)
// Comprueba que los valores de los píxeles se escalan proporcionalmente
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
        HALF_8BIT, 0, 0,
        0, HALF_8BIT, 0,
        0, 0, HALF_8BIT,
        HALF_8BIT, HALF_8BIT, HALF_8BIT
    };

    EXPECT_EQ(result.pixelData, expectedValues);
}

// Verifica la conversión correcta de una imagen de 8 bits a 16 bits
// Comprueba que el tamaño de los datos se duplica y los valores se escalan correctamente
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
    EXPECT_EQ(result.pixelData.size(), getTestImage().pixelData.size() * 2);
}

// Verifica el manejo correcto de imágenes vacías (0x0 píxeles)
// Asegura que la función puede procesar una imagen vacía sin errores
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

// Verifica que la función lance una excepción cuando el archivo de entrada no existe
// Asegura el manejo correcto de errores de lectura
TEST_F(MaxLevelTest, ThrowsOnNonexistentInputFile) {
    EXPECT_THROW(
        performMaxLevelOperation("nonexistent.ppm", getOutputPath(), static_cast<int>(HALF_8BIT)),
        std::runtime_error
    );
}

// Verifica la conversión correcta de una imagen de 16 bits a 8 bits
// Comprueba que los valores se escalan correctamente y el tamaño de los datos se reduce a la mitad
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

// Verifica el comportamiento del redondeo al convertir valores de píxeles
// Comprueba que los valores se redondean correctamente a un valor máximo arbitrario
TEST_F(MaxLevelTest, RoundingBehavior) {
    ASSERT_TRUE(writeTestImageToDisk());

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(ARBITRARY_MAX))
    );

    PPMImage result;
    ASSERT_TRUE(leerImagenPPM(getOutputPath(), result));
    EXPECT_EQ(result.pixelData[0], ARBITRARY_MAX);
}

// Verifica la conversión a un cuarto del valor máximo de 8 bits
// Comprueba la precisión de la escala en valores más pequeños
TEST_F(MaxLevelTest, ConvertToQuarter8Bit) {
    ASSERT_TRUE(writeTestImageToDisk());

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(QUARTER_8BIT))
    );

    PPMImage result;
    ASSERT_TRUE(leerImagenPPM(getOutputPath(), result));

    EXPECT_EQ(result.maxValue, static_cast<int>(QUARTER_8BIT));

    const std::vector<unsigned char> expectedValues = {
        QUARTER_8BIT, 0, 0,
        0, QUARTER_8BIT, 0,
        0, 0, QUARTER_8BIT,
        QUARTER_8BIT, QUARTER_8BIT, QUARTER_8BIT
    };

    EXPECT_EQ(result.pixelData, expectedValues);
}

// Verifica que se mantiene la relación de aspecto en imágenes no cuadradas
// Asegura que la función maneja correctamente imágenes con diferentes dimensiones
TEST_F(MaxLevelTest, PreserveAspectRatio) {
    PPMImage tallImage;
    tallImage.width = 1;
    tallImage.height = 4;
    tallImage.maxValue = static_cast<int>(MAX_8BIT);
    tallImage.pixelData = {
        MAX_8BIT, 0, 0,
        0, MAX_8BIT, 0,
        0, 0, MAX_8BIT,
        MAX_8BIT, MAX_8BIT, MAX_8BIT
    };

    ASSERT_TRUE(escribirImagenPPM(getInputPath(), tallImage));

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(HALF_8BIT))
    );

    PPMImage result;
    ASSERT_TRUE(leerImagenPPM(getOutputPath(), result));
    EXPECT_EQ(result.width, tallImage.width);
    EXPECT_EQ(result.height, tallImage.height);
}

// Verifica el manejo correcto de imágenes de un solo píxel
// Comprueba casos límite con imágenes muy pequeñas
TEST_F(MaxLevelTest, HandleSinglePixelImage) {
    PPMImage singlePixel;
    singlePixel.width = 1;
    singlePixel.height = 1;
    singlePixel.maxValue = static_cast<int>(MAX_8BIT);
    singlePixel.pixelData = {MAX_8BIT, MAX_8BIT, MAX_8BIT};

    ASSERT_TRUE(escribirImagenPPM(getInputPath(), singlePixel));

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(HALF_8BIT))
    );

    PPMImage result;
    ASSERT_TRUE(leerImagenPPM(getOutputPath(), result));
    EXPECT_EQ(result.width, 1);
    EXPECT_EQ(result.height, 1);
    EXPECT_EQ(result.pixelData.size(), 3);
    EXPECT_EQ(result.pixelData[0], HALF_8BIT);
    EXPECT_EQ(result.pixelData[1], HALF_8BIT);
    EXPECT_EQ(result.pixelData[2], HALF_8BIT);
}

// Verifica que la función maneje correctamente los errores de escritura
// Prueba el comportamiento cuando no se puede escribir en el archivo de salida
TEST_F(MaxLevelTest, ThrowsOnWriteError) {
    ASSERT_TRUE(writeTestImageToDisk());

    EXPECT_THROW(
        performMaxLevelOperation(getInputPath(), "/nonexistent/dir/output.ppm", static_cast<int>(HALF_8BIT)),
        std::runtime_error
    );
}

// Verifica que los píxeles negros (valor 0) se mantengan negros después de la conversión
// Comprueba la preservación de valores extremos
TEST_F(MaxLevelTest, PreserveBlackPixels) {
    PPMImage blackImage;
    blackImage.width = 2;
    blackImage.height = 2;
    blackImage.maxValue = static_cast<int>(MAX_8BIT);
    blackImage.pixelData = std::vector<unsigned char>(BLACK, 0);  // 4 píxeles negros

    ASSERT_TRUE(escribirImagenPPM(getInputPath(), blackImage));

    ASSERT_NO_THROW(
        performMaxLevelOperation(getInputPath(), getOutputPath(), static_cast<int>(MAX_16BIT))
    );

    PPMImage result;
    ASSERT_TRUE(leerImagenPPM(getOutputPath(), result));

    // Verificar que los píxeles negros siguen siendo negros
    for (std::size_t i = 0; i < result.pixelData.size(); i += 2) {
        EXPECT_EQ(result.pixelData[i], 0);
        EXPECT_EQ(result.pixelData[i + 1], 0);
    }
}

}  // namespace