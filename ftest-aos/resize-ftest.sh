#!/bin/bash

# Test de la función resize con la imagen lake-small.ppm y dimensiones 1000x1000
input="../input/lake-small.ppm"
output="../output/lake-small-1000.ppm"
expected="../solutions/resize2/lake-small-1000.ppm"
comparison_image="../difference.png"
threshold=5

# Crear directorios si no existen
mkdir -p "$(dirname "$output")"
#mkdir -p "$(dirname "$comparison_image")"

# Ejecutar la función de resize
../build/imtool-aos/imtool-aos "$input" "$output" resize 1000 1000

# Comparar la salida generada con la esperada
compare "$output" "$expected" "$comparison_image"

# Comprobar si las imágenes son idénticas con un umbral de comparación
temp_file1=$(mktemp)
temp_file2=$(mktemp)

# Convertir los archivos PPM a hexadecimal (omitiendo la cabecera)
tail -n +2 "$expected" | xxd > "$temp_file1"
tail -n +2 "$output" | xxd > "$temp_file2"

success=true
while IFS= read -r -u3 line1 && IFS= read -r -u4 line2; do
  bytes1=($(echo "$line1" | awk '{for(i=2;i<=NF;i++) if ($i ~ /^[0-9a-fA-F]+$/) print $i}'))
  bytes2=($(echo "$line2" | awk '{for(i=2;i<=NF;i++) if ($i ~ /^[0-9a-fA-F]+$/) print $i}'))

  if [ "${#bytes1[@]}" -ne "${#bytes2[@]}" ]; then
    success=false
    break
  fi

  for i in "${!bytes1[@]}"; do
    val1=$((16#${bytes1[i]}))
    val2=$((16#${bytes2[i]}))

    diff=$((val1 > val2 ? val1 - val2 : val2 - val1))
    if ((diff > threshold)); then
      success=false
      break
    fi
  done

  if [ "$success" = false ]; then
    break
  fi
done 3<"$temp_file1" 4<"$temp_file2"

if [ "$success" = true ]; then
  echo "Test con imagen lake-small.ppm correcto"
else
  echo "Test fallido con imagen lake-small.ppm"
  exit 1
fi

# Limpiar archivos temporales
rm "$temp_file1" "$temp_file2"
