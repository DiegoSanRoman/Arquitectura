#!/bin/bash

# Función para comparar dos bytes hexadecimales con las nuevas reglas
compare_bytes() {
  local byte1=$1
  local byte2=$2

  local val1=$((16#$byte1))
  local val2=$((16#$byte2))

  local diff=$((val1 - val2))
  diff=${diff#-}  # Obtener el valor absoluto de la diferencia

  if ((diff == 0 || diff == 1 || diff == 256 || diff == 257)); then
    return 0
  else
    return 1
  fi
}

# Función para comparar dos archivos PPM
compare_files() {
  local file1=$1
  local file2=$2

  local success=true

  while IFS= read -r -u3 line1 && IFS= read -r -u4 line2; do
    bytes1=($(echo "$line1" | awk '{for(i=2;i<=NF;i++) if ($i ~ /^[0-9a-fA-F]+$/) print $i}'))
    bytes2=($(echo "$line2" | awk '{for(i=2;i<=NF;i++) if ($i ~ /^[0-9a-fA-F]+$/) print $i}'))

    for i in "${!bytes1[@]}"; do
      if ! compare_bytes "${bytes1[i]}" "${bytes2[i]}"; then
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
# Test para deer-small con un maxlevel de 255
echo "TEST - deer-small con maxvalue = 255"
expected="exp-out-maxlevel/deer-small-255.ppm"
input="../input/deer-small.ppm"
output="act-out-maxlevel/deer-small-255-soa.ppm"

mkdir -p "$(dirname "$expected")"
mkdir -p "$(dirname "$input")"
mkdir -p "$(dirname "$output")"

if [ ! -f "../build/imtool-soa/imtool-soa" ]; then
  echo "Error: ../build/imtool-soa/imtool-soa no existe."
  exit 1
fi

if [ ! -f "$expected" ];then
  echo "Error: $expected no existe."
  exit 1
fi

if [ ! -f "$input" ]; then
  echo "Error: $input no existe."
  exit 1
fi

../build/imtool-soa/imtool-soa "$input" "$output" maxlevel 255

if [ ! -f "$output" ]; then
  echo "Error: $output no se creó."
  exit 1
fi

temp_file1=$(mktemp)
temp_file2=$(mktemp)

tail -n +2 "$expected" | xxd > "$temp_file1"
tail -n +2 "$output" | xxd > "$temp_file2"

if compare_files "$temp_file1" "$temp_file2"; then
  echo "Test con imagen deer-small.ppm y maxvalue = 255 correcto"
else
  echo "Test fallido con imagen deer-small.ppm y maxvalue = 255"
  exit 1
fi

rm "$temp_file1" "$temp_file2"

# --------------------------------------------------------------------------------------------
# Test para deer-small con un maxlevel de 65535
echo "TEST - deer-small con maxvalue = 65535"
expected="exp-out-maxlevel/deer-small-65535.ppm"
input="../input/deer-small.ppm"
output="act-out-maxlevel/deer-small-65535-soa.ppm"

mkdir -p "$(dirname "$expected")"
mkdir -p "$(dirname "$input")"
mkdir -p "$(dirname "$output")"

if [ ! -f "../build/imtool-soa/imtool-soa" ]; then
  echo "Error: ../build/imtool-soa/imtool-soa no existe."
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

../build/imtool-soa/imtool-soa "$input" "$output" maxlevel 65535

if [ ! -f "$output" ]; then
  echo "Error: $output no se creó."
  exit 1
fi

temp_file1=$(mktemp)
temp_file2=$(mktemp)

tail -n +2 "$expected" | xxd > "$temp_file1"
tail -n +2 "$output" | xxd > "$temp_file2"

if compare_files "$temp_file1" "$temp_file2"; then
  echo "Test con imagen deer-small.ppm y maxvalue = 65535 correcto"
else
  echo "Test fallido con imagen deer-small.ppm y maxvalue = 65535"
  exit 1
fi

rm "$temp_file1" "$temp_file2"

# --------------------------------------------------------------------------------------------
# Test para deer-small con un maxlevel de 100
echo "TEST - deer-small con maxvalue = 100"
expected="exp-out-maxlevel/deer-small-100.ppm"
input="../input/deer-small.ppm"
output="act-out-maxlevel/deer-small-100-soa.ppm"

mkdir -p "$(dirname "$expected")"
mkdir -p "$(dirname "$input")"
mkdir -p "$(dirname "$output")"

if [ ! -f "../build/imtool-soa/imtool-soa" ]; then
  echo "Error: ../build/imtool-soa/imtool-soa no existe."
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

../build/imtool-soa/imtool-soa "$input" "$output" maxlevel 100

if [ ! -f "$output" ]; then
  echo "Error: $output no se creó."
  exit 1
fi

temp_file1=$(mktemp)
temp_file2=$(mktemp)

tail -n +2 "$expected" | xxd > "$temp_file1"
tail -n +2 "$output" | xxd > "$temp_file2"

if compare_files "$temp_file1" "$temp_file2"; then
  echo "Test con imagen deer-small.ppm y maxvalue = 100 correcto"
else
  echo "Test fallido con imagen deer-small.ppm y maxvalue = 100"
  exit 1
fi

rm "$temp_file1" "$temp_file2"

# --------------------------------------------------------------------------------------------
# Test para deer-small con un maxlevel de 1000
echo "TEST - deer-small con maxvalue = 1000"
expected="exp-out-maxlevel/deer-small-1000.ppm"
input="../input/deer-small.ppm"
output="act-out-maxlevel/deer-small-1000-aos.ppm"

mkdir -p "$(dirname "$expected")"
mkdir -p "$(dirname "$input")"
mkdir -p "$(dirname "$output")"

if [ ! -f "../build/imtool-soa/imtool-soa" ]; then
  echo "Error: ../build/imtool-soa/imtool-soa no existe."
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

../build/imtool-soa/imtool-soa "$input" "$output" maxlevel 1000

if [ ! -f "$output" ]; then
  echo "Error: $output no se creó."
  exit 1
fi

temp_file1=$(mktemp)
temp_file2=$(mktemp)

tail -n +2 "$expected" | xxd > "$temp_file1"
tail -n +2 "$output" | xxd > "$temp_file2"

if compare_files "$temp_file1" "$temp_file2"; then
  echo "Test con imagen deer-small.ppm y maxvalue = 1000 correcto"
else
  echo "Test fallido con imagen deer-small.ppm y maxvalue = 1000"
  exit 1
fi

rm "$temp_file1" "$temp_file2"

# --------------------------------------------------------------------------------------------
# Test para lake-small con un maxlevel de 255
echo "TEST - lake-small con maxvalue = 255"
expected="exp-out-maxlevel/lake-small-255.ppm"
input="../input/lake-small.ppm"
output="act-out-maxlevel/lake-small-255-soa.ppm"

mkdir -p "$(dirname "$expected")"
mkdir -p "$(dirname "$input")"
mkdir -p "$(dirname "$output")"

if [ ! -f "../build/imtool-soa/imtool-soa" ]; then
  echo "Error: ../build/imtool-soa/imtool-soa no existe."
  exit 1
fi

if [ ! -f "$expected" ];then
  echo "Error: $expected no existe."
  exit 1
fi

if [ ! -f "$input" ]; then
  echo "Error: $input no existe."
  exit 1
fi

../build/imtool-soa/imtool-soa "$input" "$output" maxlevel 255

if [ ! -f "$output" ]; then
  echo "Error: $output no se creó."
  exit 1
fi

temp_file1=$(mktemp)
temp_file2=$(mktemp)

tail -n +2 "$expected" | xxd > "$temp_file1"
tail -n +2 "$output" | xxd > "$temp_file2"

if compare_files "$temp_file1" "$temp_file2"; then
  echo "Test con imagen lake-small.ppm y maxvalue = 255 correcto"
else
  echo "Test fallido con imagen lake-small.ppm y maxvalue = 255"
  exit 1
fi

rm "$temp_file1" "$temp_file2"

# --------------------------------------------------------------------------------------------
# Test para lake-small con un maxlevel de 65535
echo "TEST - lake-small con maxvalue = 65535"
expected="exp-out-maxlevel/lake-small-65535.ppm"
input="../input/lake-small.ppm"
output="act-out-maxlevel/lake-small-65535-soa.ppm"

mkdir -p "$(dirname "$expected")"
mkdir -p "$(dirname "$input")"
mkdir -p "$(dirname "$output")"

if [ ! -f "../build/imtool-soa/imtool-soa" ]; then
  echo "Error: ../build/imtool-soa/imtool-soa no existe."
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

../build/imtool-soa/imtool-soa "$input" "$output" maxlevel 65535

if [ ! -f "$output" ]; then
  echo "Error: $output no se creó."
  exit 1
fi

temp_file1=$(mktemp)
temp_file2=$(mktemp)

tail -n +2 "$expected" | xxd > "$temp_file1"
tail -n +2 "$output" | xxd > "$temp_file2"

if compare_files "$temp_file1" "$temp_file2"; then
  echo "Test con imagen lake-small.ppm y maxvalue = 65535 correcto"
else
  echo "Test fallido con imagen lake-small.ppm y maxvalue = 65535"
  exit 1
fi

rm "$temp_file1" "$temp_file2"