# Proyecto de programación orientada al rendimiento

**Proyecto para la asignatura de Arquitectura de Computadores**  
**Curso 2024/2025 - Universidad Carlos III de Madrid**

Este proyecto tiene como objetivo desarrollar una aplicación de procesamiento de imágenes que realice diversas transformaciones utilizando C++20, como parte de la optimización de programas secuenciales.

## Tabla de Contenidos

- [Descripción General](#descripción-general)
- [Objetivos](#objetivos)
- [Requisitos](#requisitos)
- [Estructura del Proyecto](#estructura-del-proyecto)
- [Instalación y Uso](#instalación-y-uso)
- [Colaboradores](#colaboradores)

## Descripción General

Este proyecto se centra en la optimización de programas secuenciales a través del desarrollo de una aplicación de procesamiento de imágenes en formato PPM (Portable Pixel Map). La aplicación permitirá realizar operaciones como obtención de metadatos, escalado de tamaño, escalado de intensidad, eliminación de colores menos frecuentes y compresión de imágenes.

Se trabajará con dos enfoques de implementación: **Structure of Arrays (SOA)** y **Array of Structures (AOS)**, proporcionando dos versiones de la herramienta.

## Objetivos

- Familiarizarse con la optimización de programas secuenciales en C++.
- Implementar operaciones de procesamiento de imágenes en formato PPM.
- Desarrollar y comparar versiones optimizadas utilizando las estrategias SOA y AOS.

## Requisitos

- **C++20**: Para aprovechar las mejoras de C++ en el desarrollo.
- **CMake**: Herramienta para la configuración y construcción del proyecto.
- **GoogleTest**: Framework para pruebas unitarias.
- **GSL**: Uso de la biblioteca de soporte a las C++ Core Guidelines.

## Estructura del Proyecto

- **common/**: Biblioteca con archivos comunes a las versiones SOA y AOS.
- **imgsoa/**: Biblioteca que implementa la estrategia SOA.
- **imgaos/**: Biblioteca que implementa la estrategia AOS.
- **utest-common/**: Pruebas unitarias para la biblioteca común.
- **utest-imgsoa/**: Pruebas unitarias para la biblioteca SOA.
- **utest-imgaos/**: Pruebas unitarias para la biblioteca AOS.
- **ftest-soa/**: Pruebas funcionales para la versión SOA.
- **ftest-aos/**: Pruebas funcionales para la versión AOS.
- **imtool-soa**: Ejecutable que usa la estrategia SOA.
- **imtool-aos**: Ejecutable que usa la estrategia AOS.


## Instalación y Uso

### Compilación

Para compilar el proyecto, asegúrate de tener **CMake** instalado y sigue estos pasos:

```bash
git clone https://github.com/usuario/proyecto-imagenes.git
cd proyecto-imagenes
mkdir build
cd build
cmake ..
make
```
### Ejecución

La aplicación puede ejecutarse con distintos parámetros según la operación que se quiera realizar. Ejemplo:

```bash
./imtool-soa in.ppm output.ppm resize 800 600
```

## Colaboradores

Este proyecto ha sido desarrollado por los siguientes estudiantes:

- **Nombre 1** - NIA 12345678  
  Responsable de ...

- **Nombre 2** - NIA 87654321  
  Responsable de ...

- **Nombre 3** - NIA 11223344  
  Responsable de ...

- **Nombre 4** - NIA 44332211  
  Responsable de ...

