#include "../common/binario.hpp"
#include "../imgsoa/compress.hpp"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

namespace {

constexpr unsigned int TEST_WIDTH = 2U;
constexpr unsigned int TEST_HEIGHT = 2U;
constexpr unsigned int MAX_COLOR_VALUE = 255U;

class CompressAOSTest : public ::testing::Test {
private:
    PPMImage testImage;
    const std::string testInputPath{"./test_files/lake-small.ppm"};
    const std::string testOutputPath{"./test_files/lake-compress.cppm"};

protected:
    void SetUp() override {
        // Crear el directorio si no existe
        std::filesystem::create_directories("./test_files/");
        testImage.width = TEST_WIDTH;
        testImage.height = TEST_HEIGHT;
        testImage.maxValue = MAX_COLOR_VALUE;
        testImage.pixelData = {
            MAX_COLOR_VALUE, 0, 0,               // Rojo
            0, MAX_COLOR_VALUE, 0,               // Verde
            0, 0, MAX_COLOR_VALUE,               // Azul
            MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE  // Blanco
        };
    }

    void TearDown() override {
        std::filesystem::remove_all("./test_files/");
    }

    [[nodiscard]] bool writeTestImageToDisk() const {
        return escribirImagenPPM(testInputPath, testImage);
    }

    [[nodiscard]] const std::string& getInputPath() const { return testInputPath; }
    [[nodiscard]] const std::string& getOutputPath() const { return testOutputPath; }
};

// Prueba para verificar la compresión básica de una imagen válida
TEST_F(CompressAOSTest, BasicCompression) {
    ASSERT_TRUE(writeTestImageToDisk());

    const common::CompressionPaths paths = { .inputImagePath = getInputPath(), .outputImagePath = getOutputPath() };
    const int result = compress(paths);
    EXPECT_EQ(result, 0) << "La compresión debería completarse sin errores";

    EXPECT_TRUE(std::filesystem::exists(getOutputPath())) << "El archivo de salida debería generarse correctamente";
}

// Prueba que verifica el retorno de compresión cuando el archivo de entrada no existe
TEST_F(CompressAOSTest, InvalidInputFile) {
    const common::CompressionPaths paths = { .inputImagePath = "nonexistent.ppm", .outputImagePath = getOutputPath() };

    const int result = compress(paths);
    EXPECT_EQ(result, -1) << "La compresión debería fallar y retornar -1 para un archivo de entrada inexistente";
    EXPECT_FALSE(std::filesystem::exists(getOutputPath())) << "No debería generarse un archivo de salida para una entrada inválida";
}

// Prueba que verifica la compresión de una imagen vacía (sin píxeles)
TEST_F(CompressAOSTest, HandleEmptyImage) {
    PPMImage emptyImage;
    emptyImage.width = 0;
    emptyImage.height = 0;
    emptyImage.maxValue = MAX_COLOR_VALUE;
    ASSERT_TRUE(escribirImagenPPM(getInputPath(), emptyImage));

    const common::CompressionPaths paths = { .inputImagePath = getInputPath(), .outputImagePath = getOutputPath() };
    const int result = compress(paths);
    EXPECT_EQ(result, 0) << "La compresión de una imagen vacía debería completarse sin errores";

    EXPECT_TRUE(std::filesystem::exists(getOutputPath())) << "El archivo de salida debería generarse para una imagen vacía";
}

TEST_F(CompressAOSTest, UniqueColorsCount) {
    ASSERT_TRUE(writeTestImageToDisk());

    const common::CompressionPaths paths = { .inputImagePath = getInputPath(), .outputImagePath = getOutputPath() };
    const int result = compress(paths);
    EXPECT_EQ(result, 0) << "La compresión debería completarse sin errores";

    // Verificar que el archivo de salida exista
    ASSERT_TRUE(std::filesystem::exists(getOutputPath())) << "El archivo de salida debería existir después de la compresión";

    // Verificar el encabezado C6
    std::ifstream outputFile(getOutputPath(), std::ios::binary);
    ASSERT_TRUE(outputFile.is_open()) << "No se pudo abrir el archivo de salida para verificar el encabezado";

    std::string header;
    outputFile >> header;
    EXPECT_EQ(header, "C6") << "El encabezado del archivo de salida debería ser 'C6' para un formato C6 válido";

    // Intentar leer la imagen completa después de verificar el encabezado
    PPMImage resultImage;
    ASSERT_TRUE(leerImagenCPPM(getOutputPath(), resultImage)) << "El archivo de salida debería poder leerse correctamente";

    const int expectedUniqueColors = 4; // Rojo, verde, azul y blanco
    EXPECT_EQ(resultImage.pixelData.size() / 3, expectedUniqueColors) << "El número de colores únicos debería ser el esperado";
}


}  // namespace
