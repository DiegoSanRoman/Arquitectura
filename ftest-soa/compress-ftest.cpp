#include "../common/binario.hpp"
#include "../imgsoa/compress.hpp"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <vector>

namespace {

// Rutas y constantes de prueba
constexpr unsigned int MAX_COLOR_VALUE = 255U;
constexpr unsigned int UNIQUE_COLORS_DIVERSE = 15;
constexpr unsigned int UNIQUE_COLORS_EDGE = 7;
constexpr unsigned int TEST_WIDTH = 4;
constexpr unsigned int TEST_HEIGHT = 4;

class CompressSOATest : public ::testing::Test {
public:
    PPMImage testImage;
    const std::string testInputPath{"./test_files/lake-small-soa.ppm"};
    const std::string testOutputPath{"./test_files/lake-compress-soa.cppm"};
protected:
    void SetUp() override {
        std::filesystem::create_directories("./test_files/");
        testImage.width = TEST_WIDTH;
        testImage.height = TEST_HEIGHT;
        testImage.maxValue = MAX_COLOR_VALUE;
    }

    void TearDown() override {
        std::filesystem::remove_all("./test_files/");
    }

    [[nodiscard]] bool writeTestImageToDisk() const {
        return escribirImagenPPM(testInputPath, testImage);
    }

    [[nodiscard]] bool verifyOutputHeader(unsigned int expectedWidth, unsigned int expectedHeight, unsigned int expectedMaxValue, size_t expectedUniqueColors) const {
      std::ifstream outputFile(testOutputPath, std::ios::binary);
      if (!outputFile.is_open()){ return false;}

      std::string header;
      unsigned int width = 0;
      unsigned int height = 0;
      unsigned int maxValue = 0;
      size_t uniqueColorCount = 0;

      outputFile >> header >> width >> height >> maxValue >> uniqueColorCount;

      // Debug output to see actual values
      std::cout << "Actual Header: " << header << " " << width << " " << height << " " << maxValue << " " << uniqueColorCount << "\n";
      std::cout << "Expected Header: C6 " << expectedWidth << " " << expectedHeight << " " << expectedMaxValue << " " << expectedUniqueColors << '\n';

      return header == "C6" && width == expectedWidth && height == expectedHeight &&
             maxValue == expectedMaxValue && uniqueColorCount == expectedUniqueColors;
    }


    [[nodiscard]] const std::string& getInputPath() const { return testInputPath; }
    [[nodiscard]] const std::string& getOutputPath() const { return testOutputPath; }
};

// Test funcional para la compresión de una imagen con colores diversos (caso de uso real)
TEST_F(CompressSOATest, CompressImageWithDiverseColors) {
    testImage.pixelData = {
        MAX_COLOR_VALUE, 0, 0,         0, MAX_COLOR_VALUE, 0,         0, 0, MAX_COLOR_VALUE,       MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE,
        0, MAX_COLOR_VALUE / 2, 0,     0, 0, MAX_COLOR_VALUE / 2,     MAX_COLOR_VALUE / 2, 0, 0,   MAX_COLOR_VALUE / 2, MAX_COLOR_VALUE / 2, 0,
        MAX_COLOR_VALUE / 4, 0, 0,     0, MAX_COLOR_VALUE / 4, 0,     0, 0, MAX_COLOR_VALUE / 4,   MAX_COLOR_VALUE / 4, MAX_COLOR_VALUE / 4, MAX_COLOR_VALUE / 4,
        MAX_COLOR_VALUE, 0, MAX_COLOR_VALUE, 0, MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE, 0, MAX_COLOR_VALUE, 0, MAX_COLOR_VALUE
    };

    ASSERT_TRUE(writeTestImageToDisk());

    const common::CompressionPaths paths = { .inputImagePath = getInputPath(), .outputImagePath = getOutputPath() };
    EXPECT_EQ(compress(paths), 0) << "La compresión debería completarse sin errores";
    EXPECT_TRUE(std::filesystem::exists(getOutputPath())) << "El archivo de salida debería generarse correctamente";
    EXPECT_TRUE(verifyOutputHeader(TEST_WIDTH, TEST_HEIGHT, MAX_COLOR_VALUE, UNIQUE_COLORS_DIVERSE))
        << "El encabezado del archivo de salida debería indicar los colores y dimensiones correctas";
}

// Test funcional para comprimir una imagen con un solo color (imagen homogénea)
TEST_F(CompressSOATest, CompressSingleColorImage) {
    testImage.width = 2;
    testImage.height = 2;
    testImage.pixelData = {
        MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE,
        MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE,
        MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE,
        MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE
    };

    ASSERT_TRUE(writeTestImageToDisk());

    const common::CompressionPaths paths = { .inputImagePath = getInputPath(), .outputImagePath = getOutputPath() };
    EXPECT_EQ(compress(paths), 0) << "La compresión debería completarse sin errores";
    EXPECT_TRUE(std::filesystem::exists(getOutputPath())) << "El archivo de salida debería generarse correctamente";
    EXPECT_TRUE(verifyOutputHeader(2, 2, MAX_COLOR_VALUE, 1))
        << "El encabezado del archivo de salida debería indicar un único color";
}

// Test funcional para comprimir una imagen con valores de color límite (0 y 255)
TEST_F(CompressSOATest, CompressEdgeColorValuesImage) {
    testImage.width = 3;
    testImage.height = 3;
    testImage.pixelData = {
        0, 0, 0,                       MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE,           0, MAX_COLOR_VALUE, 0,
        MAX_COLOR_VALUE, 0, 0,         0, 0, MAX_COLOR_VALUE,                                   MAX_COLOR_VALUE, MAX_COLOR_VALUE, 0,
        MAX_COLOR_VALUE / 2, MAX_COLOR_VALUE / 2, MAX_COLOR_VALUE / 2,   0, 0, 0,         MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE
    };

    ASSERT_TRUE(writeTestImageToDisk());

    const common::CompressionPaths paths = { .inputImagePath = getInputPath(), .outputImagePath = getOutputPath() };
    EXPECT_EQ(compress(paths), 0) << "La compresión debería completarse sin errores";
    EXPECT_TRUE(std::filesystem::exists(getOutputPath())) << "El archivo de salida debería generarse correctamente";
    EXPECT_TRUE(verifyOutputHeader(3, 3, MAX_COLOR_VALUE, UNIQUE_COLORS_EDGE))
        << "El encabezado del archivo de salida debería indicar los colores límite";
}

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
