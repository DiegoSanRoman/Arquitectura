#include "../common/binario.hpp"
#include <gtest/gtest.h>
#include <fstream>
namespace {
  constexpr int MAX_COLOR_VALUE = 255;
  constexpr int ALT_COLOR_VALUE = 128;
  constexpr int LOW_COLOR_VALUE = 64;
  constexpr size_t PIXEL_DATA_SIZE = 18;
}

TEST(BinarioTest, LeerImagenPPM_CorrectHeader) {
    const std::string filePath = "./test_image.ppm";
    std::ofstream file(filePath, std::ios::binary);

    // Encabezado para una imagen PPM (P6) de 3x2 con valor máximo de color 255
    file << "P6\n3 2\n" << MAX_COLOR_VALUE << "\n";

    // Escribir datos de píxeles en binario (6 píxeles)
    const std::array<char, PIXEL_DATA_SIZE> pixelData = {
        static_cast<char>(MAX_COLOR_VALUE), 0, 0,  // Rojo
        0, static_cast<char>(MAX_COLOR_VALUE), 0,  // Verde
        0, 0, static_cast<char>(MAX_COLOR_VALUE),  // Azul
        static_cast<char>(MAX_COLOR_VALUE), static_cast<char>(MAX_COLOR_VALUE), static_cast<char>(MAX_COLOR_VALUE), // Blanco
        static_cast<char>(ALT_COLOR_VALUE), static_cast<char>(ALT_COLOR_VALUE), static_cast<char>(ALT_COLOR_VALUE), // Gris
        static_cast<char>(LOW_COLOR_VALUE), static_cast<char>(LOW_COLOR_VALUE), static_cast<char>(LOW_COLOR_VALUE)     // Gris oscuro
    };
    file.write(pixelData.data(), pixelData.size());
    file.close();

    // Intentar leer la imagen
    PPMImage image;
    ASSERT_TRUE(leerImagenPPM(filePath, image)) << "Error: No se pudo leer la imagen PPM.";

    // Verificaciones
    EXPECT_EQ(image.width, 3);
    EXPECT_EQ(image.height, 2);
    EXPECT_EQ(image.maxValue, MAX_COLOR_VALUE);
    EXPECT_EQ(image.pixelData.size(), pixelData.size());
    EXPECT_EQ(image.pixelData[0], static_cast<unsigned char>(MAX_COLOR_VALUE));
    EXPECT_EQ(image.pixelData[3], 0);
    EXPECT_EQ(image.pixelData[4], static_cast<unsigned char>(MAX_COLOR_VALUE));

    // Limpieza
    (void)std::remove(filePath.c_str());
}




TEST(BinarioTest, EscribirImagenPPM_WriteReadCycle) {
    const std::string filePath = "./test_image_write.ppm";
    PPMImage image;
    image.width = 3;
    image.height = 2;
    image.maxValue = MAX_COLOR_VALUE;
    image.pixelData = {
        static_cast<uint8_t>(MAX_COLOR_VALUE), 0, 0,
        0, static_cast<uint8_t>(MAX_COLOR_VALUE), 0,
        0, 0, static_cast<uint8_t>(MAX_COLOR_VALUE),
        static_cast<uint8_t>(MAX_COLOR_VALUE), static_cast<uint8_t>(MAX_COLOR_VALUE), static_cast<uint8_t>(MAX_COLOR_VALUE),
        ALT_COLOR_VALUE, ALT_COLOR_VALUE, ALT_COLOR_VALUE,
        LOW_COLOR_VALUE, LOW_COLOR_VALUE, LOW_COLOR_VALUE
    };

    EXPECT_TRUE(escribirImagenPPM(filePath, image));

    PPMImage readImage;
    EXPECT_TRUE(leerImagenPPM(filePath, readImage));
    EXPECT_EQ(readImage.width, image.width);
    EXPECT_EQ(readImage.height, image.height);
    EXPECT_EQ(readImage.maxValue, image.maxValue);
    EXPECT_EQ(readImage.pixelData, image.pixelData);
    (void)std::remove(filePath.c_str());
}

