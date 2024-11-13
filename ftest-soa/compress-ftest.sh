#!/bin/sh
#test con deer_small
../build/imtool-soa/imtool-soa ../input/deer-small.ppm ../output/deer-compress.cppm compress

#Salida de mi funcion
deer_out="../output/deer-compress.cppm"
#Salida de referencia
deer_exp="exp-out-compress/deer-small.cppm"

if cmp "$deer_out" "$deer_exp"; then
        echo "Test con imagen deer-small.ppm correcto"
else
        echo "Test fallido con imagen deer-small.ppm"
        exit 1
fi

#test con lake_small
../build/imtool-soa/imtool-soa ../input/lake-small.ppm ../output/lake-compress.cppm compress

#Salida de mi funcion
lake_out="../output/lake-compress.cppm"
#Salida de referencia
lake_exp="exp-out-compress/lake-small.cppm"

if cmp -s "$lake_out" "$lake_exp"; then
        echo "Test con imagen lake-small.ppm correcto"
else
        echo "Test fallido con imagen lake-small.ppm"
        exit 1
fi
