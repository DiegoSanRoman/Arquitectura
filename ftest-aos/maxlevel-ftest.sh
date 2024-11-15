#!/bin/bash

# Function to compare two hexadecimal bytes with new rules
compare_bytes() {
  local byte1=$1
  local byte2=$2

  local val1=$((16#$byte1))
  local val2=$((16#$byte2))

  local diff=$((val1 - val2))
  diff=${diff#-}  # Get the absolute value of the difference

  if ((diff == 0 || diff == 1 || diff == 256 || diff == 257)); then
    return 0
  else
    return 1
  fi
}

# Function to compare two PPM files
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

# Function to run a test
run_test() {
  local test_name=$1
  local expected=$2
  local input=$3
  local output=$4
  local maxlevel=$5

  echo "TEST - $test_name con maxvalue = $maxlevel"

  mkdir -p "$(dirname "$expected")"
  mkdir -p "$(dirname "$input")"
  mkdir -p "$(dirname "$output")"

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

  ../build/imtool-aos/imtool-aos "$input" "$output" maxlevel "$maxlevel"

  if [ ! -f "$output" ]; then
    echo "Error: $output no se creó."
    exit 1
  fi

  temp_file1=$(mktemp)
  temp_file2=$(mktemp)

  tail -n +2 "$expected" | xxd > "$temp_file1"
  tail -n +2 "$output" | xxd > "$temp_file2"

  if compare_files "$temp_file1" "$temp_file2"; then
    echo "Test con imagen $input y maxvalue = $maxlevel correcto"
  else
    echo "Test fallido con imagen $input y maxvalue = $maxlevel"
    exit 1
  fi

  rm "$temp_file1" "$temp_file2"
}

# Run tests
run_test "deer-small" "exp-out-maxlevel/deer-small-255.ppm" "../in/deer-small.ppm" "act-out-maxlevel/deer-small-255-aos.ppm" 255
run_test "deer-small" "exp-out-maxlevel/deer-small-65535.ppm" "../in/deer-small.ppm" "act-out-maxlevel/deer-small-65535-aos.ppm" 65535
run_test "lake-small" "exp-out-maxlevel/lake-small-255.ppm" "../in/lake-small.ppm" "act-out-maxlevel/lake-small-255-aos.ppm" 255
run_test "lake-small" "exp-out-maxlevel/lake-small-65535.ppm" "../in/lake-small.ppm" "act-out-maxlevel/lake-small-65535-aos.ppm" 65535