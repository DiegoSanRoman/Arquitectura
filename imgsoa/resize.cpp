//
// Created by barbara on 19/10/24.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <array>

// Implementación de clamp
template <typename T>
T clamp(T valor, T minimo, T maximo) {
    if (valor < minimo) return minimo;
    if (valor > maximo) return maximo;
    return valor;
}

// Función para leer una imagen PPM en formato P6 (binario)
void leerPPM(const std::string& archivo, int& w, int& h, std::vector<unsigned char>& r, std::vector<unsigned char>& g, std::vector<unsigned char>& b) {
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

    // Inicializar los vectores para cada componente RGB
    r.resize(w * h);
    g.resize(w * h);
    b.resize(w * h);

    // Leer los píxeles en formato binario
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            char rr, gg, bb;
            entrada.read(&rr, 1);
            entrada.read(&gg, 1);
            entrada.read(&bb, 1);
            int idx = i * w + j;
            r[idx] = static_cast<unsigned char>(rr);
            g[idx] = static_cast<unsigned char>(gg);
            b[idx] = static_cast<unsigned char>(bb);
        }
    }

    entrada.close();
}

// Función para escribir una imagen PPM en formato P6
void escribirPPM(const std::string& archivo, const std::vector<unsigned char>& r, const std::vector<unsigned char>& g, const std::vector<unsigned char>& b, int w, int h) {
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
            int idx = i * w + j;
            unsigned char colores[3] = { r[idx], g[idx], b[idx] };
            salida.write(reinterpret_cast<char*>(colores), 3);
        }
    }

    salida.close();
}

// Función para interpolar linealmente entre dos valores
int interpolar(int c1, int c2, float t) {
    return c1 + t * (c2 - c1);
}

// Interpolación bilineal entre cuatro píxeles (sin usar estructuras)
void interpolacionBilineal(const std::vector<unsigned char>& r, const std::vector<unsigned char>& g, const std::vector<unsigned char>& b, int w, int h, int xl, int xh, int yl, int yh, float tx, float ty, unsigned char& rInterpolado, unsigned char& gInterpolado, unsigned char& bInterpolado) {

    // Calcular los índices de los 4 píxeles
    int idx00 = yl * w + xl; // Píxel en la esquina superior izquierda
    int idx01 = yl * w + xh; // Píxel en la esquina superior derecha
    int idx10 = yh * w + xl; // Píxel en la esquina inferior izquierda
    int idx11 = yh * w + xh; // Píxel en la esquina inferior derecha

    // Hacer la interpolación bilineal para los tres componentes
    rInterpolado = interpolar(interpolar(r[idx00], r[idx01], tx), interpolar(r[idx10], r[idx11], tx), ty);
    gInterpolado = interpolar(interpolar(g[idx00], g[idx01], tx), interpolar(g[idx10], g[idx11], tx), ty);
    bInterpolado = interpolar(interpolar(b[idx00], b[idx01], tx), interpolar(b[idx10], b[idx11], tx), ty);
}


// Función para escalar una imagen usando interpolación bilineal
void escalarImagen(const std::vector<unsigned char>& rOriginal, const std::vector<unsigned char>& gOriginal, const std::vector<unsigned char>& bOriginal,int w, int h, int w_nueva, int h_nueva, std::vector<unsigned char>& rEscalada, std::vector<unsigned char>& gEscalada, std::vector<unsigned char>& bEscalada) {

    // Inicializar los vectores para los componentes R, G, B de la imagen escalada
    rEscalada.resize(w_nueva * h_nueva);
    gEscalada.resize(w_nueva * h_nueva);
    bEscalada.resize(w_nueva * h_nueva);

    // Realizar la interpolación bilineal para cada píxel
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

            // Variables para almacenar los componentes interpolados
            unsigned char rInterpolado, gInterpolado, bInterpolado;

            // Llamar a la función de interpolación bilineal
            interpolacionBilineal(rOriginal, gOriginal, bOriginal, w, h, xl, xh, yl, yh, tx, ty, rInterpolado, gInterpolado, bInterpolado);

            // Asignar los valores interpolados en los vectores de la imagen escalada
            int idx = y_nueva * w_nueva + x_nueva;
            rEscalada[idx] = rInterpolado;
            gEscalada[idx] = gInterpolado;
            bEscalada[idx] = bInterpolado;
        }
    }
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
    std::vector<unsigned char> rOriginal, gOriginal, bOriginal;
    leerPPM(archivoEntrada, anchuraOriginal, alturaOriginal, rOriginal, gOriginal, bOriginal);

    // Inicializar los vectores para la imagen escalada
    std::vector<unsigned char> rEscalada, gEscalada, bEscalada;

    // Llamar a la función para escalar la imagen
    escalarImagen(rOriginal, gOriginal, bOriginal, anchuraOriginal, alturaOriginal, nuevaAnchura, nuevaAltura, rEscalada, gEscalada, bEscalada);

    // Guardar la imagen escalada
    escribirPPM(archivoSalida, rEscalada, gEscalada, bEscalada, nuevaAnchura, nuevaAltura);

    return 0;
}