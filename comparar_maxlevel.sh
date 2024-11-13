#!/bin/bash

# Archivos de entrada (imágenes PPM originales)
input_file1="Ejemplos/deer-small-100.ppm"
input_file2="output/deer-small-100-soa.ppm"
output="output/diff_result-soa-100.txt"
file1_txt="output/file1.txt"
file2_txt="output/file2.txt"
threshold=1

# Crear directorios si no existen
mkdir -p "$(dirname "$output")"
mkdir -p "$(dirname "$file1_txt")"
mkdir -p "$(dirname "$file2_txt")"

# Archivos temporales para el hexdump
temp_file1=$(mktemp)
temp_file2=$(mktemp)

# Limpia el archivo de salida
> "$output"

# Convierte los archivos PPM a formato hexadecimal usando xxd y guarda en archivos temporales
# Usa tail para omitir la primera línea de encabezado en el archivo PPM
tail -n +2 "$input_file1" | xxd > "$temp_file1"
tail -n +2 "$input_file2" | xxd > "$temp_file2"

# Guarda los hexdumps en los archivos de salida
cp "$temp_file1" "$file1_txt"
cp "$temp_file2" "$file2_txt"

# Compara los archivos byte a byte
while IFS= read -r -u3 line1 && IFS= read -r -u4 line2; do
  # Extrae el offset inicial
  offset1=$(echo "$line1" | awk '{print $1}')
  offset2=$(echo "$line2" | awk '{print $1}')

  # Extrae los valores hexadecimales sin los offsets iniciales
  bytes1=($(echo "$line1" | awk '{for(i=2;i<=NF;i++) if ($i ~ /^[0-9a-fA-F]+$/) print $i}'))
  bytes2=($(echo "$line2" | awk '{for(i=2;i<=NF;i++) if ($i ~ /^[0-9a-fA-F]+$/) print $i}'))

  # Asegúrate de que ambas líneas tengan la misma cantidad de bytes antes de comparar
  if [ "${#bytes1[@]}" -ne "${#bytes2[@]}" ]; then
    echo "Error: las líneas de los archivos tienen diferentes longitudes." >> "$output"
    continue
  fi

  # Recorre cada par de bytes en la línea actual
  for i in "${!bytes1[@]}"; do
    # Convierte los bytes hexadecimales a decimal, comprobando que el valor no esté vacío
    val1=$((16#${bytes1[i]:-0}))
    val2=$((16#${bytes2[i]:-0}))

    # Calcula la diferencia absoluta
    diff=$((val1 > val2 ? val1 - val2 : val2 - val1))

    # Si la diferencia excede el umbral, la escribe en el archivo de salida
    if ((diff > threshold)); then
      echo "Diferencia en offset $offset1, byte $i: $val1 (file1) != $val2 (file2) (diff = $diff)" >> "$output"
    fi
  done
done 3<"$temp_file1" 4<"$temp_file2"

# Muestra un mensaje final
if [ -s "$output" ]; then
  echo "Comparación completada. Diferencias encontradas y guardadas en $output."
else
  echo "Los archivos son iguales dentro del umbral de diferencia."
fi

# Limpia los archivos temporales
rm "$temp_file1" "$temp_file2"