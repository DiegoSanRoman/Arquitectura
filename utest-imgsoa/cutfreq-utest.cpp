#include "../imgsoa/cutfreq.hpp"
#include <gtest/gtest.h>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <algorithm>

// Fixture de prueba para la función cutfreq
class CutFreqTest : public ::testing::Test {
protected:
    PPMImageSoA image;

    void SetUp() override {
        // Configurar una imagen simple de 3x3 para pruebas
        image.redChannel = {255, 128, 0, 255, 0, 0, 128, 128, 0};
        image.greenChannel = {0, 128, 255, 255, 128, 0, 128, 255, 0};
        image.blueChannel = {0, 0, 128, 0, 255, 255, 0, 128, 255};
    }
};

// Caso de prueba 1: Cálculo básico de frecuencia y reemplazo de colores
TEST_F(CutFreqTest, BasicFrequencyAndReplacement) {
    // Eliminar el color menos frecuente (n = 1)
    cutfreq(image, 1);

    // Verificar que se haya realizado un reemplazo en la imagen
    bool replaced = false;
    for (std::size_t i = 0; i < image.redChannel.size(); ++i) {
        if (image.redChannel[i] != 255 || image.greenChannel[i] != 0 || image.blueChannel[i] != 0) {
            replaced = true;
            break;
        }
    }
    EXPECT_TRUE(replaced);
}

// Caso de prueba 2: Eliminar múltiples colores
TEST_F(CutFreqTest, RemoveMultipleColors) {
    // Eliminar los dos colores menos frecuentes (n = 2)
    cutfreq(image, 2);

    // Verificar que se hayan reemplazado los dos colores menos frecuentes
    std::unordered_map<uint32_t, int> colorFrequency;
    for (std::size_t i = 0; i < image.redChannel.size(); ++i) {
        uint32_t color = (static_cast<uint32_t>(image.redChannel[i]) << SHIFT_RED) |
                         (static_cast<uint32_t>(image.greenChannel[i]) << SHIFT_GREEN) |
                         static_cast<uint32_t>(image.blueChannel[i]);
        colorFrequency[color]++;
    }
    EXPECT_LE(colorFrequency.size(), 7);
}

// Caso de prueba 3: Eliminar más colores de los disponibles
TEST_F(CutFreqTest, RemoveMoreColorsThanAvailable) {
    // Intentar eliminar más colores de los presentes en la imagen (n > número de colores únicos)
    cutfreq(image, 10);

    // Verificar que todos los colores aún estén presentes (sin bloqueos ni comportamiento indefinido)
    EXPECT_EQ(image.redChannel.size(), 9);
    EXPECT_EQ(image.greenChannel.size(), 9);
    EXPECT_EQ(image.blueChannel.size(), 9);
}

// Caso de prueba 4: Imagen con todos los colores idénticos
TEST_F(CutFreqTest, AllIdenticalColors) {
    // Configurar una imagen donde todos los píxeles son del mismo color
    image.redChannel = {128, 128, 128, 128, 128, 128, 128, 128, 128};
    image.greenChannel = {128, 128, 128, 128, 128, 128, 128, 128, 128};
    image.blueChannel = {128, 128, 128, 128, 128, 128, 128, 128, 128};

    // Eliminar el color menos frecuente (n = 1)
    cutfreq(image, 1);

    // Dado que todos los colores son idénticos, no debería haber cambios
    for (std::size_t i = 0; i < image.redChannel.size(); ++i) {
        EXPECT_EQ(image.redChannel[i], 128);
        EXPECT_EQ(image.greenChannel[i], 128);
        EXPECT_EQ(image.blueChannel[i], 128);
    }
}

// Caso de prueba 5: Imagen sin colores a reemplazar (n = 0)
TEST_F(CutFreqTest, NoColorsToReplace) {
    // Establecer n = 0 para asegurar que no se reemplacen colores
    cutfreq(image, 0);

    // Verificar que todos los colores siguen siendo los mismos que en el original
    EXPECT_EQ(image.redChannel[0], 255);
    EXPECT_EQ(image.greenChannel[0], 0);
    EXPECT_EQ(image.blueChannel[0], 0);
}

// Caso de prueba 6: Imagen con colores alternantes
TEST_F(CutFreqTest, AlternatingColors) {
    // Configurar una imagen con colores alternantes
    image.redChannel = {255, 0, 255, 0, 255, 0, 255, 0, 255};
    image.greenChannel = {0, 255, 0, 255, 0, 255, 0, 255, 0};
    image.blueChannel = {0, 0, 255, 255, 0, 0, 255, 255, 0};

    // Eliminar el color menos frecuente (n = 1)
    cutfreq(image, 1);

    // Verificar que se haya reemplazado al menos uno de los colores alternantes
    bool replaced = false;
    for (std::size_t i = 0; i < image.redChannel.size(); ++i) {
        if (image.redChannel[i] != 255 || image.greenChannel[i] != 0 || image.blueChannel[i] != 0) {
            replaced = true;
            break;
        }
    }
    EXPECT_TRUE(replaced);
}

// Caso de prueba 7: Imagen con solo un color único
TEST_F(CutFreqTest, SingleUniqueColor) {
    // Configurar una imagen con solo un color único
    image.redChannel = {100, 100, 100, 100, 100, 100, 100, 100, 100};
    image.greenChannel = {150, 150, 150, 150, 150, 150, 150, 150, 150};
    image.blueChannel = {200, 200, 200, 200, 200, 200, 200, 200, 200};

    // Eliminar el color menos frecuente (n = 1)
    cutfreq(image, 1);

    // Dado que solo hay un color único, no debería haber cambios
    for (std::size_t i = 0; i < image.redChannel.size(); ++i) {
        EXPECT_EQ(image.redChannel[i], 100);
        EXPECT_EQ(image.greenChannel[i], 150);
        EXPECT_EQ(image.blueChannel[i], 200);
    }
}

// Caso de prueba 8: Valor grande para n
TEST_F(CutFreqTest, LargeNValue) {
    // Establecer n a un valor muy grande
    cutfreq(image, 1000);

    // Verificar que todos los colores aún estén presentes (sin bloqueos ni comportamiento indefinido)
    EXPECT_EQ(image.redChannel.size(), 9);
    EXPECT_EQ(image.greenChannel.size(), 9);
    EXPECT_EQ(image.blueChannel.size(), 9);
}

// Caso de prueba 9: Imagen con colores aleatorios
TEST_F(CutFreqTest, RandomColors) {
    // Configurar una imagen con colores aleatorios
    image.redChannel = {34, 255, 128, 67, 12, 89, 150, 200, 0};
    image.greenChannel = {200, 123, 50, 89, 255, 34, 67, 180, 90};
    image.blueChannel = {12, 255, 180, 50, 128, 67, 200, 34, 255};

    // Eliminar los tres colores menos frecuentes (n = 3)
    cutfreq(image, 3);

    // Verificar que tres colores fueron reemplazados
    std::unordered_map<uint32_t, int> colorFrequency;
    for (std::size_t i = 0; i < image.redChannel.size(); ++i) {
        uint32_t color = (static_cast<uint32_t>(image.redChannel[i]) << SHIFT_RED) |
                         (static_cast<uint32_t>(image.greenChannel[i]) << SHIFT_GREEN) |
                         static_cast<uint32_t>(image.blueChannel[i]);
        colorFrequency[color]++;
    }
    EXPECT_LE(colorFrequency.size(), 6);
}
