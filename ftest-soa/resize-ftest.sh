#!/bin/bash

# Test de la función resize con la imagen lake-small.ppm y dimensiones 100x100
in="../in/lake-small.ppm"
output="act-out-resize/lake-small-100.ppm"
expected="exp-out-resize/resize2/lake-small-100.ppm"
threshold=1  # Umbral para diferencias tolerables entre valores de color

# Crear directorios si no existen
mkdir -p "$(dirname "$output")"

# Ejecutar la función de resize
../build/imtool-soa/imtool-soa "$in" "$output" resize 100 100

# Convertir los archivos PPM a hexadecimal (omitiendo la cabecera de cada archivo)
temp_file1=$(mktemp)
temp_file2=$(mktemp)
tail -n +2 "$expected" | xxd -p -c 1 > "$temp_file1"  # Hex byte por byte
tail -n +2 "$output" | xxd -p -c 1 > "$temp_file2"    # Hex byte por byte

success=true
echo "Comparando la imagen de salida con la imagen solución..."
# Leer cada byte uno por uno y comparar con umbral
while IFS= read -r -u3 byte1 && IFS= read -r -u4 byte2; do
  val1=$((16#$byte1))
  val2=$((16#$byte2))

  # Calcular la diferencia absoluta entre los bytes
  diff=$((val1 > val2 ? val1 - val2 : val2 - val1))

  # Si la diferencia es mayor que el umbral, marcar el test como fallido
  if ((diff > threshold)); then
    success=false
    break
  fi
done 3<"$temp_file1" 4<"$temp_file2"

# Resultado final
if [ "$success" = true ]; then
  echo "Test con imagen lake-small.ppm correcto"
else
  echo "Test fallido con imagen lake-small.ppm"
  exit 1
fi

# Limpiar archivos temporales
rm "$temp_file1" "$temp_file2"
