#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

// Implementación de clamp
template <typename T>
T clamp(T valor, T minimo, T maximo) {
    if (valor < minimo) return minimo;
    if (valor > maximo) return maximo;
    return valor;
}

// Estructura para almacenar un píxel (RGB)
struct Pixel {
    int r, g, b;
};

// Función para leer una imagen PPM en formato P6 (binario)
std::vector<std::vector<Pixel>> leerPPM(const std::string& archivo, int& w, int& h) {
    std::ifstream entrada(archivo, std::ios::binary);
    std::string formato;
    int maxColor;

    if (!entrada.is_open()) {
        std::cerr << "Error al abrir el archivo " << archivo << std::endl;
        exit(1);
    }

    entrada >> formato >> w >> h >> maxColor; // Leer la cabecera del archivo
    entrada.get(); // Consumir el salto de línea que queda después de maxColor

    if (formato != "P6") {
        std::cerr << "El archivo no está en formato P6 (binario)." << std::endl;
        exit(1);
    }

    std::vector<std::vector<Pixel>> imagen(h, std::vector<Pixel>(w));

    // Leer los píxeles en formato binario
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            char r, g, b;
            entrada.read(&r, 1);
            entrada.read(&g, 1);
            entrada.read(&b, 1);
            imagen[i][j].r = static_cast<unsigned char>(r);
            imagen[i][j].g = static_cast<unsigned char>(g);
            imagen[i][j].b = static_cast<unsigned char>(b);
        }
    }

    entrada.close();
    return imagen;
}


// Función para escribir una imagen PPM
void escribirPPM(const std::string& archivo, const std::vector<std::vector<Pixel>>& imagen, int w, int h) {
    std::ofstream salida(archivo, std::ios::binary); // abrir en modo binario

    if (!salida.is_open()) {
        std::cerr << "Error al abrir el archivo para escritura: " << archivo << std::endl;
        exit(1);
    }

    // Cabecera del archivo PPM (en texto)
    salida << "P6\n" << w << " " << h << "\n255\n";

    // Escribir los píxeles en formato binario
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            unsigned char colores[3] = {
                static_cast<unsigned char>(imagen[i][j].r),
                static_cast<unsigned char>(imagen[i][j].g),
                static_cast<unsigned char>(imagen[i][j].b)
            };
            // Escribir el pixel como 3 bytes binarios (RGB)
            salida.write(reinterpret_cast<char*>(colores), 3);
        }
    }

    salida.close();
}

// Función para interpolar linealmente entre dos valores
int interpolar(int c1, int c2, float t) {
    return c1 + t * (c2 - c1);
}

// Interpolación bilineal entre cuatro píxeles
Pixel interpolacionBilineal(const Pixel& p00, const Pixel& p01, const Pixel& p10, const Pixel& p11, float tx, float ty) {
    Pixel resultado;

    resultado.r = interpolar(interpolar(p00.r, p01.r, tx), interpolar(p10.r, p11.r, tx), ty);
    resultado.g = interpolar(interpolar(p00.g, p01.g, tx), interpolar(p10.g, p11.g, tx), ty);
    resultado.b = interpolar(interpolar(p00.b, p01.b, tx), interpolar(p10.b, p11.b, tx), ty);

    return resultado;
}

// Función para escalar una imagen usando interpolación bilineal
std::vector<std::vector<Pixel>> escalarImagen(const std::vector<std::vector<Pixel>>& imagenOriginal, int w, int h, int w_nueva, int h_nueva) {
    std::vector<std::vector<Pixel>> imagenEscalada(h_nueva, std::vector<Pixel>(w_nueva));

    for (int y_nueva = 0; y_nueva < h_nueva; ++y_nueva) {
        for (int x_nueva = 0; x_nueva < w_nueva; ++x_nueva) {
            // Coordenadas correspondientes en la imagen original
            float x_original = x_nueva * (static_cast<float>(w) / w_nueva);
            float y_original = y_nueva * (static_cast<float>(h) / h_nueva);

            int xl = static_cast<int>(std::floor(x_original));
            int xh = static_cast<int>(std::ceil(x_original));
            int yl = static_cast<int>(std::floor(y_original));
            int yh = static_cast<int>(std::ceil(y_original));

            // Asegurarse de no salir del rango de la imagen original
            xl = clamp(xl, 0, w - 1);
            xh = clamp(xh, 0, w - 1);
            yl = clamp(yl, 0, h - 1);
            yh = clamp(yh, 0, h - 1);


            // Interpolación en ambas direcciones
            float tx = x_original - xl;
            float ty = y_original - yl;

            Pixel p00 = imagenOriginal[yl][xl];
            Pixel p01 = imagenOriginal[yl][xh];
            Pixel p10 = imagenOriginal[yh][xl];
            Pixel p11 = imagenOriginal[yh][xh];

            imagenEscalada[y_nueva][x_nueva] = interpolacionBilineal(p00, p01, p10, p11, tx, ty);
        }
    }

    return imagenEscalada;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Uso: " << argv[0] << " <entrada.ppm> <salida.ppm> <nueva_anchura> <nueva_altura>\n";
        return 1;
    }

    std::string archivoEntrada = argv[1];
    std::string archivoSalida = argv[2];
    int nuevaAnchura = std::stoi(argv[3]);
    int nuevaAltura = std::stoi(argv[4]);

    int anchuraOriginal, alturaOriginal;

    // Leer la imagen original
    auto imagenOriginal = leerPPM(archivoEntrada, anchuraOriginal, alturaOriginal);

    // Escalar la imagen
    auto imagenEscalada = escalarImagen(imagenOriginal, anchuraOriginal, alturaOriginal, nuevaAnchura, nuevaAltura);

    // Guardar la imagen escalada
    escribirPPM(archivoSalida, imagenEscalada, nuevaAnchura, nuevaAltura);

    return 0;
}
