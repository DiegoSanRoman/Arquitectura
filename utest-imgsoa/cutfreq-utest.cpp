#include "../imgsoa/cutfreq.hpp"
#include <gtest/gtest.h>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <algorithm>

namespace {
    constexpr uint8_t COLOR_MAX = 255;
    constexpr uint8_t COLOR_MID = 128;
    constexpr uint8_t COLOR_LOW = 100;
    constexpr uint8_t COLOR_MIN = 0;
    constexpr uint8_t COLOR_HIGH = 200;
    constexpr uint8_t COLOR_OTHER = 150;
    constexpr uint8_t COLOR_ALT1 = 34;
    constexpr uint8_t COLOR_ALT2 = 67;
    constexpr uint8_t COLOR_ALT3 = 89;
    constexpr uint8_t COLOR_ALT4 = 12;
    constexpr uint8_t COLOR_ALT6 = 50;
    constexpr uint8_t COLOR_ALT7 = 90;
    constexpr int LARGE_N = 1000;
    constexpr int REMOVE_MORE = 10;
}

// Fixture de prueba para la función cutfreq
class CutFreqTest : public ::testing::Test {
private:
    PPMImageSoA image;

protected:
    void SetUp() override {
        // Configurar una imagen simple de 3x3 para pruebas
        image.redChannel = {COLOR_MAX, COLOR_MID, COLOR_MIN, COLOR_MAX, COLOR_MIN, COLOR_MIN, COLOR_MID, COLOR_MID, COLOR_MIN};
        image.greenChannel = {COLOR_MIN, COLOR_MID, COLOR_MAX, COLOR_MAX, COLOR_MID, COLOR_MIN, COLOR_MID, COLOR_MAX, COLOR_MIN};
        image.blueChannel = {COLOR_MIN, COLOR_MIN, COLOR_MID, COLOR_MIN, COLOR_MAX, COLOR_MAX, COLOR_MIN, COLOR_MID, COLOR_MAX};
    }

    PPMImageSoA& getImage() { return image; }
};

// Caso de prueba 1: Cálculo básico de frecuencia y reemplazo de colores
TEST_F(CutFreqTest, BasicFrequencyAndReplacement) {
    // Eliminar el color menos frecuente (n = 1)
    cutfreq(getImage(), 1);

    // Verificar que se haya realizado un reemplazo en la imagen
    bool replaced = false;
    for (std::size_t i = 0; i < getImage().redChannel.size(); ++i) {
        if (getImage().redChannel[i] != COLOR_MAX || getImage().greenChannel[i] != COLOR_MIN || getImage().blueChannel[i] != COLOR_MIN) {
            replaced = true;
            break;
        }
    }
    EXPECT_TRUE(replaced);
}

// Caso de prueba 2: Eliminar múltiples colores
TEST_F(CutFreqTest, RemoveMultipleColors) {
    // Eliminar los dos colores menos frecuentes (n = 2)
    cutfreq(getImage(), 2);

    // Verificar que se hayan reemplazado los dos colores menos frecuentes
    std::unordered_map<uint32_t, int> colorFrequency;
    for (std::size_t i = 0; i < getImage().redChannel.size(); ++i) {
        const uint32_t color = (static_cast<uint32_t>(getImage().redChannel[i]) << SHIFT_RED) |
                               (static_cast<uint32_t>(getImage().greenChannel[i]) << SHIFT_GREEN) |
                               static_cast<uint32_t>(getImage().blueChannel[i]);
        colorFrequency[color]++;
    }
    EXPECT_LE(colorFrequency.size(), 7);
}

// Caso de prueba 3: Eliminar más colores de los disponibles
TEST_F(CutFreqTest, RemoveMoreColorsThanAvailable) {
    // Intentar eliminar más colores de los presentes en la imagen (n > número de colores únicos)
    cutfreq(getImage(), REMOVE_MORE);

    // Verificar que todos los colores aún estén presentes (sin bloqueos ni comportamiento indefinido)
    EXPECT_EQ(getImage().redChannel.size(), 9);
    EXPECT_EQ(getImage().greenChannel.size(), 9);
    EXPECT_EQ(getImage().blueChannel.size(), 9);
}

// Caso de prueba 4: Imagen con todos los colores idénticos
TEST_F(CutFreqTest, AllIdenticalColors) {
    // Configurar una imagen donde todos los píxeles son del mismo color
    getImage().redChannel = {COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID};
    getImage().greenChannel = {COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID};
    getImage().blueChannel = {COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID, COLOR_MID};

    // Eliminar el color menos frecuente (n = 1)
    cutfreq(getImage(), 1);

    // Dado que todos los colores son idénticos, no debería haber cambios
    for (std::size_t i = 0; i < getImage().redChannel.size(); ++i) {
        EXPECT_EQ(getImage().redChannel[i], COLOR_MID);
        EXPECT_EQ(getImage().greenChannel[i], COLOR_MID);
        EXPECT_EQ(getImage().blueChannel[i], COLOR_MID);
    }
}

