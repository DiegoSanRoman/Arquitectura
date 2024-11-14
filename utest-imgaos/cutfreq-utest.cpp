#include <gtest/gtest.h>
#include "../imgaos/cutfreq.hpp"
#include "../common/binario.hpp"
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <unordered_set>

namespace {

constexpr unsigned int MAX_8BIT = 255U;
constexpr unsigned int TEST_WIDTH = 3U;
constexpr unsigned int TEST_HEIGHT = 3U;
constexpr unsigned int COLOR_RED = 255U;
constexpr unsigned int COLOR_GREEN = 128U;
constexpr unsigned int COLOR_BLUE = 64U;
constexpr unsigned int COLOR_GRAY = 192U;
constexpr unsigned int SHIFT_RED = 16;
constexpr unsigned int SHIFT_GREEN = 8;
constexpr unsigned int COLOR_SINGLE = 100U;

class CutFreqTest : public ::testing::Test {
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
            COLOR_RED, 0, 0,      0, COLOR_RED, 0,      0, 0, COLOR_RED,
            COLOR_RED, COLOR_RED, 0,  0, COLOR_RED, COLOR_RED,  COLOR_RED, 0, COLOR_RED,
            COLOR_GREEN, COLOR_GREEN, COLOR_GREEN, COLOR_BLUE, COLOR_BLUE, COLOR_BLUE,  COLOR_GRAY, COLOR_GRAY, COLOR_GRAY
        };
    }

    void TearDown() override {
        std::filesystem::remove(testInputPath);
        std::filesystem::remove(testOutputPath);
    }

    [[nodiscard]] bool writeTestImageToDisk() const {
        return escribirImagenPPM(testInputPath, testImage);
    }

    [[nodiscard]] PPMImage& getTestImage() { return testImage; }
    void setTestImage(const PPMImage& image) { testImage = image; }
    [[nodiscard]] const std::string& getInputPath() const { return testInputPath; }
    [[nodiscard]] const std::string& getOutputPath() const { return testOutputPath; }
};

// Verifica que la función lanza una excepción para un número inválido de colores a eliminar (negativo)
TEST_F(CutFreqTest, ThrowsOnInvalidNumberOfColorsToCut) {
    ASSERT_TRUE(writeTestImageToDisk());

    EXPECT_THROW(
        cutfreq(getTestImage(), -1),
        std::invalid_argument
    );
}

// Verifica que la función lanza una excepción cuando se intenta eliminar más colores de los que existen en la imagen
TEST_F(CutFreqTest, ThrowsOnExcessiveNumberOfColorsToCut) {
    ASSERT_TRUE(writeTestImageToDisk());

    EXPECT_THROW(
        cutfreq(getTestImage(), 100),
        std::invalid_argument
    );
}

// Verifica el comportamiento con una imagen vacía
TEST_F(CutFreqTest, HandleEmptyImage) {
    PPMImage emptyImage;
    emptyImage.width = 0;
    emptyImage.height = 0;
    emptyImage.maxValue = static_cast<int>(MAX_8BIT);
    ASSERT_TRUE(escribirImagenPPM(getInputPath(), emptyImage));

    // Cambia el número de colores a eliminar a 0 ya que no hay colores en la imagen
    ASSERT_NO_THROW(
        cutfreq(emptyImage, 0)
    );

    EXPECT_EQ(emptyImage.width, 0);
    EXPECT_EQ(emptyImage.height, 0);
    EXPECT_TRUE(emptyImage.pixelData.empty());
}

// Verifica que se pueden eliminar los colores menos frecuentes sin errores
TEST_F(CutFreqTest, RemoveLeastFrequentColors) {
    ASSERT_TRUE(writeTestImageToDisk());

    PPMImage image = getTestImage();
    ASSERT_NO_THROW(
        cutfreq(image, 2)
    );

    // Verifica que el tamaño de la imagen no ha cambiado
    EXPECT_EQ(image.width, getTestImage().width);
    EXPECT_EQ(image.height, getTestImage().height);
    EXPECT_EQ(image.pixelData.size(), getTestImage().pixelData.size());
}

// Verifica que la función no cambia los colores si se solicita eliminar 0 colores
TEST_F(CutFreqTest, NoChangeWhenZeroColorsRemoved) {
    ASSERT_TRUE(writeTestImageToDisk());

    PPMImage image = getTestImage();
    ASSERT_NO_THROW(
        cutfreq(image, 0)
    );

    EXPECT_EQ(image.pixelData, getTestImage().pixelData);
}

