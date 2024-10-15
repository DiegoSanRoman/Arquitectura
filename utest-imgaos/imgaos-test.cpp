#include "imgaos/cutfreq.hpp"  // Tu archivo con las funciones de `cutfreq`
#include <gtest/gtest.h>
#include <vector>
#include <iostream>  // Para debugging


using namespace imgaos;

constexpr Color BLACK = { .r = 0, .g = 0, .b = 0 };
constexpr Color WHITE = { .r = 255, .g = 255, .b = 255 };
constexpr Color GRAY = { .r = 128, .g = 128, .b = 128 };
constexpr Color RED = { .r = 255, .g = 0, .b = 0 };
constexpr Color GREEN = { .r = 0, .g = 255, .b = 0 };
constexpr Color BLUE = { .r = 0, .g = 0, .b = 255 };

namespace {
  void printImagePixels(const ImageAOS& image) {
    for (const auto& pixel : image.pixels) {
      std::cout << "(" << pixel.r << ", " << pixel.g << ", " << pixel.b << ") ";
    }
    std::cout << "\n";
  }

}// namespace

TEST(CutFreqTest, RemoveNoColors) {
    ImageAOS image = {{ BLACK, WHITE }};
    cutfreq(image, 0);
    EXPECT_EQ(image.pixels[0], BLACK);
    EXPECT_EQ(image.pixels[1], WHITE);
}

TEST(CutFreqTest, RemoveLeastFrequentColor) {
    ImageAOS image = {{ BLACK, WHITE, WHITE, GRAY }};
    cutfreq(image, 1);  // Debe eliminar el color BLACK
    printImagePixels(image);
    std::cout << "Verificando los resultados después de aplicar cutfreq..." << "\n";
    EXPECT_NE(image.pixels[0], BLACK);
    EXPECT_EQ(image.pixels[1], WHITE);
    EXPECT_EQ(image.pixels[2], WHITE);
    EXPECT_EQ(image.pixels[3], GRAY);
}

TEST(CutFreqTest, RemoveMultipleColors) {
    ImageAOS image = {{ BLACK, WHITE, GRAY, GRAY }};
    cutfreq(image, 2);  // Debe eliminar los dos colores menos frecuentes
    printImagePixels(image);
    std::cout << "Verificando los resultados después de aplicar cutfreq..." << "\n";
    EXPECT_NE(image.pixels[0], BLACK);
    EXPECT_NE(image.pixels[1], WHITE);
    EXPECT_EQ(image.pixels[2], image.pixels[3]);  // Ambos deben ser reemplazados por un color restante
}

TEST(CutFreqTest, RemoveAllColorsExceptOne) {
    ImageAOS image = {{ BLACK, WHITE, GRAY, RED, GREEN, BLUE }};
    constexpr int REMOVE_FIVE_COLORS = 5;
    cutfreq(image, REMOVE_FIVE_COLORS);  // Debe dejar solo un color
    printImagePixels(image);
    std::cout << "Verificando los resultados después de aplicar cutfreq..." << "\n";
    const Color remainingColor = image.pixels[0];
    for (const auto& pixel : image.pixels) {
        EXPECT_EQ(pixel, remainingColor) << "El píxel no coincide con el color restante esperado. Pixel: (" << pixel.r << ", " << pixel.g << ", " << pixel.b << "), RemainingColor: (" << remainingColor.r << ", " << remainingColor.g << ", " << remainingColor.b << ")";
    }
}

TEST(CutFreqTest, AllColorsAreFrequent) {
    ImageAOS image = {{ BLACK, WHITE, GRAY, RED, GREEN, BLUE }};
    cutfreq(image, 0);  // No se debe eliminar ningún color
    printImagePixels(image);
    std::cout << "Verificando los resultados después de aplicar cutfreq..." << "\n";
    EXPECT_EQ(image.pixels[0], BLACK);
    EXPECT_EQ(image.pixels[1], WHITE);
    EXPECT_EQ(image.pixels[2], GRAY);
    EXPECT_EQ(image.pixels[3], RED);
    EXPECT_EQ(image.pixels[4], GREEN);
    EXPECT_EQ(image.pixels[5], BLUE);
}

TEST(CutFreqTest, RemoveMoreColorsThanPresent) {
    ImageAOS image = {{ BLACK, WHITE, GRAY }};
    constexpr int REMOVE_FIVE_COLORS = 5;
    cutfreq(image, REMOVE_FIVE_COLORS);  // Debe eliminar todos los colores excepto uno
    printImagePixels(image);
    std::cout << "Verificando los resultados después de aplicar cutfreq..." << "\n";
    const Color remainingColor = image.pixels[0];
    for (const auto& pixel : image.pixels) {
        EXPECT_EQ(pixel, remainingColor) << "El píxel no coincide con el color restante esperado. Pixel: (" << pixel.r << ", " << pixel.g << ", " << pixel.b << "), RemainingColor: (" << remainingColor.r << ", " << remainingColor.g << ", " << remainingColor.b << ")";
    }
}

TEST(CutFreqTest, ImageWithSingleColor) {
    ImageAOS image = {{ BLACK, BLACK, BLACK }};
    cutfreq(image, 1);  // No debe cambiar nada ya que solo hay un color
    printImagePixels(image);
    std::cout << "Verificando los resultados después de aplicar cutfreq..." << "\n";
    EXPECT_EQ(image.pixels[0], BLACK);
    EXPECT_EQ(image.pixels[1], BLACK);
    EXPECT_EQ(image.pixels[2], BLACK);
}

TEST(CutFreqTest, ImageWithTwoColorsEquallyFrequent) {
    ImageAOS image = {{ RED, GREEN, RED, GREEN }};
    cutfreq(image, 1);  // Debe eliminar uno de los dos colores
    printImagePixels(image);
    std::cout << "Verificando los resultados después de aplicar cutfreq..." <<"\n";
    const Color remainingColor = image.pixels[0];
    for (const auto& pixel : image.pixels) {
        EXPECT_EQ(pixel, remainingColor) << "El píxel no coincide con el color restante esperado. Pixel: (" << pixel.r << ", " << pixel.g << ", " << pixel.b << "), RemainingColor: (" << remainingColor.r << ", " << remainingColor.g << ", " << remainingColor.b << ")";
    }
}

TEST(CutFreqTest, ComplexImageReplacement) {
    ImageAOS image = {{ RED, GREEN, BLUE, RED, BLUE, GREEN, BLACK, WHITE, GRAY }};
    constexpr int REMOVE_SIX_COLORS = 6;
    cutfreq(image, REMOVE_SIX_COLORS);  // Debe dejar solo los tres colores más frecuentes
    printImagePixels(image);
    std::cout << "Verificando los resultados después de aplicar cutfreq..." << "\n";
    const Color remainingColor1 = image.pixels[0];
    for (const auto& pixel : image.pixels) {
        EXPECT_TRUE(pixel == remainingColor1 || pixel == BLUE || pixel == GREEN) << "El píxel no coincide con los colores restantes esperados. Pixel: (" << pixel.r << ", " << pixel.g << ", " << pixel.b << ")";
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}