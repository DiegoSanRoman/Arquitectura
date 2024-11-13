#!/bin/bash

# Test para deer-small con un maxlevel de 255
# Archivos de entrada (imágenes PPM originales)
expected="../exp-out-maxlevel/deer-small-255.ppm"
input="../input/deer-small.ppm"

# Salida de mi función
../build/imtool-aos/imtool-aos "$input" "../act-out-maxlevel/deer-small-255-aos.ppm" maxlevel 255
output="../act-out-maxlevel/deer-small-255-aos.ppm"
threshold=1

# Crear directorios si no existen
mkdir -p "$(dirname "$expected")"
mkdir -p "$(dirname "$input")"
mkdir -p "$(dirname "$output")"

# Archivos temporales para el hexdump
temp_file1=$(mktemp)
temp_file2=$(mktemp)

# Convierte los archivos PPM a formato hexadecimal usando xxd y guarda en archivos temporales
# Usa tail para omitir la primera línea de encabezado en el archivo PPM
tail -n +2 "$expected" | xxd > "$temp_file1"
tail -n +2 "$output" | xxd > "$temp_file2"

# Compara los archivos byte a byte y muestra un mensaje de exito o de fallo en general (mirando el threshold)
success=true
while IFS= read -r -u3 line1 && IFS= read -r -u4 line2; do
  # Extrae los valores hexadecimales sin los offsets iniciales
  bytes1=($(echo "$line1" | awk '{for(i=2;i<=NF;i++) if ($i ~ /^[0-9a-fA-F]+$/) print $i}'))
  bytes2=($(echo "$line2" | awk '{for(i=2;i<=NF;i++) if ($i ~ /^[0-9a-fA-F]+$/) print $i}'))

  # Asegúrate de que ambas líneas tengan la misma cantidad de bytes antes de comparar
  if [ "${#bytes1[@]}" -ne "${#bytes2[@]}" ]; then
    success=false
    break
  fi

  # Recorre cada par de bytes en la línea actual
  for i in "${!bytes1[@]}"; do
    # Convierte los bytes hexadecimales a decimal
    val1=$((16#${bytes1[i]}))
    val2=$((16#${bytes2[i]}))

    # Calcula la diferencia absoluta
    diff=$((val1 > val2 ? val1 - val2 : val2 - val1))

    # Si la diferencia excede el umbral, marca el test como fallido
    if ((diff > threshold)); then
      success=false
      break
    fi
  done

  # Si ya se ha marcado como fallido, no es necesario seguir comparando
  if [ "$success" = false ]; then
    break
  fi
done 3<"$temp_file1" 4<"$temp_file2"

# Muestra un mensaje final
if [ "$success" = true ]; then
  echo "Test con imagen deer-small.ppm correcto"
else
  echo "Test fallido con imagen deer-small.ppm"
  exit 1
fi

# Limpia los archivos temporales
rm "$temp_file1" "$temp_file2"