TEST(BinarioTest, LeerImagenPPMSoA_CorrectChannels) {
    const std::string filePath = "./test_image_soa.ppm";
    std::ofstream file(filePath, std::ios::binary);
    file << "P6\n2 2\n" << MAX_COLOR_VALUE << "\n";
    file.put(static_cast<char>(MAX_COLOR_VALUE)).put(0).put(0); // Rojo
    file.put(0).put(static_cast<char>(MAX_COLOR_VALUE)).put(0); // Verde
    file.put(0).put(0).put(static_cast<char>(MAX_COLOR_VALUE)); // Azul
    file.put(static_cast<char>(MAX_COLOR_VALUE)).put(static_cast<char>(MAX_COLOR_VALUE)).put(static_cast<char>(MAX_COLOR_VALUE)); // Blanco
    file.close();

    PPMImageSoA imageSoA;
    EXPECT_TRUE(leerImagenPPMSoA(filePath, imageSoA));
    EXPECT_EQ(imageSoA.width, 2);
    EXPECT_EQ(imageSoA.height, 2);
    EXPECT_EQ(imageSoA.maxValue, MAX_COLOR_VALUE);
    EXPECT_EQ(imageSoA.redChannel, std::vector<uint8_t>({MAX_COLOR_VALUE, 0, 0, MAX_COLOR_VALUE}));
    EXPECT_EQ(imageSoA.greenChannel, std::vector<uint8_t>({0, MAX_COLOR_VALUE, 0, MAX_COLOR_VALUE}));
    EXPECT_EQ(imageSoA.blueChannel, std::vector<uint8_t>({0, 0, MAX_COLOR_VALUE, MAX_COLOR_VALUE}));
    (void)std::remove(filePath.c_str());
}

TEST(BinarioTest, EscribirImagenPPMSoA_WriteReadCycle) {
    const std::string filePath = "./test_image_write_soa.ppm";
    PPMImageSoA imageSoA;
    imageSoA.width = 2;
    imageSoA.height = 2;
    imageSoA.maxValue = MAX_COLOR_VALUE;
    imageSoA.redChannel = {MAX_COLOR_VALUE, 0, 0, MAX_COLOR_VALUE};
    imageSoA.greenChannel = {0, MAX_COLOR_VALUE, 0, MAX_COLOR_VALUE};
    imageSoA.blueChannel = {0, 0, MAX_COLOR_VALUE, MAX_COLOR_VALUE};

    EXPECT_TRUE(escribirImagenPPMSoA(filePath, imageSoA));

    PPMImageSoA readImageSoA;
    EXPECT_TRUE(leerImagenPPMSoA(filePath, readImageSoA));
    EXPECT_EQ(readImageSoA.width, imageSoA.width);
    EXPECT_EQ(readImageSoA.height, imageSoA.height);
    EXPECT_EQ(readImageSoA.maxValue, imageSoA.maxValue);
    EXPECT_EQ(readImageSoA.redChannel, imageSoA.redChannel);
    EXPECT_EQ(readImageSoA.greenChannel, imageSoA.greenChannel);
    EXPECT_EQ(readImageSoA.blueChannel, imageSoA.blueChannel);
    (void)std::remove(filePath.c_str());
}

TEST(BinarioTest, LeerImagenCPPM_ValidHeaderAndTable) {
    const std::string filePath = "./test_image.cppm";
    std::ofstream file(filePath, std::ios::binary);
    file << "C6\n2 2\n" << MAX_COLOR_VALUE << " 4\n";
    file.put(static_cast<char>(MAX_COLOR_VALUE)).put(0).put(0); // Color 1: Rojo
    file.put(0).put(static_cast<char>(MAX_COLOR_VALUE)).put(0); // Color 2: Verde
    file.put(0).put(0).put(static_cast<char>(MAX_COLOR_VALUE)); // Color 3: Azul
    file.put(static_cast<char>(MAX_COLOR_VALUE)).put(static_cast<char>(MAX_COLOR_VALUE)).put(static_cast<char>(MAX_COLOR_VALUE)); // Color 4: Blanco
    file.put(0).put(1).put(2).put(3); // Indices de colores
    file.close();

    PPMImage image;
    EXPECT_TRUE(leerImagenCPPM(filePath, image));
    EXPECT_EQ(image.width, 2);
    EXPECT_EQ(image.height, 2);
    EXPECT_EQ(image.maxValue, MAX_COLOR_VALUE);
    EXPECT_EQ(image.pixelData, std::vector<uint8_t>({MAX_COLOR_VALUE, 0, 0, 0, MAX_COLOR_VALUE, 0, 0, 0, MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE, MAX_COLOR_VALUE}));
    (void)std::remove(filePath.c_str());
}
