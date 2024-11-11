#include "../common/binario.hpp"
#include "../imgaos/compress.hpp"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <vector>

namespace {

constexpr unsigned int MAX_COLOR_VALUE = 255U;
constexpr unsigned int UNIQUE_COLORS_DIVERSE = 15;
constexpr unsigned int UNIQUE_COLORS_EDGE = 7;

struct ImageMetadata {
    unsigned int width;
    unsigned int height;
    unsigned int maxValue;
    size_t uniqueColors;
};

class FunctionalCompressAOSTest : public ::testing::Test {
public:
    std::string testFilesDir;
    std::string testInputPath;
    std::string testOutputPath;
protected:
    FunctionalCompressAOSTest()
        : testFilesDir("./test_files/"),
          testInputPath(testFilesDir + "lake-aos.ppm"),
          testOutputPath(testFilesDir + "lake-aos-compress.cppm") {}

    void SetUp() override {
        std::filesystem::create_directories(testFilesDir);
    }

    void TearDown() override {
        std::filesystem::remove_all(testFilesDir);
    }

    struct ImageDimensions {
        unsigned int width;
        unsigned int height;
    };

    [[nodiscard]] static bool createTestImage(const std::string& filePath, ImageDimensions dimensions, const std::vector<uint8_t>& pixelData) {
        PPMImage image;
        image.width = static_cast<int>(dimensions.width);
        image.height = static_cast<int>(dimensions.height);
        image.maxValue = MAX_COLOR_VALUE;
        image.pixelData = pixelData;
        return escribirImagenPPM(filePath, image);
    }

    [[nodiscard]] static bool verifyOutputHeader(const std::string& filePath, const ImageMetadata& expectedMetadata) {
        return checkHeader(filePath, expectedMetadata);
    }

private:
    [[nodiscard]] static bool checkHeader(const std::string& filePath, const ImageMetadata& metadata) {
        std::ifstream outputFile(filePath, std::ios::binary);
        if (!outputFile.is_open()) {
            return false;
        }

        std::string header;
        unsigned int readWidth = 0;
        unsigned int readHeight = 0;
        unsigned int readMaxValue = 0;
        size_t readUniqueColors = 0;

        outputFile >> header >> readWidth >> readHeight >> readMaxValue >> readUniqueColors;

        // Imprimir valores leídos y esperados para diagnóstico
        std::cout << "Header: " << header << ", Width: " << readWidth
                  << ", Height: " << readHeight << ", Max Value: " << readMaxValue
                  << ", Unique Colors: " << readUniqueColors << '\n';

        std::cout << "Expected - Width: " << metadata.width
                  << ", Height: " << metadata.height << ", Max Value: "
                  << metadata.maxValue << ", Unique Colors: " << metadata.uniqueColors
                  << '\n';

        return header == "C6" && readWidth == metadata.width && readHeight == metadata.height &&
               readMaxValue == metadata.maxValue && readUniqueColors == metadata.uniqueColors;
    }
};

// Test funcional para la compresión de una imagen con colores diversos
TEST_F(FunctionalCompressAOSTest, CompressImageWithDiverseColors) {
    ImageDimensions const dimensions = {.width=4, .height=4};
    const std::vector<uint8_t> pixelData = {
        MAX_COLOR_VALUE, 0, 0,         0, MAX_COLOR_VALUE, 0,         0, 0, MAX_COLOR_VALUE,       MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE,
        0, MAX_COLOR_VALUE / 2, 0,     0, 0, MAX_COLOR_VALUE / 2,     MAX_COLOR_VALUE / 2, 0, 0,   MAX_COLOR_VALUE / 2, MAX_COLOR_VALUE / 2, 0,
        MAX_COLOR_VALUE / 4, 0, 0,     0, MAX_COLOR_VALUE / 4, 0,     0, 0, MAX_COLOR_VALUE / 4,   MAX_COLOR_VALUE / 4, MAX_COLOR_VALUE / 4, MAX_COLOR_VALUE / 4,
        MAX_COLOR_VALUE, 0, MAX_COLOR_VALUE, 0, MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE, 0, MAX_COLOR_VALUE, 0, MAX_COLOR_VALUE
    };

    ASSERT_TRUE(createTestImage(testInputPath, dimensions, pixelData));

    const common::CompressionPaths paths = { .inputImagePath = testInputPath, .outputImagePath = testOutputPath };
    EXPECT_EQ(compress(paths), 0) << "La compresión debería completarse sin errores";

    EXPECT_TRUE(std::filesystem::exists(testOutputPath)) << "El archivo de salida debería generarse correctamente";

    ImageMetadata const expectedMetadata = {.width=dimensions.width, .height=dimensions.height, .maxValue=MAX_COLOR_VALUE, .uniqueColors=UNIQUE_COLORS_DIVERSE};
    EXPECT_TRUE(verifyOutputHeader(testOutputPath, expectedMetadata))
        << "El encabezado del archivo de salida debería indicar los colores y dimensiones correctas";
}

// Test funcional para comprimir una imagen con un solo color (imagen homogénea)
TEST_F(FunctionalCompressAOSTest, CompressSingleColorImage) {
    ImageDimensions const dimensions = {.width=2, .height=2};
    const std::vector<uint8_t> pixelData = {
        MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE,
        MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE,
        MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE,
        MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE
    };

    ASSERT_TRUE(createTestImage(testInputPath, dimensions, pixelData));

    const common::CompressionPaths paths = { .inputImagePath = testInputPath, .outputImagePath = testOutputPath };
    EXPECT_EQ(compress(paths), 0) << "La compresión debería completarse sin errores";

    EXPECT_TRUE(std::filesystem::exists(testOutputPath)) << "El archivo de salida debería generarse correctamente";

    ImageMetadata const expectedMetadata = {.width=dimensions.width, .height=dimensions.height, .maxValue=MAX_COLOR_VALUE, .uniqueColors=1};
    EXPECT_TRUE(verifyOutputHeader(testOutputPath, expectedMetadata))
        << "El encabezado del archivo de salida debería indicar un único color";
}

// Test funcional para comprimir una imagen con valores de color límite (0 y 255)
TEST_F(FunctionalCompressAOSTest, CompressEdgeColorValuesImage) {
    ImageDimensions const dimensions = {.width=3, .height=3};
    const std::vector<uint8_t> pixelData = {
        0, 0, 0,                       MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE,           0, MAX_COLOR_VALUE, 0,
        MAX_COLOR_VALUE, 0, 0,         0, 0, MAX_COLOR_VALUE,                                   MAX_COLOR_VALUE, MAX_COLOR_VALUE, 0,
        MAX_COLOR_VALUE / 2, MAX_COLOR_VALUE / 2, MAX_COLOR_VALUE / 2,   0, 0, 0,         MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE
    };

    ASSERT_TRUE(createTestImage(testInputPath, dimensions, pixelData));

    const common::CompressionPaths paths = { .inputImagePath = testInputPath, .outputImagePath = testOutputPath };
    EXPECT_EQ(compress(paths), 0) << "La compresión debería completarse sin errores";

    EXPECT_TRUE(std::filesystem::exists(testOutputPath)) << "El archivo de salida debería generarse correctamente";

    ImageMetadata const expectedMetadata = {.width=dimensions.width, .height=dimensions.height, .maxValue=MAX_COLOR_VALUE, .uniqueColors=UNIQUE_COLORS_EDGE};
    EXPECT_TRUE(verifyOutputHeader(testOutputPath, expectedMetadata))
        << "El encabezado del archivo de salida debería indicar los colores límite";
}

}  // namespace