// Verifica que se pueden eliminar todos los colores excepto uno
TEST_F(CutFreqTest, RemoveAllButOneColor) {
    ASSERT_TRUE(writeTestImageToDisk());

    PPMImage image = getTestImage();
    ASSERT_NO_THROW(
        cutfreq(image, static_cast<int>(image.pixelData.size() / 3) - 1)
    );

    // Verifica que solo queda un color en la imagen
    std::unordered_set<uint32_t> uniqueColors;
    for (std::size_t i = 0; i < image.pixelData.size(); i += 3) {
        const uint32_t color = (static_cast<uint32_t>(image.pixelData[i]) << SHIFT_RED) |
                               (static_cast<uint32_t>(image.pixelData[i + 1]) << SHIFT_GREEN) |
                               static_cast<uint32_t>(image.pixelData[i + 2]);
        uniqueColors.insert(color);
    }
    EXPECT_EQ(uniqueColors.size(), 1);
}

// Verifica el comportamiento cuando la imagen tiene un solo color
TEST_F(CutFreqTest, SingleColorImage) {
    PPMImage singleColorImage;
    singleColorImage.width = TEST_WIDTH;
    singleColorImage.height = TEST_HEIGHT;
    singleColorImage.maxValue = MAX_8BIT;
    singleColorImage.pixelData = {
        COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE,
        COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE,
        COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE, COLOR_SINGLE
    };

    ASSERT_TRUE(escribirImagenPPM(getInputPath(), singleColorImage));

    ASSERT_NO_THROW(
        cutfreq(singleColorImage, 1)
    );

    // Verifica que la imagen siga siendo de un solo color, aunque sea el valor de reemplazo (negro)
    std::unordered_set<uint32_t> uniqueColors;
    for (std::size_t i = 0; i < singleColorImage.pixelData.size(); i += 3) {
        const uint32_t color = (static_cast<uint32_t>(singleColorImage.pixelData[i]) << SHIFT_RED) |
                               (static_cast<uint32_t>(singleColorImage.pixelData[i + 1]) << SHIFT_GREEN) |
                               static_cast<uint32_t>(singleColorImage.pixelData[i + 2]);
        uniqueColors.insert(color);
    }
    EXPECT_EQ(uniqueColors.size(), 1);
}

// Verifica el comportamiento cuando se eliminan todos los colores posibles
TEST_F(CutFreqTest, RemoveAllColors) {
    ASSERT_TRUE(writeTestImageToDisk());

    PPMImage image = getTestImage();
    ASSERT_NO_THROW(
        cutfreq(image, static_cast<int>(image.pixelData.size() / 3))
    );

    // Verifica que todos los colores hayan sido reemplazados por el valor predeterminado (negro en este caso)
    for (std::size_t i = 0; i < image.pixelData.size(); i += 3) {
        EXPECT_EQ(image.pixelData[i], 0);
        EXPECT_EQ(image.pixelData[i + 1], 0);
        EXPECT_EQ(image.pixelData[i + 2], 0);
    }
}

// Verifica el comportamiento con un número grande de colores a eliminar, que sea igual al número de colores únicos
TEST_F(CutFreqTest, RemoveExactNumberOfUniqueColors) {
    ASSERT_TRUE(writeTestImageToDisk());

    PPMImage image = getTestImage();
    std::unordered_set<uint32_t> uniqueColors;
    for (std::size_t i = 0; i < image.pixelData.size(); i += 3) {
        const uint32_t color = (static_cast<uint32_t>(image.pixelData[i]) << SHIFT_RED) |
                               (static_cast<uint32_t>(image.pixelData[i + 1]) << SHIFT_GREEN) |
                               static_cast<uint32_t>(image.pixelData[i + 2]);
        uniqueColors.insert(color);
    }
    const int uniqueColorsCount = static_cast<int>(uniqueColors.size());

    ASSERT_NO_THROW(
        cutfreq(image, uniqueColorsCount)
    );

    // Verifica que la imagen tenga solo un color después de eliminar todos los colores menos uno
    uniqueColors.clear();
    for (std::size_t i = 0; i < image.pixelData.size(); i += 3) {
        const uint32_t color = (static_cast<uint32_t>(image.pixelData[i]) << SHIFT_RED) |
                               (static_cast<uint32_t>(image.pixelData[i + 1]) << SHIFT_GREEN) |
                               static_cast<uint32_t>(image.pixelData[i + 2]);
        uniqueColors.insert(color);
    }
    EXPECT_EQ(uniqueColors.size(), 1);
}

}  // namespace
