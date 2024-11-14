#!/bin/bash

# Función para comparar archivos de imagen PPM
compare_files() {
  local file1=$1
  local file2=$2
  local success=true

  while IFS= read -r -u3 line1 && IFS= read -r -u4 line2; do
    bytes1=($(echo "$line1" | awk '{for(i=2;i<=NF;i++) if ($i ~ /^[0-9a-fA-F]+$/) print $i}'))
    bytes2=($(echo "$line2" | awk '{for(i=2;i<=NF;i++) if ($i ~ /^[0-9a-fA-F]+$/) print $i}'))

    for i in "${!bytes1[@]}"; do
      if [ "${bytes1[i]}" != "${bytes2[i]}" ]; then
        success=false
        break
      fi
    done

    if [ "$success" = false ]; then
      break
    fi
  done 3<"$file1" 4<"$file2"

  if [ "$success" = true ]; then
    return 0
  else
    return 1
  fi
}

# --------------------------------------------------------------------------------------------
# Test funcional para lake-large con cutfreq de 100000
echo "TEST - lake-large con cutfreq = 100000"
expected="exp-out-cutfreq/lake-large-100K.ppm"
input="../input/lake-large.ppm"
output="act-out-cutfreq/lake-large-100K-aos.ppm"

mkdir -p "$(dirname "$expected")" "$(dirname "$input")" "$(dirname "$output")"

if [ ! -f "../build/imtool-aos/imtool-aos" ]; then
  echo "Error: ../build/imtool-aos/imtool-aos no existe."
  exit 1
fi

if [ ! -f "$expected" ]; then
  echo "Error: $expected no existe."
  exit 1
fi

if [ ! -f "$input" ]; then
  echo "Error: $input no existe."
  exit 1
fi

../build/imtool-aos/imtool-aos "$input" "$output" cutfreq 100000

if [ ! -f "$output" ]; then
  echo "Error: $output no se creó."
  exit 1
fi

temp_file1=$(mktemp)
temp_file2=$(mktemp)

tail -n +2 "$expected" | xxd > "$temp_file1"
tail -n +2 "$output" | xxd > "$temp_file2"

if compare_files "$temp_file1" "$temp_file2"; then
  echo "Test con imagen lake-large.ppm y cutfreq = 100000 correcto"
else
  echo "Test fallido con imagen lake-large.ppm y cutfreq = 100000"
  exit 1
fi

rm "$temp_file1" "$temp_file2"

# --------------------------------------------------------------------------------------------
# Test funcional para lake-large con cutfreq de 162000
echo "TEST - lake-large con cutfreq = 162000"
expected="exp-out-cutfreq/lake-large-162K.ppm"
output="act-out-cutfreq/lake-large-162K-aos.ppm"

../build/imtool-aos/imtool-aos "$input" "$output" cutfreq 162000

if [ ! -f "$output" ]; then
  echo "Error: $output no se creó."
  exit 1
fi

tail -n +2 "$expected" | xxd > "$temp_file1"
tail -n +2 "$output" | xxd > "$temp_file2"

if compare_files "$temp_file1" "$temp_file2"; then
  echo "Test con imagen lake-large.ppm y cutfreq = 162000 correcto"
else
  echo "Test fallido con imagen lake-large.ppm y cutfreq = 162000"
  exit 1
fi

rm "$temp_file1" "$temp_file2"