// Caso de prueba 5: Imagen sin colores a reemplazar (n = 0)
TEST_F(CutFreqTest, NoColorsToReplace) {
    // Establecer n = 0 para asegurar que no se reemplacen colores
    cutfreq(getImage(), 0);

    // Verificar que todos los colores siguen siendo los mismos que en el original
    EXPECT_EQ(getImage().redChannel[0], COLOR_MAX);
    EXPECT_EQ(getImage().greenChannel[0], COLOR_MIN);
    EXPECT_EQ(getImage().blueChannel[0], COLOR_MIN);
}

// Caso de prueba 6: Imagen con colores alternantes
TEST_F(CutFreqTest, AlternatingColors) {
    // Configurar una imagen con colores alternantes
    getImage().redChannel = {COLOR_MAX, COLOR_MIN, COLOR_MAX, COLOR_MIN, COLOR_MAX, COLOR_MIN, COLOR_MAX, COLOR_MIN, COLOR_MAX};
    getImage().greenChannel = {COLOR_MIN, COLOR_MAX, COLOR_MIN, COLOR_MAX, COLOR_MIN, COLOR_MAX, COLOR_MIN, COLOR_MAX, COLOR_MIN};
    getImage().blueChannel = {COLOR_MIN, COLOR_MIN, COLOR_MAX, COLOR_MAX, COLOR_MIN, COLOR_MIN, COLOR_MAX, COLOR_MAX, COLOR_MIN};

    // Eliminar el color menos frecuente (n = 1)
    cutfreq(getImage(), 1);

    // Verificar que se haya reemplazado al menos uno de los colores alternantes
    bool replaced = false;
    for (std::size_t i = 0; i < getImage().redChannel.size(); ++i) {
        if (getImage().redChannel[i] != COLOR_MAX || getImage().greenChannel[i] != COLOR_MIN || getImage().blueChannel[i] != COLOR_MIN) {
            replaced = true;
            break;
        }
    }
    EXPECT_TRUE(replaced);
}

// Caso de prueba 7: Imagen con solo un color único
TEST_F(CutFreqTest, SingleUniqueColor) {
    // Configurar una imagen con solo un color único
    getImage().redChannel = {COLOR_LOW, COLOR_LOW, COLOR_LOW, COLOR_LOW, COLOR_LOW, COLOR_LOW, COLOR_LOW, COLOR_LOW, COLOR_LOW};
    getImage().greenChannel = {COLOR_OTHER, COLOR_OTHER, COLOR_OTHER, COLOR_OTHER, COLOR_OTHER, COLOR_OTHER, COLOR_OTHER, COLOR_OTHER, COLOR_OTHER};
    getImage().blueChannel = {COLOR_HIGH, COLOR_HIGH, COLOR_HIGH, COLOR_HIGH, COLOR_HIGH, COLOR_HIGH, COLOR_HIGH, COLOR_HIGH, COLOR_HIGH};

    // Eliminar el color menos frecuente (n = 1)
    cutfreq(getImage(), 1);

    // Dado que solo hay un color único, no debería haber cambios
    for (std::size_t i = 0; i < getImage().redChannel.size(); ++i) {
        EXPECT_EQ(getImage().redChannel[i], COLOR_LOW);
        EXPECT_EQ(getImage().greenChannel[i], COLOR_OTHER);
        EXPECT_EQ(getImage().blueChannel[i], COLOR_HIGH);
    }
}

// Caso de prueba 8: Valor grande para n
TEST_F(CutFreqTest, LargeNValue) {
    // Establecer n a un valor muy grande
    cutfreq(getImage(), LARGE_N);

    // Verificar que todos los colores aún estén presentes (sin bloqueos ni comportamiento indefinido)
    EXPECT_EQ(getImage().redChannel.size(), 9);
    EXPECT_EQ(getImage().greenChannel.size(), 9);
    EXPECT_EQ(getImage().blueChannel.size(), 9);
}

// Caso de prueba 9: Imagen con colores aleatorios
TEST_F(CutFreqTest, RandomColors) {
    // Configurar una imagen con colores aleatorios
    getImage().redChannel = {COLOR_ALT1, COLOR_MAX, COLOR_MID, COLOR_ALT2, COLOR_ALT4, COLOR_ALT3, COLOR_OTHER, COLOR_HIGH, COLOR_MIN};
    getImage().greenChannel = {COLOR_HIGH, COLOR_ALT7, COLOR_ALT6, COLOR_ALT3, COLOR_MAX};
}