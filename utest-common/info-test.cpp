// File: utest-common/info-test.cpp
#include "../common/info.hpp"
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <array>

namespace {
    constexpr int MAX_COLOR_VALUE = 255;
    constexpr std::size_t PIXEL_DATA_SIZE = 9;  // Tamaño total de los datos de píxeles para 3 píxeles RGB
}

class InfoTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Crear un archivo PPM válido para pruebas
        std::ofstream file("test.ppm", std::ios::binary);
        file << "P6\n3 2\n" << MAX_COLOR_VALUE << "\n";

        // Utilizar std::array para almacenar los valores RGB de los píxeles
        std::array<unsigned char, PIXEL_DATA_SIZE> pixelData = {MAX_COLOR_VALUE, 0, 0,   // Rojo
                                                                0, MAX_COLOR_VALUE, 0,   // Verde
                                                                0, 0, MAX_COLOR_VALUE};  // Azul

        // Crear un buffer de string para escribir los datos binarios
        std::string buffer(pixelData.begin(), pixelData.end());
        file.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        file.close();
    }

    void TearDown() override {
        // Eliminar el archivo de prueba
        (void)std::remove("test.ppm");
    }
};

TEST_F(InfoTest, ValidPPMFile) {
    // Verificar que el archivo PPM válido se lee correctamente
    EXPECT_EQ(info("test.ppm"), 0);
}

TEST_F(InfoTest, NonExistentFile) {
    // Verificar que el programa maneja correctamente un archivo inexistente
    EXPECT_EQ(info("nonexistent.ppm"), -1);
}

TEST_F(InfoTest, IncorrectMagicNumber) {
    // Crear un archivo con un número mágico incorrecto
    std::ofstream file("incorrect_magic.ppm", std::ios::binary);
    file << "P3\n3 2\n" << MAX_COLOR_VALUE << "\n";
    file.close();

    // Verificar que el programa detecta el formato incorrecto
    EXPECT_EQ(info("incorrect_magic.ppm"), -1);

    // Limpiar
    (void)std::remove("incorrect_magic.ppm");
}

TEST_F(InfoTest, IncorrectHeaderFormat) {
    // Crear un archivo con un encabezado incorrecto
    std::ofstream file("incorrect_header.ppm", std::ios::binary);
    file << "P6\ninvalid_header\n" << MAX_COLOR_VALUE << "\n";
    file.close();

    // Verificar que el programa detecta el encabezado incorrecto
    EXPECT_EQ(info("incorrect_header.ppm"), -1);

    // Limpiar
    (void)std::remove("incorrect_header.ppm");
}
